/* lcos-updates — GTK3 window (never runs as root).
 * Copyright (C) 2026 LCOS
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "window.hpp"

#include <csignal>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {
const char kHelperPath[] = "/usr/libexec/lcos-updates-helper";
const int kCheckTimeoutMs = 180 * 1000;    /* 3 minutes, matches spec */
const int kInstallTimeoutMs = 630 * 1000; /* helper upgrade is 600s */
}

UpdatesWindow::UpdatesWindow(bool check_on_start)
{
  set_title("Check for updates…");
  set_default_size(560, 420);
  set_border_width(12);
  /* Window-manager chrome only: do not call set_titlebar / HeaderBar. */

  m_status.set_line_wrap(true);
  m_status.set_xalign(0.0f);
  m_status.set_text("Click Check for updates to see if updates are available from Devuan and LCOS.");

  m_store = Gtk::ListStore::create(m_cols);
  m_view.set_model(m_store);
  m_view.append_column("Package", m_cols.package);
  m_view.append_column("Old version", m_cols.old_version);
  m_view.append_column("New version", m_cols.new_version);
  m_view.set_headers_visible(true);
  m_view.get_selection()->set_mode(Gtk::SELECTION_NONE);

  m_scroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  m_scroller.set_shadow_type(Gtk::SHADOW_IN);
  m_scroller.set_min_content_height(180);
  m_scroller.add(m_view);

  m_buttons.set_layout(Gtk::BUTTONBOX_END);
  m_buttons.set_spacing(8);
  m_spinner.set_no_show_all(true);
  m_buttons.pack_start(m_spinner, Gtk::PACK_SHRINK);
  m_install.set_sensitive(false);
  m_check.set_can_default(true);
  m_buttons.pack_start(m_install, Gtk::PACK_SHRINK);
  m_buttons.pack_start(m_check, Gtk::PACK_SHRINK);
  m_check.signal_clicked().connect(sigc::mem_fun(*this, &UpdatesWindow::on_check_clicked));
  m_install.signal_clicked().connect(sigc::mem_fun(*this, &UpdatesWindow::on_install_clicked));

  m_vbox.pack_start(m_status, Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_scroller, Gtk::PACK_EXPAND_WIDGET);
  m_vbox.pack_start(m_buttons, Gtk::PACK_SHRINK);
  add(m_vbox);
  show_all_children();
  m_check.grab_default();
  set_default(m_check);

  if (check_on_start) {
    Glib::signal_idle().connect_once(sigc::mem_fun(*this, &UpdatesWindow::on_check_clicked));
  }
}

UpdatesWindow::~UpdatesWindow()
{
  cancel_job();
}

void UpdatesWindow::set_busy(bool busy, const Glib::ustring& status)
{
  m_status.set_text(status);
  m_check.set_sensitive(!busy);
  if (busy)
    m_install.set_sensitive(false);
  if (busy) {
    m_spinner.show();
    m_spinner.start();
  } else {
    m_spinner.stop();
    m_spinner.hide();
  }
}

void UpdatesWindow::show_packages(const std::vector<PackageUpgrade>& packages)
{
  m_store->clear();
  for (const auto& pkg : packages) {
    Gtk::TreeModel::Row row = *(m_store->append());
    row[m_cols.package] = pkg.name;
    row[m_cols.old_version] = pkg.old_version;
    row[m_cols.new_version] = pkg.new_version;
  }
}

Glib::ustring UpdatesWindow::friendly_error(const std::string& msg) const
{
  const Glib::ustring m = msg;
  const Glib::ustring lower = m.lowercase();
  if (lower.find("timeout") != Glib::ustring::npos || lower.find("timed out") != Glib::ustring::npos)
    return "Timed out waiting for the update check. Check your network and try again.";
  if (lower.find("temporary failure resolving") != Glib::ustring::npos ||
      lower.find("could not resolve") != Glib::ustring::npos ||
      lower.find("network is unreachable") != Glib::ustring::npos ||
      lower.find("failed to fetch") != Glib::ustring::npos ||
      lower.find("connection timed out") != Glib::ustring::npos ||
      lower.find("unable to connect") != Glib::ustring::npos)
    return "No network connection. Connect to the Internet and try again.";
  if (m.empty())
    return "The update helper failed.";
  return m;
}

void UpdatesWindow::cancel_job()
{
  m_timeout.disconnect();
  m_out_watch.disconnect();
  m_err_watch.disconnect();
  m_child_watch.disconnect();
  m_out_ch.reset();
  m_err_ch.reset();
  if (m_out_fd >= 0) {
    ::close(m_out_fd);
    m_out_fd = -1;
  }
  if (m_err_fd >= 0) {
    ::close(m_err_fd);
    m_err_fd = -1;
  }
  if (m_have_pid) {
    const pid_t p = static_cast<pid_t>(m_pid);
    if (p > 0) {
      kill(-p, SIGTERM);
      kill(p, SIGTERM);
    }
    Glib::spawn_close_pid(m_pid);
    m_have_pid = false;
    m_pid = 0;
  }
  m_job = Job::None;
}

void UpdatesWindow::finish_job()
{
  m_timeout.disconnect();
  m_out_watch.disconnect();
  m_err_watch.disconnect();
  m_out_ch.reset();
  m_err_ch.reset();
  if (m_out_fd >= 0) {
    ::close(m_out_fd);
    m_out_fd = -1;
  }
  if (m_err_fd >= 0) {
    ::close(m_err_fd);
    m_err_fd = -1;
  }
  if (m_have_pid) {
    Glib::spawn_close_pid(m_pid);
    m_have_pid = false;
    m_pid = 0;
  }
  m_job = Job::None;
}

void UpdatesWindow::start_helper(const char* helper_arg, Job job, int timeout_ms)
{
  if (m_job != Job::None)
    return;

  m_stdout.clear();
  m_stderr.clear();
  m_job = job;

  std::vector<std::string> argv;
  argv.push_back("pkexec");
  argv.push_back(kHelperPath);
  argv.push_back(helper_arg);

  auto child_setup = []() {
    setpgid(0, 0);
  };

  int out_fd = -1;
  int err_fd = -1;
  Glib::Pid pid = 0;
  try {
    Glib::spawn_async_with_pipes(
        std::string(), argv,
        Glib::SPAWN_SEARCH_PATH | Glib::SPAWN_DO_NOT_REAP_CHILD, child_setup, &pid, nullptr,
        &out_fd, &err_fd);
  } catch (const Glib::SpawnError& e) {
    m_job = Job::None;
    set_busy(false, "Could not run pkexec. Install policykit-1 and try again.");
    m_install.set_sensitive(false);
    return;
  }

  m_pid = pid;
  m_have_pid = true;
  m_out_fd = out_fd;
  m_err_fd = err_fd;

  m_out_ch = Glib::IOChannel::create_from_fd(out_fd);
  m_err_ch = Glib::IOChannel::create_from_fd(err_fd);
  try {
    m_out_ch->set_encoding("");
    m_err_ch->set_encoding("");
  } catch (const Glib::Exception&) {
  }
  m_out_ch->set_close_on_unref(false);
  m_err_ch->set_close_on_unref(false);

  m_out_watch = Glib::signal_io().connect(sigc::mem_fun(*this, &UpdatesWindow::on_stdout), out_fd,
                                          Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR);
  m_err_watch = Glib::signal_io().connect(sigc::mem_fun(*this, &UpdatesWindow::on_stderr), err_fd,
                                          Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR);
  m_child_watch =
      Glib::signal_child_watch().connect(sigc::mem_fun(*this, &UpdatesWindow::on_child_exited), pid);
  m_timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &UpdatesWindow::on_timeout),
                                             timeout_ms);
}

bool UpdatesWindow::on_stdout(Glib::IOCondition cond)
{
  if (!m_out_ch)
    return false;
  if (cond & (Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR)) {
    gchar buf[4096];
    gsize n = 0;
    const Glib::IOStatus st = m_out_ch->read(buf, sizeof buf, n);
    if (n > 0)
      m_stdout.append(buf, n);
    if (st == Glib::IO_STATUS_EOF || (cond & (Glib::IO_HUP | Glib::IO_ERR)))
      return false;
  }
  return true;
}

bool UpdatesWindow::on_stderr(Glib::IOCondition cond)
{
  if (!m_err_ch)
    return false;
  if (cond & (Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR)) {
    gchar buf[4096];
    gsize n = 0;
    const Glib::IOStatus st = m_err_ch->read(buf, sizeof buf, n);
    if (n > 0)
      m_stderr.append(buf, n);
    if (st == Glib::IO_STATUS_EOF || (cond & (Glib::IO_HUP | Glib::IO_ERR)))
      return false;
  }
  return true;
}

bool UpdatesWindow::on_timeout()
{
  if (m_job == Job::None)
    return false;
  const Job job = m_job;
  if (m_have_pid) {
    const pid_t p = static_cast<pid_t>(m_pid);
    if (p > 0) {
      kill(-p, SIGTERM);
      kill(p, SIGTERM);
    }
  }
  cancel_job();
  if (job == Job::Check) {
    m_store->clear();
    m_install.set_sensitive(false);
    set_busy(false, "Timed out waiting for the update check. Check your network and try again.");
  } else {
    set_busy(false, "Timed out while installing updates.");
    m_install.set_sensitive(true);
  }
  m_check.set_sensitive(true);
  return false;
}

void UpdatesWindow::on_child_exited(Glib::Pid /*pid*/, int wait_status)
{
  /* Drain remaining output. */
  if (m_out_ch) {
    for (;;) {
      gchar buf[4096];
      gsize n = 0;
      const Glib::IOStatus st = m_out_ch->read(buf, sizeof buf, n);
      if (n > 0)
        m_stdout.append(buf, n);
      if (n == 0 || st == Glib::IO_STATUS_EOF || st == Glib::IO_STATUS_ERROR)
        break;
    }
  }
  if (m_err_ch) {
    for (;;) {
      gchar buf[4096];
      gsize n = 0;
      const Glib::IOStatus st = m_err_ch->read(buf, sizeof buf, n);
      if (n > 0)
        m_stderr.append(buf, n);
      if (n == 0 || st == Glib::IO_STATUS_EOF || st == Glib::IO_STATUS_ERROR)
        break;
    }
  }

  const Job job = m_job;
  const std::string out = m_stdout;
  const std::string err = m_stderr;
  finish_job();

  SimulateResult parsed = parse_protocol(out);
  if (parsed.status == SimulateResult::Error && parsed.error_msg == "No STATUS from helper") {
    if (!err.empty()) {
      parsed.error_msg = err;
      /* pkexec cancel / dismiss */
      const Glib::ustring el = Glib::ustring(err).lowercase();
      if (el.find("dismiss") != Glib::ustring::npos || el.find("not authorized") != Glib::ustring::npos ||
          el.find("authentication") != Glib::ustring::npos)
        parsed.error_msg = "Authentication was cancelled or failed.";
    }
  }

  if (job == Job::Check)
    apply_check_result(parsed, wait_status);
  else if (job == Job::Install)
    apply_install_result(parsed, wait_status);
  else
    set_busy(false, m_status.get_text());
}

void UpdatesWindow::apply_check_result(const SimulateResult& result, int wait_status)
{
  const int exit_code = WIFEXITED(wait_status) ? WEXITSTATUS(wait_status) : -1;
  if (result.status == SimulateResult::UpToDate) {
    m_store->clear();
    m_install.set_sensitive(false);
    set_busy(false, "You're up to date.");
    m_check.set_sensitive(true);
    return;
  }
  if (result.status == SimulateResult::Upgrades && !result.packages.empty()) {
    show_packages(result.packages);
    m_install.set_sensitive(true);
    set_busy(false, "Updates are available.");
    m_check.set_sensitive(true);
    return;
  }

  m_store->clear();
  m_install.set_sensitive(false);
  std::string msg = result.error_msg;
  if (msg.empty() || msg == "No STATUS from helper") {
    if (exit_code == 127 || exit_code == 126)
      msg = "Could not run the update helper (pkexec). Authentication may have been cancelled.";
    else
      msg = "The update check failed.";
  }
  set_busy(false, friendly_error(msg));
  m_check.set_sensitive(true);
}

void UpdatesWindow::apply_install_result(const SimulateResult& result, int wait_status)
{
  const int exit_code = WIFEXITED(wait_status) ? WEXITSTATUS(wait_status) : -1;
  if (result.status == SimulateResult::Success ||
      (result.status != SimulateResult::Error && exit_code == 0)) {
    m_store->clear();
    m_install.set_sensitive(false);
    set_busy(false, "Updates installed successfully. You can check again.");
    m_check.set_sensitive(true);
    return;
  }
  std::string msg = result.error_msg;
  if (msg.empty() || msg == "No STATUS from helper") {
    if (exit_code == 127 || exit_code == 126)
      msg = "Could not run the update helper (pkexec). Authentication may have been cancelled.";
    else
      msg = "apt-get upgrade failed.";
  }
  set_busy(false, friendly_error(msg));
  m_check.set_sensitive(true);
  /* Offer Check again; keep Install if we still had a list. */
  if (!m_store->children().empty())
    m_install.set_sensitive(true);
}

void UpdatesWindow::on_check_clicked()
{
  if (m_job != Job::None)
    return;
  m_install.set_sensitive(false);
  set_busy(true, "Checking for updates…");
  start_helper("simulate", Job::Check, kCheckTimeoutMs);
}

void UpdatesWindow::on_install_clicked()
{
  if (m_job != Job::None)
    return;
  set_busy(true, "Installing updates…");
  start_helper("upgrade", Job::Install, kInstallTimeoutMs);
}

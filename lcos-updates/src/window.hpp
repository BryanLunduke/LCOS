/* lcos-updates — GTK3 window (never runs as root).
 * Copyright (C) 2026 LCOS
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LCOS_UPDATES_WINDOW_HPP
#define LCOS_UPDATES_WINDOW_HPP

#include "apt-parse.hpp"

#include <gtkmm.h>
#include <string>

class UpdatesWindow : public Gtk::ApplicationWindow {
public:
  explicit UpdatesWindow(bool check_on_start);
  ~UpdatesWindow() override;

protected:
  void on_check_clicked();
  void on_install_clicked();

private:
  enum class Job { None, Check, Install };

  void set_busy(bool busy, const Glib::ustring& status);
  void start_helper(const char* helper_arg, Job job, int timeout_ms);
  void cancel_job();
  void finish_job();
  bool on_stdout(Glib::IOCondition cond);
  bool on_stderr(Glib::IOCondition cond);
  bool on_timeout();
  void on_child_exited(Glib::Pid pid, int wait_status);
  void apply_check_result(const SimulateResult& result, int wait_status);
  void apply_install_result(const SimulateResult& result, int wait_status);
  void show_packages(const std::vector<PackageUpgrade>& packages);
  Glib::ustring friendly_error(const std::string& msg) const;

  Gtk::Box m_vbox{Gtk::ORIENTATION_VERTICAL, 10};
  Gtk::Label m_status;
  Gtk::ScrolledWindow m_scroller;
  Gtk::TreeView m_view;
  Gtk::ButtonBox m_buttons{Gtk::ORIENTATION_HORIZONTAL};
  Gtk::Button m_check{"Check for updates"};
  Gtk::Button m_install{"Install updates"};
  Gtk::Spinner m_spinner;

  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns()
    {
      add(package);
      add(old_version);
      add(new_version);
    }
    Gtk::TreeModelColumn<Glib::ustring> package;
    Gtk::TreeModelColumn<Glib::ustring> old_version;
    Gtk::TreeModelColumn<Glib::ustring> new_version;
  };

  ModelColumns m_cols;
  Glib::RefPtr<Gtk::ListStore> m_store;

  Job m_job = Job::None;
  Glib::Pid m_pid = 0;
  bool m_have_pid = false;
  int m_out_fd = -1;
  int m_err_fd = -1;
  Glib::RefPtr<Glib::IOChannel> m_out_ch;
  Glib::RefPtr<Glib::IOChannel> m_err_ch;
  sigc::connection m_out_watch;
  sigc::connection m_err_watch;
  sigc::connection m_child_watch;
  sigc::connection m_timeout;
  std::string m_stdout;
  std::string m_stderr;
};

#endif

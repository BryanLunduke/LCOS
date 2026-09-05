/* lcos-updates — Check for updates… (GTK3, X11, never runs as root).
 * Copyright (C) 2026 LCOS
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "window.hpp"

#include <cstdlib>
#include <glib.h>
#include <gtkmm.h>

int main(int argc, char* argv[])
{
  if (g_getenv("GDK_BACKEND") == nullptr)
    g_setenv("GDK_BACKEND", "x11", FALSE);
  g_set_prgname("lcos-updates");

  auto app = Gtk::Application::create("org.lunduke.LcosUpdates");

  bool check_on_start = false;
  app->add_main_option_entry(Gio::Application::OPTION_TYPE_BOOL, "check", '\0',
                             "Check for updates immediately after opening");
  app->signal_handle_local_options().connect(
      [&check_on_start](const Glib::RefPtr<Glib::VariantDict>& options) -> int {
        if (options)
          options->lookup_value("check", check_on_start);
        return -1; /* continue startup */
      });

  app->signal_activate().connect([app, &check_on_start]() {
    auto* win = new UpdatesWindow(check_on_start);
    app->add_window(*win);
    win->signal_hide().connect([win]() { delete win; });
    win->present();
  });

  return app->run(argc, argv);
}

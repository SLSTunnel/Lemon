/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (c) 2008 Red Hat, Inc.
 * Copyright (c) 2008 Intel Corp.
 *
 * Based on plugin skeleton by:
 * Author: Tomas Frydrych <tf@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * LemonPlugin is the entry point for for Lemon into and out of
 * Mutter. By registering itself into Mutter using
 * meta_plugin_manager_set_plugin_type(), Mutter will call the vfuncs of the
 * plugin at the appropriate time.
 *
 * The functions in in LemonPlugin are all just stubs, which just call
 * the similar methods in LemonWm.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <cjs/gjs.h>
#include <meta/display.h>
#include <meta/meta-plugin.h>
#include <meta/util.h>

#include "lemon-global-private.h"
#include "lemon-perf-log.h"
#include "lemon-wm-private.h"

#define LEMON_TYPE_PLUGIN (lemon_plugin_get_type ())
G_DECLARE_FINAL_TYPE (LemonPlugin, lemon_plugin,
                      LEMON, PLUGIN,
                      MetaPlugin)

struct _LemonPlugin
{
  MetaPlugin parent;

  int glx_error_base;
  int glx_event_base;
  guint have_swap_event : 1;
  CoglContext *cogl_context;

  LemonGlobal *global;
};

G_DEFINE_TYPE (LemonPlugin, lemon_plugin, META_TYPE_PLUGIN)

static gboolean
lemon_plugin_has_swap_event (LemonPlugin *lemon_plugin)
{
  if (meta_is_wayland_compositor ())
  {
    return FALSE;
  }

  CoglDisplay *cogl_display =
    cogl_context_get_display (lemon_plugin->cogl_context);
  CoglRenderer *renderer = cogl_display_get_renderer (cogl_display);
  const char * (* query_extensions_string) (Display *dpy, int screen);
  Bool (* query_extension) (Display *dpy, int *error, int *event);
  Display *xdisplay;
  int screen_number;
  const char *glx_extensions;

  /* We will only get swap events if Cogl is using GLX */
  if (cogl_renderer_get_winsys_id (renderer) != COGL_WINSYS_ID_GLX)
    return FALSE;

  xdisplay = clutter_x11_get_default_display ();

  query_extensions_string =
    (void *) cogl_get_proc_address ("glXQueryExtensionsString");
  query_extension =
    (void *) cogl_get_proc_address ("glXQueryExtension");

  query_extension (xdisplay,
                   &lemon_plugin->glx_error_base,
                   &lemon_plugin->glx_event_base);

  screen_number = XDefaultScreen (xdisplay);
  glx_extensions = query_extensions_string (xdisplay, screen_number);

  return strstr (glx_extensions, "GLX_INTEL_swap_event") != NULL;
}

static void
lemon_plugin_start (MetaPlugin *plugin)
{
  LemonPlugin *lemon_plugin = LEMON_PLUGIN (plugin);
  GError *error = NULL;
  int status;
  GjsContext *gjs_context;
  ClutterBackend *backend;

  backend = clutter_get_default_backend ();
  lemon_plugin->cogl_context = clutter_backend_get_cogl_context (backend);

  lemon_plugin->have_swap_event =
    lemon_plugin_has_swap_event (lemon_plugin);

  lemon_perf_log_define_event (lemon_perf_log_get_default (),
                               "glx.swapComplete",
                               "GL buffer swap complete event received (with timestamp of completion)",
                               "x");

  lemon_plugin->global = lemon_global_get ();
  _lemon_global_set_plugin (lemon_plugin->global, META_PLUGIN (lemon_plugin));

  gjs_context = _lemon_global_get_gjs_context (lemon_plugin->global);

  if (!gjs_context_eval (gjs_context,
                         "imports.ui.environment.init();"
                         "imports.ui.main.start();",
                         -1,
                         "<main>",
                         &status,
                         &error))
    {
      g_message ("Execution of main.js threw exception: %s", error->message);
      g_error_free (error);
      /* We just exit() here, since in a development environment you'll get the
       * error in your shell output, and it's way better than a busted WM,
       * which typically manifests as a white screen.
       *
       * In production, we shouldn't crash =)  But if we do, we should get
       * restarted by the session infrastructure, which is likely going
       * to be better than some undefined state.
       *
       * If there was a generic "hook into bug-buddy for non-C crashes"
       * infrastructure, here would be the place to put it.
       */
      g_object_unref (gjs_context);
      exit (1);
    }
}

static LemonWM *
get_lemon_wm (void)
{
  LemonWM *wm;

  g_object_get (lemon_global_get (),
                "window-manager", &wm,
                NULL);
  /* drop extra ref added by g_object_get */
  g_object_unref (wm);

  return wm;
}

static void
lemon_plugin_minimize (MetaPlugin         *plugin,
                 MetaWindowActor    *actor)
{
  _lemon_wm_minimize (get_lemon_wm (),
                      actor);

}

static void
lemon_plugin_unminimize (MetaPlugin         *plugin,
                               MetaWindowActor    *actor)
{
  _lemon_wm_unminimize (get_lemon_wm (),
                      actor);

}

static void
lemon_plugin_size_changed (MetaPlugin         *plugin,
                                 MetaWindowActor    *actor)
{
  _lemon_wm_size_changed (get_lemon_wm (), actor);
}

static void
lemon_plugin_size_change (MetaPlugin         *plugin,
                                MetaWindowActor    *actor,
                                MetaSizeChange      which_change,
                                MetaRectangle      *old_frame_rect,
                                MetaRectangle      *old_buffer_rect)
{
  _lemon_wm_size_change (get_lemon_wm (), actor, which_change, old_frame_rect, old_buffer_rect);
}

static void
lemon_plugin_map (MetaPlugin         *plugin,
                        MetaWindowActor    *actor)
{
  _lemon_wm_map (get_lemon_wm (),
                 actor);
}

static void
lemon_plugin_destroy (MetaPlugin         *plugin,
                            MetaWindowActor    *actor)
{
  _lemon_wm_destroy (get_lemon_wm (),
                     actor);
}

static void
lemon_plugin_switch_workspace (MetaPlugin         *plugin,
                                     gint                from,
                                     gint                to,
                                     MetaMotionDirection direction)
{
  _lemon_wm_switch_workspace (get_lemon_wm(), from, to, direction);
}

static void
lemon_plugin_kill_window_effects (MetaPlugin         *plugin,
                                        MetaWindowActor    *actor)
{
  _lemon_wm_kill_window_effects (get_lemon_wm(), actor);
}

static void
lemon_plugin_kill_switch_workspace (MetaPlugin         *plugin)
{
  _lemon_wm_kill_switch_workspace (get_lemon_wm());
}

static void
lemon_plugin_show_tile_preview (MetaPlugin      *plugin,
                                      MetaWindow      *window,
                                      MetaRectangle   *tile_rect,
                                      int              tile_monitor)
{
  _lemon_wm_show_tile_preview (get_lemon_wm (), window, tile_rect, tile_monitor);
}

static void
lemon_plugin_hide_tile_preview (MetaPlugin *plugin)
{
  _lemon_wm_hide_tile_preview (get_lemon_wm ());
}

static void
lemon_plugin_show_window_menu (MetaPlugin         *plugin,
                                     MetaWindow         *window,
                                     MetaWindowMenuType  menu,
                                     int                 x,
                                     int                 y)
{
  _lemon_wm_show_window_menu (get_lemon_wm (), window, menu, x, y);
}

static void
lemon_plugin_show_window_menu_for_rect (MetaPlugin         *plugin,
                                              MetaWindow         *window,
                                              MetaWindowMenuType  menu,
                                              MetaRectangle      *rect)
{
  _lemon_wm_show_window_menu_for_rect (get_lemon_wm (), window, menu, rect);
}

static gboolean
lemon_plugin_xevent_filter (MetaPlugin *plugin,
                                  XEvent     *xev)
{
#ifdef GLX_INTEL_swap_event
  LemonPlugin *lemon_plugin = LEMON_PLUGIN (plugin);

  if (lemon_plugin->have_swap_event &&
      xev->type == (lemon_plugin->glx_event_base + GLX_BufferSwapComplete))
    {
      GLXBufferSwapComplete *swap_complete_event;
      swap_complete_event = (GLXBufferSwapComplete *)xev;

      /* Buggy early versions of the INTEL_swap_event implementation in Mesa
       * can send this with a ust of 0. Simplify life for consumers
       * by ignoring such events */
      if (swap_complete_event->ust != 0)
        {
          gboolean frame_timestamps;
          g_object_get (lemon_plugin->global,
                        "frame-timestamps", &frame_timestamps,
                        NULL);

          if (frame_timestamps)
            lemon_perf_log_event_x (lemon_perf_log_get_default (),
                                    "glx.swapComplete",
                                    swap_complete_event->ust);
        }
    }
#endif

  return FALSE;
}

static gboolean
lemon_plugin_keybinding_filter (MetaPlugin     *plugin,
                                      MetaKeyBinding *binding)
{
  return _lemon_wm_filter_keybinding (get_lemon_wm (), binding);
}

static void
lemon_plugin_confirm_display_change (MetaPlugin *plugin)
{
  _lemon_wm_confirm_display_change (get_lemon_wm ());
}

static const MetaPluginInfo *
lemon_plugin_plugin_info (MetaPlugin *plugin)
{
  static const MetaPluginInfo info = {
    .name = "Lemon",
    .version = "0.1",
    .author = "Various",
    .license = "GPLv2+",
    .description = "Provides Lemon core functionality"
  };

  return &info;
}

static MetaCloseDialog *
lemon_plugin_create_close_dialog (MetaPlugin *plugin,
                                        MetaWindow *window)
{
  return _lemon_wm_create_close_dialog (get_lemon_wm (), window);
}

static MetaInhibitShortcutsDialog *
lemon_plugin_create_inhibit_shortcuts_dialog (MetaPlugin *plugin,
                                                    MetaWindow *window)
{
  return _lemon_wm_create_inhibit_shortcuts_dialog (get_lemon_wm (), window);
}

static void
lemon_plugin_locate_pointer (MetaPlugin *plugin)
{
  LemonPlugin *lemon_plugin = LEMON_PLUGIN (plugin);
  // TODO
  // _lemon_global_locate_pointer (lemon_plugin->global);
}

static void
lemon_plugin_class_init (LemonPluginClass *klass)
{
  MetaPluginClass *plugin_class  = META_PLUGIN_CLASS (klass);

  plugin_class->start            = lemon_plugin_start;
  plugin_class->map              = lemon_plugin_map;
  plugin_class->minimize         = lemon_plugin_minimize;
  plugin_class->unminimize       = lemon_plugin_unminimize;
  plugin_class->size_changed     = lemon_plugin_size_changed;
  plugin_class->size_change      = lemon_plugin_size_change;
  plugin_class->destroy          = lemon_plugin_destroy;

  plugin_class->switch_workspace = lemon_plugin_switch_workspace;

  plugin_class->kill_window_effects   = lemon_plugin_kill_window_effects;
  plugin_class->kill_switch_workspace = lemon_plugin_kill_switch_workspace;

  plugin_class->show_tile_preview = lemon_plugin_show_tile_preview;
  plugin_class->hide_tile_preview = lemon_plugin_hide_tile_preview;
  plugin_class->show_window_menu = lemon_plugin_show_window_menu;
  plugin_class->show_window_menu_for_rect = lemon_plugin_show_window_menu_for_rect;

  plugin_class->xevent_filter     = lemon_plugin_xevent_filter;
  plugin_class->keybinding_filter = lemon_plugin_keybinding_filter;

  plugin_class->confirm_display_change = lemon_plugin_confirm_display_change;

  plugin_class->plugin_info       = lemon_plugin_plugin_info;

  plugin_class->create_close_dialog = lemon_plugin_create_close_dialog;
  plugin_class->create_inhibit_shortcuts_dialog = lemon_plugin_create_inhibit_shortcuts_dialog;

  plugin_class->locate_pointer = lemon_plugin_locate_pointer;
}

static void
lemon_plugin_init (LemonPlugin *lemon_plugin)
{
}

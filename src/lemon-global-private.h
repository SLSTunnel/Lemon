/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_GLOBAL_PRIVATE_H__
#define __LEMON_GLOBAL_PRIVATE_H__

#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "lemon-global.h"
#include <gio/gio.h>
#include <girepository.h>
#include <meta/meta-plugin.h>


#include "lemon-enum-types.h"
#include "lemon-global-private.h"
#include "lemon-perf-log.h"
#include "lemon-window-tracker.h"
#include "lemon-wm.h"
#include "st.h"

#include <cjs/gjs.h>

struct _LemonGlobal {
  GObject parent;

  ClutterStage *stage;

  MetaDisplay *meta_display;
  MetaWorkspaceManager *workspace_manager;
  Display *xdisplay;
  LemonScreen *lemon_screen;

  LemonStageInputMode input_mode;
  XserverRegion input_region;

  GjsContext *js_context;
  MetaPlugin *plugin;
  LemonWM *wm;
  GSettings *settings;
  GSettings *interface_settings;
  const char *datadir;
  const char *imagedir;
  const char *userdatadir;
  StFocusManager *focus_manager;

  guint work_count;
  GSList *leisure_closures;
  guint leisure_function_id;

  gint64 last_gc_end_time;
  guint ui_scale;
  gboolean session_running;
  gboolean has_modal;

  guint notif_service_id;
};

void _lemon_global_init            (const char *first_property_name,
                                    ...);
void _lemon_global_set_plugin      (LemonGlobal  *global,
                                    MetaPlugin   *plugin);

GjsContext *_lemon_global_get_gjs_context (LemonGlobal  *global);

#endif /* __LEMON_GLOBAL_PRIVATE_H__ */

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_APP_SYSTEM_H__
#define __LEMON_APP_SYSTEM_H__

#include <gio/gio.h>
#include <clutter/clutter.h>
#include <meta/window.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>

#include "lemon-app.h"

#define LEMON_TYPE_APP_SYSTEM                 (lemon_app_system_get_type ())
#define LEMON_APP_SYSTEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_APP_SYSTEM, LemonAppSystem))
#define LEMON_APP_SYSTEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_APP_SYSTEM, LemonAppSystemClass))
#define LEMON_IS_APP_SYSTEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_APP_SYSTEM))
#define LEMON_IS_APP_SYSTEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_APP_SYSTEM))
#define LEMON_APP_SYSTEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_APP_SYSTEM, LemonAppSystemClass))

typedef struct _LemonAppSystem LemonAppSystem;
typedef struct _LemonAppSystemClass LemonAppSystemClass;
typedef struct _LemonAppSystemPrivate LemonAppSystemPrivate;

struct _LemonAppSystem
{
  GObject parent;

  LemonAppSystemPrivate *priv;
};

struct _LemonAppSystemClass
{
  GObjectClass parent_class;

  void (*installed_changed)(LemonAppSystem *appsys, gpointer user_data);
  void (*favorites_changed)(LemonAppSystem *appsys, gpointer user_data);
};

GType           lemon_app_system_get_type    (void) G_GNUC_CONST;
LemonAppSystem *lemon_app_system_get_default (void);

GMenuTree      *lemon_app_system_get_tree                     (LemonAppSystem *system);

LemonApp       *lemon_app_system_lookup_app                   (LemonAppSystem  *system,
                                                               const char      *id);
LemonApp       *lemon_app_system_lookup_startup_wmclass       (LemonAppSystem *system,
                                                                     const char     *wmclass);
LemonApp       *lemon_app_system_lookup_desktop_wmclass       (LemonAppSystem *system,
                                                                     const char     *wmclass);
LemonApp       *lemon_app_system_lookup_flatpak_app_id (LemonAppSystem *system,
                                                              const char        *app_id);

GSList         *lemon_app_system_get_all                   (LemonAppSystem  *system);

GSList         *lemon_app_system_get_running               (LemonAppSystem  *self);

#endif /* __LEMON_APP_SYSTEM_H__ */

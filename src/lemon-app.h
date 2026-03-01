/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_APP_H__
#define __LEMON_APP_H__

#include <clutter/clutter.h>
#include <gio/gio.h>
#include <meta/window.h>
#include <meta/meta-startup-notification.h>

#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>
#include <gmenu-desktopappinfo.h>

G_BEGIN_DECLS

typedef struct _LemonApp LemonApp;
typedef struct _LemonAppClass LemonAppClass;
typedef struct _LemonAppPrivate LemonAppPrivate;

#define LEMON_TYPE_APP              (lemon_app_get_type ())
#define LEMON_APP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_APP, LemonApp))
#define LEMON_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_APP, LemonAppClass))
#define LEMON_IS_APP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_APP))
#define LEMON_IS_APP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_APP))
#define LEMON_APP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_APP, LemonAppClass))

struct _LemonAppClass
{
  GObjectClass parent_class;

};

typedef enum {
  LEMON_APP_STATE_STOPPED,
  LEMON_APP_STATE_STARTING,
  LEMON_APP_STATE_RUNNING
} LemonAppState;

GType lemon_app_get_type (void) G_GNUC_CONST;

const char *lemon_app_get_id (LemonApp *app);
GMenuTreeEntry *lemon_app_get_tree_entry (LemonApp *app);
GMenuDesktopAppInfo *lemon_app_get_app_info (LemonApp *app);
ClutterActor *lemon_app_create_icon_texture (LemonApp *app,
                                                int          size);
ClutterActor *lemon_app_create_icon_texture_for_window (LemonApp   *app,
                                                           int            size,
                                                           MetaWindow    *for_window);
const char *lemon_app_get_name (LemonApp *app);
const char *lemon_app_get_description (LemonApp *app);
const char *lemon_app_get_keywords (LemonApp *app);
gboolean lemon_app_get_nodisplay (LemonApp *app);

gboolean lemon_app_is_window_backed (LemonApp *app);

void lemon_app_activate_window (LemonApp *app, MetaWindow *window, guint32 timestamp);

void lemon_app_activate (LemonApp      *app);

void lemon_app_activate_full (LemonApp      *app,
                              int            workspace,
                              guint32        timestamp);

void lemon_app_open_new_window (LemonApp *app,
                                int       workspace);

gboolean lemon_app_can_open_new_window (LemonApp *app);

LemonAppState lemon_app_get_state (LemonApp *app);

gboolean lemon_app_request_quit (LemonApp *app);

guint lemon_app_get_n_windows (LemonApp *app);

GSList *lemon_app_get_windows (LemonApp *app);

GSList *lemon_app_get_pids (LemonApp *app);

gboolean lemon_app_is_on_workspace (LemonApp *app, MetaWorkspace *workspace);

gboolean lemon_app_launch (LemonApp     *app,
                           guint         timestamp,
                           GList        *uris,
                           int           workspace,
                           char        **startup_id,
                           GError      **error);

gboolean lemon_app_launch_offloaded (LemonApp     *app,
                           guint         timestamp,
                           GList        *uris,
                           int           workspace,
                           char        **startup_id,
                           GError      **error);

gboolean lemon_app_get_is_flatpak (LemonApp *app);
char * lemon_app_get_flatpak_app_id (LemonApp *app);

G_END_DECLS

#endif /* __LEMON_APP_H__ */

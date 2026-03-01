/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_GLOBAL_H__
#define __LEMON_GLOBAL_H__

#include <clutter/clutter.h>
#include <glib-object.h>
#include <meta/meta-plugin.h>
#include "lemon-screen.h"

G_BEGIN_DECLS

typedef struct _LemonGlobal      LemonGlobal;
typedef struct _LemonGlobalClass LemonGlobalClass;

#define LEMON_TYPE_GLOBAL              (lemon_global_get_type ())
#define LEMON_GLOBAL(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_GLOBAL, LemonGlobal))
#define LEMON_GLOBAL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_GLOBAL, LemonGlobalClass))
#define LEMON_IS_GLOBAL(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_GLOBAL))
#define LEMON_IS_GLOBAL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_GLOBAL))
#define LEMON_GLOBAL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_GLOBAL, LemonGlobalClass))

struct _LemonGlobalClass
{
  GObjectClass parent_class;
};

GType lemon_global_get_type (void) G_GNUC_CONST;

LemonGlobal   *lemon_global_get                       (void);

ClutterStage  *lemon_global_get_stage                 (LemonGlobal *global);
LemonScreen *lemon_global_get_screen                (LemonGlobal *global);
MetaDisplay   *lemon_global_get_display               (LemonGlobal *global);
GList         *lemon_global_get_window_actors         (LemonGlobal *global);
GSettings     *lemon_global_get_settings              (LemonGlobal *global);
guint32        lemon_global_get_current_time          (LemonGlobal *global);
pid_t          lemon_global_get_pid                   (LemonGlobal *global);
gchar         *lemon_global_get_md5_for_string        (LemonGlobal *global, const gchar *string);
void           lemon_global_dump_gjs_stack            (LemonGlobal *global);

/* Input/event handling */
gboolean lemon_global_begin_modal            (LemonGlobal         *global,
                                              guint32              timestamp,
                                              MetaModalOptions    options);
void     lemon_global_end_modal              (LemonGlobal         *global,
                                              guint32              timestamp);

typedef enum {
  LEMON_STAGE_INPUT_MODE_NONREACTIVE,
  LEMON_STAGE_INPUT_MODE_NORMAL,
  LEMON_STAGE_INPUT_MODE_FOCUSED,
  LEMON_STAGE_INPUT_MODE_FULLSCREEN
} LemonStageInputMode;

void     lemon_global_set_stage_input_mode   (LemonGlobal         *global,
                                              LemonStageInputMode  mode);
void     lemon_global_set_stage_input_region (LemonGlobal         *global,
                                              GSList              *rectangles);

/* X utilities */
typedef enum {
  LEMON_CURSOR_NOT_ALLOWED,
  LEMON_CURSOR_NO_DROP,
  LEMON_CURSOR_MOVE,
  LEMON_CURSOR_COPY,
  LEMON_CURSOR_POINTER,
  LEMON_CURSOR_RESIZE_BOTTOM,
  LEMON_CURSOR_RESIZE_TOP,
  LEMON_CURSOR_RESIZE_LEFT,
  LEMON_CURSOR_RESIZE_RIGHT,
  LEMON_CURSOR_RESIZE_BOTTOM_RIGHT,
  LEMON_CURSOR_RESIZE_BOTTOM_LEFT,
  LEMON_CURSOR_RESIZE_TOP_RIGHT,
  LEMON_CURSOR_RESIZE_TOP_LEFT,
  LEMON_CURSOR_CROSSHAIR,
  LEMON_CURSOR_TEXT
} LemonCursor;

void    lemon_global_set_cursor              (LemonGlobal         *global,
                                              LemonCursor          type);
void    lemon_global_unset_cursor            (LemonGlobal         *global);

guint32 lemon_global_create_pointer_barrier  (LemonGlobal         *global,
                                              int                  x1,
                                              int                  y1,
                                              int                  x2,
                                              int                  y2,
                                              int                  directions);
void    lemon_global_destroy_pointer_barrier (LemonGlobal         *global,
                                              guint32              barrier);

void    lemon_global_get_pointer             (LemonGlobal         *global,
                                              int                 *x,
                                              int                 *y,
                                              ClutterModifierType *mods);
void    lemon_global_set_pointer             (LemonGlobal         *global,
                                              int                 x,
                                              int                 y);

/* Run-at-leisure API */
void lemon_global_begin_work     (LemonGlobal          *global);
void lemon_global_end_work       (LemonGlobal          *global);

typedef void (*LemonLeisureFunction) (gpointer data);

void lemon_global_run_at_leisure (LemonGlobal          *global,
                                  LemonLeisureFunction  func,
                                  gpointer              user_data,
                                  GDestroyNotify        notify);


/* Misc utilities / Lemon API */

void     lemon_global_sync_pointer              (LemonGlobal  *global);

GAppLaunchContext *
         lemon_global_create_app_launch_context (LemonGlobal  *global);

void     lemon_global_notify_error              (LemonGlobal  *global,
                                                 const char   *msg,
                                                 const char   *details);

void     lemon_global_real_restart              (LemonGlobal  *global);
void     lemon_global_reexec_self               (LemonGlobal  *global);

void     lemon_global_segfault                  (LemonGlobal  *global);
void     lemon_global_alloc_leak                (LemonGlobal  *global,
                                                    gint             mb);

gulong   lemon_global_get_stage_xwindow         (LemonGlobal  *global);

G_END_DECLS

#endif /* __LEMON_GLOBAL_H__ */

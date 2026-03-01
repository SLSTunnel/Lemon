/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_WINDOW_TRACKER_H__
#define __LEMON_WINDOW_TRACKER_H__

#include <glib-object.h>
#include <glib.h>
#include <meta/window.h>

#include "lemon-app.h"
#include "lemon-app-system.h"

G_BEGIN_DECLS

typedef struct _LemonWindowTracker LemonWindowTracker;
typedef struct _LemonWindowTrackerClass LemonWindowTrackerClass;
typedef struct _LemonWindowTrackerPrivate LemonWindowTrackerPrivate;

#define LEMON_TYPE_WINDOW_TRACKER              (lemon_window_tracker_get_type ())
#define LEMON_WINDOW_TRACKER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_WINDOW_TRACKER, LemonWindowTracker))
#define LEMON_WINDOW_TRACKER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_WINDOW_TRACKER, LemonWindowTrackerClass))
#define LEMON_IS_WINDOW_TRACKER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_WINDOW_TRACKER))
#define LEMON_IS_WINDOW_TRACKER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_WINDOW_TRACKER))
#define LEMON_WINDOW_TRACKER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_WINDOW_TRACKER, LemonWindowTrackerClass))

struct _LemonWindowTrackerClass
{
  GObjectClass parent_class;
};

GType lemon_window_tracker_get_type (void) G_GNUC_CONST;

LemonWindowTracker* lemon_window_tracker_get_default(void);

LemonApp *lemon_window_tracker_get_window_app (LemonWindowTracker *tracker, MetaWindow *metawin);

LemonApp *lemon_window_tracker_get_app_from_pid (LemonWindowTracker *tracker, int pid);

gboolean lemon_window_tracker_is_window_interesting (LemonWindowTracker *tracker, MetaWindow *window);

GSList *lemon_window_tracker_get_startup_sequences (LemonWindowTracker *tracker);

G_END_DECLS

#endif /* __LEMON_WINDOW_TRACKER_H__ */

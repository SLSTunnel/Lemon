/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_DOC_SYSTEM_H__
#define __LEMON_DOC_SYSTEM_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

#define LEMON_TYPE_DOC_SYSTEM                 (lemon_doc_system_get_type ())
#define LEMON_DOC_SYSTEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_DOC_SYSTEM, LemonDocSystem))
#define LEMON_DOC_SYSTEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_DOC_SYSTEM, LemonDocSystemClass))
#define LEMON_IS_DOC_SYSTEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_DOC_SYSTEM))
#define LEMON_IS_DOC_SYSTEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_DOC_SYSTEM))
#define LEMON_DOC_SYSTEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_DOC_SYSTEM, LemonDocSystemClass))

typedef struct _LemonDocSystem LemonDocSystem;
typedef struct _LemonDocSystemClass LemonDocSystemClass;
typedef struct _LemonDocSystemPrivate LemonDocSystemPrivate;

struct _LemonDocSystem
{
  GObject parent;

  LemonDocSystemPrivate *priv;
};

struct _LemonDocSystemClass
{
  GObjectClass parent_class;
};

GType lemon_doc_system_get_type (void) G_GNUC_CONST;

LemonDocSystem* lemon_doc_system_get_default (void);

GSList *lemon_doc_system_get_all (LemonDocSystem    *system);

#endif /* __LEMON_DOC_SYSTEM_H__ */

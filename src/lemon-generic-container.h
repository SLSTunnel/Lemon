/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_GENERIC_CONTAINER_H__
#define __LEMON_GENERIC_CONTAINER_H__

#include "st.h"

#define LEMON_TYPE_GENERIC_CONTAINER                 (lemon_generic_container_get_type ())
#define LEMON_GENERIC_CONTAINER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_GENERIC_CONTAINER, LemonGenericContainer))
#define LEMON_GENERIC_CONTAINER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_GENERIC_CONTAINER, LemonGenericContainerClass))
#define LEMON_IS_GENERIC_CONTAINER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_GENERIC_CONTAINER))
#define LEMON_IS_GENERIC_CONTAINER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_GENERIC_CONTAINER))
#define LEMON_GENERIC_CONTAINER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_GENERIC_CONTAINER, LemonGenericContainerClass))

typedef struct {
  float min_size;
  float natural_size;

  /* <private> */
  guint _refcount;
} LemonGenericContainerAllocation;

#define LEMON_TYPE_GENERIC_CONTAINER_ALLOCATION (lemon_generic_container_allocation_get_type ())
GType lemon_generic_container_allocation_get_type (void);

typedef struct _LemonGenericContainer        LemonGenericContainer;
typedef struct _LemonGenericContainerClass   LemonGenericContainerClass;

typedef struct _LemonGenericContainerPrivate LemonGenericContainerPrivate;

struct _LemonGenericContainer
{
    StWidget parent;

    LemonGenericContainerPrivate *priv;
};

struct _LemonGenericContainerClass
{
    StWidgetClass parent_class;
};

GType    lemon_generic_container_get_type         (void) G_GNUC_CONST;

guint    lemon_generic_container_get_n_skip_paint (LemonGenericContainer *self);

gboolean lemon_generic_container_get_skip_paint   (LemonGenericContainer *self,
                                                   ClutterActor          *child);
void     lemon_generic_container_set_skip_paint   (LemonGenericContainer *self,
                                                   ClutterActor          *child,
                                                   gboolean               skip);

#endif /* __LEMON_GENERIC_CONTAINER_H__ */

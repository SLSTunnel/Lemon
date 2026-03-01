/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_STACK_H__
#define __LEMON_STACK_H__

#include "st.h"
#include <gtk/gtk.h>

#define LEMON_TYPE_STACK                 (lemon_stack_get_type ())
#define LEMON_STACK(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_STACK, LemonStack))
#define LEMON_STACK_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_STACK, LemonStackClass))
#define LEMON_IS_STACK(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_STACK))
#define LEMON_IS_STACK_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_STACK))
#define LEMON_STACK_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_STACK, LemonStackClass))

typedef struct _LemonStack        LemonStack;
typedef struct _LemonStackClass   LemonStackClass;

typedef struct _LemonStackPrivate LemonStackPrivate;

struct _LemonStack
{
    StWidget parent;

    LemonStackPrivate *priv;
};

struct _LemonStackClass
{
    StWidgetClass parent_class;
};

GType lemon_stack_get_type (void) G_GNUC_CONST;

#endif /* __LEMON_STACK_H__ */

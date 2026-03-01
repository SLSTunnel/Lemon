/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_SLICER_H__
#define __LEMON_SLICER_H__

#include "st.h"

#define LEMON_TYPE_SLICER                 (lemon_slicer_get_type ())
#define LEMON_SLICER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_SLICER, LemonSlicer))
#define LEMON_SLICER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_SLICER, LemonSlicerClass))
#define LEMON_IS_SLICER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_SLICER))
#define LEMON_IS_SLICER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_SLICER))
#define LEMON_SLICER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_SLICER, LemonSlicerClass))

typedef struct _LemonSlicer        LemonSlicer;
typedef struct _LemonSlicerClass   LemonSlicerClass;

typedef struct _LemonSlicerPrivate LemonSlicerPrivate;

struct _LemonSlicer
{
    StBin parent;

    LemonSlicerPrivate *priv;
};

struct _LemonSlicerClass
{
    StBinClass parent_class;
};

GType lemon_slicer_get_type (void) G_GNUC_CONST;

#endif /* __LEMON_SLICER_H__ */

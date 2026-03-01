/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_EMBEDDED_WINDOW_H__
#define __LEMON_EMBEDDED_WINDOW_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

#define LEMON_TYPE_EMBEDDED_WINDOW (lemon_embedded_window_get_type ())
G_DECLARE_DERIVABLE_TYPE (LemonEmbeddedWindow, lemon_embedded_window,
                          LEMON, EMBEDDED_WINDOW, GtkWindow)

struct _LemonEmbeddedWindowClass
{
  GtkWindowClass parent_class;
};

GtkWidget *lemon_embedded_window_new (void);

#endif /* __LEMON_EMBEDDED_WINDOW_H__ */

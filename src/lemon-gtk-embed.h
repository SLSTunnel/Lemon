/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_GTK_EMBED_H__
#define __LEMON_GTK_EMBED_H__

#include <clutter/clutter.h>

#include "lemon-embedded-window.h"

#define LEMON_TYPE_GTK_EMBED (lemon_gtk_embed_get_type ())
G_DECLARE_DERIVABLE_TYPE (LemonGtkEmbed, lemon_gtk_embed,
                          LEMON, GTK_EMBED, ClutterClone)

struct _LemonGtkEmbedClass
{
    ClutterCloneClass parent_class;
};

ClutterActor *lemon_gtk_embed_new (LemonEmbeddedWindow *window);

#endif /* __LEMON_GTK_EMBED_H__ */

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_EMBEDDED_WINDOW_PRIVATE_H__
#define __LEMON_EMBEDDED_WINDOW_PRIVATE_H__

#include "lemon-embedded-window.h"
#include "lemon-gtk-embed.h"

void _lemon_embedded_window_set_actor (LemonEmbeddedWindow      *window,
                       LemonGtkEmbed            *embed);

void _lemon_embedded_window_allocate (LemonEmbeddedWindow *window,
                      int                  x,
                      int                  y,
                      int                  width,
                      int                  height);

void _lemon_embedded_window_map (LemonEmbeddedWindow *window);
void _lemon_embedded_window_unmap (LemonEmbeddedWindow *window);

#endif /* __LEMON_EMBEDDED_WINDOW_PRIVATE_H__ */

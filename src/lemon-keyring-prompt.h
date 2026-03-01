/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* lemon-keyring-prompt.c - prompt handler for gnome-keyring-daemon

   Copyright (C) 2011 Stefan Walter

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Author: Stef Walter <stef@thewalter.net>
*/

#ifndef __LEMON_KEYRING_PROMPT_H__
#define __LEMON_KEYRING_PROMPT_H__

#include <glib-object.h>
#include <glib.h>

#include <clutter/clutter.h>

G_BEGIN_DECLS

typedef struct _LemonKeyringPrompt LemonKeyringPrompt;

#define LEMON_TYPE_KEYRING_PROMPT (lemon_keyring_prompt_get_type ())
G_DECLARE_FINAL_TYPE (LemonKeyringPrompt, lemon_keyring_prompt,
                      LEMON, KEYRING_PROMPT, GObject)

LemonKeyringPrompt * lemon_keyring_prompt_new                  (void);

ClutterText *           lemon_keyring_prompt_get_password_actor   (LemonKeyringPrompt *self);

void                    lemon_keyring_prompt_set_password_actor   (LemonKeyringPrompt *self,
                                                                      ClutterText           *password_actor);

ClutterText *           lemon_keyring_prompt_get_confirm_actor    (LemonKeyringPrompt *self);

void                    lemon_keyring_prompt_set_confirm_actor    (LemonKeyringPrompt *self,
                                                                      ClutterText           *confirm_actor);

gboolean                lemon_keyring_prompt_complete             (LemonKeyringPrompt *self);

void                    lemon_keyring_prompt_cancel               (LemonKeyringPrompt *self);

G_END_DECLS

#endif /* __LEMON_KEYRING_PROMPT_H__ */


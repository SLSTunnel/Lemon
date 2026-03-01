/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2008 Iain Holmes
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Suite 500, Boston, MA
 * 02110-1335, USA.
 */

#ifndef LEMON_SCREEN_H
#define LEMON_SCREEN_H

#include <glib-object.h>
#include <meta/types.h>
#include <meta/workspace.h>
#include <meta/display.h>
#include <meta/meta-workspace-manager.h>

#define LEMON_TYPE_SCREEN (lemon_screen_get_type ())
G_DECLARE_FINAL_TYPE (LemonScreen, lemon_screen, LEMON, SCREEN, GObject)

LemonScreen *lemon_screen_new (MetaDisplay *display);

MetaDisplay *lemon_screen_get_display (LemonScreen *screen);

void lemon_screen_get_size (LemonScreen *screen,
                           int        *width,
                           int        *height);

GList *lemon_screen_get_workspaces (LemonScreen *screen);

int lemon_screen_get_n_workspaces (LemonScreen *screen);

MetaWorkspace* lemon_screen_get_workspace_by_index (LemonScreen    *screen,
                                                   int            index);
void lemon_screen_remove_workspace (LemonScreen    *screen,
                                   MetaWorkspace *workspace,
                                   guint32        timestamp);

MetaWorkspace *lemon_screen_append_new_workspace (LemonScreen    *screen,
                                                 gboolean       activate,
                                                 guint32        timestamp);

int lemon_screen_get_active_workspace_index (LemonScreen *screen);

MetaWorkspace * lemon_screen_get_active_workspace (LemonScreen *screen);

void lemon_screen_show_desktop (LemonScreen *screen,
                                   guint32         timestamp);

void lemon_screen_toggle_desktop (LemonScreen *screen,
                                     guint32         timestamp);
                                
void lemon_screen_unshow_desktop (LemonScreen *screen);

int  lemon_screen_get_n_monitors       (LemonScreen    *screen);
int  lemon_screen_get_primary_monitor  (LemonScreen    *screen);
int  lemon_screen_get_current_monitor  (LemonScreen    *screen);
void lemon_screen_get_monitor_geometry (LemonScreen    *screen,
                                       int            monitor,
                                       MetaRectangle *geometry);

gboolean lemon_screen_get_monitor_in_fullscreen (LemonScreen  *screen,
                                                int          monitor);

int lemon_screen_get_monitor_index_for_rect (LemonScreen    *screen,
                                            MetaRectangle *rect);

MetaWindow* lemon_screen_get_mouse_window (LemonScreen *screen,
                                              MetaWindow *not_this_one);

void lemon_screen_override_workspace_layout (LemonScreen      *screen,
                                                MetaDisplayCorner starting_corner,
                                                gboolean         vertical_layout,
                                                int              n_rows,
                                                int              n_columns);

unsigned long lemon_screen_get_xwindow_for_window (LemonScreen *screen,
                                                      MetaWindow     *window);

#endif

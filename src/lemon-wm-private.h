/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_WM_PRIVATE_H__
#define __LEMON_WM_PRIVATE_H__

#include "lemon-wm.h"

G_BEGIN_DECLS

/* These forward along the different effects from LemonPlugin */

void _lemon_wm_minimize   (LemonWM         *wm,
                           MetaWindowActor *actor);
void _lemon_wm_unminimize (LemonWM         *wm,
                           MetaWindowActor *actor);
void _lemon_wm_size_changed(LemonWM         *wm,
                            MetaWindowActor *actor);
void _lemon_wm_size_change(LemonWM         *wm,
                           MetaWindowActor *actor,
                           MetaSizeChange   which_change,
                           MetaRectangle   *old_frame_rect,
                           MetaRectangle   *old_buffer_rect);
void _lemon_wm_map        (LemonWM         *wm,
                           MetaWindowActor *actor);
void _lemon_wm_destroy    (LemonWM         *wm,
                           MetaWindowActor *actor);

void _lemon_wm_switch_workspace      (LemonWM             *wm,
                                      gint                 from,
                                      gint                 to,
                                      MetaMotionDirection  direction);
void _lemon_wm_kill_window_effects   (LemonWM             *wm,
                                      MetaWindowActor     *actor);
void _lemon_wm_kill_switch_workspace (LemonWM             *wm);

void _lemon_wm_show_tile_preview     (LemonWM         *wm,
                                         MetaWindow         *window,
                                         MetaRectangle      *tile_rect,
                                      int                  tile_monitor);
void _lemon_wm_hide_tile_preview     (LemonWM         *wm);
void _lemon_wm_show_window_menu      (LemonWM             *wm,
                                      MetaWindow          *window,
                                      MetaWindowMenuType   menu,
                                      int                  x,
                                      int                  y);
void _lemon_wm_show_window_menu_for_rect (LemonWM             *wm,
                                          MetaWindow          *window,
                                          MetaWindowMenuType   menu,
                                          MetaRectangle       *rect);

gboolean _lemon_wm_filter_keybinding (LemonWM             *wm,
                                      MetaKeyBinding      *binding);

void _lemon_wm_confirm_display_change (LemonWM            *wm);

MetaCloseDialog * _lemon_wm_create_close_dialog (LemonWM     *wm,
                                                 MetaWindow  *window);

MetaInhibitShortcutsDialog * _lemon_wm_create_inhibit_shortcuts_dialog (LemonWM     *wm,
                                                                        MetaWindow  *window);

G_END_DECLS

#endif /* __LEMON_WM_PRIVATE_H__ */

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_WM_H__
#define __LEMON_WM_H__

#include <glib-object.h>
#include <meta/meta-plugin.h>

G_BEGIN_DECLS


#define LEMON_TYPE_WM              (lemon_wm_get_type ())
G_DECLARE_FINAL_TYPE (LemonWM, lemon_wm, LEMON, WM, GObject)

LemonWM *lemon_wm_new                        (MetaPlugin      *plugin);

void     lemon_wm_completed_minimize         (LemonWM         *wm,
                                              MetaWindowActor *actor);
void     lemon_wm_completed_unminimize       (LemonWM         *wm,
                                              MetaWindowActor *actor);
void     lemon_wm_completed_size_change      (LemonWM         *wm,
                                              MetaWindowActor *actor);
void     lemon_wm_completed_map              (LemonWM         *wm,
                                              MetaWindowActor *actor);
void     lemon_wm_completed_destroy          (LemonWM         *wm,
                                              MetaWindowActor *actor);
void     lemon_wm_completed_switch_workspace (LemonWM         *wm);
void     lemon_wm_complete_display_change    (LemonWM         *wm,
                                              gboolean         ok);

G_END_DECLS

#endif /* __LEMON_WM_H__ */

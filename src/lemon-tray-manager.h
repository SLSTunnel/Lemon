/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#ifndef __LEMON_TRAY_MANAGER_H__
#define __LEMON_TRAY_MANAGER_H__

#include <clutter/clutter.h>
#include "st.h"

G_BEGIN_DECLS

#define LEMON_TYPE_TRAY_MANAGER (lemon_tray_manager_get_type ())
G_DECLARE_FINAL_TYPE (LemonTrayManager, lemon_tray_manager,
                      LEMON, TRAY_MANAGER, GObject)

LemonTrayManager *lemon_tray_manager_new          (void);
void              lemon_tray_manager_manage_screen (LemonTrayManager *manager,
                                                    StWidget         *theme_widget);
void              lemon_tray_manager_unmanage_screen (LemonTrayManager *manager);
void              lemon_tray_manager_redisplay (LemonTrayManager *manager);
void              lemon_tray_manager_set_orientation (LemonTrayManager *manager,
                                                         ClutterOrientation   orientation);
G_END_DECLS

#endif /* __LEMON_TRAY_MANAGER_H__ */

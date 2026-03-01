/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_TRAY_ICON_H__
#define __LEMON_TRAY_ICON_H__

#include "lemon-gtk-embed.h"

#define LEMON_TYPE_TRAY_ICON                 (lemon_tray_icon_get_type ())
#define LEMON_TRAY_ICON(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_TRAY_ICON, LemonTrayIcon))
#define LEMON_TRAY_ICON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_TRAY_ICON, LemonTrayIconClass))
#define LEMON_IS_TRAY_ICON(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_TRAY_ICON))
#define LEMON_IS_TRAY_ICON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_TRAY_ICON))
#define LEMON_TRAY_ICON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_TRAY_ICON, LemonTrayIconClass))

typedef struct _LemonTrayIcon        LemonTrayIcon;
typedef struct _LemonTrayIconClass   LemonTrayIconClass;
typedef struct _LemonTrayIconPrivate LemonTrayIconPrivate;

struct _LemonTrayIcon
{
    LemonGtkEmbed parent;

    LemonTrayIconPrivate *priv;
};

struct _LemonTrayIconClass
{
    LemonGtkEmbedClass parent_class;
};


GType         lemon_tray_icon_get_type (void) G_GNUC_CONST;
ClutterActor *lemon_tray_icon_new      (LemonEmbeddedWindow *window);

gboolean      lemon_tray_icon_handle_event (LemonTrayIcon *icon,
                                               ClutterEventType  event_type,
                                               ClutterEvent     *event);

#endif /* __LEMON_TRAY_ICON_H__ */

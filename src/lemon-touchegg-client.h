#ifndef __LEMON_TOUCHEGG_CLIENT_H__
#define __LEMON_TOUCHEGG_CLIENT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define LEMON_TYPE_TOUCHEGG_CLIENT (lemon_touchegg_client_get_type ())
G_DECLARE_FINAL_TYPE (LemonToucheggClient, lemon_touchegg_client, LEMON, TOUCHEGG_CLIENT, GObject)

LemonToucheggClient  *lemon_touchegg_client_new (void);

G_END_DECLS

#endif  /* __LEMON_TOUCHEGG_CLIENT_H__ */

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_NETWORK_AGENT_H__
#define __LEMON_NETWORK_AGENT_H__

#include <glib-object.h>
#include <glib.h>
#include <NetworkManager.h>
#include <nm-secret-agent-old.h>

G_BEGIN_DECLS

typedef enum {
  LEMON_NETWORK_AGENT_CONFIRMED,
  LEMON_NETWORK_AGENT_USER_CANCELED,
  LEMON_NETWORK_AGENT_INTERNAL_ERROR
} LemonNetworkAgentResponse;

typedef struct _LemonNetworkAgent         LemonNetworkAgent;
typedef struct _LemonNetworkAgentClass    LemonNetworkAgentClass;
typedef struct _LemonNetworkAgentPrivate  LemonNetworkAgentPrivate;

#define LEMON_TYPE_NETWORK_AGENT                  (lemon_network_agent_get_type ())
#define LEMON_NETWORK_AGENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LEMON_TYPE_NETWORK_AGENT, LemonNetworkAgent))
#define LEMON_IS_NETWORK_AGENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LEMON_TYPE_NETWORK_AGENT))
#define LEMON_NETWORK_AGENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_NETWORK_AGENT, LemonNetworkAgentClass))
#define LEMON_IS_NETWORK_AGENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_NETWORK_AGENT))
#define LEMON_NETWORK_AGENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_NETWORK_AGENT, LemonNetworkAgentClass))

struct _LemonNetworkAgent
{
  /*< private >*/
  NMSecretAgentOld parent_instance;

  LemonNetworkAgentPrivate *priv;
};

struct _LemonNetworkAgentClass
{
  /*< private >*/
  NMSecretAgentOldClass parent_class;
};

/* used by LEMON_TYPE_NETWORK_AGENT */
GType lemon_network_agent_get_type (void);

void               lemon_network_agent_add_vpn_secret (LemonNetworkAgent *self,
                                                          gchar                *request_id,
                                                          gchar                *setting_key,
                                                          gchar                *setting_value);
void               lemon_network_agent_set_password (LemonNetworkAgent *self,
                                                        gchar                *request_id,
                                                        gchar                *setting_key,
                                                        gchar                *setting_value);
void               lemon_network_agent_respond      (LemonNetworkAgent        *self,
                                                        gchar                       *request_id,
                                                        LemonNetworkAgentResponse response);

void               lemon_network_agent_search_vpn_plugin (LemonNetworkAgent *self,
                                                             const char           *service,
                                                             GAsyncReadyCallback   callback,
                                                             gpointer              user_data);
NMVpnPluginInfo   *lemon_network_agent_search_vpn_plugin_finish (LemonNetworkAgent  *self,
                                                                    GAsyncResult          *result,
                                                                    GError               **error);

/* If these are kept in sync with nm-applet, secrets will be shared */
#define LEMON_KEYRING_UUID_TAG "connection-uuid"
#define LEMON_KEYRING_SN_TAG "setting-name"
#define LEMON_KEYRING_SK_TAG "setting-key"

G_END_DECLS

#endif /* __LEMON_NETWORK_AGENT_H__ */

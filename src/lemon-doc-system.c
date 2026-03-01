/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include "lemon-doc-system.h"

#include "lemon-global.h"


/**
 * SECTION:lemon-doc-system
 * @short_description: Track recently used documents
 *
 * Wraps #GtkRecentManager, caching recently used document information, and adds
 * APIs for asynchronous queries.
 */
enum {
  CHANGED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

struct _LemonDocSystemPrivate {
  GtkRecentManager *manager;
  GSList *infos_by_timestamp;

  guint idle_recent_changed_id;
};

G_DEFINE_TYPE(LemonDocSystem, lemon_doc_system, G_TYPE_OBJECT);

// Showing all recent items isn't viable, GTK.RecentManager can contain
// up to 1000 items and this can significantly affect performance
// in the JS layer.
static const int MAX_RECENT_ITEMS = 20;

/**
 * lemon_doc_system_get_all:
 * @system: A #LemonDocSystem
 *
 * Returns the currently cached set of recent files. Recent files are read initially
 * from the underlying #GtkRecentManager, and updated when it changes.
 * This function does not perform I/O.
 *
 * Returns: (transfer none) (element-type GtkRecentInfo): Cached recent file infos
 */
GSList *
lemon_doc_system_get_all (LemonDocSystem    *self)
{
  return self->priv->infos_by_timestamp;
}

typedef struct {
  LemonDocSystem *self;
  GtkRecentInfo *info;
} LemonDocSystemRecentQueryData;

static int
sort_infos_by_timestamp_descending (gconstpointer a,
                                    gconstpointer b)
{
  GtkRecentInfo *info_a = (GtkRecentInfo*)a;
  GtkRecentInfo *info_b = (GtkRecentInfo*)b;
  time_t modified_a, modified_b;

  modified_a = gtk_recent_info_get_modified (info_a);
  modified_b = gtk_recent_info_get_modified (info_b);

  return modified_b - modified_a;
}

static void load_items (LemonDocSystem *self)
{
  LemonDocSystemPrivate *priv = self->priv;
  GList *items, *iter;
  int i;

  self->priv->infos_by_timestamp = NULL;
  items = (GList*) g_slist_sort ((GSList*) gtk_recent_manager_get_items (priv->manager),
                                 sort_infos_by_timestamp_descending);
  i = 0;
  for (iter = items; iter; iter = iter->next)
    {
      GtkRecentInfo *info = iter->data;
      if (i < MAX_RECENT_ITEMS) {
        priv->infos_by_timestamp = g_slist_prepend (priv->infos_by_timestamp, info);
      }
      else {
        gtk_recent_info_unref (info);
      }
      i++;
    }
  priv->infos_by_timestamp = g_slist_reverse (priv->infos_by_timestamp);
  g_list_free (items);
}

static gboolean
idle_handle_recent_changed (gpointer data)
{
  LemonDocSystem *self = LEMON_DOC_SYSTEM (data);
  self->priv->idle_recent_changed_id = 0;
  g_slist_free_full (self->priv->infos_by_timestamp, (GDestroyNotify) gtk_recent_info_unref);
  load_items(self);
  g_signal_emit (self, signals[CHANGED], 0);

  return FALSE;
}

static void
lemon_doc_system_on_recent_changed (GtkRecentManager  *manager,
                                    LemonDocSystem    *self)
{
  if (self->priv->idle_recent_changed_id != 0)
    return;
  self->priv->idle_recent_changed_id = g_timeout_add (0, idle_handle_recent_changed, self);
}

static void
lemon_doc_system_class_init(LemonDocSystemClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  signals[CHANGED] =
    g_signal_new ("changed",
		  LEMON_TYPE_DOC_SYSTEM,
		  G_SIGNAL_RUN_LAST,
		  0,
                 NULL, NULL, NULL,
		  G_TYPE_NONE, 0);

  g_type_class_add_private (gobject_class, sizeof (LemonDocSystemPrivate));
}

static void
lemon_doc_system_init (LemonDocSystem *self)
{
  LemonDocSystemPrivate *priv;
  self->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                                   LEMON_TYPE_DOC_SYSTEM,
                                                   LemonDocSystemPrivate);

  self->priv->manager = gtk_recent_manager_get_default ();
  load_items(self);
  g_signal_connect (self->priv->manager, "changed", G_CALLBACK(lemon_doc_system_on_recent_changed), self);
}

/**
 * lemon_doc_system_get_default:
 *
 * Return Value: (transfer none): The global #LemonDocSystem singleton
 */
LemonDocSystem *
lemon_doc_system_get_default (void)
{
  static LemonDocSystem *instance = NULL;

  if (instance == NULL)
    instance = g_object_new (LEMON_TYPE_DOC_SYSTEM, NULL);

  return instance;
}

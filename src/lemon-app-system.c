/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include "lemon-app-system.h"
#include <string.h>

#include <gio/gio.h>
#include <glib/gi18n.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-desktopappinfo.h>

#include "lemon-app-private.h"
#include "lemon-window-tracker-private.h"
#include "lemon-app-system-private.h"
#include "lemon-global.h"
#include "lemon-util.h"

// Set to 1 to enable debugging of the duplicate renaming stuff.
#define DEBUG_APPSYS_RENAMING 0

#if DEBUG_APPSYS_RENAMING
#define DEBUG_RENAMING(format, ...) \
  g_printerr ("DEBUG APPSYS_RENAMING: " format, ##__VA_ARGS__)
#else
#define DEBUG_RENAMING(format, ...) \
  G_STMT_START { } G_STMT_END
#endif

/* Vendor prefixes are something that can be prepended to a .desktop
 * file name.  Undo this.
 */
static const char*const vendor_prefixes[] = { "gnome-",
                                              "fedora-",
                                              "mozilla-",
                                              NULL };

enum {
   PROP_0,

};

enum {
  APP_STATE_CHANGED,
  INSTALLED_CHANGED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

struct _LemonAppSystemPrivate {
  GMenuTree *apps_tree;

  GHashTable *running_apps;
  GHashTable *id_to_app;
  GHashTable *startup_wm_class_to_app;
  GHashTable *flatpak_id_to_app;

  GSList *known_vendor_prefixes;
};

static void lemon_app_system_finalize (GObject *object);
static void on_apps_tree_changed_cb (GMenuTree *tree, gpointer user_data);
LemonApp * lookup_heuristic_basename (LemonAppSystem *system, const char *name);
gchar *strip_extension (gchar *wm_class);
gboolean case_insensitive_search (const char *key,
                                  const char *value,
                                  gpointer user_data);

G_DEFINE_TYPE(LemonAppSystem, lemon_app_system, G_TYPE_OBJECT);

static void lemon_app_system_class_init(LemonAppSystemClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  gobject_class->finalize = lemon_app_system_finalize;

  signals[APP_STATE_CHANGED] = g_signal_new ("app-state-changed",
                                             LEMON_TYPE_APP_SYSTEM,
                                             G_SIGNAL_RUN_LAST,
                                             0,
                                             NULL, NULL, NULL,
                                             G_TYPE_NONE, 1,
                                             LEMON_TYPE_APP);
  signals[INSTALLED_CHANGED] =
    g_signal_new ("installed-changed",
		  LEMON_TYPE_APP_SYSTEM,
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (LemonAppSystemClass, installed_changed),
		  NULL, NULL, NULL,
		  G_TYPE_NONE, 0);

  g_type_class_add_private (gobject_class, sizeof (LemonAppSystemPrivate));
}

static void
setup_merge_dir_symlink(void)
{
    gchar *user_config = (gchar *) g_get_user_config_dir();
    gchar *merge_path = g_build_filename (user_config, "menus", "applications-merged", NULL);
    GFile *merge_file = g_file_new_for_path (merge_path);
    gchar *sym_path;
    GFile *sym_file;

    g_file_make_directory_with_parents (merge_file, NULL, NULL);

    sym_path = g_build_filename (user_config, "menus", "lemon-applications-merged", NULL);
    sym_file = g_file_new_for_path (sym_path);
    if (!g_file_query_exists (sym_file, NULL)) {
        g_file_make_symbolic_link (sym_file, merge_path, NULL, NULL);
    }

    g_free (merge_path);
    g_free (sym_path);
    g_object_unref (merge_file);
    g_object_unref (sym_file);
}

static void
lemon_app_system_init (LemonAppSystem *self)
{
  LemonAppSystemPrivate *priv;

  self->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                                   LEMON_TYPE_APP_SYSTEM,
                                                   LemonAppSystemPrivate);

  priv->running_apps = g_hash_table_new_full (NULL, NULL, (GDestroyNotify) g_object_unref, NULL);
  priv->id_to_app = g_hash_table_new_full (g_str_hash, g_str_equal,
                                           NULL,
                                           (GDestroyNotify)g_object_unref);

  priv->startup_wm_class_to_app = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                         NULL,
                                                         (GDestroyNotify)g_object_unref);

  priv->flatpak_id_to_app = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                         g_free,
                                                         (GDestroyNotify)g_object_unref);
/* According to desktop spec, since our menu file is called 'lemon-applications', our
 * merged menu folders need to be called 'lemon-applications-merged'.  We'll setup the folder
 * 'applications-merged' if it doesn't exist yet, and a symlink pointing to it in the
 * ~/.config/menus directory
 */
  setup_merge_dir_symlink();

  /* For now, we want to pick up Evince, Nemo, etc.  We'll
   * handle NODISPLAY semantics at a higher level or investigate them
   * case by case.
   */
  priv->apps_tree = gmenu_tree_new ("lemon-applications.menu", GMENU_TREE_FLAGS_INCLUDE_NODISPLAY);
  g_signal_connect (priv->apps_tree, "changed", G_CALLBACK (on_apps_tree_changed_cb), self);

  on_apps_tree_changed_cb (priv->apps_tree, self);
}

static void
lemon_app_system_finalize (GObject *object)
{
  LemonAppSystem *self = LEMON_APP_SYSTEM (object);
  LemonAppSystemPrivate *priv = self->priv;

  g_object_unref (priv->apps_tree);
  g_hash_table_destroy (priv->running_apps);
  g_hash_table_destroy (priv->id_to_app);
  g_hash_table_destroy (priv->startup_wm_class_to_app);
  g_hash_table_destroy (priv->flatpak_id_to_app);
  g_slist_free_full (priv->known_vendor_prefixes, g_free);
  priv->known_vendor_prefixes = NULL;

  G_OBJECT_CLASS (lemon_app_system_parent_class)->finalize (object);
}

static char *
get_prefix_for_entry (GMenuTreeEntry *entry)
{
  char *prefix = NULL, *file_prefix = NULL;
  const char *id;
  GFile *file;
  char *name;
  int i = 0;

  id = gmenu_tree_entry_get_desktop_file_id (entry);
  file = g_file_new_for_path (gmenu_tree_entry_get_desktop_file_path (entry));
  name = g_file_get_basename (file);

  if (!name)
    {
      g_object_unref (file);
      return NULL;
    }
  for (i = 0; vendor_prefixes[i]; i++)
    {
      if (g_str_has_prefix (name, vendor_prefixes[i]))
        {
          file_prefix = g_strdup (vendor_prefixes[i]);
          break;
        }
    }

  while (strcmp (name, id) != 0)
    {
      char *t;
      char *pname;
      GFile *parent = g_file_get_parent (file);

      if (!parent)
        {
          g_warn_if_reached ();
          break;
        }

      pname = g_file_get_basename (parent);
      if (!pname)
        {
          g_object_unref (parent);
          break;
        }
      if (!g_strstr_len (id, -1, pname))
        {
          /* handle <LegacyDir prefix="..."> */
          char *t1;
          size_t name_len = strlen (name);
          size_t id_len = strlen (id);
          char *t_id = g_strdup (id);

          t_id[id_len - name_len] = '\0';
          t1 = g_strdup(t_id);
          g_free (prefix);
          g_free (t_id);
          g_free (name);
          name = g_strdup (id);
          prefix = t1;

          g_object_unref (file);
          file = parent;
          g_free (pname);
          g_free (file_prefix);
          file_prefix = NULL;
          break;
        }

      t = g_strconcat (pname, "-", name, NULL);
      g_free (name);
      name = t;

      t = g_strconcat (pname, "-", prefix, NULL);
      g_free (prefix);
      prefix = t;

      g_object_unref (file);
      file = parent;
      g_free (pname);
    }

  if (file)
    g_object_unref (file);

  if (strcmp (name, id) == 0)
    {
      g_free (name);
      if (file_prefix && !prefix)
        return file_prefix;
      if (file_prefix)
        {
          char *t = g_strconcat (prefix, "-", file_prefix, NULL);
          g_free (prefix);
          g_free (file_prefix);
          prefix = t;
        }
      return prefix;
    }

  g_free (name);
  g_free (prefix);
  g_free (file_prefix);
  g_return_val_if_reached (NULL);
}

typedef struct
{
  GPtrArray *group;
  const gchar *key;
} RenameAppData;

static gchar *
capitalize (const gchar *name)
{
  gchar *first_letter_capped;
  gchar *ret;

  first_letter_capped = g_utf8_strup (name, 1);

  ret = g_strconcat (first_letter_capped, (name + 1), NULL);

  g_free (first_letter_capped);

  return ret;
}

static gboolean
apps_have_same_name_and_exec (LemonApp *app1,
                              LemonApp *app2)
{
    const gchar *name1, *name2;
    const gchar *exec1, *exec2;

    name1 = lemon_app_get_name (app1);
    name2 = lemon_app_get_name (app2);

    if (g_strcmp0 (name1, name2) != 0)
      {
        return FALSE;
      }

    exec1 = _lemon_app_get_executable (app1);
    exec2 = _lemon_app_get_executable (app2);

    return g_strcmp0 (exec1, exec2) == 0;
}

static void
rename_app (LemonApp *app,
            gpointer     user_data,
            gboolean     flatpak_iteration)
{
  RenameAppData *data;
  const gchar *common_name;
  gchar *unique_name, *basename, *capitalized_exec;
  guint i;

  data = (RenameAppData *) user_data;

  common_name = data->key;

  if (_lemon_app_get_unique_name (app) != NULL)
    {
      DEBUG_RENAMING ("      Skipping rename of app at %s - it is already modified (%s)\n",
                      lemon_app_get_id (app),
                      _lemon_app_get_unique_name (app));

      return;
    }

  if (lemon_app_get_is_flatpak (app))
    {
      unique_name = g_strdup_printf ("%s (Flatpak)",
                                     common_name);

      DEBUG_RENAMING ("      Marking app at %s as a flatpak (%s)\n",
                      lemon_app_get_id (app),
                      unique_name);

      _lemon_app_set_unique_name (app, unique_name);
    }

  if (flatpak_iteration)
    {
      return;
    }

  basename = g_path_get_basename (_lemon_app_get_executable (app));

  capitalized_exec = capitalize (basename);
  g_free (basename);

  if (g_strcmp0 (capitalized_exec, common_name) == 0)
    {
      DEBUG_RENAMING ("      Skipping rename of app at %s since its display name "
                      "is the same as its executable name (%s)\n",
                      lemon_app_get_id (app),
                      common_name);

      _lemon_app_set_unique_name (app, g_strdup (common_name));

      g_free (capitalized_exec);

      return;
    }

  unique_name = g_strdup_printf ("%s (%s)",
                                 common_name,
                                 capitalized_exec);

  g_free (capitalized_exec);

  DEBUG_RENAMING ("      Renaming app at '%s' from '%s' to '%s'\n",
                  lemon_app_get_id (app),
                  common_name,
                  unique_name);

  _lemon_app_set_unique_name (app, unique_name);

  for (i = 0; i < data->group->len; i++)
    {
      LemonApp *other_app = g_ptr_array_index (data->group, i);

      if (other_app == app)
        {
          continue;
        }

      if (apps_have_same_name_and_exec (app, other_app))
        {
          DEBUG_RENAMING ("        Hiding app at '%s' as functional duplicate to app at '%s'\n",
                          lemon_app_get_id (app),
                          lemon_app_get_id (other_app));

          _lemon_app_set_hidden_as_duplicate (other_app, TRUE);
        }
    }
}

static gboolean
still_has_names_duplicated (RenameAppData *data)
{
    guint i;
    gboolean duplicated;
    GHashTable *name_register;

    name_register = g_hash_table_new_full (g_str_hash, g_str_equal,
                                           (GDestroyNotify) NULL,
                                           (GDestroyNotify) NULL);

    duplicated = FALSE;

    for (i = 0; i < data->group->len; i++)
      {
        LemonApp *app = g_ptr_array_index (data->group, i);

        if (!g_hash_table_add (name_register, (gpointer) lemon_app_get_name (app)))
          {
            duplicated = TRUE;
            break;
          }
      }

    g_hash_table_destroy (name_register);

    return duplicated;
}

static void
deduplicate_apps (GPtrArray   *app_array,
                  gpointer    *key)
{
  RenameAppData data;
  guint i;

  DEBUG_RENAMING ("%d apps with conflicting names: '%s'.  Renaming.\n",
                  app_array->len, (gchar *) key);

  data.group = app_array;
  data.key = (gchar *) key;

  i = 0;

  DEBUG_RENAMING (" - flatpak iteration\n");

  while (i < app_array->len)
    {
      rename_app (g_ptr_array_index (app_array, i), &data, TRUE);

      i++;
    }

  if (still_has_names_duplicated (&data))
    {
      i = 0;
      DEBUG_RENAMING (" - still have duplicates, handling the rest\n");

      while (i < app_array->len)
        {
          rename_app (g_ptr_array_index (app_array, i), &data, FALSE);

          i++;
        }
    }

  DEBUG_RENAMING ("Done renaming for '%s'\n\n", (gchar *) key);
}

static void
get_flattened_entries_recurse (GMenuTreeDirectory *dir,
                               GHashTable         *entry_set)
{
  GMenuTreeIter *iter = gmenu_tree_directory_iter (dir);
  GMenuTreeItemType next_type;

  while ((next_type = gmenu_tree_iter_next (iter)) != GMENU_TREE_ITEM_INVALID)
    {
      gpointer item = NULL;

      switch (next_type)
        {
        case GMENU_TREE_ITEM_ENTRY:
          {
            GMenuTreeEntry *entry;
            GMenuDesktopAppInfo *info;
            item = entry = gmenu_tree_iter_get_entry (iter);
            info = gmenu_tree_entry_get_app_info (entry);
            /* Key is owned by entry */

            if (info == NULL)
              {
                break;
              }
            g_hash_table_replace (entry_set,
                                  (char *) gmenu_tree_entry_get_desktop_file_id (entry),
                                  gmenu_tree_item_ref (entry));
          }
          break;
        case GMENU_TREE_ITEM_DIRECTORY:
          {
            item = gmenu_tree_iter_get_directory (iter);
            get_flattened_entries_recurse ((GMenuTreeDirectory*)item, entry_set);
          }
          break;
        case GMENU_TREE_ITEM_INVALID:
        case GMENU_TREE_ITEM_SEPARATOR:
        case GMENU_TREE_ITEM_HEADER:
        case GMENU_TREE_ITEM_ALIAS:
          break;
        default:
          break;
        }
      if (item != NULL)
        gmenu_tree_item_unref (item);
    }

  gmenu_tree_iter_unref (iter);
}

static GHashTable *
get_flattened_entries_from_tree (GMenuTree *tree)
{
  GHashTable *table;
  GMenuTreeDirectory *root;

  table = g_hash_table_new_full (g_str_hash, g_str_equal,
                                 (GDestroyNotify) NULL,
                                 (GDestroyNotify) gmenu_tree_item_unref);

  root = gmenu_tree_get_root_directory (tree);

  if (root != NULL)
    get_flattened_entries_recurse (root, table);

  gmenu_tree_item_unref (root);

  return table;
}

static void
on_apps_tree_changed_cb (GMenuTree *tree,
                         gpointer   user_data)
{
  LemonAppSystem *self = LEMON_APP_SYSTEM (user_data);
  GError *error = NULL;
  GHashTable *new_apps, *display_names;
  GHashTableIter iter;
  gpointer key, value;

  g_assert (tree == self->priv->apps_tree);

  g_slist_free_full (self->priv->known_vendor_prefixes, g_free);
  self->priv->known_vendor_prefixes = NULL;

  if (!gmenu_tree_load_sync (self->priv->apps_tree, &error))
    {
      if (error)
        {
          g_warning ("Failed to load apps: %s", error->message);
          g_error_free (error);
        }
      else
        {
          g_warning ("Failed to load apps");
        }
      return;
    }

  new_apps = get_flattened_entries_from_tree (self->priv->apps_tree);

  display_names = g_hash_table_new_full (g_str_hash, g_str_equal,
                                         (GDestroyNotify) g_free,
                                         (GDestroyNotify) g_ptr_array_unref);

  g_hash_table_iter_init (&iter, new_apps);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *id = key;
      GMenuTreeEntry *entry = value;
      GMenuTreeEntry *old_entry;
      char *prefix;
      LemonApp *app;

      GMenuDesktopAppInfo *info;
      const char *startup_wm_class;
      prefix = get_prefix_for_entry (entry);

      if (prefix != NULL
          && !g_slist_find_custom (self->priv->known_vendor_prefixes, prefix,
                                   (GCompareFunc)g_strcmp0))
        self->priv->known_vendor_prefixes = g_slist_append (self->priv->known_vendor_prefixes,
                                                            prefix);
      else
        g_free (prefix);

      app = g_hash_table_lookup (self->priv->id_to_app, id);
      if (app != NULL)
        {
          /* We hold a reference to the original entry temporarily,
           * because otherwise the hash table would be referencing
           * potentially free'd memory until we replace it below with
           * the new data.
           */
          old_entry = lemon_app_get_tree_entry (app);
          gmenu_tree_item_ref (old_entry);

#if DEBUG_APPSYS_RENAMING
          if (g_strcmp0 (_lemon_app_get_desktop_path (app),
                         gmenu_desktopappinfo_get_filename (gmenu_tree_entry_get_app_info (entry))) == 0)
            {
              DEBUG_RENAMING ("Existing match found for app: '%s'.  Source unchanged ('%s')\n",
                              _lemon_app_get_common_name (app),
                              _lemon_app_get_desktop_path (app));

            }
          else
            {
              DEBUG_RENAMING ("Existing match found for app: '%s'.  Source is changing from '%s' to '%s'\n",
                              _lemon_app_get_common_name (app),
                              _lemon_app_get_desktop_path (app),
                              gmenu_desktopappinfo_get_filename (gmenu_tree_entry_get_app_info (entry)));
            }
#endif

          _lemon_app_set_entry (app, entry);

          g_object_ref (app);  /* Extra ref, removed in _replace below */
        }
      else
        {
          old_entry = NULL;
          app = _lemon_app_new (entry);

          DEBUG_RENAMING ("New app entry: '%s' with source '%s'\n",
                          _lemon_app_get_common_name (app),
                          _lemon_app_get_desktop_path (app));

        }
      /* Note that "id" and "flatpak_app_id" are owned by app->entry.  Since we're always
       * setting a new entry, even if the app already exists in the
       * hash tables we need to replace the keys so that the new id and flatpak app id
       * string are pointed to.
       */
      g_hash_table_replace (self->priv->id_to_app, (char*)id, app);

      if (lemon_app_get_is_flatpak (app))
      {
        gchar *flatpak_app_id = lemon_app_get_flatpak_app_id (app);
        gchar **split = g_strsplit (id, ".desktop", -1);

        if (g_strv_length (split) > 0 && g_strcmp0 (split[0], flatpak_app_id) == 0)
        {
          g_hash_table_replace (self->priv->flatpak_id_to_app, flatpak_app_id, g_object_ref (app));
        }
        g_strfreev (split);
      }
      // if (!gmenu_tree_entry_get_is_nodisplay_recurse (entry))
      //    g_hash_table_replace (self->priv->visible_id_to_app, (char*)id, app);

      if (old_entry)
        {
          GMenuDesktopAppInfo *old_info;
          const gchar *old_startup_wm_class;

          old_info = gmenu_tree_entry_get_app_info (old_entry);
          old_startup_wm_class = gmenu_desktopappinfo_get_startup_wm_class (old_info);

          if (old_startup_wm_class)
            g_hash_table_remove (self->priv->startup_wm_class_to_app, old_startup_wm_class);
        }

      info = lemon_app_get_app_info (app);
      if (info)
        {
          startup_wm_class = gmenu_desktopappinfo_get_startup_wm_class (info);
          if (startup_wm_class)
            g_hash_table_replace (self->priv->startup_wm_class_to_app,
                                  (char*)startup_wm_class, g_object_ref (app));
        }

      if (old_entry)
        {
          gmenu_tree_item_unref (old_entry);
        }


      if (!lemon_app_get_nodisplay (app))
        {
          GPtrArray *same_name_apps;
          const char *common_name;

          common_name = _lemon_app_get_common_name (app);

          DEBUG_RENAMING ("Adding '%s' (at %s) to check for uniqueness\n",
                          _lemon_app_get_common_name (app), id);

          same_name_apps = g_hash_table_lookup (display_names, common_name);

          if (same_name_apps == NULL)
            {
              same_name_apps = g_ptr_array_new ();
              g_hash_table_insert (display_names,
                                   g_strdup (common_name),
                                   same_name_apps);
            }

          g_ptr_array_add (same_name_apps, (gpointer) app);
        }
      else
        {
          DEBUG_RENAMING ("...Skipping '%s' (at %s): App is nodisplay\n",
                          _lemon_app_get_common_name (app), id);
        }
    }
  /* Now iterate over the apps again; we need to unreference any apps
   * which have been removed.  The JS code may still be holding a
   * reference; that's fine.
   */
  g_hash_table_iter_init (&iter, self->priv->id_to_app);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *id = key;

      if (!g_hash_table_lookup (new_apps, id))
        g_hash_table_iter_remove (&iter);
    }

  g_hash_table_iter_init (&iter, display_names);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      GPtrArray *array = (GPtrArray *) value;

      if (array->len > 1)
        {
          deduplicate_apps (array, key);
        }
    }

  g_hash_table_destroy (new_apps);
  g_hash_table_destroy (display_names);

  g_signal_emit (self, signals[INSTALLED_CHANGED], 0);
}

/**
 * lemon_app_system_get_tree:
 *
 * Return Value: (transfer none): The #GMenuTree for apps
 */
GMenuTree *
lemon_app_system_get_tree (LemonAppSystem *self)
{
  return self->priv->apps_tree;
}

/**
 * lemon_app_system_get_default:
 *
 * Return Value: (transfer none): The global #LemonAppSystem singleton
 */
LemonAppSystem *
lemon_app_system_get_default (void)
{
  static LemonAppSystem *instance = NULL;

  if (instance == NULL)
    instance = g_object_new (LEMON_TYPE_APP_SYSTEM, NULL);

  return instance;
}

gboolean
case_insensitive_search (const char *key,
                         const char *value,
                         gpointer user_data)
{
  char *given_id = (char *) user_data;

  if (g_ascii_strcasecmp(key, given_id) == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * lemon_app_system_lookup_app:
 *
 * Find a #LemonApp corresponding to an id.
 *
 * Return value: (transfer none): The #LemonApp for id, or %NULL if none
 */
LemonApp *
lemon_app_system_lookup_app (LemonAppSystem   *self,
                             const char       *id)
{
  LemonApp *result;

  result = g_hash_table_lookup (self->priv->id_to_app, id);
  if (result == NULL) {
    result = g_hash_table_find (self->priv->id_to_app, (GHRFunc) case_insensitive_search, (gpointer) id);
  }
  return result;
}

/*
 * Find a valid application corresponding to a given
 * heuristically determined application identifier
 * string, or %NULL if none.
 */
LemonApp *
lookup_heuristic_basename (LemonAppSystem *system,
                                            const char     *name)
{
  LemonApp *result;
  GSList *prefix;

  result = lemon_app_system_lookup_app (system, name);
  if (result != NULL)
    return result;

  for (prefix = system->priv->known_vendor_prefixes; prefix; prefix = g_slist_next (prefix))
    {
      char *tmpid = g_strconcat ((char*)prefix->data, name, NULL);
      result = lemon_app_system_lookup_app (system, tmpid);
      g_free (tmpid);
      if (result != NULL)
        return result;
    }

  return NULL;
}

gchar *
strip_flatpak_suffix (gchar *wm_class)
{
  char *result;
    if (g_str_has_suffix (wm_class, GMENU_DESKTOPAPPINFO_FLATPAK_SUFFIX)) {
            result = g_strndup (wm_class, strlen (wm_class) - strlen (GMENU_DESKTOPAPPINFO_FLATPAK_SUFFIX));
    } else {
        result = g_strdup (wm_class);
    }
    return result;
}

gchar *
strip_extension (gchar *wm_class)
{
    char *result;
    if (g_str_has_suffix (wm_class, ".py") ||
        g_str_has_suffix (wm_class, ".sh")) {
            result = g_strndup (wm_class, strlen (wm_class) - 3);
    } else {
        result = g_strdup (wm_class);
    }
    return result;
}



/**
 * lemon_app_system_lookup_desktop_wmclass:
 * @system: a #LemonAppSystem
 * @wmclass: (nullable): A WM_CLASS value
 *
 * Find a valid application whose .desktop file, without the extension
 * and properly canonicalized, matches @wmclass.
 *
 * Returns: (transfer none): A #LemonApp for @wmclass
 */
LemonApp *
lemon_app_system_lookup_desktop_wmclass (LemonAppSystem *system,
                                            const char        *wmclass)
{
  char *canonicalized;
  char *after_flatpak_strip;
  char *desktop_file;
  char *stripped_name;
  gboolean is_flatpak;
  LemonApp *app;

  if (wmclass == NULL)
    return NULL;

  is_flatpak = g_str_has_suffix (wmclass, GMENU_DESKTOPAPPINFO_FLATPAK_SUFFIX);

  canonicalized = g_ascii_strdown (wmclass, -1);

  after_flatpak_strip = strip_flatpak_suffix (canonicalized);

  stripped_name = strip_extension(after_flatpak_strip);

  /* This handles "Fedora Eclipse", probably others.
   * Note g_strdelimit is modify-in-place. */
  g_strdelimit (stripped_name, " ", '-');

  if (is_flatpak)
  {
    desktop_file = g_strconcat (stripped_name, ".desktop", GMENU_DESKTOPAPPINFO_FLATPAK_SUFFIX, NULL);
  } else {
    desktop_file = g_strconcat (stripped_name, ".desktop", NULL);
  }
  app = lookup_heuristic_basename (system, desktop_file);

  g_free (after_flatpak_strip);
  g_free (canonicalized);
  g_free (stripped_name);
  g_free (desktop_file);

  return app;
}

/**
 * lemon_app_system_lookup_startup_wmclass:
 * @system: a #LemonAppSystem
 * @wmclass: (nullable): A WM_CLASS value
 *
 * Find a valid application whose .desktop file contains a
 * StartupWMClass entry matching @wmclass.
 *
 * Returns: (transfer none): A #LemonApp for @wmclass
 */
LemonApp *
lemon_app_system_lookup_startup_wmclass (LemonAppSystem *system,
                                            const char     *wmclass)
{
  if (wmclass == NULL)
    return NULL;

  return g_hash_table_lookup (system->priv->startup_wm_class_to_app, wmclass);
}

/**
 * lemon_app_system_lookup_flatpak_app_id:
 *
 * Find a #LemonApp corresponding to a flatpak app id.
 *
 * Return value: (transfer none): The #LemonApp for app_id, or %NULL if none
 */
LemonApp *
lemon_app_system_lookup_flatpak_app_id (LemonAppSystem *system,
                                           const char        *app_id)
{
  if (app_id == NULL)
    return NULL;

  return g_hash_table_lookup (system->priv->flatpak_id_to_app, app_id);
}

/**
 * lemon_app_system_get_all:
 * @system:
 *
 * Returns: (transfer container) (element-type LemonApp): All installed applications
 */
GSList *
lemon_app_system_get_all (LemonAppSystem  *self)
{
  GSList *result = NULL;
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init (&iter, self->priv->id_to_app);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      LemonApp *app = value;

      if (!gmenu_desktopappinfo_get_nodisplay (lemon_app_get_app_info (app)))
        result = g_slist_prepend (result, app);
    }
  return result;
}

void
_lemon_app_system_notify_app_state_changed (LemonAppSystem *self,
                                            LemonApp       *app)
{
  LemonAppState state = lemon_app_get_state (app);

  switch (state)
    {
    case LEMON_APP_STATE_RUNNING:
      g_hash_table_insert (self->priv->running_apps, g_object_ref (app), NULL);
      break;
    case LEMON_APP_STATE_STARTING:
    case LEMON_APP_STATE_STOPPED:
      break;
    default:
      g_warning("lemon_app_system_notify_app_state_changed: default case");
    break;
    }
  g_signal_emit (self, signals[APP_STATE_CHANGED], 0, app);

  if (state == LEMON_APP_STATE_STOPPED)
    {
      g_hash_table_remove (self->priv->running_apps, app);
    }
}

/**
 * lemon_app_system_get_running:
 * @self: A #LemonAppSystem
 *
 * Returns the set of applications which currently have at least one
 * open window in the given context.
 *
 * Returns: (element-type LemonApp) (transfer container): Active applications
 */
GSList *
lemon_app_system_get_running (LemonAppSystem *self)
{
  gpointer key, value;
  GSList *ret;
  GHashTableIter iter;

  g_hash_table_iter_init (&iter, self->priv->running_apps);

  ret = NULL;
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      LemonApp *app = key;

      ret = g_slist_prepend (ret, app);
    }

  return ret;
}

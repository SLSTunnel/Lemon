/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <girepository.h>
#include <gtk/gtk.h>
#include <meta/display.h>
#include <meta/util.h>

#include "lemon-tray-manager.h"
#include "na-tray-manager.h"

#include "lemon-tray-icon.h"
#include "lemon-embedded-window.h"
#include "lemon-global.h"

typedef struct _LemonTrayManagerPrivate LemonTrayManagerPrivate;

struct _LemonTrayManager
{
  GObject parent_instance;

  LemonTrayManagerPrivate *priv;
};

struct _LemonTrayManagerPrivate {
  NaTrayManager *na_manager;
  ClutterColor bg_color;

  GHashTable *icons;
  StWidget *theme_widget;
};

typedef struct {
  LemonTrayManager *manager;
  GtkWidget *socket;
  GtkWidget *window;
  ClutterActor *actor;
} LemonTrayManagerChild;

enum {
  PROP_0,

  PROP_BG_COLOR
};

/* Signals */
enum
{
  TRAY_ICON_ADDED,
  TRAY_ICON_REMOVED,
  LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (LemonTrayManager, lemon_tray_manager, G_TYPE_OBJECT);

static guint lemon_tray_manager_signals [LAST_SIGNAL] = { 0 };

static const ClutterColor default_color = { 0x00, 0x00, 0x00, 0xff };

static void lemon_tray_manager_release_resources (LemonTrayManager *manager);

static void na_tray_icon_added (NaTrayManager *na_manager, GtkWidget *child, gpointer manager);
static void na_tray_icon_removed (NaTrayManager *na_manager, GtkWidget *child, gpointer manager);

static void
free_tray_icon (gpointer data)
{
  LemonTrayManagerChild *child = data;

  gtk_widget_destroy (child->window);
  if (child->actor)
    {
      g_signal_handlers_disconnect_matched (child->actor, G_SIGNAL_MATCH_DATA,
                                            0, 0, NULL, NULL, child);
      g_object_unref (child->actor);
    }
  g_slice_free (LemonTrayManagerChild, child);
}

static void
lemon_tray_manager_set_property(GObject         *object,
                                guint            prop_id,
                                const GValue    *value,
                                GParamSpec      *pspec)
{
  LemonTrayManager *manager = LEMON_TRAY_MANAGER (object);

  switch (prop_id)
    {
    case PROP_BG_COLOR:
      {
        ClutterColor *color = g_value_get_boxed (value);
        if (color)
          manager->priv->bg_color = *color;
        else
          manager->priv->bg_color = default_color;
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
lemon_tray_manager_get_property(GObject         *object,
                                guint            prop_id,
                                GValue          *value,
                                GParamSpec      *pspec)
{
  LemonTrayManager *manager = LEMON_TRAY_MANAGER (object);

  switch (prop_id)
    {
    case PROP_BG_COLOR:
      g_value_set_boxed (value, &manager->priv->bg_color);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
lemon_tray_manager_init (LemonTrayManager *manager)
{
  manager->priv = lemon_tray_manager_get_instance_private (manager);

  manager->priv->bg_color = default_color;
}

static void
lemon_tray_manager_finalize (GObject *object)
{
  LemonTrayManager *manager = LEMON_TRAY_MANAGER (object);

  lemon_tray_manager_release_resources (manager);

  G_OBJECT_CLASS (lemon_tray_manager_parent_class)->finalize (object);
}

static void
lemon_tray_manager_class_init (LemonTrayManagerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = lemon_tray_manager_finalize;
  gobject_class->set_property = lemon_tray_manager_set_property;
  gobject_class->get_property = lemon_tray_manager_get_property;

  lemon_tray_manager_signals[TRAY_ICON_ADDED] =
    g_signal_new ("tray-icon-added",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  CLUTTER_TYPE_ACTOR);
  lemon_tray_manager_signals[TRAY_ICON_REMOVED] =
    g_signal_new ("tray-icon-removed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  CLUTTER_TYPE_ACTOR);

  /* Lifting the CONSTRUCT_ONLY here isn't hard; you just need to
   * iterate through the icons, reset the background pixmap, and
   * call na_tray_child_force_redraw()
   */
  g_object_class_install_property (gobject_class,
                                   PROP_BG_COLOR,
                                   g_param_spec_boxed ("bg-color",
                                                       "BG Color",
                                                       "Background color (only if we don't have transparency)",
                                                       CLUTTER_TYPE_COLOR,
                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

LemonTrayManager *
lemon_tray_manager_new (void)
{
  if (meta_is_wayland_compositor ())
    {
      g_critical ("Lemon's TrayManager is not available in a Wayland session!");
    }

  return g_object_new (LEMON_TYPE_TRAY_MANAGER, NULL);
}

static void
lemon_tray_manager_ensure_resources (LemonTrayManager *manager)
{
  if (manager->priv->na_manager != NULL)
    return;

  manager->priv->icons = g_hash_table_new_full (NULL, NULL,
                                                NULL, free_tray_icon);

  manager->priv->na_manager = na_tray_manager_new ();

  g_signal_connect (manager->priv->na_manager, "tray-icon-added",
                    G_CALLBACK (na_tray_icon_added), manager);
  g_signal_connect (manager->priv->na_manager, "tray-icon-removed",
                    G_CALLBACK (na_tray_icon_removed), manager);
}

static void
lemon_tray_manager_release_resources (LemonTrayManager *manager)
{
  g_clear_object (&manager->priv->na_manager);
  g_clear_pointer (&manager->priv->icons, g_hash_table_destroy);
}

static void
lemon_tray_manager_style_changed (StWidget *theme_widget,
                                  gpointer  user_data)
{
  LemonTrayManager *manager = user_data;
  StThemeNode *theme_node;
  StIconColors *icon_colors;

  if (manager->priv->na_manager == NULL)
    return;

  theme_node = st_widget_get_theme_node (theme_widget);
  icon_colors = st_theme_node_get_icon_colors (theme_node);
  na_tray_manager_set_colors (manager->priv->na_manager,
                              &icon_colors->foreground, &icon_colors->warning,
                              &icon_colors->error, &icon_colors->success);
}

static void
lemon_tray_manager_manage_screen_internal (LemonTrayManager *manager)
{
  lemon_tray_manager_ensure_resources (manager);
  na_tray_manager_manage_screen (manager->priv->na_manager);
}

void
lemon_tray_manager_manage_screen (LemonTrayManager *manager,
                                  StWidget         *theme_widget)
{
  MetaDisplay *display = lemon_global_get_display (lemon_global_get ());

  g_set_weak_pointer (&manager->priv->theme_widget, theme_widget);

  if (meta_display_get_x11_display (display) != NULL)
    lemon_tray_manager_manage_screen_internal (manager);

  g_signal_connect_object (display, "x11-display-setup",
                           G_CALLBACK (lemon_tray_manager_manage_screen_internal),
                           manager, G_CONNECT_SWAPPED);
  g_signal_connect_object (display, "x11-display-closing",
                           G_CALLBACK (lemon_tray_manager_release_resources),
                           manager, G_CONNECT_SWAPPED);

  g_signal_connect_object (theme_widget, "style-changed",
                           G_CALLBACK (lemon_tray_manager_style_changed),
                           manager, 0);
  lemon_tray_manager_style_changed (theme_widget, manager);
}

void
lemon_tray_manager_unmanage_screen (LemonTrayManager *manager)
{
  MetaDisplay *display = lemon_global_get_display (lemon_global_get ());

  g_signal_handlers_disconnect_by_data (display, manager);

  if (manager->priv->theme_widget != NULL)
    {
      g_signal_handlers_disconnect_by_func (manager->priv->theme_widget,
                                            G_CALLBACK (lemon_tray_manager_style_changed),
                                            manager);
    }
  g_set_weak_pointer (&manager->priv->theme_widget, NULL);

  lemon_tray_manager_release_resources (manager);
}

static void
lemon_tray_manager_child_on_realize (GtkWidget             *widget,
                                     LemonTrayManagerChild *child)
{
  /* If the tray child is using an RGBA colormap (and so we have real
   * transparency), we don't need to worry about the background. If
   * not, we obey the bg-color property by creating a cairo pattern of
   * that color and setting it as our background. Then "parent-relative"
   * background on the socket and the plug within that will cause
   * the icons contents to appear on top of our background color.
   */
  if (!na_tray_child_has_alpha (NA_TRAY_CHILD (child->socket)))
    {
      ClutterColor color = child->manager->priv->bg_color;
      cairo_pattern_t *bg_pattern;

      bg_pattern = cairo_pattern_create_rgb (color.red / 255.,
                                             color.green / 255.,
                                             color.blue / 255.);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
      gdk_window_set_background_pattern (gtk_widget_get_window (widget),
                                         bg_pattern);
G_GNUC_END_IGNORE_DEPRECATIONS

      cairo_pattern_destroy (bg_pattern);
    }
}

static void
on_plug_added (GtkSocket        *socket,
               LemonTrayManager *manager)
{
  LemonTrayManagerChild *child;

  g_signal_handlers_disconnect_by_func (socket, on_plug_added, manager);

  child = g_hash_table_lookup (manager->priv->icons, socket);

  child->actor = lemon_tray_icon_new (LEMON_EMBEDDED_WINDOW (child->window));
  g_object_ref_sink (child->actor);

  g_signal_emit (manager, lemon_tray_manager_signals[TRAY_ICON_ADDED], 0,
                 child->actor);
}

static void
na_tray_icon_added (NaTrayManager *na_manager, GtkWidget *socket,
                    gpointer user_data)
{
  LemonTrayManager *manager = user_data;
  GtkWidget *win;
  LemonTrayManagerChild *child;

  win = lemon_embedded_window_new ();
  gtk_container_add (GTK_CONTAINER (win), socket);

  /* The visual of the socket matches that of its contents; make
   * the window we put it in match that as well */
  gtk_widget_set_visual (win, gtk_widget_get_visual (socket));

  child = g_slice_new0 (LemonTrayManagerChild);
  child->manager = manager;
  child->window = win;
  child->socket = socket;

  g_signal_connect (win, "realize",
                    G_CALLBACK (lemon_tray_manager_child_on_realize), child);

  gtk_widget_show_all (win);

  g_hash_table_insert (manager->priv->icons, socket, child);

  g_signal_connect (socket, "plug-added", G_CALLBACK (on_plug_added), manager);
}

static void
na_tray_icon_removed (NaTrayManager *na_manager, GtkWidget *socket,
                      gpointer user_data)
{
  LemonTrayManager *manager = user_data;
  LemonTrayManagerChild *child;

  child = g_hash_table_lookup (manager->priv->icons, socket);
  g_return_if_fail (child != NULL);

  if (child->actor != NULL)
    {
      /* Only emit signal if a corresponding tray-icon-added signal was emitted,
         that is, if embedding did not fail and we got a plug-added
      */
      g_signal_emit (manager,
                     lemon_tray_manager_signals[TRAY_ICON_REMOVED], 0,
                     child->actor);
    }
  g_hash_table_remove (manager->priv->icons, socket);
}

void
lemon_tray_manager_set_orientation (LemonTrayManager *manager,
                                       ClutterOrientation   orientation)
{
  if (orientation == CLUTTER_ORIENTATION_HORIZONTAL)
    {
      na_tray_manager_set_orientation (manager->priv->na_manager, GTK_ORIENTATION_HORIZONTAL);
    }
  else
    {
      na_tray_manager_set_orientation (manager->priv->na_manager, GTK_ORIENTATION_VERTICAL);
    }
}

static void
lemon_tray_manager_child_redisplay (gpointer socket_pointer, gpointer child_pointer, gpointer user_data)
{
  LemonTrayManagerChild *child = child_pointer;

  g_return_if_fail(child != NULL);

  if (child->actor && CLUTTER_IS_ACTOR(child->actor)) {
    clutter_actor_destroy(child->actor);
  }

  on_plug_added(socket_pointer, child->manager);
}

void lemon_tray_manager_redisplay (LemonTrayManager *manager)
{
  g_hash_table_foreach(manager->priv->icons, lemon_tray_manager_child_redisplay, manager);
}

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include "lemon-embedded-window-private.h"
#include "lemon-global.h"
#include "lemon-util.h"

#include <gdk/gdkx.h>
#include <meta/display.h>
#include <meta/window.h>

enum {
   PROP_0,

   PROP_WINDOW
};

typedef struct _LemonGtkEmbedPrivate LemonGtkEmbedPrivate;

struct _LemonGtkEmbedPrivate
{
  LemonEmbeddedWindow *window;

  ClutterActor *window_actor;
  gulong window_actor_destroyed_handler;

  gulong window_created_handler;
};

G_DEFINE_TYPE_WITH_PRIVATE (LemonGtkEmbed, lemon_gtk_embed, CLUTTER_TYPE_CLONE);

static void lemon_gtk_embed_set_window (LemonGtkEmbed       *embed,
                                        LemonEmbeddedWindow *window);

static void
lemon_gtk_embed_on_window_destroy (GtkWidget     *object,
                                   LemonGtkEmbed *embed)
{
  lemon_gtk_embed_set_window (embed, NULL);
}

static void
lemon_gtk_embed_remove_window_actor (LemonGtkEmbed *embed)
{
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);

  if (priv->window_actor)
    {
      g_clear_signal_handler (&priv->window_actor_destroyed_handler,
                              priv->window_actor);

      g_object_unref (priv->window_actor);
      priv->window_actor = NULL;
    }

  clutter_clone_set_source (CLUTTER_CLONE (embed), NULL);
}

static void
maintain_transparency (ClutterActor *actor,
                       GParamSpec *pspec,
                       gpointer user_data)
{
    if (clutter_actor_get_opacity (actor) != 0) {
        g_signal_stop_emission_by_name (actor, "notify::opacity");
        g_object_set (actor, "opacity", 0, NULL);
    }
}

static void
lemon_gtk_embed_window_created_cb (MetaDisplay   *display,
                                   MetaWindow    *window,
                                   LemonGtkEmbed *embed)
{
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);
  Window xwindow = meta_window_get_xwindow (window);
  GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (priv->window));

  if (gdk_window && xwindow == gdk_x11_window_get_xid (gdk_window))
    {
      ClutterActor *window_actor =
        CLUTTER_ACTOR (meta_window_get_compositor_private (window));
      GCallback remove_cb = G_CALLBACK (lemon_gtk_embed_remove_window_actor);
      cairo_region_t *empty_region;

      clutter_clone_set_source (CLUTTER_CLONE (embed), window_actor);

      /* We want to explicitly clear the clone source when the window
         actor is destroyed because otherwise we might end up keeping
         it alive after it has been disposed. Otherwise this can cause
         a crash if there is a paint after mutter notices that the top
         level window has been destroyed, which causes it to dispose
         the window, and before the tray manager notices that the
         window is gone which would otherwise reset the window and
         unref the clone */
      priv->window_actor = g_object_ref (window_actor);
      priv->window_actor_destroyed_handler =
        g_signal_connect_swapped (window_actor,
                                  "destroy",
                                  remove_cb,
                                  embed);

      /* Hide the original actor otherwise it will appear in the scene
         as a normal window */
      clutter_actor_set_opacity (window_actor, 0);
      g_signal_connect (window_actor, "notify::opacity", G_CALLBACK (maintain_transparency), NULL);

      /* Also make sure it (or any of its children) doesn't block
         events on wayland */
      lemon_util_set_hidden_from_pick (window_actor, TRUE);

      /* Set an empty input shape on the window so that it can't get
         any input. This probably isn't the ideal way to achieve this.
         It would probably be better to force the window to go behind
         Mutter's guard window, but this is quite difficult to do as
         Mutter doesn't manage the stacking for override redirect
         windows and the guard window is repeatedly lowered to the
         bottom of the stack. */
      empty_region = cairo_region_create ();
      gdk_window_input_shape_combine_region (gdk_window,
                                             empty_region,
                                             0, 0 /* offset x/y */);
      cairo_region_destroy (empty_region);

      gdk_window_lower (gdk_window);

      /* Now that we've found the window we don't need to listen for
         new windows anymore */
      g_clear_signal_handler (&priv->window_created_handler,
                              display);
    }
}

static void
lemon_gtk_embed_on_window_mapped (GtkWidget     *object,
                                  LemonGtkEmbed *embed)
{
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);
  MetaDisplay *display = lemon_global_get_display (lemon_global_get ());

  if (priv->window_created_handler == 0 && priv->window_actor == NULL)
    /* Listen for new windows so we can detect when Mutter has
       created a MutterWindow for this window */
    priv->window_created_handler =
      g_signal_connect (display,
                        "window-created",
                        G_CALLBACK (lemon_gtk_embed_window_created_cb),
                        embed);
}

static void
lemon_gtk_embed_set_window (LemonGtkEmbed       *embed,
                            LemonEmbeddedWindow *window)
{
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);
  MetaDisplay *display = lemon_global_get_display (lemon_global_get ());

  if (priv->window)
    {
      g_clear_signal_handler (&priv->window_created_handler, display);

      lemon_gtk_embed_remove_window_actor (embed);

      _lemon_embedded_window_set_actor (priv->window, NULL);

      g_object_unref (priv->window);

      g_signal_handlers_disconnect_by_func (priv->window,
                                            (gpointer)lemon_gtk_embed_on_window_destroy,
                                            embed);

      g_signal_handlers_disconnect_by_func (priv->window,
                                            (gpointer)lemon_gtk_embed_on_window_mapped,
                                            embed);
    }

  priv->window = window;

  if (priv->window)
    {
      g_object_ref (priv->window);

      _lemon_embedded_window_set_actor (priv->window, embed);

      g_signal_connect (priv->window, "destroy",
                        G_CALLBACK (lemon_gtk_embed_on_window_destroy), embed);

      g_signal_connect (priv->window, "map",
                        G_CALLBACK (lemon_gtk_embed_on_window_mapped), embed);
    }

  clutter_actor_queue_relayout (CLUTTER_ACTOR (embed));
}

static void
lemon_gtk_embed_set_property (GObject         *object,
                              guint            prop_id,
                              const GValue    *value,
                              GParamSpec      *pspec)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (object);

  switch (prop_id)
    {
    case PROP_WINDOW:
      lemon_gtk_embed_set_window (embed, (LemonEmbeddedWindow *)g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
lemon_gtk_embed_get_property (GObject         *object,
                              guint            prop_id,
                              GValue          *value,
                              GParamSpec      *pspec)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (object);
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);

  switch (prop_id)
    {
    case PROP_WINDOW:
      g_value_set_object (value, priv->window);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
lemon_gtk_embed_get_preferred_width (ClutterActor *actor,
                                     float         for_height,
                                     float        *min_width_p,
                                     float        *natural_width_p)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (actor);
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);

  if (priv->window
      && gtk_widget_get_visible (GTK_WIDGET (priv->window)))
    {
      GtkRequisition min_req, natural_req;
      gtk_widget_get_preferred_size (GTK_WIDGET (priv->window), &min_req, &natural_req);

      *min_width_p = min_req.width;
      *natural_width_p = natural_req.width;
    }
  else
    *min_width_p = *natural_width_p = 0;
}

static void
lemon_gtk_embed_get_preferred_height (ClutterActor *actor,
                                      float         for_width,
                                      float        *min_height_p,
                                      float        *natural_height_p)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (actor);
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);

  if (priv->window
      && gtk_widget_get_visible (GTK_WIDGET (priv->window)))
    {
      GtkRequisition min_req, natural_req;
      gtk_widget_get_preferred_size (GTK_WIDGET (priv->window), &min_req, &natural_req);

      *min_height_p = min_req.height;
      *natural_height_p = natural_req.height;
    }
  else
    *min_height_p = *natural_height_p = 0;
}

static void
lemon_gtk_embed_allocate (ClutterActor          *actor,
                          const ClutterActorBox *box,
                          ClutterAllocationFlags flags)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (actor);
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);
  float wx, wy;

  CLUTTER_ACTOR_CLASS (lemon_gtk_embed_parent_class)->
    allocate (actor, box, flags);

  /* Find the actor's new coordinates in terms of the stage (which is
   * priv->window's parent window.
   */
  clutter_actor_get_transformed_position (actor, &wx, &wy);

  _lemon_embedded_window_allocate (priv->window,
                                   (int)(0.5 + wx), (int)(0.5 + wy),
                                   box->x2 - box->x1,
                                   box->y2 - box->y1);
}

static void
lemon_gtk_embed_map (ClutterActor *actor)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (actor);
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);

  _lemon_embedded_window_map (priv->window);

  CLUTTER_ACTOR_CLASS (lemon_gtk_embed_parent_class)->map (actor);
}

static void
lemon_gtk_embed_unmap (ClutterActor *actor)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (actor);
  LemonGtkEmbedPrivate *priv = lemon_gtk_embed_get_instance_private (embed);

  _lemon_embedded_window_unmap (priv->window);

  CLUTTER_ACTOR_CLASS (lemon_gtk_embed_parent_class)->unmap (actor);
}

static void
lemon_gtk_embed_dispose (GObject *object)
{
  LemonGtkEmbed *embed = LEMON_GTK_EMBED (object);

  G_OBJECT_CLASS (lemon_gtk_embed_parent_class)->dispose (object);

  lemon_gtk_embed_set_window (embed, NULL);
}

static void
lemon_gtk_embed_class_init (LemonGtkEmbedClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  object_class->get_property = lemon_gtk_embed_get_property;
  object_class->set_property = lemon_gtk_embed_set_property;
  object_class->dispose      = lemon_gtk_embed_dispose;

  actor_class->get_preferred_width = lemon_gtk_embed_get_preferred_width;
  actor_class->get_preferred_height = lemon_gtk_embed_get_preferred_height;
  actor_class->allocate = lemon_gtk_embed_allocate;
  actor_class->map = lemon_gtk_embed_map;
  actor_class->unmap = lemon_gtk_embed_unmap;

  g_object_class_install_property (object_class,
                                   PROP_WINDOW,
                                   g_param_spec_object ("window",
                                                        "Window",
                                                        "LemonEmbeddedWindow to embed",
                                                        LEMON_TYPE_EMBEDDED_WINDOW,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void
lemon_gtk_embed_init (LemonGtkEmbed *embed)
{
}

/*
 * Public API
 */
ClutterActor *
lemon_gtk_embed_new (LemonEmbeddedWindow *window)
{
  g_return_val_if_fail (LEMON_IS_EMBEDDED_WINDOW (window), NULL);

  return g_object_new (LEMON_TYPE_GTK_EMBED,
                       "window", window,
                       NULL);
}

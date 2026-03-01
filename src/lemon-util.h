/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#ifndef __LEMON_UTIL_H__
#define __LEMON_UTIL_H__

#include <gio/gio.h>
#include <clutter/clutter.h>
#include <meta/meta-window-actor.h>

G_BEGIN_DECLS

char    *lemon_util_get_label_for_uri          (const char       *text_uri);
GIcon   *lemon_util_get_icon_for_uri           (const char       *text_uri);

void     lemon_util_set_hidden_from_pick       (ClutterActor     *actor,
                                                gboolean          hidden);

void     lemon_util_get_transformed_allocation (ClutterActor     *actor,
                                                ClutterActorBox  *box);

int      lemon_util_get_week_start             (void);

char    *lemon_util_format_date                (const char       *format,
                                                gint64            time_ms);

ClutterModifierType
         lemon_get_event_state                 (ClutterEvent     *event);

gboolean lemon_write_string_to_stream          (GOutputStream    *stream,
                                                const char       *str,
                                                GError          **error);

char    *lemon_get_file_contents_utf8_sync     (const char       *path,
                                                GError          **error);
/**
 * LemonFileContentsCallback:
 * @utf8_contents: The contents of the file
 * @user_data: (closure): Data passed to lemon_get_file_contents_utf8()
 *
 * Callback type for lemon_get_file_contents_utf8()
 *
 * Since: 3.1
 */
typedef void (* LemonFileContentsCallback) (const gchar *utf8_contents,
                                               gpointer     user_data);

void     lemon_get_file_contents_utf8         (const char                   *path,
                                                  LemonFileContentsCallback  callback,
                                                  gpointer                      user_data);

ClutterContent * lemon_util_get_content_for_window_actor (MetaWindowActor *window_actor,
                                                             MetaRectangle   *window_rect);

cairo_surface_t * lemon_util_composite_capture_images (ClutterCapture  *captures,
                                                          int              n_captures,
                                                          int              x,
                                                          int              y,
                                                          int              target_width,
                                                          int              target_height,
                                                          float            target_scale);

void     lemon_breakpoint                      (void);

gboolean lemon_parse_search_provider           (const char       *data,
                                                char            **name,
                                                char            **url,
                                                GList           **langs,
                                                char            **icon_data_uri,
                                                GError          **error);

void lemon_shader_effect_set_double_uniform (ClutterShaderEffect *effect,
                                             const gchar         *name,
                                             gdouble             value);

gboolean lemon_util_wifexited (int               status,
                                  int              *exit);

G_END_DECLS

#endif /* __LEMON_UTIL_H__ */

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_RECORDER_H__
#define __LEMON_RECORDER_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

/**
 * SECTION:LemonRecorder
 * short_description: Record from a #ClutterStage
 *
 * The #LemonRecorder object is used to make recordings ("screencasts")
 * of a #ClutterStage. Recording is done via #GStreamer. The default is
 * to encode as a Theora movie and write it to a file in the current
 * directory named after the date, but the encoding and output can
 * be configured.
 */
typedef struct _LemonRecorder      LemonRecorder;
typedef struct _LemonRecorderClass LemonRecorderClass;

#define LEMON_TYPE_RECORDER              (lemon_recorder_get_type ())
#define LEMON_RECORDER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_RECORDER, LemonRecorder))
#define LEMON_RECORDER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_RECORDER, LemonRecorderClass))
#define LEMON_IS_RECORDER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_RECORDER))
#define LEMON_IS_RECORDER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_RECORDER))
#define LEMON_RECORDER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_RECORDER, LemonRecorderClass))

GType              lemon_recorder_get_type     (void) G_GNUC_CONST;

LemonRecorder     *lemon_recorder_new (ClutterStage  *stage);

void               lemon_recorder_set_framerate (LemonRecorder *recorder,
                                                    int framerate);
void               lemon_recorder_set_file_template (LemonRecorder *recorder,
                                                        const char    *file_template);
void               lemon_recorder_set_pipeline (LemonRecorder *recorder,
                                                   const char    *pipeline);
void               lemon_recorder_set_draw_cursor (LemonRecorder *recorder,
                                                      gboolean       draw_cursor);
void lemon_recorder_set_area (LemonRecorder *recorder,
                                 int               x,
                                 int               y,
                                 int               width,
                                 int               height);
gboolean           lemon_recorder_record       (LemonRecorder  *recorder,
                                                   char          **filename_used);
void               lemon_recorder_close        (LemonRecorder *recorder);
gboolean           lemon_recorder_is_recording (LemonRecorder *recorder);

G_END_DECLS

#endif /* __LEMON_RECORDER_H__ */

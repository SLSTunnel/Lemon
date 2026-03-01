/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_RECORDER_SRC_H__
#define __LEMON_RECORDER_SRC_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * LemonRecorderSrc:
 *
 * lemonrecordersrc a custom source element is pretty much like a very
 * simple version of the stander GStreamer 'appsrc' element, without
 * any of the provisions for seeking, generating data on demand,
 * etc. In both cases, the application supplies the buffers and the
 * element pushes them into the pipeline. The main reason for not using
 * appsrc is that it wasn't a supported element until gstreamer 0.10.22,
 * and as of 2009-03, many systems still have 0.10.21.
 */
typedef struct _LemonRecorderSrc      LemonRecorderSrc;
typedef struct _LemonRecorderSrcClass LemonRecorderSrcClass;

#define LEMON_TYPE_RECORDER_SRC              (lemon_recorder_src_get_type ())
#define LEMON_RECORDER_SRC(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_RECORDER_SRC, LemonRecorderSrc))
#define LEMON_RECORDER_SRC_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_RECORDER_SRC, LemonRecorderSrcClass))
#define LEMON_IS_RECORDER_SRC(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_RECORDER_SRC))
#define LEMON_IS_RECORDER_SRC_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_RECORDER_SRC))
#define LEMON_RECORDER_SRC_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_RECORDER_SRC, LemonRecorderSrcClass))

GType              lemon_recorder_src_get_type     (void) G_GNUC_CONST;

void lemon_recorder_src_register (void);

void lemon_recorder_src_add_buffer (LemonRecorderSrc *src,
				    GstBuffer        *buffer);
void lemon_recorder_src_close      (LemonRecorderSrc *src);

G_END_DECLS

#endif /* __LEMON_RECORDER_SRC_H__ */

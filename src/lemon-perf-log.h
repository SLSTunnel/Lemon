/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_PERF_LOG_H__
#define __LEMON_PERF_LOG_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _LemonPerfLog LemonPerfLog;
typedef struct _LemonPerfLogClass LemonPerfLogClass;

#define LEMON_TYPE_PERF_LOG              (lemon_perf_log_get_type ())
#define LEMON_PERF_LOG(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_PERF_LOG, LemonPerfLog))
#define LEMON_PERF_LOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_PERF_LOG, LemonPerfLogClass))
#define LEMON_IS_PERF_LOG(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_PERF_LOG))
#define LEMON_IS_PERF_LOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_PERF_LOG))
#define LEMON_PERF_LOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_PERF_LOG, LemonPerfLogClass))

GType lemon_perf_log_get_type (void) G_GNUC_CONST;

LemonPerfLog *lemon_perf_log_get_default (void);

void lemon_perf_log_set_enabled (LemonPerfLog *perf_log,
				 gboolean      enabled);

void lemon_perf_log_define_event (LemonPerfLog *perf_log,
				  const char   *name,
				  const char   *description,
				  const char   *signature);
void lemon_perf_log_event        (LemonPerfLog *perf_log,
				  const char   *name);
void lemon_perf_log_event_i      (LemonPerfLog *perf_log,
				  const char   *name,
				  gint32        arg);
void lemon_perf_log_event_x      (LemonPerfLog *perf_log,
				  const char   *name,
				  gint64        arg);
void lemon_perf_log_event_s      (LemonPerfLog *perf_log,
				  const char   *name,
				  const char   *arg);

void lemon_perf_log_define_statistic (LemonPerfLog *perf_log,
                                      const char   *name,
                                      const char   *description,
                                      const char   *signature);

void lemon_perf_log_update_statistic_i (LemonPerfLog *perf_log,
                                        const char   *name,
                                        int           value);
void lemon_perf_log_update_statistic_x (LemonPerfLog *perf_log,
                                        const char   *name,
                                        gint64        value);

typedef void (*LemonPerfStatisticsCallback) (LemonPerfLog *perf_log,
                                             gpointer      data);

void lemon_perf_log_add_statistics_callback (LemonPerfLog               *perf_log,
                                             LemonPerfStatisticsCallback callback,
                                             gpointer                    user_data,
                                             GDestroyNotify              notify);

void lemon_perf_log_collect_statistics (LemonPerfLog *perf_log);

typedef void (*LemonPerfReplayFunction) (gint64      time,
					 const char *name,
					 const char *signature,
					 GValue     *arg,
                                         gpointer    user_data);

void lemon_perf_log_replay (LemonPerfLog            *perf_log,
			    LemonPerfReplayFunction  replay_function,
                            gpointer                 user_data);

gboolean lemon_perf_log_dump_events (LemonPerfLog   *perf_log,
                                     GOutputStream  *out,
                                     GError        **error);
gboolean lemon_perf_log_dump_log    (LemonPerfLog   *perf_log,
                                     GOutputStream  *out,
                                     GError        **error);

G_END_DECLS

#endif /* __LEMON_PERF_LOG_H__ */

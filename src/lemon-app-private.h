/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_APP_PRIVATE_H__
#define __LEMON_APP_PRIVATE_H__

#include "lemon-app.h"
#include "lemon-app-system.h"

G_BEGIN_DECLS

LemonApp* _lemon_app_new_for_window (MetaWindow *window);

LemonApp* _lemon_app_new (GMenuTreeEntry *entry);

void _lemon_app_set_entry (LemonApp *app, GMenuTreeEntry *entry);

void _lemon_app_handle_startup_sequence (LemonApp *app, MetaStartupSequence *sequence);

void _lemon_app_add_window (LemonApp *app, MetaWindow *window);

void _lemon_app_remove_window (LemonApp *app, MetaWindow *window);

void _lemon_app_do_match (LemonApp         *app,
                          GSList           *terms,
                          GSList          **prefix_results,
                          GSList          **substring_results);
const char * _lemon_app_get_common_name (LemonApp *app);
void         _lemon_app_set_unique_name (LemonApp *app, gchar *unique_name);
const char * _lemon_app_get_unique_name (LemonApp *app);
const char * _lemon_app_get_executable (LemonApp *app);
const char * _lemon_app_get_desktop_path (LemonApp *app);
void         _lemon_app_set_hidden_as_duplicate (LemonApp *app, gboolean hide);
G_END_DECLS

#endif /* __LEMON_APP_PRIVATE_H__ */

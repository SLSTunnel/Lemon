/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_WINDOW_TRACKER_PRIVATE_H__
#define __LEMON_WINDOW_TRACKER_PRIVATE_H__

#include "lemon-window-tracker.h"

void _lemon_window_tracker_add_child_process_app (LemonWindowTracker *tracker,
                                                  GPid                pid,
                                                  LemonApp           *app);

#endif

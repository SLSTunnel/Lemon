/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_SCREENSHOT_H__
#define __LEMON_SCREENSHOT_H__

/**
 * SECTION:lemon-screenshot
 * @short_description: Grabs screenshots of areas and/or windows
 *
 * The #LemonScreenshot object is used to take screenshots of screen
 * areas or windows and write them out as png files.
 *
 */

typedef struct _LemonScreenshot      LemonScreenshot;
typedef struct _LemonScreenshotClass LemonScreenshotClass;

#define LEMON_TYPE_SCREENSHOT              (lemon_screenshot_get_type ())
#define LEMON_SCREENSHOT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), LEMON_TYPE_SCREENSHOT, LemonScreenshot))
#define LEMON_SCREENSHOT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), LEMON_TYPE_SCREENSHOT, LemonScreenshotClass))
#define LEMON_IS_SCREENSHOT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), LEMON_TYPE_SCREENSHOT))
#define LEMON_IS_SCREENSHOT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LEMON_TYPE_SCREENSHOT))
#define LEMON_SCREENSHOT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), LEMON_TYPE_SCREENSHOT, LemonScreenshotClass))

GType lemon_screenshot_get_type (void) G_GNUC_CONST;

LemonScreenshot *lemon_screenshot_new (void);

typedef void (*LemonScreenshotCallback)  (LemonScreenshot *screenshot,
                                           gboolean success,
                                           cairo_rectangle_int_t *screenshot_area);

typedef void (*LemonScreenshotPickColorCallback)  (LemonScreenshot *screenshot,
                                                      gboolean success,
                                                      ClutterColor *color);

void    lemon_screenshot_screenshot_area      (LemonScreenshot *screenshot,
                                                gboolean include_cursor,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                const char *filename,
                                                LemonScreenshotCallback callback);

void    lemon_screenshot_screenshot_window    (LemonScreenshot *screenshot,
                                                gboolean include_frame,
                                                gboolean include_cursor,
                                                const char *filename,
                                                LemonScreenshotCallback callback);

void    lemon_screenshot_screenshot           (LemonScreenshot *screenshot,
                                                gboolean include_cursor,
                                                const char *filename,
                                                LemonScreenshotCallback callback);

void    lemon_screenshot_pick_color         (LemonScreenshot *screenshot,
                                                int x,
                                                int y,
                                                LemonScreenshotPickColorCallback callback);

#endif /* ___LEMON_SCREENSHOT_H__ */

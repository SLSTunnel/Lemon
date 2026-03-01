/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __LEMON_GLSL_EFFECT_H__
#define __LEMON_GLSL_EFFECT_H__

#include "st.h"
#include <gtk/gtk.h>

/**
 * LemonSnippetHook:
 * Temporary hack to work around Cogl not exporting CoglSnippetHook in
 * the 1.0 API. Don't use.
 */
typedef enum {
  /* Per pipeline vertex hooks */
  LEMON_SNIPPET_HOOK_VERTEX = 0,
  LEMON_SNIPPET_HOOK_VERTEX_TRANSFORM,

  /* Per pipeline fragment hooks */
  LEMON_SNIPPET_HOOK_FRAGMENT = 2048,

  /* Per layer vertex hooks */
  LEMON_SNIPPET_HOOK_TEXTURE_COORD_TRANSFORM = 4096,

  /* Per layer fragment hooks */
  LEMON_SNIPPET_HOOK_LAYER_FRAGMENT = 6144,
  LEMON_SNIPPET_HOOK_TEXTURE_LOOKUP
} LemonSnippetHook;

#define LEMON_TYPE_GLSL_EFFECT (lemon_glsl_effect_get_type ())
G_DECLARE_DERIVABLE_TYPE (LemonGLSLEffect, lemon_glsl_effect, LEMON, GLSL_EFFECT, ClutterOffscreenEffect)

struct _LemonGLSLEffectClass
{
  ClutterOffscreenEffectClass parent_class;

  CoglPipeline *base_pipeline;

  void (*build_pipeline) (LemonGLSLEffect *effect);
};

void lemon_glsl_effect_add_glsl_snippet (LemonGLSLEffect  *effect,
                                            LemonSnippetHook  hook,
                                            const char          *declarations,
                                            const char          *code,
                                            gboolean             is_replace);

int  lemon_glsl_effect_get_uniform_location (LemonGLSLEffect *effect,
                                                const char         *name);
void lemon_glsl_effect_set_uniform_float    (LemonGLSLEffect *effect,
                                                int                 uniform,
                                                int                 n_components,
                                                int                 total_count,
                                                const float        *value);

#endif /* __LEMON_GLSL_EFFECT_H__ */

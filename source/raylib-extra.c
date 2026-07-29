/* -*- c-basic-offset: 2; indent-tabs-mode: nil; -*- */
#include "raylib-extra.h"
#include <chad/utils.h>
#include <stdlib.h>
#include <stddef.h>

static bool internal_window_open = 0;
Font DefaultFont = {};

// EM NOTE: I am still not a fan of the Pascal naming in anything ever.
// the translator in meat DOES work, and we could use it verbatim.
// Note that this file is written in Emilism, forgive me.

// Should just be in Raylib:

bool
RL_EXTRA_PREFIX(InitWindowV)(const v2 area_maybe, const char * name)
#undef InitWindow
{
  InitWindow(area_maybe.x, area_maybe.y, name);
  return (internal_window_open = IsWindowReady());
}

bool
RL_EXTRA_PREFIX(InitAudioDevice2)(void) {
  InitAudioDevice();
  return IsAudioDeviceReady();
}

RenderTexture2D
RL_EXTRA_PREFIX(LoadRenderTextureV)(const v2 area) {
  return LoadRenderTexture(area.x, area.y);
}

v2
RL_EXTRA_PREFIX(GetRenderArea)(void) {
  return (v2) { (float) GetRenderWidth(), (float) GetRenderHeight() };
}

bool
RL_EXTRA_PREFIX(WindowOpen)(void)
#undef WindowShouldClose
{
  internal_window_open &= !WindowShouldClose();
  return internal_window_open;
}

void
RL_EXTRA_PREFIX(PleaseCloseWindow)(void) {
  internal_window_open = 0;
}

int
RL_EXTRA_PREFIX(SafeGetShaderLocation)(Shader shader, const char * uniform_name) {
  int location = QUOTE(GetShaderLocation)(shader, uniform_name);

  if (location < 0) {
    TraceLog(LOG_FATAL, TextFormat("shader uniform not found: %s\n", uniform_name));
    abort();
  }

  return location;
}

void
RL_EXTRA_PREFIX(DrawTextAlt)(const char * text, float posX, float posY, int fontSize, Color color) {
  if (DefaultFont.glyphs == NULL) DefaultFont = GetFontDefault();
  DrawTextEx(DefaultFont, text, (v2) {posX, posY}, fontSize, 1, color);
}

void BeginScissorModeRec(Rectangle r) {
  // who fucking designed this shit
  BeginScissorMode((int)r.x, (int)r.y, (int)r.width, (int)r.height);
}

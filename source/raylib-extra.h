#ifndef RAYLIB_EXTRA_H_
#define RAYLIB_EXTRA_H_
/* Stapling ontop raylib.
 * Consistency functions, biases, and extras.
 */

#include <raylib.h>
#include <raymath.h>
#include <chad/utils.h>

// debug
#define debug_print_rectangle(rect) \
    printf(STRINGIFY(rect) " x %f y %f width %f height %f\n", rect.x, rect.y, rect.width, rect.height)
#define debug_print_v2(evv) printf(STRINGIFY(Ev2) " x %f y %f\n", evv.x, evv.y)
#define debug_print_v3(evv) printf(STRINGIFY(Ev3) " x %f y %f z %f\n", evv.x, evv.y, evv.z)
#define debug_print_v4(evv) printf(STRINGIFY(Ev4) " x %f y %f z %f w %f\n", evv.x, evv.y, evv.z, evv.w)

// Enables PleaseCloseWindow works correctly.
// WindowOpen will return 0 if this is bypassed as the internval would not be set.
#define InitWindow(x, y, title) InitWindowV((v2) { x, y }, title)
#define WindowShouldClose() !WindowOpen()

#define RL_EXTRA_PREFIX(x) x // very, very important to me spiritually

typedef Vector2 v2;
typedef Vector3 v3;
typedef Vector4 v4;

static inline Rectangle
v4_rect(v4 v) {
  return (Rectangle) { v.x, v.y, v.z, v.w };
}
static inline v4
rect_v4(Rectangle v) {
  return (v4) { v.x, v.y, v.width, v.height };
}

#define GREY GRAY

// Return value of whether windowing/raylib is ready
// Additionally allows PleaseCloseWindow()
bool RL_EXTRA_PREFIX(InitWindowV)(const v2 area_maybe, const char * name);
// Return value of whether audio is ready
bool RL_EXTRA_PREFIX(InitAudioDevice2)(void);
// Load Render Texture with v2
RenderTexture2D RL_EXTRA_PREFIX(LoadRenderTextureV)(const v2 area);
// Same as GetRenderWidth, GetRenderHeight
v2 RL_EXTRA_PREFIX(GetRenderArea)(void);
// !WindowShouldClose() with support for PleaseCloseWindow()
bool RL_EXTRA_PREFIX(WindowOpen)(void); // Inverse of WindowShouldClose
// Request the window should close
void RL_EXTRA_PREFIX(PleaseCloseWindow)(void);
// Abort when shader binding is clearly wrong
#define GetShaderLocation(...) USE_SafeGetShaderLocation_RETARD
int RL_EXTRA_PREFIX(SafeGetShaderLocation)(Shader shader, const char * uniform_name);

void RL_EXTRA_PREFIX(DrawTextAlt)(const char * text, float posX, float posY, int fontSize, Color color);

#define DrawText(...) DrawTextAlt(__VA_ARGS__)
extern Font DefaultFont;

void BeginScissorModeRec(Rectangle rec);

// Conversions are covered by C++ *somewhat nightmarishly!*

#endif // RAYLIB_EXTRA_H_

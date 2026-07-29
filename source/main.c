/* WARNING: this code may be obscuring its TRUE length!!!!!
 * REMAIN SAFE,
 * KEEP CALM,
 * AND KEEP
 * TABULATED.
 */

#include <stdio.h>
#include <stdlib.h>
#include <chad/random/grand.h>
#include <chad/change_directory.h>
#include <chad/utils.h>
#include <chad/terry.h>
#include <raylib.h>
#include "raylib-extra.h"

Font g_font;

bool is_dev = DEBUG;

Rectangle current_display_shape(void)
{ int monitor = GetCurrentMonitor();
  return (Rectangle) { 0, 0, (float) GetMonitorWidth(monitor), (float) GetMonitorHeight(monitor) };
}

static void stderr_log_callback(int log_level, const char * text, va_list args)
{ vfprintf(stderr, text, args);
  fputc('\n', stderr);
}

v2 raylib_init(const char * title)
{ SetTraceLogLevel(is_dev ? LOG_ALL : LOG_WARNING);
  SetTraceLogCallback(stderr_log_callback);

  Rectangle screen[1] = (Rectangle[1]){0, 0, 1920, 1080};

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

  InitWindow(screen->width, screen->height, title);
  SetWindowMinSize(800, 600);
  *screen = current_display_shape();
  SetWindowSize(screen->width, screen->height);
  screen->width = GetRenderWidth();
  screen->height = GetRenderHeight();
  SetTargetFPS(60); // but the 60 per second over the course of 100'000 seconds!! how horrific!

  g_font = LoadFont("resource/atkinson-mono.ttf");

  if (!IsFontValid(g_font)) { g_font = GetFontDefault(); }

  DefaultFont = g_font;

  return (v2) { screen->x, screen->y };
}

void
raylib_deinit(void) { CloseWindow(); }

enum {
  BOAT,
  ISLAND,
  WATER_1,
  BULLET,
  CANNON,
  EXPLOSION,
  textures,
};

#define islands 64
#define decals 255
#define bullets 128
#define players 4
typedef struct game_t
{ RenderTexture render_texture[1];
  v2 screen[1], scaled[1], margin[1];
  size_t player_count;
  u32 fc;
  /* v3 = x y pos, z scale */
  v3 island[islands][1];
  /* v3 = x y pos, z deg */
  v3 decal[decals][1];
  v3 bullet[players][bullets][1];
  v3 bullet_velocity[players][bullets][1];
  float bullet_hypot[players][bullets][1];
  v3 player[players][1];
  v2 player_velocity[players][1];
  int player_health[players][1];
  int player_invuln[players][1];
  float player_last_shot[players][1];
  v2 direction[players][1];
  f32 turnspeed[players][1];
  Texture texture[textures][1];
} game_t;

Color color[players] =
  { RED,
    YELLOW,
    PINK,
    ORANGE,
  };

int load_textures(struct game_t * game)
{ const char * resource[textures] =
  { "resource/boat.png",
    "resource/island.png",
    "resource/decal/water1.png",
    "resource/bullet.png",
    "resource/cannon.png",
    "resource/explosion.png",
  };
  int s = 0;
  for (size_t i = 0; i < textures; ++i)
  { *game->texture[i] = LoadTexture(resource[i]);
    if (!IsTextureValid(*game->texture[i]))
    { fprintf(stderr, "Failed to load: '%s'\n", resource[i]);
      s = 1;
    }
  }
  return s;
}

void start(game_t * game)
{ size_t i, j;
  for (i = 0; i < islands; ++i) *game->island[i] = (v3) { grand_f64() * game->screen->x, grand_f64() * game->screen->y, grand_range_f64(10, 40) };
  for (i = 0; i <  decals; ++i) *game->decal [i] = (v3) { grand_f64() * game->screen->x * 1.2 - game->screen->x * 0.2, grand_f64() * game->screen->y * 1.2 - game->screen->y * 0.2, grand_f64() * 360 };
  for (i = 0; i < players; ++i)
  { for (j = 0; j < bullets; ++j)
    { *game->bullet[i][j] = (v3) {0};
      *game->bullet_velocity[i][j] = (v3) {0};
      *game->bullet_hypot[i][j] = 0;
    }
    *game->player[i] = (v3) { grand_f64() * (game->screen->x - game->margin->x) + game->margin->x/2, grand_f64() * (game->screen->y - game->margin->y) + game->margin->y/2, grand_range_f64(0,  360) };
    *game->player_health[i] = 3;
    *game->player_invuln[i] = 0;
    *game->player_last_shot[i] = 10;
  }
}

Rectangle texture_shape(Texture * texture) {
  return (Rectangle) { 0, 0, texture->width, texture->height };
}

Rectangle rectangle_wh_invert(Rectangle * rectangle) {
  return (Rectangle) { rectangle->x, rectangle->y, -rectangle->width, -rectangle->height };
}

void
DrawCentered(Texture * texture, Rectangle source, Rectangle dest, float degrees, Color color)
{ Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
  DrawTexturePro(*texture, source, dest, origin, degrees, color);
}

Vector2
WrapOffset(Vector2 center, float hw, float hh, Rectangle area)
{ float ox = center.x - hw < area.x             ? area.width
           : center.x + hw > area.x + area.width ? -area.width
           : 0;
  float oy = center.y - hh < area.y              ? area.height
           : center.y + hh > area.y + area.height ? -area.height
           : 0;
  return (Vector2){ ox, oy };
}

void
DrawCenteredWrapped(Texture * texture, Rectangle source, Rectangle dest, Rectangle area, float degrees, Color color)
{ float rad = degrees * (PI / 180.0f);
  float c  = fabsf(cosf(rad));
  float s  = fabsf(sinf(rad));
  float hw = dest.width  / 2.0f;
  float hh = dest.height / 2.0f;
  float mx = hw * c + hh * s;   /* rotated half-width  */
  float my = hw * s + hh * c;   /* rotated half-height */
  Vector2 o = WrapOffset((Vector2){ dest.x, dest.y }, mx, my, area);

                  DrawCentered(texture, source, dest, degrees, color);
  if (o.x)        DrawCentered(texture, source, (Rectangle){ dest.x + o.x, dest.y,       dest.width, dest.height }, degrees, color);
  if (o.y)        DrawCentered(texture, source, (Rectangle){ dest.x,       dest.y + o.y, dest.width, dest.height }, degrees, color);
  if (o.x && o.y) DrawCentered(texture, source, (Rectangle){ dest.x + o.x, dest.y + o.y, dest.width, dest.height }, degrees, color);
}

void
DrawCircleWrapped(Vector2 center, float radius, Rectangle area, Color color)
{ Vector2 o = WrapOffset(center, radius, radius, area);

                  DrawCircleV(center, radius, color);
  if (o.x)        DrawCircleV((Vector2){ center.x + o.x, center.y       }, radius, color);
  if (o.y)        DrawCircleV((Vector2){ center.x,       center.y + o.y }, radius, color);
  if (o.x && o.y) DrawCircleV((Vector2){ center.x + o.x, center.y + o.y }, radius, color);
}

void update_water_decals(game_t * game)
{ for (size_t i = 0; i < decals; ++i) game->decal[i]->z += grand_f64(); }

void draw_water_decals(game_t * game)
{ for (size_t i = 0; i < decals; ++i)
  { Rectangle dest = (Rectangle) { sinf(game->fc * 0.01) * 100 + game->decal[i]->x, cosf(game->fc * 0.01) * 100 + game->decal[i]->y, game->texture[WATER_1]->width + 40 * cosf(game->fc * 0.001), game->texture[WATER_1]->height + 40 * sinf(game->fc * 0.01) };
    Rectangle shape = texture_shape(game->texture[WATER_1]); /* this is a cope */
    v2 origin = (v2) { game->texture[WATER_1]->width * 0.5, game->texture[WATER_1]->height * 0.5 }; /* I am tormented by demons day and night */
    Rectangle screen = (Rectangle) { 0, 0, game->screen->x, game->screen->y };
    DrawCenteredWrapped(game->texture[WATER_1], shape, dest, screen, game->decal[i]->z, (Color) {129, 229, 230, grand_range_f64(100,180) } );
    dest.x += 20;
    dest.y += 20;
    DrawCenteredWrapped(game->texture[WATER_1], rectangle_wh_invert(&shape), dest, screen, game->decal[i]->z, (Color) {139, 239, 240, grand_range_f64(120,200) } );
  }
}

void draw_islands(game_t * game)
{ size_t i;
  for (i = 0; i < islands; ++i) DrawCircleWrapped((v2) { game->island[i]->x, game->island[i]->y }, game->island[i]->z, (Rectangle) { 0, 0, game->screen->x, game->screen->y }, ColorAlpha(GREY, 0.5));
  for (i = 0; i < islands; ++i) DrawCenteredWrapped(game->texture[ISLAND], texture_shape(game->texture[ISLAND]), (Rectangle) { game->island[i]->x, game->island[i]->y, game->island[i]->z * 2, game->island[i]->z * 2 }, (Rectangle) { 0, 0, game->screen->x, game->screen->y }, 0, GREEN);
}

void wrap(v2 * position, Rectangle * screen) {
  if (position->x > screen->width)  position->x -= screen->width;
  if (position->x < 0)              position->x += screen->width;
  if (position->y > screen->height) position->y -= screen->height;
  if (position->y < 0)              position->y += screen->height;
}

#define BOAT_RADIUS 6.0f  /* tune to your sprite; assumes a roughly circular hull */

static void
bounce_off_islands(game_t * game)
{ for (size_t i = 0; i < game->player_count; ++i)
  { size_t j;
    float heading = DEG2RAD * game->player[i]->z;
    float vx = cosf(heading) * game->player_velocity[i]->y;
    float vy = sinf(heading) * game->player_velocity[i]->y;

    for (j = 0; j < islands; ++j)
    { float dx = game->player[i]->x - game->island[j]->x;
      float dy = game->player[i]->y - game->island[j]->y;
      float min_dist = game->island[j]->z + BOAT_RADIUS;
      float dist_sq = dx * dx + dy * dy;
      float dist, nx, ny, overlap, vn;

      if (dist_sq >= min_dist * min_dist || dist_sq == 0.0f) continue;

      dist = sqrtf(dist_sq);
      nx = dx / dist;
      ny = dy / dist;
      overlap = min_dist - dist;
      vn = vx * nx + vy * ny;

      game->player[i]->x += nx * overlap;
      game->player[i]->y += ny * overlap;

      if (vn < 0.0f)
      { vx -= 2.0f * vn * nx;
        vy -= 2.0f * vn * ny;
      }
    }

    game->player_velocity[i]->y = sqrtf(vx * vx + vy * vy);
    game->player[i]->z   = atan2f(vy, vx) * RAD2DEG;
  }
}

void update_bullets(game_t * game)
{ Rectangle screen = (Rectangle) { 0, 0, game->screen->x, game->screen->y };
  for (size_t i = 0; i < players; ++i)
  { for (size_t j = 0; j < bullets; ++j)
    { game->bullet[i][j]->x += game->bullet_velocity[i][j]->x * cosf(DEG2RAD * game->bullet[i][j]->z) - game->bullet_velocity[i][j]->y * sinf(DEG2RAD * game->bullet[i][j]->z);
      game->bullet[i][j]->y += game->bullet_velocity[i][j]->x * sinf(DEG2RAD * game->bullet[i][j]->z) + game->bullet_velocity[i][j]->y * cosf(DEG2RAD * game->bullet[i][j]->z);
      game->bullet[i][j]->z += game->bullet_velocity[i][j]->z;
      *game->bullet_hypot[i][j] = hypot(game->bullet_velocity[i][j]->x, game->bullet_velocity[i][j]->y);
      game->bullet_velocity[i][j]->x *= 0.99;
      game->bullet_velocity[i][j]->y *= 0.99;
      game->bullet_velocity[i][j]->z *= 0.99;
    }
    wrap((v2*)game->bullet[i], &screen);
  }
}

void draw_bullets(game_t * game)
{ float radius = 5;
  for (size_t i = 0; i < players; ++i) for (size_t j = 0; j < bullets; ++j)
  { *game->bullet_hypot[i][j] = hypot(game->bullet_velocity[i][j]->x, game->bullet_velocity[i][j]->y);
    float alpha = *game->bullet_hypot[i][j] > 0.5 ? 255 : *game->bullet_hypot[i][j] * 2 + (*game->bullet_hypot[i][j] - 0.5); /* seemless fadeout */
    if (*game->bullet_hypot[i][j] > 0.1) DrawCenteredWrapped(game->texture[BULLET], texture_shape(game->texture[BULLET]), (Rectangle) { game->bullet[i][j]->x, game->bullet[i][j]->y, radius * 2, radius * 2 }, (Rectangle) { 0, 0, game->screen->x, game->screen->y }, 0, ColorAlpha(color[i], alpha));
  }
}

/* what do you mean it won't support inserting seventy megabullets per angstronit on my toaster from '89? */
/* silently does nothing if we run out of memory */
void insert_bullet(game_t * game, int pi, v3 position, v3 velocity)
{ size_t lowest_index = 0, lowest_value = 1000;
  for (size_t j = 0; j < bullets; ++j) {
    if (*game->bullet_hypot[pi][j] < 0.1)
    { lowest_index = j;
      break;
    }
    if (lowest_value > *game->bullet_hypot[pi][j])
    { lowest_value = *game->bullet_hypot[pi][j];
      lowest_index = j;
    }
  }
  *game->bullet[pi][lowest_index] = position;
  *game->bullet_velocity[pi][lowest_index] = velocity;
  *game->bullet_hypot[pi][lowest_index] = hypot(game->bullet_velocity[pi][lowest_index]->x, game->bullet_velocity[pi][lowest_index]->y);
}

void update_players(game_t * game)
{ size_t i, j, k;
  int keys[players][5] =
    { { KEY_W,       KEY_S,    KEY_A,     KEY_D, KEY_E },
      { KEY_I,       KEY_K,    KEY_J,     KEY_L, KEY_O },
      { KEY_UP,   KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER },
      { KEY_KP_8, KEY_KP_2, KEY_KP_4,  KEY_KP_6, KEY_KP_ENTER },
    };
  v2 speed[1]  = { 0.35, 0.2 };
  v2 dampen[1] = { 0.91, 0.97 };
  /* -- */
  v3 bullet_speed = (v3) {5, 5, 0};
  Rectangle screen = (Rectangle) { 0, 0, game->screen->x, game->screen->y };
  #define nullcancel(player_index, axis, prograde, retrograde) do \
  { if (IsKeyPressed (prograde)                                             ) game->direction[player_index]->axis = 1; \
    if (IsKeyReleased(prograde)   && game->direction[player_index]->axis > 0) game->direction[player_index]->axis = IsKeyDown(retrograde) ? -1 : 0; \
    if (IsKeyPressed (retrograde)                                           ) game->direction[player_index]->axis = -1; \
    if (IsKeyReleased(retrograde) && game->direction[player_index]->axis < 0) game->direction[player_index]->axis = IsKeyDown(prograde) ? 1 : 0; \
  } while (0);
  /* player_index is base 1, total Pascal victory */
  #define polar(player_index, north, south, west, east) do \
    { nullcancel(player_index, x, east, west);             \
      nullcancel(player_index, y, north, south);           \
    } while (0);
  for (i = 0; i < game->player_count; ++i)
  { if (*game->player_health[i] == 0) continue;

    polar(i, keys[i][0], keys[i][1], keys[i][2], keys[i][3]);

    game->player_velocity[i]->y += game->direction[i]->y * speed->y;
    game->player_velocity[i]->x += game->direction[i]->x * speed->x;
    game->player_velocity[i]->y *= dampen->y;
    game->player_velocity[i]->x *= dampen->x;

    game->player[i]->z += game->player_velocity[i]->x;
    game->player[i]->x += cosf(DEG2RAD * game->player[i]->z) * game->player_velocity[i]->y;
    game->player[i]->y += sinf(DEG2RAD * game->player[i]->z) * game->player_velocity[i]->y;

    wrap((v2*)game->player[i], &screen);

    if (++(*game->player_last_shot[i]) > 13 && IsKeyPressed(keys[i][4]))
    { v3 rotate = *game->player[i];
      rotate.z += 45;
      insert_bullet(game, i, rotate, bullet_speed);
      rotate.z -= 180;
      insert_bullet(game, i, rotate, bullet_speed);
      *game->player_last_shot[i] = 0;
    }

    if (*game->player_invuln[i] > 0) --*game->player_invuln[i];
    else for (j = 0; j < game->player_count; ++j)
    { if (j == i) continue;
      if (*game->player_health[j] == 0) continue;
      for (k = 0; k < bullets; ++k)
      { if (*game->bullet_hypot[j][k] > 0.2 && CheckCollisionPointRec(*(v2*) game->bullet[j][k], (Rectangle) { game->player[i]->x - 15, game->player[i]->y - 15, 30, 30 }))

        { *game->player_invuln[i] = 60;
          *game->bullet_velocity[j][k] = (v3) {0};
          --*game->player_health[i];
          printf("Hit!\n");
          goto next;
        }
      }
    }
  next:;
  }
  size_t count = 0;
  for (i = 0; i < game->player_count; ++i) count += *game->player_health[i] > 0;
  if (count < 2) { start(game); return; }
  bounce_off_islands(game);
}

void draw_players(game_t * game)
{ Rectangle screen = (Rectangle) { 0, 0, game->screen->x, game->screen->y };
  for (size_t i = 0; i < game->player_count; ++i)
  { Color c = ColorAlpha(color[i], *game->player_invuln[i] % 2 ? 0 : 255);
    Texture * boat = *game->player_invuln[i] > 0 ? game->texture[EXPLOSION] : game->texture[BOAT];
    DrawCircleWrapped((v2) { game->player[i]->x + 5, game->player[i]->y + 5 }, 30, screen, ColorAlpha(GREY, 0.5));
    if (*game->player_health[i] == 0) continue;
    DrawCenteredWrapped(boat, (Rectangle) { 0, 0, game->texture[BOAT]->width, game->texture[BOAT]->height }, (Rectangle) { game->player[i]->x, game->player[i]->y, 70, 70 }, screen, game->player[i]->z + 90, c);
    DrawCenteredWrapped(game->texture[CANNON], (Rectangle) { 0, 0, game->texture[CANNON]->width, game->texture[CANNON]->height }, (Rectangle) { game->player[i]->x - 10, game->player[i]->y, 30, 30 }, screen, game->player[i]->z - 90, c);
    DrawCenteredWrapped(game->texture[CANNON], (Rectangle) { 0, 0, game->texture[CANNON]->width, game->texture[CANNON]->height }, (Rectangle) { game->player[i]->x + 10, game->player[i]->y, 30, 30 }, screen, game->player[i]->z + 90, c);
    /* DrawRectangleRec((Rectangle) { game->player[i]->x - 15, game->player[i]->y - 15, 30, 30 }, RED); */
  }
}

int
main([[maybe_unused]] int ac, char ** av)
{ const char * program_name = av[0];
  change_directory(program_name);

  size_t i;
  game_t game[1] = {0};

  game->player_count = strtol(av[1] ? av[1] : "2", NULL, 10);
  game->player_count = CLAMP(game->player_count, 2, 4);

  *game->screen = (v2) {2000, 1000 };
  *game->margin = (v2) {game->screen->x * 0.1, game->screen->y * 0.1 };
  *game->scaled = raylib_init("Acer-Lata");
  *game->render_texture = LoadRenderTexture(game->screen->x, game->screen->y);

  load_textures(game);
  start(game);

  while (WindowOpen()) {
    ++game->fc;
    // Update
    { /* for (i = 0; i < 4; ++i) if (fabsf(game->direction[i]->y) > 0.1 || fabs(game->direction[i]->x) > 0.1) printf("[+NS, +EW] %f %f\n", game->direction[i]->y, game->direction[i]->x); // test polarity */
      game->scaled->x = GetRenderWidth();
      game->scaled->y = GetRenderHeight();
      if (IsKeyPressed(KEY_F1)) start(game);
      update_water_decals(game);
      update_bullets(game);
      update_players(game);
    }
    // Draw
    {
      BeginDrawing();
      BeginTextureMode(*game->render_texture);
      ClearBackground((Color) {98, 170, 188, 255});
      draw_water_decals(game);
      draw_islands(game);
      draw_players(game);
      draw_bullets(game);
      EndTextureMode();
      DrawTexturePro(game->render_texture->texture, (Rectangle) { 0, 0, game->screen->x, -game->screen->y }, (Rectangle) { 0, 0, game->scaled->x, -game->scaled->y }, (v2) {0}, 0, WHITE);
      EndDrawing();
    }
  }

  raylib_deinit();
  return 0;
}

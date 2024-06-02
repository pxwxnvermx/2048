#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

typedef unsigned char u_char;

#define SQ_SIZE 128
#define PADDING 8
#define BOARD_SIZE 4
#define FONT_SIZE 48

#define SCREEN_W SQ_SIZE *BOARD_SIZE + PADDING
#define SCREEN_H SQ_SIZE *BOARD_SIZE + PADDING

typedef struct {
  int exp;
  int old_x;
  int old_y;
  float frames;
} cell_t;

typedef cell_t world_t[BOARD_SIZE][BOARD_SIZE];

Color ColorLerp(Color a, Color b, float amount) {
  Quaternion lerped = QuaternionLerp(
      (Quaternion){(float)a.r, (float)a.g, (float)a.b, (float)a.a},
      (Quaternion){(float)b.r, (float)b.g, (float)b.b, (float)b.a}, amount);
  return (Color){(u_char)lerped.x, (u_char)lerped.y, (u_char)lerped.z,
                 (u_char)lerped.w};
}

void spawn_cell(world_t *world) {
  for (size_t i = 0; i < BOARD_SIZE; i++) {
    for (size_t j = 0; j < BOARD_SIZE; j++) {
      if (world[i][j]->exp == -1 && GetRandomValue(0, 1) == 1) {
        world[i][j]->exp = 0;
        world[i][j]->old_x = i;
        world[i][j]->old_y = j;
        world[i][j]->frames = 0.0f;
        break;
      }
    }
  }
}

int main() {
  InitWindow(SCREEN_W, SCREEN_H, "2048");
  SetTraceLogLevel(LOG_ALL);
  SetTargetFPS(30);

  // store exponents only
  world_t world;
  for (size_t y = 0; y < BOARD_SIZE; y++) {
    for (size_t x = 0; x < BOARD_SIZE; x++) {
      world[y][x].exp = -1;
      world[y][x].old_x = x;
      world[y][x].old_y = y;
      world[y][x].frames = 0.0f;
    }
  }

  for (size_t i = 0; i < 2; i++) {
    world[GetRandomValue(0, 3)][GetRandomValue(0, 3)].exp = 0;
  }

  float acc = 0.0f;

  while (!WindowShouldClose()) {
    const float delta = GetFrameTime();
    acc += delta;

    if (IsKeyPressed(KEY_DOWN)) {
      for (int i = BOARD_SIZE - 1; i >= 0; i--) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j].exp == -1)
            continue;

          for (size_t k = i; k < BOARD_SIZE - 1; k++) {
            if (world[k + 1][j].exp == -1) {
              world[k + 1][j].exp = world[k][j].exp;
              world[k + 1][j].old_x = k;
              world[k + 1][j].old_y = j;
              world[k + 1][j].frames = 0.0f;
              world[k][j].exp = -1;
            }
            if (world[k][j].exp == world[k + 1][j].exp) {
              world[k + 1][j].exp++;
              world[k + 1][j].old_x = k;
              world[k + 1][j].old_y = j;
              world[k + 1][j].frames = 0.0f;
              world[k][j].exp = -1;
            }
          }
        }
      }

      if (acc > 0.15f) {
        spawn_cell(&world);
        acc = 0.0f;
      }
    } else if (IsKeyPressed(KEY_UP)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j].exp == -1)
            continue;

          for (int k = i; k > 0; k--) {
            if (world[k - 1][j].exp == -1) {
              world[k - 1][j].exp = world[k][j].exp;
              world[k - 1][j].old_x = k;
              world[k - 1][j].old_y = j;
              world[k - 1][j].frames = 0.0f;
              world[k][j].exp = -1;
            }
            if (world[k][j].exp == world[k - 1][j].exp) {
              world[k - 1][j].exp++;
              world[k - 1][j].old_x = k;
              world[k - 1][j].old_y = j;
              world[k - 1][j].frames = 0.0f;
              world[k][j].exp = -1;
            }
          }
        }
      }

      if (acc > 0.15f) {
        spawn_cell(&world);
        acc = 0.0f;
      }
    } else if (IsKeyPressed(KEY_LEFT)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j].exp == -1)
            continue;

          for (int k = j; k > 0; k--) {
            if (world[i][k - 1].exp == -1) {
              world[i][k - 1].exp = world[i][k].exp;
              world[i][k - 1].old_x = i;
              world[i][k - 1].old_y = k;
              world[i][k - 1].frames = 0.0f;
              world[i][k].exp = -1;
            }
            if (world[i][k].exp == world[i][k - 1].exp) {
              world[i][k - 1].exp++;
              world[i][k - 1].old_x = i;
              world[i][k - 1].old_y = k;
              world[i][k - 1].frames = 0.0f;
              world[i][k].exp = -1;
            }
          }
        }
      }

      if (acc > 0.15f) {
        spawn_cell(&world);
        acc = 0.0f;
      }
    } else if (IsKeyPressed(KEY_RIGHT)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (int j = BOARD_SIZE - 1; j > -1; j--) {
          if (world[i][j].exp == -1)
            continue;

          for (size_t k = j; k < BOARD_SIZE - 1; k++) {
            if (world[i][k + 1].exp == -1) {
              world[i][k + 1].exp = world[i][k].exp;
              world[i][k + 1].old_x = i;
              world[i][k + 1].old_y = k;
              world[i][k + 1].frames = 0.0f;
              world[i][k].exp = -1;
            }
            if (world[i][k].exp == world[i][k + 1].exp) {
              world[i][k + 1].exp++;
              world[i][k + 1].old_x = i;
              world[i][k + 1].old_y = k;
              world[i][k + 1].frames = 0.0f;
              world[i][k].exp = -1;
            }
          }
        }
      }

      if (acc > 0.15f) {
        spawn_cell(&world);
        acc = 0.0f;
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (size_t y = 0; y < BOARD_SIZE; y++) {
      for (size_t x = 0; x < BOARD_SIZE; x++) {
        DrawRectangle(x * SQ_SIZE, y * SQ_SIZE, SQ_SIZE + PADDING,
                      SQ_SIZE + PADDING, RAYWHITE);
        cell_t *cell = &world[y][x];
        Color col = world[y][x].exp >= 0 ? ColorLerp(GRAY, RED, world[y][x].exp)
                                         : LIGHTGRAY;
        cell->frames += delta * 10.0f;
        int pos_x = Lerp(cell->old_x * SQ_SIZE, x * SQ_SIZE, cell->frames);
        int pos_y = Lerp(cell->old_y * SQ_SIZE, y * SQ_SIZE, cell->frames);
        cell->old_x = Lerp(cell->old_x, x, cell->frames);
        cell->old_y = Lerp(cell->old_y, y, cell->frames);
        if (cell->frames > 1.0f)
          cell->frames = 0.0f;

        DrawRectangle(pos_x + PADDING / 2, pos_y + PADDING / 2, SQ_SIZE,
                      SQ_SIZE, ColorAlpha(col, 0.3f));

        if (world[y][x].exp > -1) {
          const char *text = TextFormat("%d", 2 << world[y][x].exp);
          size_t font_size = FONT_SIZE;
          size_t text_w = MeasureText(text, font_size);
          while (text_w > SQ_SIZE - PADDING * 2) {
            font_size -= 2;
            text_w = MeasureText(text, font_size);
          }
          DrawText(text, pos_x + SQ_SIZE / 2 - text_w / 2,
                   pos_y + SQ_SIZE / 2 - font_size / 2, font_size, col);
        }
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

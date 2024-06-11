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

void world_spawn_cell(world_t world) {
  for (size_t i = 0; i < BOARD_SIZE; i++) {
    for (size_t j = 0; j < BOARD_SIZE; j++) {
      if (world[i][j].exp == -1 && GetRandomValue(0, 1) == 1) {
        world[i][j].exp = 0;
        world[i][j].old_x = j;
        world[i][j].old_y = i;
        world[i][j].frames = 0.0f;
        break;
      }
    }
  }
}

void world_merge_swap_cell(world_t world, int x, int y, int dir_x, int dir_y,
                           bool *valid_move) {
  cell_t *cell_1 = &world[x][y];
  cell_t *cell_2 = &world[x + dir_x][y + dir_y];
  if (cell_2->exp == -1 || cell_1->exp == cell_2->exp) {
    cell_2->exp = cell_2->exp == -1 ? cell_1->exp : cell_2->exp + 1;
    cell_2->old_x = x;
    cell_2->old_y = y;
    cell_2->frames = 0.0f;
    cell_1->exp = -1;
    *valid_move = true;
  }
}

int main() {
  InitWindow(SCREEN_W, SCREEN_H, "2048");
  SetTraceLogLevel(LOG_ALL);
  SetTargetFPS(30);

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
    bool valid_move = false;

    if (IsKeyPressed(KEY_DOWN)) {
      for (int i = BOARD_SIZE - 1; i >= 0; i--) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j].exp == -1)
            continue;

          for (size_t k = i; k < BOARD_SIZE - 1; k++) {
            world_merge_swap_cell(world, k, j, 1, 0, &valid_move);
          }
        }
      }
    } else if (IsKeyPressed(KEY_UP)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j].exp == -1)
            continue;

          for (int k = i; k > 0; k--) {
            world_merge_swap_cell(world, k, j, -1, 0, &valid_move);
          }
        }
      }
    } else if (IsKeyPressed(KEY_LEFT)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j].exp == -1)
            continue;

          for (int k = j; k > 0; k--) {
            world_merge_swap_cell(world, i, k, 0, -1, &valid_move);
          }
        }
      }

    } else if (IsKeyPressed(KEY_RIGHT)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (int j = BOARD_SIZE - 1; j > -1; j--) {
          if (world[i][j].exp == -1)
            continue;

          for (size_t k = j; k < BOARD_SIZE - 1; k++) {
            world_merge_swap_cell(world, i, k, 0, 1, &valid_move);
          }
        }
      }
    }

    if (acc > 0.015f && valid_move) {
      // world_spawn_cell(world);
      acc = 0.0f;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (size_t y = 0; y < BOARD_SIZE; y++) {
      for (size_t x = 0; x < BOARD_SIZE; x++) {
        DrawRectangle(x * SQ_SIZE, y * SQ_SIZE, SQ_SIZE + PADDING,
                      SQ_SIZE + PADDING, RAYWHITE);
        cell_t *cell = &world[y][x];
        Color col =
            cell->exp >= 0 ? ColorLerp(GRAY, RED, cell->exp) : LIGHTGRAY;
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

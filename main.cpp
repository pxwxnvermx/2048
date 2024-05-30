#include <array>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>

#define SQ_SIZE 128
#define PADDING 8
#define BOARD_SIZE 4
#define FONT_SIZE 48

#define SCREEN_W 800
#define SCREEN_H 800

typedef std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> world_t;

Color ColorLerp(Color a, Color b, float amount) {
  Quaternion lerped = QuaternionLerp(
      (Quaternion){(float)a.r, (float)a.g, (float)a.b, (float)a.a},
      (Quaternion){(float)b.r, (float)b.g, (float)b.b, (float)b.a}, amount);
  return (Color){(u_char)lerped.x, (u_char)lerped.y, (u_char)lerped.z,
                 (u_char)lerped.w};
}

void merge_swap_cell(world_t &world, int x, int y, int dir_x = 0,
                     int dir_y = 0) {
  // Swap with next if next is -1
  if (world[x][y] != -1 && world[x + dir_x][y + dir_y] == -1) {
    world[x + dir_x][y + dir_y] = world[x][y];
    world[x][y] = -1;
  }

  // Merge with next if next is equal
  if (world[x][y] == world[x + dir_x][y + dir_y]) {
    world[x + dir_x][y + dir_y]++;
    world[x][y] = -1;
  }
}

void spawn_cell(world_t &world) {
  for (size_t i = 0; i < BOARD_SIZE; i++) {
    for (size_t j = 0; j < BOARD_SIZE; j++) {
      if (world[i][j] == -1 && GetRandomValue(0, 1) == 1) {
        world[i][j] = 0;
        break;
      }
    }
  }
}

int main() {
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_W, SCREEN_H, "Just trying something");
  SetTraceLogLevel(LOG_ALL);

  // store exponents only
  world_t world = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  };
  for (size_t i = 0; i < 2; i++) {
    world[GetRandomValue(0, 3)][GetRandomValue(0, 3)] = 0;
    world[0][0] = 0;
    world[0][1] = 0;
  }

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_DOWN)) {
      for (size_t i = 0; i < BOARD_SIZE - 1; i++) {
        for (size_t j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j] == -1)
            continue;
          merge_swap_cell(world, i, j, 1, 0);
        }
      }
      spawn_cell(world);
    } else if (IsKeyPressed(KEY_UP)) {
      for (size_t i = BOARD_SIZE - 1; i > 0; i--) {
        for (size_t j = 0; j < BOARD_SIZE; j++) {
          if (world[i][j] == -1)
            continue;
          merge_swap_cell(world, i, j, -1, 0);
        }
      }
      spawn_cell(world);
    } else if (IsKeyPressed(KEY_LEFT)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (size_t j = BOARD_SIZE - 1; j > 0; j--) {
          if (world[i][j] == -1)
            continue;
          merge_swap_cell(world, i, j, 0, -1);
        }
      }
      spawn_cell(world);
    } else if (IsKeyPressed(KEY_RIGHT)) {
      for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (size_t j = 0; j < BOARD_SIZE - 1; j++) {
          if (world[i][j] == -1)
            continue;
          merge_swap_cell(world, i, j, 0, 1);
        }
      }
      spawn_cell(world);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (size_t y = 0; y < BOARD_SIZE; y++) {
      for (size_t x = 0; x < BOARD_SIZE; x++) {
        Color col =
            world[y][x] >= 0 ? ColorLerp(GRAY, RED, world[y][x]) : LIGHTGRAY;
        int pos_x = x * SQ_SIZE;
        int pos_y = y * SQ_SIZE;
        DrawRectangle(pos_x, pos_y, SQ_SIZE + PADDING, SQ_SIZE + PADDING,
                      RAYWHITE);
        DrawRectangle(pos_x + PADDING / 2, pos_y + PADDING / 2, SQ_SIZE,
                      SQ_SIZE, ColorAlpha(col, 0.3f));
        if (world[y][x] > -1) {
          const char *text = TextFormat("%d", 2 << world[y][x]);
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

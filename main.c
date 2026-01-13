#define SDL_MAIN_HANDLED

#include "chip8.h"
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define SCALE 10
#define WINDOW_WIDTH (64 * SCALE)
#define WINDOW_HEIGHT (32 * SCALE)

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int sdl_init(void) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    SDL_Log("SDL_Init error: %s\n", SDL_GetError());
    return 0;
  }

  window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!window || !renderer) {
    SDL_Log("SDL error: %s", SDL_GetError());
    return 0;
  }

  return 1;
}

void draw_screen(uint8_t *gfx) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      if (gfx[x + y * 64]) {
        SDL_Rect pixel = {
          x * SCALE,
          y * SCALE,
          SCALE,
          SCALE
        };

        SDL_RenderFillRect(renderer, &pixel);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
  srand(time(NULL));

  Chip8 chip8;
  chip8_init(&chip8);
  chip8_load_rom(&chip8, argv[1]);

  if (!sdl_init()) {
    return 1;
  }

  int running = 1;
  SDL_Event e;

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = 0;
      }
    }

    chip8_cycle(&chip8);
    draw_screen(chip8.gfx);

    SDL_Delay(16);

    //TODO: Input
    //TODO: Timers
  }

  SDL_Quit();
  return 0;
}

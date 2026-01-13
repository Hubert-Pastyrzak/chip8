#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define MEM_SIZE 4096
#define GFX_WIDTH 64
#define GFX_HEIGHT 32

typedef struct {
  uint8_t memory[MEM_SIZE];
  uint8_t V[16];
  uint16_t I;
  uint16_t pc;

  uint8_t gfx[GFX_WIDTH * GFX_HEIGHT];
  uint8_t delay_timer;
  uint8_t sound_timer;

  uint16_t stack[16];
  uint8_t sp;

  uint8_t keypad[16];
} Chip8;

void chip8_init(Chip8 *c8);
void chip8_load_rom(Chip8 *c8, const char *filename);
void chip8_cycle(Chip8 *c8);

#endif

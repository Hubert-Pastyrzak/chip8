#include "chip8.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static uint8_t fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void chip8_init(Chip8 *c8) {
  memset(c8, 0, sizeof(Chip8));
  c8->pc = 0x200;

  for (int i = 0; i < 80; i++) {
    c8->memory[i] = fontset[i];
  }
}

void chip8_load_rom(Chip8 *c8, const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("ROM");
    return;
  }

  fread(&c8->memory[0x200], 1, MEM_SIZE - 0x200, f);
  fclose(f);
}

void chip8_cycle(Chip8 *c8) {
  uint16_t opcode =
    c8->memory[c8->pc] << 8 |
    c8->memory[c8->pc + 1];

  c8->pc += 2;

  uint16_t nnn = opcode & 0x0FFF;
  uint8_t  nn  = opcode & 0x00FF;
  uint8_t  n   = opcode & 0x000F;
  uint8_t  x   = (opcode & 0x0F00) >> 8;
  uint8_t  y   = (opcode & 0x00F0) >> 4;

  switch (opcode & 0xF000) {
    case 0x0000: {
      if (opcode == 0x00E0) {
        memset(c8->gfx, 0, sizeof(c8->gfx));
      } else if (opcode == 0x00EE) {
        c8->sp--;
        c8->pc = c8->stack[c8->sp];
      } else {
        printf("Machine code calls not implemented\n");
      }
      
      break;
    }

    case 0x1000: {
      c8->pc = nnn;
      break;
    }

    case 0x2000: {
      c8->stack[c8->sp] = c8->pc;
      c8->sp++;

      break;
    }

    case 0x3000: {
      if (c8->V[x] == nn) {
        c8->pc += 2;
      }

      break;
    }

    case 0x4000: {
      if (c8->V[x] != nn) {
        c8->pc += 2;
      }

      break;
    }

    case 0x5000: {
      if ((opcode & 0xF00F) == 0x5000) {
        if (c8->V[x] == c8->V[y]) {
          c8->pc += 2;
        }
      } else {
        printf("Not implemented\n");
      }

      break;
    }

    case 0x6000: {
      c8->V[x] = nn;
      break;
    }

    case 0x7000: {
      c8->V[x] += nn;
      break;
    }

    case 0x8000: {
      switch (opcode & 0xF00F) {
        case 0x8000: {
          c8->V[x] = c8->V[y];
          break;
        }

        case 0x8001: {
          c8->V[x] |= c8->V[y];
          break;
        }

        case 0x8002: {
          c8->V[x] &= c8->V[y];
          break;
        }

        case 0x8003: {
          c8->V[x] ^= c8->V[y];
          break;
        }

        case 0x8004: {
          uint8_t oldValue = c8->V[x];
          c8->V[x] += c8->V[y];

          if (c8->V[x] < oldValue) {
            c8->V[0xF] = 1;
          } else {
            c8->V[0xF] = 0;
          }

          break;
        }

        case 0x8005: {
          uint8_t oldValue = c8->V[x];
          c8->V[x] -= c8->V[y];

          if (c8->V[x] > oldValue) {
            c8->V[0xF] = 0;
          } else {
            c8->V[0xF] = 1;
          }

          break;
        }

        case 0x8006: {
          uint8_t bitShiftedOut = c8->V[x] & 0x1;
          c8->V[x] >>= 1;

          c8->V[0xF] = bitShiftedOut;

          break;
        }

        case 0x8007: {
          uint8_t oldValue = c8->V[y];
          c8->V[x] = c8->V[y] - c8->V[x];

          if (c8->V[x] > oldValue) {
            c8->V[0xF] = 0;
          } else {
            c8->V[0xF] = 1;
          }

          break;
        }

        case 0x800E: {
          uint8_t bitShiftedOut = (c8->V[x] & 0x80) >> 7;
          c8->V[x] <<= 1;

          c8->V[0xF] = bitShiftedOut;

          break;
        }

        case 0x9000: {
          if ((opcode & 0xF00F) == 0x9000) {
            if (c8->V[x] != c8->V[y]) {
              c8->pc += 2;
            }
          } else {
            printf("Not implemented\n");
          }

          break;
        }
      }

      break;
    }

    case 0xA000: {
      c8->I = nnn;
      break;
    }

    case 0xB000: {
      c8->pc = nnn + c8->V[0];
      break;
    }

    case 0xC000: {
      c8->V[x] = rand() & nn;
      break;
    }

    case 0xD000: {
      //Temporary
      c8->V[0xF] = 0;

      uint16_t _i = c8->I;

      for (int _y = 0; _y < n; _y++) {
        for (int _x = 0; _x < 8; _x++) {
          uint8_t pixel = c8->memory[_i] & (0x80 >> _x);
          if (pixel) {
            int addr = ((c8->V[x] + _x) & 63) + (c8->V[y] + _y) * 64;
            c8->gfx[addr] ^= 1;

            if (c8->gfx[addr] == 0) {
              c8->V[0xF] = 1;
            }
          }
        }

        _i++;
      }

      break;
    }

    case 0xE000: {
      switch (opcode & 0xF0FF) {
        case 0xE09E: {
          printf("Skip next instruction if V%X is pressed\n", x);
          //TODO
          break;
        }

        case 0xE0A1: {
          printf("Skip next instruction if V%X is not pressed\n", x);
          //TODO
          break;
        }

        default: {
          printf("Not implemented\n");
        }
      }

      break;
    }

    case 0xF000: {
      switch (opcode & 0xF0FF) {
        case 0xF007: {
          c8->V[x] = c8->delay_timer;
          break;
        }
        
        case 0xF00A: {
          printf("V%X = get_key()\n", x);
          break;
        }

        case 0xF015: {
          c8->delay_timer = c8->V[x];
          break;
        }

        case 0xF018: {
          c8->sound_timer = c8->V[x];
          break;
        }

        case 0xF01E: {
          c8->I += c8->V[x];
          break;
        }

        case 0xF029: {
          c8->I = (c8->V[x] & 0x0F) * 5;
          break;
        }

        case 0xF033: {
          printf("BCD\n");
          break;
        }

        case 0xF055: {
          for (int i = 0; i <= x; i++) {
            c8->memory[c8->I] = c8->V[i];
            c8->I++;
          }

          break;
        }

        case 0xF065: {
          for (int i = 0; i <= x; i++) {
            c8->V[i] = c8->memory[c8->I];
            c8->I++;
          }

          break;
        }
      }

      break;
    }

    default: {
      printf("Unknown opcode: %X\n", opcode);
    }
  }
}

//Where?
//if (c8->delay_timer > 0)
// c8->delay_timer--;
//
//if (c8->sound_timer > 0)
// c8->sound_timer--;

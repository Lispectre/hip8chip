#ifndef MEMORY_H
#define MEMORY_H


typedef unsigned char byte;
typedef struct ChipMemory {
    byte memory[4096];
    byte V[16];
    unsigned int I:16;
    unsigned int PC:16;
    byte SP:8;
    byte dly:8;
    byte snd:8;
    unsigned int stack[16];
} CHIP_MEMORY;

CHIP_MEMORY machine;
#endif
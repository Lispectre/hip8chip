#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memory.h"
#include "instructions.h"


int main(int argc, char *argv[]){
    // Getting a few things ready for the "machine".
    FILE *rom;
    machine.PC = 0;
    machine.I = 0;
    char display[32*64] = {0};
    byte sprites[16][5] = {{0xF0, 0x90, 0x90, 0x90, 0xF0},    // 0
                        {0x20, 0x60, 0x20, 0x20, 0x70},     // 1
                        {0xF0, 0x10, 0xF0, 0x80, 0xF0},     // 2
                        {0xF0, 0x10, 0xF0, 0x10, 0xF0},     // 3
                        {0x90, 0x90, 0xF0, 0x10, 0x10},     // 4
                        {0xF0, 0x80, 0xF0, 0x10, 0xF0},     // 5
                        {0xF0, 0x80, 0xF0, 0x90, 0xF0},     // 6
                        {0xF0, 0x10, 0x20, 0x40, 0x40},     // 7
                        {0xF0, 0x90, 0xF0, 0x90, 0xF0},     // 8
                        {0xF0, 0x90, 0xF0, 0x10, 0xF0},     // 9
                        {0xF0, 0x90, 0xF0, 0x90, 0x90},     // A
                        {0xE0, 0x90, 0xE0, 0x90, 0xE0},     // B
                        {0xF0, 0x80, 0x80, 0x80, 0xF0},     // C
                        {0xE0, 0x90, 0x90, 0x90, 0xE0},     // D
                        {0xF0, 0x80, 0xF0, 0x80, 0xF0},     // E
                        {0xF0, 0x80, 0xF0, 0x80, 0x80}};    // F
    for (int b = 0; b < 80; b++){
        machine.memory[0x0+b] = sprites[b/5][b%5]; // loading the sprites into memory
    }
    // Importing the rom into the memory.
    rom = fopen(argv[argc-1], "rb");
    if (argc == 1){
        printf("No ROM provided. Please use the program as follows:\n $ hip8chip [optional flags] [rom filename]\n");
        exit(1);
    }
    else if (rom == NULL){
        printf("Could not open the ROM.\n");
        exit(1);
    }
    fseek(rom, 0, SEEK_END);
    unsigned long length = ftell(rom);
    rewind(rom);
    unsigned char buffer[length];
    fread(buffer, length, 1, rom);
    fclose(rom);
    for (int i = 0; i < length; i++) {machine.memory[0x200+i] = buffer[i];}
    machine.PC = 0x200;
    /* Creating a window.
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    if (initGraphics(&window, &renderer, &texture) == 0) {
        printf("Error when initializing the window!\n");
        exit(1);
    }
    int loop = 1;
    SDL_Event event;
    */
    // Main loop for running the emulator
    while(1){
        
        unsigned int instruction = machine.memory[machine.PC]*0x100+machine.memory[machine.PC+1];
        printf("Executing %x - PC: %x \n", instruction, machine.PC);
        int test = returnDigit(instruction, 0) + returnDigit(instruction, 1)*0x10;
        switch(returnDigit(instruction, 3)){
            case 0x0:
                if (returnDigit(instruction, 0) == 0x0){
                    CLS();
                }
                if (returnDigit(instruction, 0) == 0xE){
                    RET(&machine);
                }
                break;
            case 0x1:
                JP(&machine, instruction);
                break;
            case 0x2:
                CALL(&machine, instruction);
                break;
            case 0x3:
                SEVxByte(&machine, instruction);
                break;
            case 0x4:
                SNEVxByte(&machine, instruction);
                break;
            case 0x5:
                SEVxVy(&machine, instruction);
                break;
            case 0x6:
                LDVxByte(&machine, instruction);
                break;
            case 0x7:
                ADDVxByte(&machine, instruction);
                break;
            case 0x8:
                switch(returnDigit(instruction, 0)){
                    case 0x0:
                        LDVxVy(&machine, instruction);
                        break;
                    case 0x1:
                        ORVxVy(&machine, instruction);
                        break;
                    case 0x2:
                        ANDVxVy(&machine, instruction);
                        break;
                    case 0x3:
                        XORVxVy(&machine, instruction);
                        break;
                    case 0x4:
                        ADDVxVy(&machine, instruction);
                        break;
                    case 0x5:
                        SUBVxVy(&machine, instruction);
                        break;
                    case 0x6:
                        SHRVxVY(&machine, instruction);
                        break;
                    case 0x7:
                        SUBNVxVy(&machine, instruction);
                        break;
                    case 0xE:
                        SNEVxVy(&machine, instruction);
                        break;
                }
                break;
            case 0x9:
                SNEVxVy(&machine, instruction);
                break;
            case 0xA:
                LDIaddr(&machine, instruction);
                break;
            case 0xB:
                JPV0addr(&machine, instruction);
                break;
            case 0xC:
                RNDVxbyte(&machine, instruction);
                break;
            case 0xD:
                DRWVxVyNib(&machine, instruction, (char *) display);
                break;
            case 0xE: // these two have to get the window context in order to receive key inputs. 
                if(returnDigit(instruction, 0) == 0xE){
                    SKPVx(&machine, instruction);
                }
                if(returnDigit(instruction, 0) == 0x1){
                    SKPNVx(&machine, instruction);
                }
                break;
            case 0xF:
                switch(test){
                    case 0x07:
                        LDVxDT(&machine, instruction);
                        break;
                    case 0x0A:
                        LDVxK(&machine, instruction);
                        break;
                    case 0x15:
                        LDDTVx(&machine, instruction);
                        break;
                    case 0x18:
                        LDSTVx(&machine, instruction);
                        break;
                    case 0x1E:
                        ADDIVx(&machine, instruction);
                        break;
                    case 0x29:
                        LDFVx(&machine, instruction);
                        break;
                    case 0x33:
                        LDBVx(&machine, instruction);
                        break;
                    case 0x55:
                        LDIVx(&machine, instruction);
                        break;
                    case 0x65:
                        LDVxI(&machine, instruction);
                        break;
                }
                break;
            default:
                printf("Unknown opcode: %x", instruction);
                break;
        }
        //updateGraphics(&window, &renderer, &texture, display);
        machine.PC += 2;
        sleep(1/60); // 60hz
    }
    return 0;
}

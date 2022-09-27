
// Help functions:
void PCToTop(CHIP_MEMORY *ram){
    for (int i = 0; i<16; i++){
        if (ram->stack[i] != 0){
            ram->PC = i;
        }
        else break;
    }
}
int sixteens(int power){ // can't be bothered with importing whole <math.h> and dumb float-returning pow()
    int result = 1;
    for (int i = 0; i < power; i++){
        result *= 16;
    }
    return result;
}
int returnDigit(unsigned int instruction, int power){
    int dig = instruction/sixteens(power);
    return dig%16;
}
int hexToBin(unsigned int hex, int power){
    int result = 1;
    for (int i = 0; i < power; i++){
        result *= 2;
    }
    int re = hex/result;
    return re%2;
}
void hextoBinArray(unsigned int hex, int * array){

}
byte randomByte(){
    return rand();
}
// Opcodes:
void CLS(){
    // write an SDL clearing here lol
}
void RET(CHIP_MEMORY *ram){
    PCToTop(ram);
    ram->SP--;
}
void JP(CHIP_MEMORY *ram, unsigned int instruction){
    instruction = instruction - 0x1000;
    ram->PC = instruction;
    ram->PC -= 2;
}
void CALL(CHIP_MEMORY *ram, unsigned int instruction){
    unsigned int nnn = instruction - 0x2000;
    ram->SP++;
    PCToTop(ram);
    ram->PC = nnn;
}
void SEVxByte(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    if (ram->V[vx] == instruction-vx*0x100-0x3000) ram->PC += 2;
}
void SNEVxByte(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    if (ram->V[vx] != instruction-vx*0x100-0x4000) ram->PC += 2;
}
void SEVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    if(ram->V[vx] == ram->V[vy]) ram->PC += 2;
}
void LDVxByte(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    ram->V[vx] = instruction - 0x6000 - (vx*0x100);
}
void ADDVxByte(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    ram->V[vx] += instruction - 0x7000 - (vx*0x100);
}
void LDVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    ram->V[vx] = ram->V[vy];
}
void ORVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    ram->V[vx] |= ram->V[vy];
}
void ANDVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    ram->V[vx] &= ram->V[vy];
}
void XORVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    ram->V[vx] ^= ram->V[vy];
}
void ADDVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    if (ram->V[vx] + ram->V[vy] > 255) {
        ram->V[0xF] = 1;
    }
    else {
        ram->V[0xF] = 0;
    }
    ram->V[vx] += ram->V[vy];
}
void SUBVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    if (ram->V[vx] > ram->V[vy]) {
        ram->V[0xF] = 1;
    }
    else {
        ram->V[0xF] = 0;
    }
    ram->V[vx] -= ram->V[vy];
}
void SHRVxVY(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    if (ram->V[vx] % 2 == 1){
        ram->V[0xF] = 1;
    }
    else ram->V[0xF] = 0;
    ram->V[vx] /= 2;
}
void SUBNVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    if (ram->V[vy] > ram->V[vx]) {
        ram->V[0xF] = 1;
    }
    else {
        ram->V[0xF] = 0;
    }
    ram->V[vx] = ram->V[vy] - ram->V[vx];
}
void SHLVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    if(ram->V[vx] > 127){
        ram->V[0xF] = 1;
    }
    else ram->V[0xF] = 0;
    ram->V[vx] *= 2;
}
void SNEVxVy(CHIP_MEMORY *ram, unsigned int instruction){
    int vx = returnDigit(instruction, 2);
    int vy = returnDigit(instruction, 1);
    if (ram->V[vx] != ram->V[vy]){
        ram->PC += 2;
    }
}
void LDIaddr(CHIP_MEMORY *ram, unsigned int instruction){
    int nnn = instruction - 0xA000;
    ram->I = nnn;
}
void JPV0addr(CHIP_MEMORY *ram, unsigned int instruction){
    int nnn = instruction - 0xB000;
    ram->PC = nnn + ram->V[0x0];
}
void RNDVxbyte(CHIP_MEMORY *ram, unsigned int instruction){
    int x = returnDigit(instruction, 2);
    int kk = instruction - 0xC000 - x*0x100;
    ram->V[x] = randomByte() & kk;
}
void DRWVxVyNib(CHIP_MEMORY *ram, unsigned int instruction, char *display){
    int n = returnDigit(instruction, 0); // used later as i for height, j is width
    int vx = ram->V[returnDigit(instruction, 2)];
    int vy = ram->V[returnDigit(instruction, 1)];
    ram->V[0xF] = 0;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < 8; j++){
            if(!(ram->memory[ram->I+i] & (0x80>>j))){
                int pixel = ((vx+j) + 64*(vy+i)) % 2048;
                if(display[pixel] == 1){
                    ram->V[0xF] = 1;
                }
                display[pixel] ^= 1;
            }
        }
    }
}
void SKPVx(CHIP_MEMORY *ram, unsigned int instruction){
    //TODO: SDL get char
}
void SKPNVx(CHIP_MEMORY *ram, unsigned int instruction){
    //TODO: SDL get char
}
void LDVxDT(CHIP_MEMORY *ram, unsigned int instruction){
    int x = returnDigit(instruction, 2);
    ram->V[x] = ram->dly;
}
void LDVxK(CHIP_MEMORY *ram, unsigned int instruction){
    //TODO: SDL get char
}
void LDDTVx(CHIP_MEMORY *ram, unsigned int instruction){
    int x = returnDigit(instruction, 2);
    ram->dly = ram->V[x];
}
void LDSTVx(CHIP_MEMORY *ram, unsigned int instruction){
    int x = returnDigit(instruction, 2);
    ram->snd = ram->V[x];
}
void ADDIVx(CHIP_MEMORY *ram, unsigned int instruction){
    int x = returnDigit(instruction, 2);
    ram->I += ram->V[x];
}
void LDFVx(CHIP_MEMORY *ram, unsigned int instruction){
    int x = returnDigit(instruction, 2);
    ram->I = x*0x5;
}
void LDBVx(CHIP_MEMORY *ram, unsigned int instruction){
    int x = ram->V[returnDigit(instruction, 2)];
    int b = x/100;
    int c = (x-b*100)/10;
    int d = x-(b*100)-(c*10);
    ram->memory[ram->I] = b;
    ram->memory[ram->I+1] = c;
    ram->memory[ram->I+2] = d;
}
void LDIVx(CHIP_MEMORY *ram, unsigned int instruction){
    for (int i = 0x0; i<0x10; i++){
        ram->memory[ram->I + i] = ram->V[i];
    }
}
void LDVxI(CHIP_MEMORY *ram, unsigned int instruction){
    for (int i = 0x0; i<0x10; i++){
        ram->V[i] = ram->memory[ram->I + i];
    }
}
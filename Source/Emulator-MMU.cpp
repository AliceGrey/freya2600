#include "Emulator.hpp"

#include <cstdio>

uint8_t Emulator::ReadByte(word address)
{
    // bit mask
    address = (address & 0b0001'1111'1111'1111);

    // $00 - $7F TIA 
    // $00-$2C TIA (write)
    if (address >= 0x00 && address <= 0x2C) {
        printf("TIA READ 0x%04hX \n", address);

        //--------------------------------------------
        //$0000 - $003F = TIA.......(write)......(read)
        //--------------------------------------------
        //$0000 = TIA Address $00 - (VSYNC)......(CXM0P)
        //$0001 = TIA Address $01 - (VBLANK).....(CXM1P)
        //$0002 = TIA Address $02 - (WSYNC)......(CXP0FB)
        //$0003 = TIA Address $03 - (RSYNC)......(CXP1FB)
        //$0004 = TIA Address $04 - (NUSIZ0).....(CXM0FB)
        //$0005 = TIA Address $05 - (NUSIZ1).....(CXM1FB)
        //$0006 = TIA Address $06 - (COLUP0).....(CXBLPF)
        //$0007 = TIA Address $07 - (COLUP1).....(CXPPMM)
        //$0008 = TIA Address $08 - (COLUPF).....(INPT0)
        //$0009 = TIA Address $09 - (COLUBK).....(INPT1)
        //$000A = TIA Address $0A - (CTRLPF).....(INPT2)
        //$000B = TIA Address $0B - (REFP0)......(INPT3)
        //$000C = TIA Address $0C - (REFP1)......(INPT4)
        //$000D = TIA Address $0D - (PF0)........(INPT5)
        //$000E = TIA Address $0E - (PF1)........(UNDEFINED)
        //$000F = TIA Address $0F - (PF2)........(UNDEFINED)
        //$0010 = TIA Address $10 - (RESP0)......(CXM0P)
        //$0011 = TIA Address $11 - (RESP1)......(CXM1P)
        //$0012 = TIA Address $12 - (RESM0)......(CXP0FB)
        //$0013 = TIA Address $13 - (RESM1)......(CXP1FB)
        //$0014 = TIA Address $14 - (RESBL)......(CXM0FB)
        //$0015 = TIA Address $15 - (AUDC0)......(CXM1FB)
        //$0016 = TIA Address $16 - (AUDC1)......(CXBLPF)
        //$0017 = TIA Address $17 - (AUDF0)......(CXPPMM)
        //$0018 = TIA Address $18 - (AUDF1)......(INPT0)
        //$0019 = TIA Address $19 - (AUDV0)......(INPT1)
        //$001A = TIA Address $1A - (AUDV1)......(INPT2)
        //$001B = TIA Address $1B - (GRP0).......(INPT3)
        //$001C = TIA Address $1C - (GRP1).......(INPT4)
        //$001D = TIA Address $1D - (ENAM0)......(INPT5)
        //$001E = TIA Address $1E - (ENAM1)......(UNDEFINED)
        //$001F = TIA Address $1F - (ENABL)......(UNDEFINED)
        //$0020 = TIA Address $20 - (HMP0).......(CXM0P)
        //$0021 = TIA Address $21 - (HMP1).......(CXM1P)
        //$0022 = TIA Address $22 - (HMM0).......(CXP0FB)
        //$0023 = TIA Address $23 - (HMM1).......(CXP1FB)
        //$0024 = TIA Address $24 - (HMBL).......(CXM0FB)
        //$0025 = TIA Address $25 - (VDELP0).....(CXM1FB)
        //$0026 = TIA Address $26 - (VDELP1).....(CXBLPF)
        //$0027 = TIA Address $27 - (VDELBL).....(CXPPMM)
        //$0028 = TIA Address $28 - (RESMP0).....(INPT0)
        //$0029 = TIA Address $29 - (RESMP1).....(INPT1)
        //$002A = TIA Address $2A - (HMOVE)......(INPT2)
        //$002B = TIA Address $2B - (HMCLR)......(INPT3)
        //$002C = TIA Address $2C - (CXCLR)......(INPT4)
    }
    // $30-$3D TIA (read)
    if (address >= 0x30 && address <= 0x3D) {
        printf("TIA READ 0x%04hX \n", address); 
        //---------------------------------------------
        //$0000 - $003F = TIA.......(write)......(read)
        //---------------------------------------------
        //$0030 = TIA Address $30 - (UNDEFINED)..(CXM0P)
        //$0031 = TIA Address $31 - (UNDEFINED)..(CXM1P)
        //$0032 = TIA Address $32 - (UNDEFINED)..(CXP0FB)
        //$0033 = TIA Address $33 - (UNDEFINED)..(CXP1FB)
        //$0034 = TIA Address $34 - (UNDEFINED)..(CXM0FB)
        //$0035 = TIA Address $35 - (UNDEFINED)..(CXM1FB)
        //$0036 = TIA Address $36 - (UNDEFINED)..(CXBLPF)
        //$0037 = TIA Address $37 - (UNDEFINED)..(CXPPMM)
        //$0038 = TIA Address $38 - (UNDEFINED)..(INPT0)
        //$0039 = TIA Address $39 - (UNDEFINED)..(INPT1)
        //$003A = TIA Address $3A - (UNDEFINED)..(INPT2)
        //$003B = TIA Address $3B - (UNDEFINED)..(INPT3)
        //$003C = TIA Address $3C - (UNDEFINED)..(INPT4)
        //$003D = TIA Address $3D - (UNDEFINED)..(INPT5)
    }

    // RIOT RAM
    if (address >= 0x80 && address <= 0xFF) {
        return RAM[address - 0x80];
    }
    // RIOT (I/O, Timer)
    if (address >= 0x280 && address <= 0x297) {
        printf("RIOT I/O READ 0x%04hX \n", address);
        //$0280 = (RIOT $00) - Read DRA
        //$0281 = (RIOT $01) - Read DDRA
        //$0282 = (RIOT $02) - Read DRB
        //$0283 = (RIOT $03) - Read DDRB
        //$0284 = (RIOT $04) - Read timer, disable interrupt (2)
        //$0285 = (RIOT $05) - Read interrupt flag
        //$0286 = (RIOT $06) - Read timer, disable interrupt (2)
        //$0287 = (RIOT $07) - Read interrupt flag
        //$0288 = (RIOT $08) - Read DRA
        //$0289 = (RIOT $09) - Read DDRA
        //$028A = (RIOT $0A) - Read DRB
        //$028B = (RIOT $0B) - Read DDRB
        //$028C = (RIOT $0C) - Read timer, enable interrupt (2)
        //$028D = (RIOT $0D) - Read interrupt flag
        //$028E = (RIOT $0E) - Read timer, enable interrupt (2)
        //$028F = (RIOT $0F) - Read interrupt flag
        //$0290 = (RIOT $10) - Read DRA
        //$0291 = (RIOT $11) - Read DDRA
        //$0292 = (RIOT $12) - Read DRB
        //$0293 = (RIOT $13) - Read DDRB
        //$0294 = (RIOT $14) - Read timer, disable interrupt (2)
        //$0295 = (RIOT $15) - Read interrupt flag
        //$0296 = (RIOT $16) - Read timer, disable interrupt (2)
        //$0297 = (RIOT $17) - Read interrupt flag
        //$0298 = (RIOT $18) - Read DRA
        //$0299 = (RIOT $19) - Read DDRA
        //$029A = (RIOT $1A) - Read DRB
        //$029B = (RIOT $1B) - Read DDRB
        //$029C = (RIOT $1C) - Read timer, enable interrupt (2)
        //$029D = (RIOT $1D) - Read interrupt flag
        //$029E = (RIOT $1E) - Read timer, enable interrupt (2)
        //$029F = (RIOT $1F) - Read interrupt flag
    }

    // MAX GO AWAY STOP ATTACKING ME
    // - SLW 2021

    // RAMR
    // if (address >= 0x1080 && address <= 0x11FF) {
    //     return EXTRAM[address - 0x1080];
    // }

    // $F000 to $F200 (vaguely the EXTRAM)
    // $F000 to $F0FF (writing to EXTRAM)
    // $F100 to $F1FF (reading to EXTRAM)

    // ROM
    if (address >= 0x1000 && address <= 0x1FFF) {
        return ROM[ROMBank][address - 0x1000];
    }

    //sTr0be BANK
    // The bank number is selected by reading from (or by writing any value to) specific addresses, the addresses and corresponding bank numbers are:
    // Size   Banks  FFF4 FFF5 FFF6 FFF7 FFF8 FFF9 FFFA FFFB
    // 2K,4K  1      -    -    -    -    -    -    -    -
    // 8K     2      -    -    -    -    0    1    -    -
    // 12K    3      -    -    -    -    0    1    2    -
    // 16K    4      -    -    0    1    2    3    -    -
    // 32K    8      0    1    2    3    4    5    6    7
    if (address >= 0x1200 && address <= 0x1FFF) {
        
        // Execute functions 
    }

    return 0;
}

void Emulator::WriteByte(word address, byte data)
{
    address = (address & 0b0001'1111'1111'1111);

       // $00 - $7F TIA 
    // $00-$2C TIA (write)
    if (address >= 0x00 && address <= 0x2C) {
        printf("TIA WRITE 0x%04hX \n", address);
        //--------------------------------------------
        //$0000 - $003F = TIA.......(write)......(read)
        //--------------------------------------------
        //$0000 = TIA Address $00 - (VSYNC)......(CXM0P)
        //$0001 = TIA Address $01 - (VBLANK).....(CXM1P)
        //$0002 = TIA Address $02 - (WSYNC)......(CXP0FB)
        //$0003 = TIA Address $03 - (RSYNC)......(CXP1FB)
        //$0004 = TIA Address $04 - (NUSIZ0).....(CXM0FB)
        //$0005 = TIA Address $05 - (NUSIZ1).....(CXM1FB)
        //$0006 = TIA Address $06 - (COLUP0).....(CXBLPF)
        //$0007 = TIA Address $07 - (COLUP1).....(CXPPMM)
        //$0008 = TIA Address $08 - (COLUPF).....(INPT0)
        //$0009 = TIA Address $09 - (COLUBK).....(INPT1)
        //$000A = TIA Address $0A - (CTRLPF).....(INPT2)
        //$000B = TIA Address $0B - (REFP0)......(INPT3)
        //$000C = TIA Address $0C - (REFP1)......(INPT4)
        //$000D = TIA Address $0D - (PF0)........(INPT5)
        //$000E = TIA Address $0E - (PF1)........(UNDEFINED)
        //$000F = TIA Address $0F - (PF2)........(UNDEFINED)
        //$0010 = TIA Address $10 - (RESP0)......(CXM0P)
        //$0011 = TIA Address $11 - (RESP1)......(CXM1P)
        //$0012 = TIA Address $12 - (RESM0)......(CXP0FB)
        //$0013 = TIA Address $13 - (RESM1)......(CXP1FB)
        //$0014 = TIA Address $14 - (RESBL)......(CXM0FB)
        //$0015 = TIA Address $15 - (AUDC0)......(CXM1FB)
        //$0016 = TIA Address $16 - (AUDC1)......(CXBLPF)
        //$0017 = TIA Address $17 - (AUDF0)......(CXPPMM)
        //$0018 = TIA Address $18 - (AUDF1)......(INPT0)
        //$0019 = TIA Address $19 - (AUDV0)......(INPT1)
        //$001A = TIA Address $1A - (AUDV1)......(INPT2)
        //$001B = TIA Address $1B - (GRP0).......(INPT3)
        //$001C = TIA Address $1C - (GRP1).......(INPT4)
        //$001D = TIA Address $1D - (ENAM0)......(INPT5)
        //$001E = TIA Address $1E - (ENAM1)......(UNDEFINED)
        //$001F = TIA Address $1F - (ENABL)......(UNDEFINED)
        //$0020 = TIA Address $20 - (HMP0).......(CXM0P)
        //$0021 = TIA Address $21 - (HMP1).......(CXM1P)
        //$0022 = TIA Address $22 - (HMM0).......(CXP0FB)
        //$0023 = TIA Address $23 - (HMM1).......(CXP1FB)
        //$0024 = TIA Address $24 - (HMBL).......(CXM0FB)
        //$0025 = TIA Address $25 - (VDELP0).....(CXM1FB)
        //$0026 = TIA Address $26 - (VDELP1).....(CXBLPF)
        //$0027 = TIA Address $27 - (VDELBL).....(CXPPMM)
        //$0028 = TIA Address $28 - (RESMP0).....(INPT0)
        //$0029 = TIA Address $29 - (RESMP1).....(INPT1)
        //$002A = TIA Address $2A - (HMOVE)......(INPT2)
        //$002B = TIA Address $2B - (HMCLR)......(INPT3)
        //$002C = TIA Address $2C - (CXCLR)......(INPT4)
    }
    // $30-$3D TIA (read)
    if (address >= 0x30 && address <= 0x3D) { 
        printf("TIA WRITE 0x%04hX \n", address);
        //---------------------------------------------
        //$0000 - $003F = TIA.......(write)......(read)
        //---------------------------------------------
        //$0030 = TIA Address $30 - (UNDEFINED)..(CXM0P)
        //$0031 = TIA Address $31 - (UNDEFINED)..(CXM1P)
        //$0032 = TIA Address $32 - (UNDEFINED)..(CXP0FB)
        //$0033 = TIA Address $33 - (UNDEFINED)..(CXP1FB)
        //$0034 = TIA Address $34 - (UNDEFINED)..(CXM0FB)
        //$0035 = TIA Address $35 - (UNDEFINED)..(CXM1FB)
        //$0036 = TIA Address $36 - (UNDEFINED)..(CXBLPF)
        //$0037 = TIA Address $37 - (UNDEFINED)..(CXPPMM)
        //$0038 = TIA Address $38 - (UNDEFINED)..(INPT0)
        //$0039 = TIA Address $39 - (UNDEFINED)..(INPT1)
        //$003A = TIA Address $3A - (UNDEFINED)..(INPT2)
        //$003B = TIA Address $3B - (UNDEFINED)..(INPT3)
        //$003C = TIA Address $3C - (UNDEFINED)..(INPT4)
        //$003D = TIA Address $3D - (UNDEFINED)..(INPT5)
    }

    // RIOT RAM
    if (address >= 0x0080 && address <= 0x00FF) {
        RAM[address - 0x0080] = data;
    }

    // RIOT (I/O, Timer)
    if (address >= 0x280 && address <= 0x297) {
        printf("RIOT I/O WRITE 0x%04hX \n", address);
        //$0280 = (RIOT $00) - Write DRA
        //$0281 = (RIOT $01) - Write DDRA
        //$0282 = (RIOT $02) - Write DRB
        //$0283 = (RIOT $03) - Write DDRB
        //$0284 = (RIOT $04) - Write edge detect control - negative edge, disable int (1)
        //$0285 = (RIOT $05) - Write edge detect control - positive edge, disable int (1)
        //$0286 = (RIOT $06) - Write edge detect control - negative edge, enable int (1)
        //$0287 = (RIOT $07) - Write edge detect control - positive edge, enable int (1)
        //$0288 = (RIOT $08) - Write DRA
        //$0289 = (RIOT $09) - Write DDRA
        //$028A = (RIOT $0A) - Write DRB
        //$028B = (RIOT $0B) - Write DDRB
        //$028C = (RIOT $0C) - Write edge detect control - negative edge, disable int (1)
        //$028D = (RIOT $0D) - Write edge detect control - positive edge, disable int (1)
        //$028E = (RIOT $0E) - Write edge detect control - negative edge, enable int (1)
        //$028F = (RIOT $0F) - Write edge detect control - positive edge, enable int (1)
        //$0290 = (RIOT $10) - Write DRA
        //$0291 = (RIOT $11) - Write DDRA
        //$0292 = (RIOT $12) - Write DRB
        //$0293 = (RIOT $13) - Write DDRB
        //$0294 = (RIOT $14) - Write timer (div by 1)    - disable int (2)
        //$0295 = (RIOT $15) - Write timer (div by 8)    - disable int (2)
        //$0296 = (RIOT $16) - Write timer (div by 64)   - disable int (2)
        //$0297 = (RIOT $17) - Write timer (div by 1024) - disable int (2)
        //$0298 = (RIOT $18) - Write DRA
        //$0299 = (RIOT $19) - Write DDRA
        //$029A = (RIOT $1A) - Write DRB
        //$029B = (RIOT $1B) - Write DDRB
        //$029C = (RIOT $1C) - Write timer (div by 1)    - enable int (2)
        //$029D = (RIOT $1D) - Write timer (div by 8)    - enable int (2)
        //$029E = (RIOT $1E) - Write timer (div by 64)   - enable int (2)
        //$029F = (RIOT $1F) - Write timer (div by 1024) - enable int (2)
    }

    // // RAMW
    // if (address >= 0x1000 && address <= 0x1100) {
    //     EXTRAM[address - 0xF000] = data;
    // }
    // ROM
    if (address >= 0x1000 && address <= 0x1FFF) {
        // LMAO NICE TRY - FAFO
    }

    //BANK
    if (address == 0x3F) {
        ROMBank = (data & 0b00000011);
    }

    //sTr0be BANK
    // The bank number is selected by reading from (or by writing any value to) specific addresses, the addresses and corresponding bank numbers are:
    // Size   Banks  FFF4 FFF5 FFF6 FFF7 FFF8 FFF9 FFFA FFFB
    // 2K,4K  1      -    -    -    -    -    -    -    -
    // 8K     2      -    -    -    -    0    1    -    -
    // 12K    3      -    -    -    -    0    1    2    -
    // 16K    4      -    -    0    1    2    3    -    -
    // 32K    8      0    1    2    3    4    5    6    7
    if (address >= 0x1200 && address <= 0x1FFF) {
        
        // Execute functions 
    }
}
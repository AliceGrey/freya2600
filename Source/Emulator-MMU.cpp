#include "Emulator.hpp"
#include "Constants.hpp"

#include <cstdio>

uint8_t Emulator::ReadByte(word address)
{
    // bit mask
    address = (address & 0b0001'1111'1111'1111);

    // $00 - $7F TIA 
    // $00-$2C TIA (write)
    // $30-$3D TIA (read)
    if (address >= 0x00 && address <= 0x3D) {
        switch (address & 0x0F) {
        case CXM0P:  // Read: Collision D7=(M0;P1); D6=(M0,P0)
            printf("READ CXM0P\n");
        case CXM1P:  // Read: Collision D7=(M1;P0); D6=(M1,P1)
            printf("READ CXM1P\n");
        case CXP0FB:  // Read: Collision D7=(P0;PF); D6=(P0,BL)
            printf("READ CXP0FB\n");
        case CXP1FB:  // Read: Collision D7=(P1;PF); D6=(P1;BL)
            printf("READ CXP1FB\n");
        case CXM0FB:  // Read: Collision D7=(M0;PF); D6=(M0;BL)
            printf("READ CXM0FB\n");
        case CXM1FB:  // Read: Collision D7=(M1;PF); D6=(M1;BL)
            printf("READ CXM1FB\n");
        case CXBLPF:  // Read: Collision D7=(BL;PF); D6=(unused)
            printf("READ CXBLPF\n");
        case CXPPMM:  // Read: Collision D7=(P0;P1); D6=(M0;M1)
            printf("READ CXPPMM\n");
        case INPT0:  // Read: Pot port D7
            printf("READ INPT0\n");
        case INPT1:  // Read: Pot port D7
            printf("READ INPT1\n");
        case INPT2:  // Read: Pot port D7
            printf("READ INPT2\n");
        case INPT3:  // Read: Pot port D7
            printf("READ INPT3\n");
        case INPT4:  // Read: P1 joystick trigger: D7
            printf("READ INPT4\n");
        case INPT5:  // Read: P2 joystick trigger: D7
            printf("READ INPT5\n");
        default:
            printf("UNDEFINED READ IN TIA AREA 0x%04X \n", address & 0x0F);
        }

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
    // $30-$3D TIA (read)
    if (address >= 0x00 && address <= 0x2C) {
        switch (address) {
            case VSYNC:
                printf("WRITE VSYNC\n");  // Write: VSYNC set-clear (D1)
            case VBLANK:
                printf("WRITE VBLANK\n");  // Write: VBLANK set-clear (D7-6,D1)
            case WSYNC :
                printf("WRITE WSYNC\n");  // Write: Wait for leading edge of hrz. blank (strobe)
            case RSYNC:
                printf("WRITE RSYNC\n");  // Write: Reset hrz. sync counter (strobe)
            case NUSIZ0:
                printf("WRITE NUSIZ0\n");  // Write: Number-size player-missle 0 (D5-0)
            case NUSIZ1:
                printf("WRITE NUSIZ1\n");  // Write: Number-size player-missle 1 (D5-0)
            case COLUP0:
                printf("WRITE COLUP0\n");  // Write: Color-lum player 0 (D7-1)
            case COLUP1:
                printf("WRITE COLUP1\n");  // Write: Color-lum player 1 (D7-1)
            case COLUPF:
                printf("WRITE COLUPF\n");  // Write: Color-lum playfield (D7-1)
            case COLUBK:
                printf("WRITE COLUBK\n");  // Write: Color-lum background (D7-1)
            case CTRLPF:
                printf("WRITE CTRLPF\n");  // Write: Contrl playfield ballsize & coll. (D5-4,D2-0)
            case REFP0:
                printf("WRITE REFP0\n");  // Write: Reflect player 0 (D3)
            case REFP1:
                printf("WRITE REFP1\n");  // Write: Reflect player 1 (D3)
            case PF0:
                printf("WRITE PF0\n");  // Write: Playfield register byte 0 (D7-4)
            case PF1:
                printf("WRITE PF1\n");  // Write: Playfield register byte 1 (D7-0)
            case PF2:
                printf("WRITE PF2\n");  // Write: Playfield register byte 2 (D7-0)
            case RESP0:
                printf("WRITE RESP0\n");  // Write: Reset player 0 (strobe)
            case RESP1:
                printf("WRITE RESP1\n");  // Write: Reset player 1 (strobe)
            case RESM0:
                printf("WRITE RESM0\n");  // Write: Reset missle 0 (strobe)
            case RESM1:
                printf("WRITE RESM1\n");  // Write: Reset missle 1 (strobe)
            case RESBL:
                printf("WRITE RESBL\n");  // Write: Reset ball (strobe)
            case AUDC0:
                printf("WRITE AUDC0\n");  // Write: Audio control 0 (D3-0)
            case AUDC1:
                printf("WRITE AUDC1\n");  // Write: Audio control 1 (D4-0)
            case AUDF0:
                printf("WRITE AUDF0\n");  // Write: Audio frequency 0 (D4-0)
            case AUDF1:
                printf("WRITE AUDF1\n");  // Write: Audio frequency 1 (D3-0)
            case AUDV0:
                printf("WRITE AUDV0\n");  // Write: Audio volume 0 (D3-0)
            case AUDV1:
                printf("WRITE AUDV1\n");  // Write: Audio volume 1 (D3-0)
            case GRP0:
                printf("WRITE GRP0\n");  // Write: Graphics player 0 (D7-0)
            case GRP1:
                printf("WRITE GRP1\n");  // Write: Graphics player 1 (D7-0)
            case ENAM0:
                printf("WRITE ENAM0\n");  // Write: Graphics (enable) missle 0 (D1)
            case ENAM1:
                printf("WRITE ENAM1\n");  // Write: Graphics (enable) missle 1 (D1)
            case ENABL:
                printf("WRITE ENABL\n");  // Write: Graphics (enable) ball (D1)
            case HMP0:
                printf("WRITE HMP0\n");  // Write: Horizontal motion player 0 (D7-4)
            case HMP1:
                printf("WRITE HMP1\n");  // Write: Horizontal motion player 1 (D7-4)
            case HMM0:
                printf("WRITE HMM0\n");  // Write: Horizontal motion missle 0 (D7-4)
            case HMM1:
                printf("WRITE HMM1\n");  // Write: Horizontal motion missle 1 (D7-4)
            case HMBL:
                printf("WRITE HMBL\n");  // Write: Horizontal motion ball (D7-4)
            case VDELP0:
                printf("WRITE VDELP0\n");  // Write: Vertical delay player 0 (D0)
            case VDELP1:
                printf("WRITE VDELP1\n");  // Write: Vertical delay player 1 (D0)
            case VDELBL:
                printf("WRITE VDELBL\n");  // Write: Vertical delay ball (D0)
            case RESMP0:
                printf("WRITE RESMP0\n");  // Write: Reset missle 0 to player 0 (D1)
            case RESMP1:
                printf("WRITE RESMP1\n");  // Write: Reset missle 1 to player 1 (D1)
            case HMOVE:
                printf("WRITE HMOVE\n");  // Write: Apply horizontal motion (strobe)
            case HMCLR:
                printf("WRITE HMCLR\n");  // Write: Clear horizontal motion registers (strobe)
            case CXCLR:
                printf("WRITE CXCLR\n");  // Write: Clear collision latches (strobe)
            default:
                printf("UNDEFINTED WRITE IN TIA AREA 0x%04hX \n", address);
        }
    }

    if (address >= 0x2D && address <= 0x3F) {
        printf("ILLEGAL WRITE IN TIA AREA 0x%04hX \n", address);
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
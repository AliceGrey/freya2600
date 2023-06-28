#include "Emulator.hpp"
#include "Constants.hpp"

#include <cstdio>

uint8_t Emulator::ReadByte(word address, bool tick /*= true*/)
{
    if (tick) {
        ++CPUCycleCount;
    }

    // bit mask
    address = (address & 0b0001'1111'1111'1111);

    //printf("ReadByte Address: 0x%04X\n", address);

    // TIA Chip
    // $00 - $7F TIA
    // $00-$2C TIA (write)
    // $30-$3D TIA (read)
    if (address >= 0x00 && address <= 0x3D) {
        switch (address & 0x0F) {
        case ADDR_CXM0P:  // Read: Collision D7=(M0;P1); D6=(M0,P0)
            //printf("READ CXM0P\n");
            break;
        case ADDR_CXM1P:  // Read: Collision D7=(M1;P0); D6=(M1,P1)
            //printf("READ CXM1P\n");
            break;
        case ADDR_CXP0FB:  // Read: Collision D7=(P0;PF); D6=(P0,BL)
            //printf("READ CXP0FB\n");
            break;
        case ADDR_CXP1FB:  // Read: Collision D7=(P1;PF); D6=(P1;BL)
            //printf("READ CXP1FB\n");
            break;
        case ADDR_CXM0FB:  // Read: Collision D7=(M0;PF); D6=(M0;BL)
            //printf("READ CXM0FB\n");
            break;
        case ADDR_CXM1FB:  // Read: Collision D7=(M1;PF); D6=(M1;BL)
            //printf("READ CXM1FB\n");
            break;
        case ADDR_CXBLPF:  // Read: Collision D7=(BL;PF); D6=(unused)
            //printf("READ CXBLPF\n");
            break;
        case ADDR_CXPPMM:  // Read: Collision D7=(P0;P1); D6=(M0;M1)
            //printf("READ CXPPMM\n");
            break;
        case ADDR_INPT0:  // Read: Pot port D7
            //printf("READ INPT0\n");
            break;
        case ADDR_INPT1:  // Read: Pot port D7
            //printf("READ INPT1\n");
            break;
        case ADDR_INPT2:  // Read: Pot port D7
            //printf("READ INPT2\n");
            break;
        case ADDR_INPT3:  // Read: Pot port D7
            //printf("READ INPT3\n");
            break;
        case ADDR_INPT4:  // Read: P1 joystick trigger: D7
            //printf("READ INPT4\n");
            break;
        case ADDR_INPT5:  // Read: P2 joystick trigger: D7
            //printf("READ INPT5\n");
            break;
        default:
            printf("UNDEFINED READ IN TIA AREA 0x%04X \n", address);
            break;
        }

    }

    // Actual RAM
    if (address >= 0x80 && address <= 0xFF) {
        return RAM[address - 0x80];
    }
    // PIA (AKA RIOT) (I/O, Timer)
    if (address >= 0x280 && address <= 0x297) {
        switch (address) {
            //I/O
            case ADDR_SWCHA: // Port A; input or output (read or write) Used for controllers (joystick, paddles, etc.)
                return SWCHA._raw;
            case ADDR_SWACNT: // Port A data direction register, 0= input, 1=output
                return SWACNT;
            case ADDR_SWCHB: // Port B; console switches (read only)
                return SWCHB._raw;
            case ADDR_SWBCNT: // Port B data direction register (hardwired as input) 
                return SWBCNT;
            case ADDR_INTIM:  // Timer output (read only)
                TIMINT.Timer = 0;
                return INTIM;
            case ADDR_TIMINT: // Timer Interupt Flag
                TIMINT.EdgeDetect = 0;
                return TIMINT._raw;

            default:
                printf("RIOT I/O READ 0x%04hX \n", address);
                break;
        }
    }

    // MAX GO AWAY STOP ATTACKING ME
    // - SLW 2021

    //TODO
    // RAMR
    // if (address >= 0x1080 && address <= 0x11FF) {
    //     return EXTRAM[address - 0x1080];
    // }

    // $F000 to $F200 (vaguely the EXTRAM)
    // $F000 to $F0FF (writing to EXTRAM)
    // $F100 to $F1FF (reading to EXTRAM)

    // ROM
    if (address >= 0x1000 && address <= 0x1FFF) {
        //return ROM[ROMBank][address - 0x1000];
        int offset = (address - BANK_SWITCH_ADDRESS) % ROM_BANK_SIZE;
        return ROM[ROMBank][offset];
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
        //TODO

        // Execute functions
    }

    return 0;
}

void Emulator::WriteByte(word address, byte data)
{
    ++CPUCycleCount;

    address = (address & 0b0001'1111'1111'1111);

    // TIA Chip
    // $00 - $7F TIA
    // $00-$2C TIA (write)
    // $30-$3D TIA (read)
    if (address >= 0x00 && address <= 0x2C) {
        switch (address) {

            #define TIA_WRITE(REG) \
                case ADDR_##REG: \
                    REG._raw = data; \
                    break

            case ADDR_WSYNC:  // Write: Wait for leading edge of hrz. blank (strobe)
                WSYNC = true;
                LastWSYNC = CPUCycleCount;
                break;
            case ADDR_RSYNC:  // Write: Reset hrz. sync counter (strobe)
                break;
                
            // TIA_WRITE(VSYNC);  // Write: VSYNC set-clear (D1)
            case ADDR_VSYNC:
                VSYNC._raw = data;
                if (!VSYNC.Enabled) {
                    MemoryLine = 0;
                    MemoryColumn = 0;
                }
                break;
            TIA_WRITE(VBLANK); // Write: VBLANK set-clear (D7-6,D1)
            TIA_WRITE(NUSIZ0); // Write: Number-size player-missle 0 (D5-0)
            TIA_WRITE(NUSIZ1); // Write: Number-size player-missle 1 (D5-0)
            TIA_WRITE(COLUP0); // Write: Color-lum player 0 (D7-1)
            TIA_WRITE(COLUP1); // Write: Color-lum player 1 (D7-1)
            TIA_WRITE(COLUPF); // Write: Color-lum playfield (D7-1)
            TIA_WRITE(COLUBK); // Write: Color-lum background (D7-1)
            TIA_WRITE(CTRLPF); // Write: Contrl playfield ballsize & coll. (D5-4,D2-0)
            TIA_WRITE(REFP0);  // Write: Reflect player 0 (D3)
            TIA_WRITE(REFP1);  // Write: Reflect player 1 (D3)
            TIA_WRITE(AUDC0);  // Write: Audio control 0 (D3-0)
            TIA_WRITE(AUDC1);  // Write: Audio control 1 (D4-0)
            TIA_WRITE(AUDF0);  // Write: Audio frequency 0 (D4-0)
            TIA_WRITE(AUDF1);  // Write: Audio frequency 1 (D3-0)
            TIA_WRITE(AUDV0);  // Write: Audio volume 0 (D3-0)
            TIA_WRITE(AUDV1);  // Write: Audio volume 1 (D3-0)
            TIA_WRITE(ENAM0);  // Write: Graphics (enable) missle 0 (D1)
            TIA_WRITE(ENAM1);  // Write: Graphics (enable) missle 1 (D1)
            TIA_WRITE(ENABL);  // Write: Graphics (enable) ball (D1)
            TIA_WRITE(HMP0);   // Write: Horizontal motion player 0 (D7-4)
            TIA_WRITE(HMP1);   // Write: Horizontal motion player 1 (D7-4)
            TIA_WRITE(HMM0);   // Write: Horizontal motion missle 0 (D7-4)
            TIA_WRITE(HMM1);   // Write: Horizontal motion missle 1 (D7-4)
            TIA_WRITE(HMBL);   // Write: Horizontal motion ball (D7-4)
            TIA_WRITE(VDELP0); // Write: Vertical delay player 0 (D0)
            TIA_WRITE(VDELP1); // Write: Vertical delay player 1 (D0)
            TIA_WRITE(VDELBL); // Write: Vertical delay ball (D0)
            TIA_WRITE(RESMP0); // Write: Reset missle 0 to player 0 (D1)
            TIA_WRITE(RESMP1); // Write: Reset missle 1 to player 1 (D1)

            case ADDR_GRP0:    // Write: Graphics player 0 (D7-0)
                GRP0 = data;
                break;
            case ADDR_GRP1:    // Write: Graphics player 1 (D7-0)
                GRP1 = data;
                break;
            case ADDR_PF0:    // Write: Playfield register byte 0 (D7-4)
                PF[0] = data;
                break;
            case ADDR_PF1:    // Write: Playfield register byte 1 (D7-0)
                PF[1] = data;
                break;
            case ADDR_PF2:    // Write: Playfield register byte 2 (D7-0)
                PF[2] = data;
                break;
            case ADDR_RESP0:  // Write: Reset player 0 (strobe)
                SpriteCounterP0 = 8;
                break;
            case ADDR_RESP1:  // Write: Reset player 1 (strobe)
                SpriteCounterP1 = 8;
                break;
            case ADDR_RESM0:  // Write: Reset missle 0 (strobe)
                break;
            case ADDR_RESM1:  // Write: Reset missle 1 (strobe)
                break;
            case ADDR_RESBL:  // Write: Reset ball (strobe)
                break;
            case ADDR_HMOVE:  // Write: Apply horizontal motion (strobe)
                break;
            case ADDR_HMCLR:  // Write: Clear horizontal motion registers (strobe)
                break;
            case ADDR_CXCLR:  // Write: Clear collision latches (strobe)
                break;

            default:
                printf("UNDEFINTED WRITE IN TIA AREA 0x%04hX \n", address);
                break;
        }
    }

    if (address >= 0x2D && address <= 0x3F) {
        //printf("ILLEGAL WRITE IN TIA AREA 0x%04hX \n", address);
    }

    // Actual RAM
    if (address >= 0x80 && address <= 0xFF) {
        RAM[address - 0x80] = data;
    }

    // PIA (AKA RIOT) (I/O, Timer)
    if (address >= 0x280 && address <= 0x297) {

        if (address >= ADDR_TIM1T && address <= ADDR_T1024T) {
            TimerInterval = TIMER_INTERVALS[address - ADDR_TIM1T];
            TimerCounter = TimerInterval - 2; // ?
            INTIM = data;
            TIMINT.Timer = 0;
            return;
        }

        switch (address) {
            //IO
            //TODO
            // constexpr uint16_t ADDR_SWCHA   = 0x280; // Port A; input or output (read or write) Used for controllers (joystick, paddles, etc.)
            // constexpr uint16_t ADDR_SWACNT  = 0x281; // Port A data direction register, 0= input, 1=output
            // constexpr uint16_t ADDR_SWCHB   = 0x282; // Port B; console switches (read only)
            // constexpr uint16_t ADDR_SWBCNT  = 0x283; // Port B data direction register (hardwired as input)

            case ADDR_SWCHB:
                SWCHB._raw &= ~SWBCNT;
                SWCHB._raw |= (data & SWBCNT);
                // SWCHB._raw = data;
                // SWCHB.ColorEnabled = 1;
                // SWCHB._raw = (data | ~SWBCNT) & (SWCHB._raw | SWBCNT);
                break;
            case ADDR_SWBCNT:
                SWBCNT = data;
                break;
            case ADDR_SWCHA:
                SWCHA._raw &= ~SWACNT;
                SWCHA._raw |= (data & SWACNT);
                break;
            case ADDR_SWACNT:
                SWACNT = data;
                break;
            default:
                 printf("RIOT I/O WRITE 0x%04hX \n", address);
                 break;
        }
    }

    // // RAMW
    // if (address >= 0x1000 && address <= 0x1100) {
    //     EXTRAM[address - 0xF000] = data;
    // }
    // ROM
    if (address >= 0x1000 && address <= 0x1FFF) {
        // LMAO NICE TRY - FAFO
    }

    if (address == BANK_SWITCH_ADDRESS) {
        ROMBank = data;
        printf("Switched to ROM bank: %d\n", ROMBank);
    }
    //TODO: DO we need this? Does the above replace this?
    //BANK
    //if (address == 0x3F) {
    //    ROMBank = (data & 0b00000011);
    //}

    //sTr0be BANK
    // The bank number is selected by reading from (or by writing any value to) specific addresses, the addresses and corresponding bank numbers are:
    // Size   Banks  FFF4 FFF5 FFF6 FFF7 FFF8 FFF9 FFFA FFFB
    // 2K,4K  1      -    -    -    -    -    -    -    -
    // 8K     2      -    -    -    -    0    1    -    -
    // 12K    3      -    -    -    -    0    1    2    -
    // 16K    4      -    -    0    1    2    3    -    -
    // 32K    8      0    1    2    3    4    5    6    7
    if (address >= 0x1200 && address <= 0x1FFF) {
        //TODO

        // Execute functions
    }
}
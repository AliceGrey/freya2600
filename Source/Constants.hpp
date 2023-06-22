#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstddef>
#include <cstdint>

constexpr size_t SCREEN_WIDTH = 160;
constexpr size_t SCREEN_HEIGHT = 192;
constexpr size_t SCREEN_BUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT * 3; // RGB

constexpr size_t DISPLAY_WIDTH = 320;
constexpr size_t DISPLAY_HEIGHT = 240;

//WRITE TIA
constexpr uint16_t ADDR_VSYNC   = 0x00;  // Write: VSYNC set-clear (D1)
constexpr uint16_t ADDR_VBLANK  = 0x01;  // Write: VBLANK set-clear (D7-6,D1)
constexpr uint16_t ADDR_WSYNC   = 0x02;  // Write: Wait for leading edge of hrz. blank (strobe)
constexpr uint16_t ADDR_RSYNC   = 0x03;  // Write: Reset hrz. sync counter (strobe)
constexpr uint16_t ADDR_NUSIZ0  = 0x04;  // Write: Number-size player-missle 0 (D5-0)
constexpr uint16_t ADDR_NUSIZ1  = 0x05;  // Write: Number-size player-missle 1 (D5-0)
constexpr uint16_t ADDR_COLUP0  = 0x06;  // Write: Color-lum player 0 (D7-1)
constexpr uint16_t ADDR_COLUP1  = 0x07;  // Write: Color-lum player 1 (D7-1)
constexpr uint16_t ADDR_COLUPF  = 0x08;  // Write: Color-lum playfield (D7-1)
constexpr uint16_t ADDR_COLUBK  = 0x09;  // Write: Color-lum background (D7-1)
constexpr uint16_t ADDR_CTRLPF  = 0x0A;  // Write: Contrl playfield ballsize & coll. (D5-4,D2-0)
constexpr uint16_t ADDR_REFP0   = 0x0B;  // Write: Reflect player 0 (D3)
constexpr uint16_t ADDR_REFP1   = 0x0C;  // Write: Reflect player 1 (D3)
constexpr uint16_t ADDR_PF0     = 0x0D;  // Write: Playfield register byte 0 (D7-4)
constexpr uint16_t ADDR_PF1     = 0x0E;  // Write: Playfield register byte 1 (D7-0)
constexpr uint16_t ADDR_PF2     = 0x0F;  // Write: Playfield register byte 2 (D7-0)
constexpr uint16_t ADDR_RESP0   = 0x10;  // Write: Reset player 0 (strobe)
constexpr uint16_t ADDR_RESP1   = 0x11;  // Write: Reset player 1 (strobe)
constexpr uint16_t ADDR_RESM0   = 0x12;  // Write: Reset missle 0 (strobe)
constexpr uint16_t ADDR_RESM1   = 0x13;  // Write: Reset missle 1 (strobe)
constexpr uint16_t ADDR_RESBL   = 0x14;  // Write: Reset ball (strobe)
constexpr uint16_t ADDR_AUDC0   = 0x15;  // Write: Audio control 0 (D3-0)
constexpr uint16_t ADDR_AUDC1   = 0x16;  // Write: Audio control 1 (D4-0)
constexpr uint16_t ADDR_AUDF0   = 0x17;  // Write: Audio frequency 0 (D4-0)
constexpr uint16_t ADDR_AUDF1   = 0x18;  // Write: Audio frequency 1 (D3-0)
constexpr uint16_t ADDR_AUDV0   = 0x19;  // Write: Audio volume 0 (D3-0)
constexpr uint16_t ADDR_AUDV1   = 0x1A;  // Write: Audio volume 1 (D3-0)
constexpr uint16_t ADDR_GRP0    = 0x1B;  // Write: Graphics player 0 (D7-0)
constexpr uint16_t ADDR_GRP1    = 0x1C;  // Write: Graphics player 1 (D7-0)
constexpr uint16_t ADDR_ENAM0   = 0x1D;  // Write: Graphics (enable) missle 0 (D1)
constexpr uint16_t ADDR_ENAM1   = 0x1E;  // Write: Graphics (enable) missle 1 (D1)
constexpr uint16_t ADDR_ENABL   = 0x1F;  // Write: Graphics (enable) ball (D1)
constexpr uint16_t ADDR_HMP0    = 0x20;  // Write: Horizontal motion player 0 (D7-4)
constexpr uint16_t ADDR_HMP1    = 0x21;  // Write: Horizontal motion player 1 (D7-4)
constexpr uint16_t ADDR_HMM0    = 0x22;  // Write: Horizontal motion missle 0 (D7-4)
constexpr uint16_t ADDR_HMM1    = 0x23;  // Write: Horizontal motion missle 1 (D7-4)
constexpr uint16_t ADDR_HMBL    = 0x24;  // Write: Horizontal motion ball (D7-4)
constexpr uint16_t ADDR_VDELP0  = 0x25;  // Write: Vertical delay player 0 (D0)
constexpr uint16_t ADDR_VDELP1  = 0x26;  // Write: Vertical delay player 1 (D0)
constexpr uint16_t ADDR_VDELBL  = 0x27;  // Write: Vertical delay ball (D0)
constexpr uint16_t ADDR_RESMP0  = 0x28;  // Write: Reset missle 0 to player 0 (D1)
constexpr uint16_t ADDR_RESMP1  = 0x29;  // Write: Reset missle 1 to player 1 (D1)
constexpr uint16_t ADDR_HMOVE   = 0x2A;  // Write: Apply horizontal motion (strobe)
constexpr uint16_t ADDR_HMCLR   = 0x2B;  // Write: Clear horizontal motion registers (strobe)
constexpr uint16_t ADDR_CXCLR   = 0x2C;  // Write: Clear collision latches (strobe)

//READ TIA
constexpr uint16_t ADDR_CXM0P   = 0x00;  // Read: Collision D7=(M0;P1); D6=(M0,P0)
constexpr uint16_t ADDR_CXM1P   = 0x01;  // Read: Collision D7=(M1;P0); D6=(M1,P1)
constexpr uint16_t ADDR_CXP0FB  = 0x02;  // Read: Collision D7=(P0;PF); D6=(P0,BL)
constexpr uint16_t ADDR_CXP1FB  = 0x03;  // Read: Collision D7=(P1;PF); D6=(P1;BL)
constexpr uint16_t ADDR_CXM0FB  = 0x04;  // Read: Collision D7=(M0;PF); D6=(M0;BL)
constexpr uint16_t ADDR_CXM1FB  = 0x05;  // Read: Collision D7=(M1;PF); D6=(M1;BL)
constexpr uint16_t ADDR_CXBLPF  = 0x06;  // Read: Collision D7=(BL;PF); D6=(unused)
constexpr uint16_t ADDR_CXPPMM  = 0x07;  // Read: Collision D7=(P0;P1); D6=(M0;M1)
constexpr uint16_t ADDR_INPT0   = 0x08;  // Read: Pot port D7
constexpr uint16_t ADDR_INPT1   = 0x09;  // Read: Pot port D7
constexpr uint16_t ADDR_INPT2   = 0x0A;  // Read: Pot port D7
constexpr uint16_t ADDR_INPT3   = 0x0B;  // Read: Pot port D7
constexpr uint16_t ADDR_INPT4   = 0x0C;  // Read: P1 joystick trigger: D7
constexpr uint16_t ADDR_INPT5   = 0x0D;  // Read: P2 joystick trigger: D7

#endif // CONSTANTS_HPP
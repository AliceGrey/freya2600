#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstddef>
#include <cstdint>

//WRITE TIA
constexpr size_t VSYNC   = 0x00;  // Write: VSYNC set-clear (D1)
constexpr size_t VBLANK  = 0x01;  // Write: VBLANK set-clear (D7-6,D1)
constexpr size_t WSYNC   = 0x02;  // Write: Wait for leading edge of hrz. blank (strobe)
constexpr size_t RSYNC   = 0x03;  // Write: Reset hrz. sync counter (strobe)
constexpr size_t NUSIZ0  = 0x04;  // Write: Number-size player-missle 0 (D5-0)
constexpr size_t NUSIZ1  = 0x05;  // Write: Number-size player-missle 1 (D5-0)
constexpr size_t COLUP0  = 0x06;  // Write: Color-lum player 0 (D7-1)
constexpr size_t COLUP1  = 0x07;  // Write: Color-lum player 1 (D7-1)
constexpr size_t COLUPF  = 0x08;  // Write: Color-lum playfield (D7-1)
constexpr size_t COLUBK  = 0x09;  // Write: Color-lum background (D7-1)
constexpr size_t CTRLPF  = 0x0A;  // Write: Contrl playfield ballsize & coll. (D5-4,D2-0)
constexpr size_t REFP0   = 0x0B;  // Write: Reflect player 0 (D3)
constexpr size_t REFP1   = 0x0C;  // Write: Reflect player 1 (D3)
constexpr size_t PF0     = 0x0D;  // Write: Playfield register byte 0 (D7-4)
constexpr size_t PF1     = 0x0E;  // Write: Playfield register byte 1 (D7-0)
constexpr size_t PF2     = 0x0F;  // Write: Playfield register byte 2 (D7-0)
constexpr size_t RESP0   = 0x10;  // Write: Reset player 0 (strobe)
constexpr size_t RESP1   = 0x11;  // Write: Reset player 1 (strobe)
constexpr size_t RESM0   = 0x12;  // Write: Reset missle 0 (strobe)
constexpr size_t RESM1   = 0x13;  // Write: Reset missle 1 (strobe)
constexpr size_t RESBL   = 0x14;  // Write: Reset ball (strobe)
constexpr size_t AUDC0   = 0x15;  // Write: Audio control 0 (D3-0)
constexpr size_t AUDC1   = 0x16;  // Write: Audio control 1 (D4-0)
constexpr size_t AUDF0   = 0x17;  // Write: Audio frequency 0 (D4-0)
constexpr size_t AUDF1   = 0x18;  // Write: Audio frequency 1 (D3-0)
constexpr size_t AUDV0   = 0x19;  // Write: Audio volume 0 (D3-0)
constexpr size_t AUDV1   = 0x1A;  // Write: Audio volume 1 (D3-0)
constexpr size_t GRP0    = 0x1B;  // Write: Graphics player 0 (D7-0)
constexpr size_t GRP1    = 0x1C;  // Write: Graphics player 1 (D7-0)
constexpr size_t ENAM0   = 0x1D;  // Write: Graphics (enable) missle 0 (D1)
constexpr size_t ENAM1   = 0x1E;  // Write: Graphics (enable) missle 1 (D1)
constexpr size_t ENABL   = 0x1F;  // Write: Graphics (enable) ball (D1)
constexpr size_t HMP0    = 0x20;  // Write: Horizontal motion player 0 (D7-4)
constexpr size_t HMP1    = 0x21;  // Write: Horizontal motion player 1 (D7-4)
constexpr size_t HMM0    = 0x22;  // Write: Horizontal motion missle 0 (D7-4)
constexpr size_t HMM1    = 0x23;  // Write: Horizontal motion missle 1 (D7-4)
constexpr size_t HMBL    = 0x24;  // Write: Horizontal motion ball (D7-4)
constexpr size_t VDELP0  = 0x25;  // Write: Vertical delay player 0 (D0)
constexpr size_t VDELP1  = 0x26;  // Write: Vertical delay player 1 (D0)
constexpr size_t VDELBL  = 0x27;  // Write: Vertical delay ball (D0)
constexpr size_t RESMP0  = 0x28;  // Write: Reset missle 0 to player 0 (D1)
constexpr size_t RESMP1  = 0x29;  // Write: Reset missle 1 to player 1 (D1)
constexpr size_t HMOVE   = 0x2A;  // Write: Apply horizontal motion (strobe)
constexpr size_t HMCLR   = 0x2B;  // Write: Clear horizontal motion registers (strobe)
constexpr size_t CXCLR   = 0x2C;  // Write: Clear collision latches (strobe)

//READ TIA
constexpr size_t CXM0P   = 0x00;  // Read: Collision D7=(M0;P1); D6=(M0,P0)
constexpr size_t CXM1P   = 0x01;  // Read: Collision D7=(M1;P0); D6=(M1,P1)
constexpr size_t CXP0FB  = 0x02;  // Read: Collision D7=(P0;PF); D6=(P0,BL)
constexpr size_t CXP1FB  = 0x03;  // Read: Collision D7=(P1;PF); D6=(P1;BL)
constexpr size_t CXM0FB  = 0x04;  // Read: Collision D7=(M0;PF); D6=(M0;BL)
constexpr size_t CXM1FB  = 0x05;  // Read: Collision D7=(M1;PF); D6=(M1;BL)
constexpr size_t CXBLPF  = 0x06;  // Read: Collision D7=(BL;PF); D6=(unused)
constexpr size_t CXPPMM  = 0x07;  // Read: Collision D7=(P0;P1); D6=(M0;M1)
constexpr size_t INPT0   = 0x08;  // Read: Pot port D7
constexpr size_t INPT1   = 0x09;  // Read: Pot port D7
constexpr size_t INPT2   = 0x0A;  // Read: Pot port D7
constexpr size_t INPT3   = 0x0B;  // Read: Pot port D7
constexpr size_t INPT4   = 0x0C;  // Read: P1 joystick trigger: D7
constexpr size_t INPT5   = 0x0D;  // Read: P2 joystick trigger: D7

#endif // CONSTANTS_HPP
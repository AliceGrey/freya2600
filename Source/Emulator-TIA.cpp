#include "Emulator.hpp"

#include <array>

constexpr uint8_t NTSC[128][3] = {
    { 0, 0, 0 },
    { 64, 64, 64 },
    { 108, 108, 108 },
    { 144, 144, 144 },
    { 176, 176, 176 },
    { 200, 200, 200 },
    { 220, 220, 220 },
    { 236, 236, 236 },

    { 68, 68, 0 },
    { 100, 100, 16 },
    { 132, 132, 36 },
    { 160, 160, 52 },
    { 184, 184, 64 },
    { 208, 208, 80 },
    { 232, 232, 92 },
    { 252, 252, 104 },

    { 112, 40, 0 },
    { 132, 68, 20 },
    { 152, 92, 40 },
    { 172, 120, 60 },
    { 188, 140, 76 },
    { 184, 156, 88 },
    { 220, 180, 104 },
    { 236, 200, 120 },

    { 132, 24, 0 },
    { 152, 52, 24 },
    { 172, 80, 48 },
    { 192, 104, 72 },
    { 208, 128, 92},
    { 224, 148, 112},
    { 236, 168, 128},
    { 252, 188, 148},

    { 136, 0, 0 },
    { 156, 32, 32 },
    { 176, 60, 60 },
    { 192, 88, 88 },
    { 208, 112, 112 },
    { 224, 136, 136 },
    { 236, 160, 160 },
    { 252, 180, 180 },

    { 120, 0, 92 },
    { 140, 32, 116 },
    { 160, 60, 136 },
    { 176, 88, 156 },
    { 192, 112, 176 },
    { 208, 132, 192 },
    { 220, 156, 208 },
    { 236, 176, 224 },

    { 72, 0, 120 },
    { 96, 32, 144 },
    { 120, 60, 164 },
    { 140, 88, 184 },
    { 160, 112, 204 },
    { 180, 132, 220 },
    { 196, 156, 236 },
    { 212, 176, 252 },

    { 20, 0, 132 },
    { 48, 32, 152 },
    { 76, 60, 172 },
    { 104, 88, 192 },
    { 124, 112, 208 },
    { 148, 136, 224 },
    { 168, 160, 236 },
    { 188, 180, 252 },
    
    { 0, 0, 136} ,
    { 28, 32, 156 },
    { 56, 64, 176 },
    { 80, 92, 192 },
    { 104, 116, 208 },
    { 124, 140, 224 },
    { 144, 164, 236 },
    { 164, 200, 252 },

    { 0, 24, 124 },
    { 28, 56, 144 },
    { 56, 84, 168 },
    { 80, 112, 188 },
    { 104, 136, 204 },
    { 124, 56, 220 },
    { 144, 180, 236 },
    { 164, 200, 252 },

    { 0, 44, 92 },
    { 28, 76, 120 },
    { 56, 104, 144 },
    { 80, 132, 172 },
    { 104, 156, 192 },
    { 124, 180, 212 },
    { 144, 204, 232 },
    { 164, 224, 252 },

    { 0, 60, 44 },
    { 28, 92, 72 },
    { 56, 124, 100 },
    { 80, 156, 128 },
    { 104, 180, 148 },
    { 124, 208, 172 },
    { 144, 228, 192 },
    { 164, 252, 212 },

    { 0, 60, 0 },
    { 32, 92, 32 },
    { 64, 124, 64 },
    { 92, 156, 92},
    { 116, 180, 116 },
    { 140, 208, 140 },
    { 164, 220, 164 },
    { 184, 252, 184 },

    { 20, 56, 0 },
    { 52, 92, 28 },
    { 80, 124, 56 },
    { 108, 152, 80 },
    { 132, 180, 104 },
    { 156, 204, 192 },
    { 180, 228, 144 }, 
    { 200, 252, 164 },

    { 44, 48, 0 },
    { 100, 72, 24 },
    { 104, 112, 52 },
    { 132, 140, 76 },
    { 156, 168, 100 },
    { 180, 192, 120 },
    { 180, 228, 144 },
    { 200, 252, 164 },

    { 68, 40, 0 },
    { 100, 72, 24 },
    { 132, 104, 48 },
    { 160, 132, 68 },
    { 184, 156, 88 },
    { 208, 180, 108 },
    { 236, 200, 120 },
    { 252, 224, 140 }
};

void Emulator::TickTIA()
{
    // The current column being computed
    static unsigned MemoryColumn = 0;
    if (MemoryLine <= VBLANK_CUTOFF){
        DrawState = IN_VBLANK;
    }
    if (MemoryColumn <= HBLANK_CUTOFF){
        DrawState = IN_HBLANK;
    }
    if (MemoryLine >= OVERSCAN_CUTOFF){
        DrawState = IN_OVERSCAN;
    }
    if (MemoryColumn >= HBLANK_CUTOFF && MemoryLine >= VBLANK_CUTOFF && MemoryLine < OVERSCAN_CUTOFF) {
        DrawState = VISIBLE;
    }

    if (MemoryColumn == 0) {
        WSYNC = false;
    }
    
    switch(DrawState){
        case IN_VBLANK:
            //TODO
            break;
        
        case IN_HBLANK:
            //TODO
            break;
        
        case IN_OVERSCAN:
            //TODO
            break;
        
        case VISIBLE:
            unsigned x = MemoryColumn - HBLANK_CUTOFF;
            unsigned y = MemoryLine - VBLANK_CUTOFF;
            unsigned offset = ((y * SCREEN_WIDTH) + x) * 3; // RGB

            auto& background = NTSC[COLUBK.Index];
            auto& playerField = NTSC[COLUPF.Index];

            ScreenBuffer[offset + 0] = background[0]; // R
            ScreenBuffer[offset + 1] = background[1]; // G
            ScreenBuffer[offset + 2] = background[2]; // B

            unsigned bits[] = {
                4, 5, 6, 7,
                7, 6, 5, 4, 3, 2, 1, 0,
                0, 1, 2, 3, 4, 5, 6, 7
            };

            // shhhh
            unsigned byte = 0;
            if (x >= 4) {
                byte = 1;
                if (x >= 12) {
                    byte = 2;
                }
            }

            if (PF[byte] & (1 << bits[x])) {
                ScreenBuffer[offset + 0] = playerField[0];
                ScreenBuffer[offset + 1] = playerField[1];
                ScreenBuffer[offset + 2] = playerField[2];   
            }

            //TODO

            // unsigned x = MemoryColumn - HBLANK_CUTOFF;
            // unsigned y = MemoryLine - VBLANK_CUTOFF;
            // unsigned offset = ((y * SCREEN_WIDTH) + x) * 3; // RGB

            // //O
            // if (x > 10 && x < 20 && y > 80 && y < 82) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 10 && x < 20 && y > 100 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 10 && x < 12 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 18 && x < 20 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // //H
            // if (x > 24 && x < 34 && y > 90 && y < 92) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 24 && x < 26 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 32 && x < 34 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // //H
            // if (x > 44 && x < 54 && y > 90 && y < 92) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 44 && x < 46 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 52 && x < 54 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }

            // //I
            // if (x > 54 && x < 64 && y > 100 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 58 && x < 60 && y > 80 && y < 102) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            // if (x > 54 && x < 64 && y > 80 && y < 82) {
            // ScreenBuffer[offset + 0] = 0; // R
            // ScreenBuffer[offset + 1] = 0; // G
            // ScreenBuffer[offset + 2] = 0; // B
            // break;
            // }
            
    }

    ++MemoryColumn;
    // Once we hit the end of the line
    if (MemoryColumn == 228) {
        MemoryColumn = 0;
        ++MemoryLine;

        // Once we hit the last line
        if (MemoryLine == 262) {
            MemoryLine = 0;
        }
    }
    
}

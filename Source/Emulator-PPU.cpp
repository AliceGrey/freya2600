#include "Emulator.hpp"

void Emulator::TickPPU()
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

            //O
            if (x > 10 && x < 20 && y > 80 && y < 82) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 10 && x < 20 && y > 100 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 10 && x < 12 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 18 && x < 20 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            //H
            if (x > 24 && x < 34 && y > 90 && y < 92) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 24 && x < 26 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 32 && x < 34 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            //H
            if (x > 44 && x < 54 && y > 90 && y < 92) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 44 && x < 46 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 52 && x < 54 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }

            //I
            if (x > 54 && x < 64 && y > 100 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 58 && x < 60 && y > 80 && y < 102) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            if (x > 54 && x < 64 && y > 80 && y < 82) {
            ScreenBuffer[offset + 0] = 0; // R
            ScreenBuffer[offset + 1] = 0; // G
            ScreenBuffer[offset + 2] = 0; // B
            break;
            }
            
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

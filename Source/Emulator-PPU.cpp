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

            ScreenBuffer[offset + 0] = y; // R
            ScreenBuffer[offset + 1] = y; // G
            ScreenBuffer[offset + 2] = x; // B
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

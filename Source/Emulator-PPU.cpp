#include "Emulator.hpp"

void Emulator::TickPPU()
{
    // The current column being computed
    static unsigned column = 0;

    ++column;
    // Once we hit the end of the line
    if (column == 228) {
        column = 0;

        ++ScanLine;
        // Once we hit the last line
        if (ScanLine == 262) {
            ScanLine = 0;
        }
    }
    // If we're in the display area
    if (ScanLine >= 40 && ScanLine <= 232 && column >= 68) {
        unsigned x = column - 68;
        unsigned y = ScanLine - 40;

        unsigned offset = ((y * SCREEN_WIDTH) + x) * 3; // RGB

        // //Official Test Pattern ;) 
        // if (y < 38 || y > 154) {
        //     ScreenBuffer[offset + 0] = 91; // R
        //     ScreenBuffer[offset + 1] = 206; // G
        //     ScreenBuffer[offset + 2] = 250; // B
        // }
        // else if (y < 76 || y > 116) {
        //     ScreenBuffer[offset + 0] = 245; // R
        //     ScreenBuffer[offset + 1] = 169; // G
        //     ScreenBuffer[offset + 2] = 184; // B
        // }
        // else {
        //     ScreenBuffer[offset + 0] = 255; // R
        //     ScreenBuffer[offset + 1] = 255; // G
        //     ScreenBuffer[offset + 2] = 255; // B
        // }
    }
}

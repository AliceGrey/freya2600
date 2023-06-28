#include "Emulator.hpp"

#include <array>

// The PIA (AKA RIOT) chip is an off-the-shelf 6532 Peripheral Interface Adaptor which has three functions: 
//  -a programmable timer, 
// - 128 bytes of RAM
// - two 8 bit parallel I/O ports. 


/*
The PIA uses the same clock as the microprocessor so that one PIA cycle
occurs for each machine cycle. The PIA can be set for one of four different
"intervals", where each interval is some multiple of the clock (and therefore
machine cycles). A value from 1 to 255 is loaded into the PIA which will be
decremented by one at each interval. The timer can now be read by the
microprocessor to determine elapsed time for timing various software
operations and keep them synchronized with the hardware (TIA chip).
*/
void Emulator::TickPIA()
{
    // if (WSYNC) {
    //     return;
    // }

    /*
    The timer is set by writing a value or count (from 1 to 255) to the address of
    the desired interval setting according to the following table :
    Hex Address Interval Mnemonic
    294 1 clock TIM1T
    295 8 clocks TIM8T
    296 64 clocks TIM64T
    297 1024 clocks T1024T
    
    For example, if the value of 100 were written to TIM64T (HEX address 296)
    the timer would decrement to 0 in 6400 clocks (64 clocks per interval x 100
    intervals) which would also be 6400 microprocessor machine cycles.

    When the timer reaches zero
    The PIA decrements the value or count loaded into it once each interval
    until it reaches 0. It holds that 0 counts for one interval, then the counter
    flips to FF(HEX) and decrements once each clock cycle, rather than once per
    interval. The purpose of this feature is to allow the programmer to
    determine how long ago the timer zeroed out in the event the timer was
    read after it passed zero.*/


    ++TimerCounter;
    if (TimerCounter >= TimerInterval) {
        TimerCounter = 0;

        if (INTIM == 0) {
            TimerInterval = 1;
            TIMINT.Timer = 1;
        }

        --INTIM;
    }
}

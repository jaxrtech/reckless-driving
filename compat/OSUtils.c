// http://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/Memory_Manag_nt_Utilities/memorymanutil/FunctionGroupIndex.html

#include <Carbon/Carbon.h>

#undef __i386__
#define __x86_64__
#include <SDL2/SDL_timer.h>
#undef __x86_64__
#define __i386__

#define TICKS_TO_MILLIS(X) ((X) * (1.f / 60.f * 1000.f))
#define MILLIS_TO_TICKS(X) ((X) / (1.f / 60.f * 1000.f))

/**
 * Delays execture for the specified amount of time.
 */
void Delay(
        unsigned long numTicks,
        unsigned long *finalTicks)
{
    *finalTicks = numTicks;
    SDL_Delay(TICKS_TO_MILLIS(numTicks));
}

/**
 * Obtains the current number of ticks (a tick is approximately 1/60 of a
 * second) since the system last started up.
 *
 * @remarks
 * The TickCount function returns an unsigned 32-bit integer that indicates the
 * current number of ticks since the system last started up. You can use this
 * value to compare the number of ticks that have elapsed since a given event or
 * other action occurred. For example, you could compare the current value
 * returned by TickCount with the value of the when field of an event structure.
 * <br/>
 * The tick count is incremented during the vertical retrace interrupt, but
 * this interrupt can be disabled. Your application should not rely on the
 * tick count to increment with absolute precision. Your application also should
 * not assume that the tick count always increments by 1 an interrupt task might
 * keep control for more than one tick. If your application keeps track of the
 * previous tick count and then compares this value with the current tick count,
 * your application should compare the two values by checking for a “greater
 * than or equal” condition rather than “equal to previous tick count plus 1.”
 * <br/>
 * Do not rely on the tick count being exact; it is usually accurate to within
 * one tick, but this level of accuracy is not guaranteed.
 */
UInt32 TickCount()
{
    return MILLIS_TO_TICKS(SDL_GetTicks());
}

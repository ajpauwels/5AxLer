//
//  Clock.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/10/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Clock.hpp"
#ifdef __APPLE__
#include <sys/time.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

using namespace mapmqp;

/**
 * Constructor simply initializes the timer at the current time
 * by calling delta()
 */
Clock::Clock() {
    delta();
}

/**
 * Gets the difference between the current time and the last
 * time delta was called (or if it's never been called, since
 * when the object was created). Sets the new start point to the
 * current time.
 *
 * @return A long giving the time elapsed (in milliseconds)
 */
long int Clock::delta() {
    long int currentTime = wallTime();
    long int delta = currentTime - prevTime;
    prevTime = currentTime;
    return delta;
}

/**
 * Gets the difference between the current time and the last
 * time delta was called (or if it's never been called, since
 * when the object was created). Does not reset the start point
 * to the current time.
 *
 * @return A long giving the time elapsed (in milliseconds)
 */
long int Clock::split() const {
    return wallTime() - prevTime;
}

/**
 * Gets the current system time, the number of milliseconds since
 * January 1, 1970 00:00:00
 *
 * @return A long giving the number of milliseconds since the epoch
 */
long int Clock::wallTime() {
#ifdef __APPLE__
    timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    return (currentTime.tv_sec * 1000) + (currentTime.tv_usec / 1000); //converts seconds and microseconds to milliseconds
#endif
#ifdef _WIN32
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    return (currentTime.wSecond * 1000) + currentTime.wMilliseconds;
#endif
}

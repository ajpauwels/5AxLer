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

Clock::Clock() {
    delta();
}

long int Clock::delta() {
    long int currentTime = wallTime();
    long int delta = currentTime - prevTime;
    prevTime = currentTime;
    return delta;
}

long int Clock::split() const {
    return wallTime() - prevTime;
}

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

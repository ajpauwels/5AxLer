//
//  Clock.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/10/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Clock.hpp"

#include <ctime>
#ifdef __APPLE__
#include <sys/time.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

using namespace mapmqp;
using namespace std;

Clock::Clock() {
    delta();
}

long int Clock::delta() {
    long int currentTime = epochTime();
    long int delta = currentTime - prevTime;
    prevTime = currentTime;
    return delta;
}

long int Clock::split() const {
    return epochTime() - prevTime;
}

long int Clock::epochTime() {
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

string Clock::wallTimeString(string dateSeperator, string dateTimeSeperator, string timeSeperator) {
    time_t rawTime;
    struct tm * timeInfo;
    char buffer[64];
    
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    
    string format = "%d" + dateSeperator + "%m" + dateSeperator + "%Y" + dateTimeSeperator + "%I" + timeSeperator + "%M" + timeSeperator + "%S";
    
    strftime(buffer, 64, format.c_str(), timeInfo);
    string str(buffer);
    
    return str;
}

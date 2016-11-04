//
//  Clock.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/10/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Clock_hpp
#define Clock_hpp

#include <string>

namespace mapmqp {
    class Clock {
    public:
        Clock();
        
        long int delta(); 		// Returns number of milliseconds since last delta call (or constructor) and resets delta
        long int split() const; // Returns number of milliseconds since last delta call, without resetting delta
        
        static long int epochTime(); //returns number of milliseconds from Jan 1, 1970, 00:00:00
        static std::string wallTimeString(std::string dateSeparator = "/", std::string dateTimeSeparator = " ", std::string timeSeparator = ":"); //returns current date and time in format DD/MM/YYYY HH:MM:SS, where "/", " ", and ":" are speficied in parameters
        
    private:
        long int prevTime = 0; // Last delta() call in milliseconds
    };
}

#endif /* Clock_hpp */

//
//  Clock.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/10/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Clock_hpp
#define Clock_hpp

namespace mapmqp {
    class Clock {
    public:
        Clock();
        
        long int delta(); 		// Returns number of milliseconds since last delta call (or constructor) and resets delta
        long int split() const; // Returns number of milliseconds since last delta call, without resetting delta
        
        static long int wallTime(); // Returns number of milliseconds from Jan 1, 1970, 00:00:00
        
    private:
        long int prevTime = 0; // Last delta() call in milliseconds
    };
}

#endif /* Clock_hpp */

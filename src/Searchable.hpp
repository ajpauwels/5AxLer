//
//  Searchable.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/21/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Searchable_hpp
#define Searchable_hpp

#include "Utility.hpp"

namespace mapmqp {
    template<typename T>
    class Searchable {
    public:
        typedef std::pair<uint64_t, uint16_t> ID;
        
        Searchable() {
            static uint64_t power = 1; //represents power of 2
            static uint16_t index = 0; //index of vector to check power of 2
            
            m_id = std::pair<uint64_t, uint16_t>(power, index);
            
            power <<= 1;
            if (power == 0) { //power has gone past range
                power = 1; //reset power
                index++; //increase index
                if (index == 0) {
                    writeLog(WARNING, "index of power of two has reached limit");
                }
            }
        }
        
        ID id() const {
            return m_id;
        }
        
        class Roster {
        public:
            Roster() { }
            
            void add(const T & element) {
                ID id = element.id();
                if (rosters.size() <= id.second) {
                    rosters.resize(id.second + 1);
                }
                rosters[id.second] |= id.first;
            }
            
            bool contains(const T & element) const {
                ID id = element.id();
                return (rosters.size() > id.second) ? (rosters[id.second] & id.first) : false;
            }
            
        private:
            std::vector<uint64_t> rosters;
        };
        
    private:
        ID m_id;
    };
}

#endif /* Searchable_hpp */

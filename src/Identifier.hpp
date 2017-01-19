//
//  Identifier.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/21/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Identifier_hpp
#define Identifier_hpp

#include "Utility.hpp"

namespace mapmqp {
    template<typename T>
    class Identifier {
    public:
        typedef std::pair<uint64_t, uint16_t> ID;
        
        Identifier() {
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
            
            void add(const Identifier<T> & element) {
                ID id = element.id();
                if (rosters.size() <= id.second) {
                    rosters.resize(id.second + 1);
                }
                if (rosters[id.second] & id.first) {
                    writeLog(WARNING, "adding element to Roster that already contains element");
                }
                rosters[id.second] |= id.first;
            }
            
            void remove(const Identifier<T> & element) {
                ID id = element.id();
                if ((rosters.size() <= id.second) || !(rosters[id.second] & id.first)) {
                    writeLog(WARNING, "removing element to Roster that does not contain element");
                } else {
                    rosters[id.second] &= ~id.first;
                }
            }
            
            bool contains(const Identifier<T> & element) const {
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

#endif /* Identifier_hpp */

//
//  BuildMapToMATLAB.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/9/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef BuildMapToMATLAB_hpp
#define BuildMapToMATLAB_hpp

#include <memory>
#include <string>

#include "BuildMap.hpp"

namespace mapmqp {
    class BuildMapToMATLAB {
    public:
        enum OutputType {
            PLANE,
            SPHERE
        };
        
        static bool parseBuildMapToMATLAB(std::string filePath, const BuildMap & buildMap, OutputType type, int precision = 1);
    };
}

#endif /* BuildMapToMATLAB_hpp */

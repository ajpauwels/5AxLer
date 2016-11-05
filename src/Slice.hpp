//
//  Slice.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Slice_hpp
#define Slice_hpp

#include <vector>

#include "Vector3D.hpp"
#include "Island.hpp"

namespace mapmqp {
    class Slice {
    public:
        Slice(const Plane & slicePlane);
        
        const std::vector<Island> & islands() const;
        const Plane & slicePlane() const;
        
        void addIsland(Island island);
        
    private:
        std::vector<Island> islands_;
        Plane slicePlane_;
    };
}

#endif /* Slice_hpp */

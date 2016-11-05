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
        Slice(Vector3D slicePlaneNormal, Vector3D slicePlaneOrigin);
        
        const std::vector<Island> & islands() const;
        Vector3D slicePlaneNormal() const;
        Vector3D slicePlaneOrigin() const;
        
        void addIsland(Island island);
        
    private:
        std::vector<Island> islands_;
        Vector3D slicePlaneNormal_, slicePlaneOrigin_;
    };
}

#endif /* Slice_hpp */

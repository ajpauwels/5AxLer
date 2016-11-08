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
        
        const Plane & slicePlane() const;
        const std::vector<std::shared_ptr<const Island>> & p_islands() const;
        
        void addIsland(std::shared_ptr<const Island> p_island);
        
    private:
        Plane m_slicePlane;
        std::vector<std::shared_ptr<const Island>> m_p_islands;
    };
}

#endif /* Slice_hpp */

//
//  Polygons.hpp
//  5AxLer
//
//  Created by Alexandre Pauwels on 11/28/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Polygons_hpp
#define Polygons_hpp

namespace mapmqp {
    class Polygons {
    public:
        // All polygons must be on the same plane
        Polygons(const std::vector<Polygon> & polygons);

        // Getter
        std::vector<Polygon> m_polygons();
        
    private:
        std::vector<Polygon> m_polygons;
    };
}

#endif /* Polygons_hpp */

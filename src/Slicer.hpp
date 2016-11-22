/**
 * Takes in a vector normal to the plane being used to slice and
 * some other parameters and continuously returns the next slice
 * of a Mesh.
 *
 * Created by Alexandre Pauwels on 11/5/2016
 * Copyright © 2016 MAP MQP. All rights reserved.
 */

#ifndef Slicer_hpp
#define Slicer_hpp

#include <memory>

#include "Vector3D.hpp"
#include "Plane.hpp"
#include "Mesh.hpp"
#include "Island.hpp"

namespace mapmqp {
	// Class definition
	class Slicer {
	public:
        struct Slice {
            Slice(Plane plane, std::vector<std::shared_ptr<const Island>> p_islands) :
            plane(plane), p_islands(p_islands) { }
            
            Plane plane;
            std::vector<std::shared_ptr<const Island>> p_islands;
        };
        
        // Constructor
        Slicer(std::shared_ptr<const Mesh> p_mesh);

	// Slice iterator
        Slice slice(const Plane & plane) const;

	private:
        //functions
        
        //slice plane with limited search space
        //returns slice and vector of ptrs to Mesh::Face that contained slice
        std::pair<Slice, std::vector<std::shared_ptr<const Mesh::Face>>> slice(const Plane & plane, const std::vector<std::shared_ptr<const Mesh::Face>> & p_facesSearchSpace) const;
        
        //variables
        
        std::shared_ptr<const Mesh> m_p_mesh;
	};
}

#endif /* Slicer_hpp */

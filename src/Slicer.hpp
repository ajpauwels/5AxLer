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
        class Slice {
        public:
            Slice(Plane plane, std::vector<std::shared_ptr<const Island>> p_islands) :
            m_plane(plane), m_p_islands(p_islands) { }

            std::vector<std::shared_ptr<const Island>> & islands();
            Plane plane() const;
            std::vector<std::shared_ptr<const Mesh::Face>> faces() const;
            
            // Returns a Polygon object in the shape of the slice
            std::vector<Polygon> toPoly();
        private:
            Plane m_plane;
            std::vector<std::shared_ptr<const Island>> m_p_islands;
        };
        
        // Constructor
        Slicer(std::shared_ptr<const Mesh> p_mesh);

        // Use to begin the slicing process and get the first slice in a particular orientation
        Slice slice(const Plane & plane);

        // Once the first slice is retrieved, use this function to iterate through the object
        Slice nextSlice();
        
    private:
        //functions
        
        //slice plane with limited search space
        //returns slice and vector of ptrs to Mesh::Face that contained slice
        std::pair<Slice, std::vector<std::shared_ptr<const Mesh::Face>>> slice(const Plane & plane, const std::vector<std::shared_ptr<const Mesh::Face>> & p_facesSearchSpace) const;
        
        //TODO this may not be the best way to pass data to this function
        std::vector<std::shared_ptr<const Mesh::Face>> expandSearchSpace(std::vector<std::shared_ptr<const Mesh::Face>> & p_facesSearchSpace, const Plane & originalPlane, const Plane & nextPlan) const;
        
        //variables
        std::shared_ptr<const Mesh> m_p_mesh;
        Plane m_originalSlicingPlane;
        Plane m_currentSlicingPlane;
        std::vector<std::shared_ptr<const Mesh::Face>> m_searchSpace;

    };
}

#endif /* Slicer_hpp */

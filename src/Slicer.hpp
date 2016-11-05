/**
 * Takes in a vector normal to the plane being used to slice and
 * some other parameters and continuously returns the next slice
 * of a Mesh.
 *
 * Created by Alexandre Pauwels on 11/5/2016
 * Copyright © 2016 MAP MQP. All rights reserved.
 */

namespace mapmqp {
	// Class definition
	class Slicer {
	public:
		// Constructor
		Slicer(std::shared_ptr<Mesh> p_m, Plane slicePlane);

		// Sets the height relative to the slicing plane that the next slice should be at
		void setHeight(uint32_t height);

		// Gets the next slice as defined by the sliceHeight variable
		Slice nextSlice();

	private:
		std::shared_ptr<Mesh> p_mesh_;
		Plane slicingPlane_;
		uint32_t sliceHeight_;
	};
}
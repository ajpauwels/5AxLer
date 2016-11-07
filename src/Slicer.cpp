#include "Slicer.hpp"

using namespace mqp;
using namespace std;

/**
 * Constructor sets the mesh and slicing plane variables, and sets the initial
 * slice height to 0
 *
 * @param p_m The mesh the slice
 * @param slicePlane The plane from which each slice will be taken
 */
Slicer::Slicer(std::shared_ptr<Mesh> p_m, Plane slicePlane) : p_mesh_(p_m), slicingPlane_(slicePlane) {
	// Set the initial slice height to be exactly where the plane is
	sliceHeight = 0;
}

/**
 * Sets the height relative to the plane's origin in microns,
 * where the z-axis is the normal to the plane.
 *
 * @param height The new height in microns
 */
void Slicer::setHeight(uint32_t height) {
	slicingHeight_ = height;
}

Slicer Slicer::nextSlice() {
	p_mesh_.findFaceAt()
}
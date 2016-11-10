//
//  BuildMap.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef BuildMap_hpp
#define BuildMap_hpp

#include "../libs/clipper/clipper.hpp"

#include "Utility.hpp"
#include "Vector3D.hpp"
#include "Angle.hpp"

namespace mapmqp {
    class BuildMap {
    public:
        BuildMap(Vector3D faceNormals[], double faceAreas[], int faceCount);
        
        bool solve();
        double area() const;
        bool checkVector(const Vector3D & v, bool includeEdges = true) const;
        Vector3D findValidVector() const;
        Vector3D findBestVector() const; //TODO parameters should be passed in neater fashion
        std::pair<double, bool> weighVector(const Vector3D & v) const;
        
        static int phiToAAxisRange(const Angle & phi);
        static int thetaToBAxisRange(const Angle & theta);
        static Angle aAxisValToPhi(double aAxisVal);
        static Angle bAxisValToTheta(double bAxisVal);
        
    private: //TODO make private - just for debugging
        //surface info
        Vector3D * m_faceNormals;
        double * m_faceAreas;
        unsigned int m_faceCount;
        
        //x->theta, y->phi
        ClipperLib::Paths m_buildMap2D;
        bool m_solved = false;
        
        Vector3D findValidVectorRecursive(int xStart, int yStart, int width, int height) const;
        Vector3D findBestVectorRecursive(int x, int y, int dx, int dy, double heuristic) const;
    };
}

#endif /* BuildMap_hpp */

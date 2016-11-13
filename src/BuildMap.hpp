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
        Vector3D findBestVector() const;
        double averageCuspHeight(const Vector3D & v) const;
        
        static Vector3D mapToVector(int x, int y);
        static std::pair<int, int> vector3DToMap(const Vector3D & v);
        static int thetaToBAxisRange(const Angle & theta);
        static int phiToAAxisRange(const Angle & phi);
        static Angle bAxisValToTheta(double bAxisVal);
        static Angle aAxisValToPhi(double aAxisVal);
        
    private:
        //surface info
        Vector3D * m_faceNormals;
        double * m_faceAreas;
        unsigned int m_faceCount;
        
        ClipperLib::Paths m_buildMap2D; //x->theta, y->phi
        bool m_solved = false;
        bool m_phiZeroAvailable = true; //whether or not the point at phi = 0 is true
        
        Vector3D findValidVectorRecursive(int xStart, int yStart, int width, int height) const;
        std::pair<Vector3D, double> findBestVectorRecursive(int x, int y, int dx, int dy, double prevHeuristic) const;
    };
}

#endif /* BuildMap_hpp */

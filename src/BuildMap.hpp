//
//  BuildMap.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef BuildMap_hpp
#define BuildMap_hpp

#include "Utility.hpp"
#include "Vector3D.hpp"
#include "Angle.hpp"
#include "libs/clipper.hpp"

namespace mapmqp {
    class BuildMap {
    public:
        BuildMap();
        
        bool checkOrientation(Angle theta, Angle phi, bool includeEdges = true);
        void addConstraintVector(Vector3D v);
        
        static int phiToAAxisRange(Angle phi);
        static int thetaToBAxisRange(Angle theta);
        static Angle aAxisValToPhi(double aAxisVal);
        static Angle bAxisValToTheta(double bAxisVal);
        
    //private: //TODO make private - just for debugging
        //x->theta, y->phi
        ClipperLib::Paths buildMap2D_, holes_;
        bool solved_ = false;
    };
}

#endif /* BuildMap_hpp */

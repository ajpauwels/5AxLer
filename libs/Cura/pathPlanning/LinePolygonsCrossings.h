/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef PATH_PLANNING_LINE_POLYGONS_CROSSINGS_H
#define PATH_PLANNING_LINE_POLYGONS_CROSSINGS_H

#include "../utils/polygon.h"

#include "CombPath.h"

namespace cura 
{

/*!
 * Class for generating a combing move action from point a to point b and avoiding collision with other parts when moving through air.
 * See LinePolygonsCrossings::comb.
 * 
 * The general implementation is by rotating everything such that the the line segment from a to b is aligned with the x-axis.
 * We call the line on which a and b lie the 'scanline'.
 * 
 * The basic path is generated by following the scanline until it hits a polygon, then follow the polygon until the last point where it hits the scanline, 
 * follow the scanline again, etc.
 * The path is offsetted from the polygons, so that it doesn't intersect with them.
 * 
 * Next the basic path is optimized by taking shortcuts where possible. Only shortcuts which skip a single point are considered, in order to reduce computational complexity.
 */
class LinePolygonsCrossings
{
private:
    
    /*!
     * A Crossing holds data on a single point where a polygon crosses the scanline.
     */
    struct Crossing
    {
        int64_t x; //!< x coordinate of crossings between the polygon and the scanline.
        unsigned int point_idx; //!< The index of the first point of the line segment which crosses the scanline
        
        /*!
         * Creates a Crossing with minimal initialization
         * \param x The x-coordinate in transformed space
         * \param point_idx The index of the first point of the line segment which crosses the scanline
         */
        Crossing(int64_t x, unsigned int point_idx)
        : x(x), point_idx(point_idx)
        {
        }
    };
    
    /*!
     * A PolyCrossings holds data on where a polygon crosses the scanline. Only the Crossing with lowest Crossing::x and highest are recorded.
     */
    struct PolyCrossings
    {
        unsigned int poly_idx; //!< The index of the polygon which crosses the scanline
        Crossing min; //!< The point where the polygon first crosses the scanline.
        Crossing max; //!< The point where the polygon last crosses the scanline.
        int n_crossings; //!< The number of times the polygon crossed the scanline.
        /*!
         * Create a PolyCrossings with minimal initialization. PolyCrossings::min and PolyCrossings::max are not yet computed.
         * \param poly_idx The index of the polygon in LinePolygonsCrossings::boundary
         */
        PolyCrossings(unsigned int poly_idx) 
        : poly_idx(poly_idx)
        , min(INT64_MAX, NO_INDEX), max(INT64_MIN, NO_INDEX) 
        , n_crossings(0)
        { 
        }
    };

    /*!
     * A PolyCrossings list: for every polygon a PolyCrossings.
     */
    struct PartCrossings : public std::vector<PolyCrossings>
    {
        //unsigned int part_idx;
    };
    
    
    PartCrossings crossings; //!< All crossings of polygons in the LinePolygonsCrossings::boundary with the scanline.
    unsigned int min_crossing_idx; //!< The index into LinePolygonsCrossings::crossings to the crossing with the minimal PolyCrossings::min crossing of all PolyCrossings's.
    unsigned int max_crossing_idx; //!< The index into LinePolygonsCrossings::crossings to the crossing with the maximal PolyCrossings::max crossing of all PolyCrossings's.
    
    Polygons& boundary; //!< The boundary not to cross during combing.
    Point startPoint; //!< The start point of the scanline.
    Point endPoint; //!< The end point of the scanline.
    
    int64_t dist_to_move_boundary_point_outside; //!< The distance used to move outside or inside so that a boundary point doesn't intersect with the boundary anymore. Neccesary due to computational rounding problems. Use negative value for insicde combing.
    
    PointMatrix transformation_matrix; //!< The transformation which rotates everything such that the scanline is aligned with the x-axis.
    Point transformed_startPoint; //!< The LinePolygonsCrossings::startPoint as transformed by Comb::transformation_matrix such that it has (roughly) the same Y as transformed_endPoint
    Point transformed_endPoint; //!< The LinePolygonsCrossings::endPoint as transformed by Comb::transformation_matrix such that it has (roughly) the same Y as transformed_startPoint

    
    /*!
     * Check if we are crossing the boundaries, and pre-calculate some values.
     * 
     * Sets Comb::transformation_matrix, Comb::transformed_startPoint and Comb::transformed_endPoint
     * \return Whether the line segment from LinePolygonsCrossings::startPoint to LinePolygonsCrossings::endPoint collides with the boundary
     */
    bool lineSegmentCollidesWithBoundary();
    
    /*!
     * Calculate Comb::crossings, Comb::min_crossing_idx and Comb::max_crossing_idx.
     * \param fail_on_unavoidable_obstacles When moving over other parts is inavoidable, stop calculation early and return false.
     * \return Whether combing succeeded, i.e. when fail_on_unavoidable_obstacles: we didn't cross any gaps/other parts
     */
    bool calcScanlineCrossings(bool fail_on_unavoidable_obstacles);
    
    /*! 
     * Get the basic combing path and optimize it.
     * 
     * \param combPath Output parameter: the points along the combing path.
     * \param fail_on_unavoidable_obstacles When moving over other parts is inavoidable, stop calculation early and return false.
     * \return Whether combing succeeded, i.e. we didn't cross any gaps/other parts
     */
    bool getCombingPath(CombPath& combPath, int64_t max_comb_distance_ignored, bool fail_on_unavoidable_obstacles);
    
    /*! 
     * Get the basic combing path, without shortcuts. The path goes straight toward the endPoint and follows the boundary when it hits it, until it passes the scanline again.
     * 
     * Walk trough the crossings, for every boundary we cross, find the initial cross point and the exit point. Then add all the points in between
     * to the \p combPath and continue with the next boundary we will cross, until there are no more boundaries to cross.
     * This gives a path from the start to finish curved around the holes that it encounters.
     * 
     * \param combPath Output parameter: the points along the combing path.
     */
    void getBasicCombingPath(CombPath& combPath);
    
    /*! 
     * Get the basic combing path, following a single boundary polygon when it hits it, until it passes the scanline again.
     * 
     * Find the initial cross point and the exit point. Then add all the points in between
     * to the \p combPath and continue with the next boundary we will cross, until there are no more boundaries to cross.
     * This gives a path from the start to finish curved around the polygon that it encounters.
     * 
     * \param combPath Output parameter: where to add the points along the combing path.
     */
    void getBasicCombingPath(PolyCrossings& crossings, CombPath& combPath);
    
    /*!
     * Find the first polygon cutting the scanline after \p x.
     * 
     * Note that this function only looks at the first segment cutting the scanline (see Comb::minX)!
     * It doesn't return the next polygon which crosses the scanline, but the first polygon crossing the scanline for the first time.
     * 
     * \param x The point on the scanline from where to look.
     * \return The next PolyCrossings fully beyond \p x or one with PolyCrossings::poly_idx set to NO_INDEX if there's none left.
     */
    PolyCrossings* getNextPolygonAlongScanline(int64_t x);
    
    /*!
     * Optimize the \p comb_path: skip each point we could already reach by not crossing a boundary. This smooths out the path and makes it skip some unneeded corners.
     * 
     * \param comb_path The unoptimized combing path.
     * \param optimized_comb_path Output parameter: The points of optimized combing path
     * \return Whether it turns out that the basic comb path already crossed a boundary
     */
    bool optimizePath(CombPath& comb_path, CombPath& optimized_comb_path);
    
    /*!
     * Create a LinePolygonsCrossings with minimal initialization.
     * \param boundary The boundary which not to cross during combing
     * \param start the starting point
     * \param end the end point
     * \param dist_to_move_boundary_point_outside Distance used to move a point from a boundary so that it doesn't intersect with it anymore. (Precision issue)
     */
    LinePolygonsCrossings(Polygons& boundary, Point& start, Point& end, int64_t dist_to_move_boundary_point_outside)
    : boundary(boundary), startPoint(start), endPoint(end), dist_to_move_boundary_point_outside(dist_to_move_boundary_point_outside)
    {
    }
    
public: 
    
    /*!
     * The main function of this class: calculate one combing path within the boundary.
     * \param boundary The polygons to follow when calculating the basic combing path
     * \param startPoint From where to start the combing move.
     * \param endPoint Where to end the combing move.
     * \param combPath Output parameter: the combing path generated.
     * \param fail_on_unavoidable_obstacles When moving over other parts is inavoidable, stop calculation early and return false.
     * \return Whether combing succeeded, i.e. we didn't cross any gaps/other parts
     */
    static bool comb(Polygons& boundary, Point startPoint, Point endPoint, CombPath& combPath, int64_t dist_to_move_boundary_point_outside, int64_t max_comb_distance_ignored, bool fail_on_unavoidable_obstacles)
    {
        LinePolygonsCrossings linePolygonsCrossings(boundary, startPoint, endPoint, dist_to_move_boundary_point_outside);
        return linePolygonsCrossings.getCombingPath(combPath, max_comb_distance_ignored, fail_on_unavoidable_obstacles);
    };
};

}//namespace cura

#endif//PATH_PLANNING_LINE_POLYGONS_CROSSINGS_H

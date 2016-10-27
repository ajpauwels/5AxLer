/** Copyright (C) 2016 Ultimaker - Released under terms of the AGPLv3 License */
#ifndef UTILS_LIST_POLY_IT_H
#define UTILS_LIST_POLY_IT_H

#include <vector>
#include <list>


#include "intpoint.h"
#include "polygon.h"


namespace cura 
{

/*!
 * A wrapper class for a ListPolygon::iterator and a reference to the containing ListPolygon
 */
class ListPolyIt
{
public:
    ListPolygon* poly; //!< The polygon
    ListPolygon::iterator it; //!< The iterator into ListPolyIt::poly
    ListPolyIt(const ListPolyIt& other)
    : poly(other.poly)
    , it(other.it)
    {
    }
    ListPolyIt(ListPolygon& poly, ListPolygon::iterator it)
    : poly(&poly)
    , it(it)
    {
    }
    Point& p() const
    {
        return *it;
    }
    /*!
     * Test whether two iterators refer to the same polygon in the same polygon list.
     * 
     * \param other The ListPolyIt to test for equality
     * \return Wether the right argument refers to the same polygon in the same ListPolygon as the left argument.
     */
    bool operator==(const ListPolyIt& other) const
    {
        return poly == other.poly && it == other.it;
    }
    bool operator!=(const ListPolyIt& other) const
    {
        return poly != other.poly || it != other.it;
    }
    void operator=(const ListPolyIt& other)
    {
        poly = other.poly;
        it = other.it;
    }
    //! move the iterator forward (and wrap around at the end)
    ListPolyIt& operator++() 
    { 
        ++it; 
        if (it == poly->end()) { it = poly->begin(); }
        return *this; 
    }
    //! move the iterator backward (and wrap around at the beginning)
    ListPolyIt& operator--() 
    { 
        if (it == poly->begin()) { it = poly->end(); }
        --it; 
        return *this; 
    }
    //! move the iterator forward (and wrap around at the end)
    ListPolyIt next() const 
    {
        ListPolyIt ret(*this);
        ++ret;
        return ret;
    }
    //! move the iterator backward (and wrap around at the beginning)
    ListPolyIt prev() const 
    {
        ListPolyIt ret(*this);
        --ret;
        return ret;
    }
    //! Remove this point from the list polygon
    void remove() const
    {
        poly->erase(it);
    }
    /*!
     * Convert Polygons to ListPolygons
     * 
     * \param polys The polygons to convert
     * \param result The converted polygons
     */
    static void convertPolygonsToLists(Polygons& polys, ListPolygons& result);
    /*!
     * Convert Polygons to ListPolygons
     * 
     * \param polys The polygons to convert
     * \param result The converted polygons
     */
    static void convertPolygonToList(PolygonRef poly, ListPolygon& result);
    /*!
     * Convert ListPolygons to Polygons
     * 
     * \param list_polygons The polygons to convert
     * \param polygons The converted polygons
     */
    static void convertListPolygonsToPolygons(ListPolygons& list_polygons, Polygons& polygons);
    /*!
     * Convert ListPolygons to Polygons
     * 
     * \param list_polygons The polygons to convert
     * \param polygons The converted polygons
     */
    static void convertListPolygonToPolygon(ListPolygon& list_polygon, PolygonRef polygon);
};


}//namespace cura

namespace std
{
/*!
 * Hash function for \ref ListPolyIt
 */
template <>
struct hash<cura::ListPolyIt>
{
    size_t operator()(const cura::ListPolyIt& lpi) const
    {
        return std::hash<cura::Point>()(lpi.p());
    }
};
}//namespace std



#endif//UTILS_LIST_POLY_IT_H

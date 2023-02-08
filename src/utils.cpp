/**
 * @file utils.hpp
 * @author Nilusink
 * @brief A few useful function, mosly for numerical interpolation
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright FrenchBakery (c) 2023
 * 
 */
#include "utils.hpp"


float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    // magic
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


point_t lerp(float t, point_t a, point_t b)
{
    point_t out;
    out.x = t * (a.x - b.x) + a.x;
    out.y = t * (a.y - b.y) + a.y;

    return out;
}


point_t bezier(float t, point_t start, point_t end, point_t middle)
{
    // calculate first pair of points
    point_t sm = lerp(t, start, middle);
    point_t me = lerp(t, middle, end);
    
    // calculate final point
    return lerp(t, sm, me);
}

point_t bezier(float t, point_t start, point_t end, point_t a, point_t b)
{
    // calculate first three points
    point_t sa = lerp(t, start, a);
    point_t ab = lerp(t, a, b);
    point_t be = lerp(t, b, end);

    // calculate second two points
    point_t sa_ab = lerp(t, sa, ab);
    point_t ab_be = lerp(t, ab, be);

    // calculate final point
    return lerp(t, sa_ab, ab_be);
}

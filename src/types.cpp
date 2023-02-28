/**
 * @file types.hpp
 * @author Nilusink
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright FrenchBakery (c) 2023
 * 
 */
#include "types.hpp"
using namespace Geometry;


// private
void vec2_t::recalculate(int from)
{
    switch (from)
    {
        case 0:  // from cartesian
        {
            l = sqrt(pow(p.x, 2) + pow(p.y, 2));
            d = tan(p.y / p.x);
            break;
        }

        case 1:  // from polar
        {
            p.x = l * sin(d);
            p.y = l * cos(d);
            break;
        }
    }
}


// public
vec2_t::vec2_t(float angle, float length)
    : d(angle), l(length)
{
    // recalculate x and y values
    recalculate(1);
}

vec2_t::vec2_t(const point_t &c)
    : p(c)
{
    // recalculate length and angle
    recalculate(0);
}

vec2_t::vec2_t()
    : d(0), l(0)
{
    point_t p;
    p.x = 0;
    p.y = 0;
};


// property getters and setters
float vec2_t::length()
{
    return l;
}

void vec2_t::setLength(float value)
{
    l = value;
    recalculate(1);
}


float vec2_t::angle()
{
    return d;
}

void vec2_t::setAngle(float value)
{
    d = value;
    recalculate(1);
}


const point_t &vec2_t::cartesian()
{
    return p;
}

void vec2_t::setCartesian(point_t value)
{
    p = value;
    recalculate(0);
}


float vec2_t::x()
{
    return p.x;
}

void vec2_t::setX(float value)
{
    p.x = value;
    recalculate(0);
}


float vec2_t::y()
{
    return p.y;
}

void vec2_t::setY(float value)
{
    p.y = value;
    recalculate(0);
}

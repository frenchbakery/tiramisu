/**
 * @file types.hpp
 * @author Nilusink
 * @brief contains a few useful types
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright FrenchBakery (c) 2023
 * 
 */
#include <cmath>


namespace Geometry
{
    /**
     * @brief a simple two dimensional point containing a x and a y value
     * 
     */
    struct point_t
    {
        float x;
        float y;
    };

    /**
     * @brief simple two dimensional vector class
     */
    struct vec2_t
    {
        private:
            float d;
            float l;
        
            point_t p;

        void recalculate(int from);

        public:
            vec2_t(float angle, float length);
            vec2_t(const point_t &c);
            vec2_t();

            // polar stuff
            float length();
            void setLength(float value);

            float angle();
            void setAngle(float value);

            // cartesian stuff
            /**
             * @brief get the cartesian form of the vector (x and y)
             * 
             * @return Point 
             */
            const point_t &cartesian();

            /**
             * @brief set the cartesian form of the vector (x, y)
             */
            void setCartesian(point_t value);

            float x();
            void setX(float value);

            float y();
            void setY(float value);
    };
}

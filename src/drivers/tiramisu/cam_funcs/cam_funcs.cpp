#include "cam_funcs.hpp"

#include "global_objects.hpp"
#include <kipr/create/create.h>
#include <kipr/camera/camera.h>
#include <kipr/time/time.h>
#include <iostream>


#define CAM_MIN 0
#define CAM_MAX 320
#define CAM_FOV_DEG 47.583  // degrees
#define CAM_FOV 0.83 // radian
#define CAM_FOV_2 0.412 // radian / 2
#define MIN_SPEED 30
#define TOLERANCE 2
#define AIM_AT (CAM_MAX - CAM_MIN) / 2
#define MAX_Y 40


double Cam::look_at(uint8_t channel_id, bool forward)
{
    camera_update();
    int y = get_object_count(channel_id);

    if (y > 0)
    {
        int curr_ma_i = 0;
        int curr_ma = get_object_area(channel_id, 0);

        // find biggest box
        for (int i = 0; i < y; i++)
        {
            int ar = get_object_area(channel_id, i);
            if (ar > curr_ma && get_object_center_y(channel_id, i) < MAX_Y)
            {
                curr_ma = ar;
                curr_ma_i = i;
            }
        }

        // center on box
        int cen_x = get_object_center_x(channel_id, curr_ma_i);
        int cen_y = get_object_center_y(channel_id, curr_ma_i);

        std::cout << "off: " << cen_x - AIM_AT << ", x: " << cen_x << "\tfw: " << forward << "\t";

        // regulate to 32
        int off = (cen_x - AIM_AT);
        int ccw_speed = (std::abs(off) > MIN_SPEED) ? -off : MIN_SPEED;
        int cw_speed = (std::abs(off) > MIN_SPEED) ? off : MIN_SPEED;

        // calculate offset angle
        float off_a = -CAM_FOV_2 * ((2.f * off) / (CAM_MAX - CAM_MIN));

        std::cout << "rotating by: " << off_a * (180 / M_PI) << "°\n";
        return off_a * .6;
    }
    else
    {
        std::cout << "nothing found\n";
        return 69420.f;
    }
}

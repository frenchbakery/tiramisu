#include "cam_funcs.hpp"

#include <kipr/create/create.h>
#include <kipr/camera/camera.h>
#include <iostream>


#define MIN_SPEED 20
#define TOLERANCE 2
#define AIM_AT 80
#define MAX_Y 40


void Cam::look_at(uint8_t channel_id, bool forward)
{
    for (;;)
        {
            camera_update();
            int y = get_object_count(YELLOW_CHANNEL);

            if (y > 0)
            {
                int curr_ma_i = 0;
                int curr_ma = get_object_area(YELLOW_CHANNEL, 0);

                // find biggest box
                for (int i = 0; i < y; i++)
                {
                    int ar = get_object_area(YELLOW_CHANNEL, i);
                    if (ar > curr_ma && get_object_center_y(YELLOW_CHANNEL, i) < MAX_Y)
                    {
                        curr_ma = ar;
                        curr_ma_i = i;
                    }
                }

                // center on box
                int cen_x = get_object_center_x(YELLOW_CHANNEL, curr_ma_i);
                int cen_y = get_object_center_y(YELLOW_CHANNEL, curr_ma_i);

                std::cout << "off: " << cen_x - AIM_AT << ", height: " << cen_y << "\n";
                // regulate to 32
                int speed = cen_x - AIM_AT;
                int ccw_speed = (std::abs(speed) > MIN_SPEED) ? -speed : MIN_SPEED;
                int cw_speed = (std::abs(speed) > MIN_SPEED) ? speed : MIN_SPEED;

                if (cen_x < AIM_AT - TOLERANCE)
                {
                    if (forward)
                    {
                        create_drive_direct(cw_speed, ccw_speed);
                    }
                    else
                    {
                        create_spin_CCW(ccw_speed);
                    }
                }
                else if (cen_x > AIM_AT + TOLERANCE)
                {
                    if (forward)
                    {
                        create_drive_direct(cw_speed, ccw_speed);
                    }
                    else
                    {
                        create_spin_CW(cw_speed);
                    }
                }
                else
                {
                    // target found
                    std::cout << "found object: " << cen_x << "\n";
                    return;
                }
            }
            else
            {
                std::cout << "nothing found\n";

                if (forward)
                    return;
            }
        }
}
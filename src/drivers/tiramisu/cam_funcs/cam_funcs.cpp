#include "cam_funcs.hpp"

#include <kipr/create/create.h>
#include <kipr/camera/camera.h>
#include <iostream>


#define MIN_SPEED 20


void Cam::look_at(uint8_t channel_id)
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
                    if (ar > curr_ma)
                    {
                        curr_ma = ar;
                        curr_ma_i = i;
                    }
                }

                // center on box
                int cen_x = get_object_center_x(YELLOW_CHANNEL, curr_ma_i);
                int cen_y = get_object_center_y(YELLOW_CHANNEL, curr_ma_i);

                std::cout << "bbox: " << cen_x << ", " << cen_y << " s: " << curr_ma << "\n";
                // regulate to 32
                if (cen_x < 78)
                {
                    create_spin_CCW((cen_x < 80 - MIN_SPEED) ? 80 - cen_x : MIN_SPEED);
                }
                else if (cen_x > 82)
                {
                    create_spin_CW((cen_x > 90 + MIN_SPEED) ? cen_x - 80 : MIN_SPEED);
                }
                else
                {
                    // target found
                    std::cout << "found object: " << cen_x << "\n";
                    create_stop();
                    return;
                }
            }
            else
            {
                create_stop();
                std::cout << "nothing found\n";
            }
        }
}
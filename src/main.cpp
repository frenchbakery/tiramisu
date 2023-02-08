#include "drivers/tiramisu/arm/arm.hpp"
#include <kipr/util.h>
#include <iostream>


int main()
{
    Motor tm(0);
    Servo ss(0);
    Digital mas(8);
    Digital mis(9);

    Arm my_arm(tm, ss, mas, mis);

    std::cout << "calibrating\n";

    my_arm.calibrate();

    std::cout << "done calibrating\n";

    my_arm.moveEllbowTo(100);

    // my_arm.moveEllbowTo(75, 1024);
    // msleep(1000);
    // my_arm.moveEllbowTo(20, 1024);
    // msleep(1000);
    // my_arm.moveEllbowTo(10, 1024);
    // msleep(1000);

    // my_arm.moveShoulderToAngle(30);
    // msleep(3000);

    // my_arm.moveShoulderToAngle(100);
    // msleep(3000);

    std::cout << "moved to half\n";

    return 0;
}
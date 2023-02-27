#include "drivers/tiramisu/grepper/grepper.hpp"
#include "drivers/tiramisu/arm/arm.hpp"
#include <kipr/botball/botball.h>
#include <kipr/analog/analog.hpp>
#include <kipr/create/create.h>
#include <kipr/time/time.h>
#include <iostream>

using namespace kipr::motor;
using namespace kipr::servo;
using namespace kipr::digital;
using namespace kipr::analog;


int main()
{
    // shut_down_in(115);

    // initialize arm
    Motor tm(0);
    BackEMF tmemf(0);
    Servo ss(0);
    Digital mas(8);
    Digital mis(9);

    Arm my_arm(tmemf, tm, ss, mas, mis);

    // Digital end_sensor(0);
    // Motor l_motor(2);
    // Motor r_motor(3);
    // Servo grab_servo(2);

    // Gripper gripper(grab_servo, l_motor, r_motor, end_sensor);

    Analog dist_sens(0);
    Servo grab_serv(2);
    Servo turn_serv(3);

    grab_serv.enable();
    turn_serv.enable();

    turn_serv.setPosition(1024);
    grab_serv.setPosition(1630);

    if (!create_connect())
    {
        my_arm.calibrate();
        std::cout << "done calibrating\n";

        my_arm.moveEllbowTo(45);

        // create_start();

        // program
        // get closer
        create_drive_straight(40);
        while (dist_sens.value() < 2500) { msleep(10); };
        std::cout << "lower";
        create_drive_straight(0);
 
        grab_serv.setPosition(1100);
        msleep(500);
        my_arm.moveEllbowTo(80);
        create_drive_straight(-100);
        msleep(2000);
        create_drive_straight(0);

        create_spin_block(100, 180);

        // turn around and 
   }
    

    std::cout << "done\n";

    return 0;
}
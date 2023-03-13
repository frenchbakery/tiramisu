/**
 * @file main.cpp
 * @author Nilusink
 * @brief contains tiramisu entry point
 * @version 0.1
 * @date 2023-03-10
 * 
 * @copyright Copyright FrenchBakery(c) 2023
 * 
 */

#include <kipr/botball/botball.h>
#include <kipr/analog/analog.hpp>
#include <kipr/create/create.h>
#include <kipr/create/create.hpp>
#include <kipr/camera/camera.h>
#include <kipr/time/time.h>
#include <kiprplus/create_motor.hpp>
#include <kiprplus/aggregation_engine.hpp>
#include <iostream>
#include <iomanip>
#include <thread>
#include "pitches.h"
#include "drivers/navigation/tiramisu/tinav.hpp"
#include "drivers/tiramisu/cam_funcs/cam_funcs.hpp"
#include "drivers/tiramisu/grepper/grepper.hpp"
#include "drivers/tiramisu/arm/arm.hpp"

using namespace kipr::motor;
using namespace kipr::servo;
using namespace kipr::digital;
using namespace kipr::analog;

#define ANGLE_MUlT 1 / 1.3


namespace go
{
    Navigation *nav = nullptr;
};



void grab_cube(Analog &dist_sens, Servo &grab_serv, Arm &my_arm)
{
    grab_serv.setPosition(1630);

    my_arm.moveEllbowTo(48);
    my_arm.moveShoulderToAngle(115);

    std::cout << "set stuff\n";

    Cam::look_at(YELLOW_CHANNEL);

    std::cout << "centered\n";

    // program
    // get closer
    create_drive_straight(20);
    while (dist_sens.value() < 2500)
    {
        msleep(10);
        create_drive_straight(40);
    };
    create_stop();

    grab_serv.setPosition(1000);
    msleep(500);
    my_arm.moveEllbowTo(80);
    create_drive_straight(-100);
    msleep(1500);
    create_stop();
}




int main()
{
    kp::CreateMotor::globalCreateConnect();
    go::nav = new TINav;
    go::nav->initialize();
    msleep(1000);

    go::nav->driveVector(el::cart_t(20, -20));
    go::nav->rotateBy(M_PI_2);
    go::nav->rotateBy(-M_PI_2);
    go::nav->startSequence();
    go::nav->awaitSequenceComplete();
    msleep(1000);

    /*for (;;)
    {
        int x, y;
        std::cout << "x, y = ";
        scanf("%d, %d", &x, &y);
        if (x == 1234) break;
        go::nav->driveVector(el::cart_t(x, y));
    }*/


    /*go::nav->driveDistance(20);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->rotateBy(M_PI_2);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->driveDistance(20);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->rotateBy(M_PI_2);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->driveDistance(20);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->rotateBy(M_PI_2);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->driveDistance(20);
    go::nav->awaitTargetReached();
    msleep(1000);
    go::nav->rotateBy(M_PI_2);
    go::nav->awaitTargetReached();*/
    
    create_stop();
    msleep(1000);
    go::nav->terminate();
    kp::CreateMotor::globalCreateDisconnect();
    delete go::nav;

    return 0;
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

    int c_open = camera_open_device_model_at_res(0, BLACK_2017, Resolution::LOW_RES);
    camera_load_config("cubes.conf");

    Analog dist_sens(0);
    Servo grab_serv(2);
    Servo turn_serv(3);

    grab_serv.enable();
    turn_serv.enable();

    turn_serv.setPosition(1024);
    grab_serv.setPosition(1630);

    get_create_total_angle();

    if (!create_connect() && c_open)
    {
        // ss.setPosition(1900);
        // return 0;

        my_arm.calibrate();
        std::cout << "done calibrating\n";

        // center on cube
        grab_cube(dist_sens, grab_serv, my_arm);

        create_spin_block(100, -180 * ANGLE_MUlT);
        create_stop();

        // drop first cube
        my_arm.moveEllbowTo(10);
        my_arm.moveShoulderToAngle(30);
        msleep(1000);

        grab_serv.setPosition(1630);
        msleep(1000);

        my_arm.moveShoulderToAngle(110);
        grab_serv.setPosition(1100);
        create_spin_block(100, 80 * ANGLE_MUlT);

        // drive to second cube
        set_create_distance(0);
        create_drive_straight(100);
        while (get_create_distance() < 900)
        {
            msleep(10);
        };
        create_stop();
        std::cout << "drove 900: " << get_create_distance() << "\n";

        create_spin_block(100, 90 * ANGLE_MUlT);
        create_stop();

        // grab second cube
        grab_cube(dist_sens, grab_serv, my_arm);

        create_spin_block(100, -90 * ANGLE_MUlT);
        set_create_distance(0);
        create_drive_straight(-100);
        while (get_create_distance() > -900)
        {
            msleep(10);
        };
        create_stop();

        create_spin_block(100, -90 * ANGLE_MUlT);
        create_stop();

        // drop second cube
        my_arm.moveEllbowTo(10);
        my_arm.moveShoulderToAngle(40);
        msleep(1000);

        grab_serv.setPosition(1630);
        msleep(1000);

        my_arm.moveShoulderToAngle(110);
        grab_serv.setPosition(1100);

        msleep(1000);
        // turn around and
    }

    std::cout << "done\n";
    create_disconnect();
    camera_close();
    create_stop();

    return 0;
}
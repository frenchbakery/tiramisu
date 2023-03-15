#include <kipr/wait_for/wait_for.h>
#include <kipr/analog/analog.hpp>
#include <kipr/create/create.h>
#include <kipr/camera/camera.h>
#include <kipr/time/time.h>
#include <iostream>
#include <thread>
#include "drivers/tiramisu/ball_sorter/ball_sorter.hpp"
#include "drivers/tiramisu/arm/arm.hpp"
#include "drivers/tiramisu/cam_funcs/cam_funcs.hpp"
#include "routines/create.hpp"
#include "global_objects.hpp"
#include "term_colors.h"

#define BALL_MOTOR_PORT 1
#define BALL_SERVO_PIN 0
#define BALL_END_PIN 9

#define ARM_ELLBOW_PIN 0
#define ARM_SHOULDER_PIN 3 // + up
#define ARM_GRAB_PIN 1 // 2047 closed
#define ARM_WRIST_PIN 2 // 0 up
#define ARM_MAX_PIN 8
#define ARM_MIN_PIN 7
#define LIGHT_PIN 5
#define DIST_PIN 0


#define LIGHT_CALIB_ACCURACY 100
int ambient_light = -1;
int light_range = -1;


namespace go
{
    TINav *nav;
    Arm *arm;
}


// void grab_cube(Analog &dist_sens, Servo &grab_serv, Arm &my_arm)
// {
//     grab_serv.setPosition(1630);

//     my_go::arm->moveEllbowTo(70);
//     my_go::arm->moveShoulderToAngle(120);

//     std::cout << "set stuff\n";

//     Cam::look_at(YELLOW_CHANNEL);

//     std::cout << "centered\n";

//     // program
//     // get closer

//     grab_serv.setPosition(1000);
//     msleep(500);
//     my_go::arm->moveEllbowTo(90);
//     create_drive_straight(-100);
//     msleep(1500);
//     create_stop();
// }


int main()
{
    int current_stack = 0;
    go::nav = new TINav;
    go::nav->initialize();

    go::nav->rotateBy(M_PI * 4);
    msleep(10000);

    go::nav->rotateBy(-M_PI * 4);
    msleep(10000);

    return 0;
    camera_open_device_model_at_res(0, BLACK_2017, Resolution::MED_RES);
    camera_load_config("cubes.conf");

    BallSorter balls(BALL_MOTOR_PORT, BALL_SERVO_PIN, BALL_END_PIN);


    kipr::digital::Digital dist(DIST_PIN);
    kipr::analog::Analog light(LIGHT_PIN);

    go::arm = new Arm(
        ARM_ELLBOW_PIN,
        ARM_SHOULDER_PIN,
        ARM_WRIST_PIN,
        ARM_GRAB_PIN,
        ARM_MAX_PIN,
        ARM_MIN_PIN
    );

    std::cout << "initialized\n";

    std::thread bal_cal_thread(&BallSorter::calibrate, &balls);
    std::cout << "thread on\n";
    go::arm->calibrate();

    // home create
    reset_position();

    go::arm->moveGripperTo(50);

    std::cout << CLR_BLUE << "calibrated" << CLR_RESET << std::endl;
    go::arm->park();

    std::cout << CLR_BLUE << "parked" << CLR_RESET << std::endl;

    // calibrate ligtht sensor
    int sum = 0;
    int now_val;
    int max_value = 1;
    int min_value = 4086;
    for (int sensor_value_index = 0; sensor_value_index < LIGHT_CALIB_ACCURACY; sensor_value_index++)
    {
        now_val = light.value();
        sum += now_val;
        max_value = std::max(now_val, max_value);
        min_value = std::min(now_val, min_value);

        msleep(20);
    }
    ambient_light = sum / LIGHT_CALIB_ACCURACY;  // average light value
    light_range = max_value - min_value;

    // wait for threads
    if (bal_cal_thread.joinable())
        bal_cal_thread.join();

    while (light.value() > ambient_light - (light_range)) msleep(10);

    go::arm->moveGripperTo(100);
    go::arm->moveWristToRelativeAngle(90);
    go::arm->awaitWristDone();
    navigation_sequences::remove_first_pom();
    go::nav->startSequence();
    std::cout << CLR_BLUE << "waiting for pom sequence" << CLR_RESET << std::endl;
    go::nav->awaitSequenceComplete();

    go::arm->moveGripperTo(0);;
    go::arm->unpark();
    go::arm->awaitAllDone();

    go::arm->moveEllbowTo(95);
    go::arm->moveWristToRelativeAngle(-56);
    go::arm->awaitAllDone();

    // rotate to cube
    double off_a = 0;
    for (int i = 0; i < 2; i++)
        off_a = Cam::look_at(0);

    if (off_a == 69420.f)
    {   // cube not found
        go::arm->moveGripperTo(100);
        std::cout << CLR_RED << "Cube 1 not found!" << CLR_RESET << std::endl;
        navigation_sequences::yellow_cubes::alternate_drop_first_cube();
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
    }
    else
    {   // cube found
        std::cout << "moving by " << off_a * (180 / M_PI) << "°\n";

        go::nav->rotateBy(off_a);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
        std::cout << "moved by " << off_a * (180 / M_PI) << "°\n";

        go::nav->driveDistance(8);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        std::cout << "gripping\n";

        go::arm->moveGripperTo(95);;
        go::arm->awaitGripperDone();

        msleep(200);

        go::arm->moveWristToRelativeAngle(0);
        go::arm->awaitWristDone();

        go::nav->rotateBy(-off_a);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        navigation_sequences::yellow_cubes::drop_first_cube();
        std::cout << CLR_BLUE << "waiting for cube drop 1 sequence" << CLR_RESET << std::endl;
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        navigation_sequences::yellow_cubes::stack_cube(current_stack);
        current_stack++;
   }

    // go::arm->unpark();
    go::arm->moveShoulderTo(5);
    go::arm->moveEllbowTo(50);
    go::arm->moveWristToRelativeAngle(90);
    go::arm->awaitAllDone();
    go::arm->moveGripperTo(100);

    navigation_sequences::yellow_cubes::goto_second_cube();
    std::cout << CLR_BLUE << "waiting for cube 2 sequence" << CLR_RESET << std::endl;
    go::nav->startSequence();
    go::nav->awaitSequenceComplete();

    go::arm->unpark();
    go::arm->moveGripperTo(0);
    go::arm->awaitAllDone();

    // rotate to cube
    for (int i = 0; i < 100; i++)
        off_a = Cam::look_at(0);

    if (off_a == 69420.f)
    {   // cube not found
        std::cout << CLR_RED << "Cube 2 not found!" << CLR_RESET << std::endl;
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
    }
    else
    {   // cube found
        std::cout << "moving by " << off_a * (180 / M_PI) << "°\n";

        go::nav->rotateBy(off_a);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
        std::cout << "moved by " << off_a * (180 / M_PI) << "°\n";

        go::nav->driveDistance(18);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        std::cout << "gripping\n";

        go::arm->moveGripperTo(95);;
        go::arm->awaitGripperDone();

        msleep(200);


        go::arm->moveWristToRelativeAngle(30);
        go::arm->awaitWristDone();

        go::nav->rotateBy(-off_a);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        navigation_sequences::yellow_cubes::drop_second_cube();
        std::cout << CLR_BLUE << "waiting for cube drop 2 sequence" << CLR_RESET << std::endl;
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        navigation_sequences::yellow_cubes::stack_cube(current_stack);
        current_stack++;
    }

    go::arm->awaitAllDone();
    go::nav->startSequence();
    go::nav->awaitSequenceComplete();

    // go::arm->awaitAllDone();

    // go::arm->moveShoulderToAngle(110);
    // go::arm->moveEllbowTo(30);
    // go::arm->moveWristToRelativeAngle(-20);

    // if (1)
    // {
    //     create_full();
    //     Cam::look_at(0);
    //     create_drive_straight(40);
    //     while (dist.value() < 1800)
    //     {
    //         std::cout << "dist: " << dist.value() <<  "\n";
    //         create_drive_straight(50);
    //         Cam::look_at(YELLOW_CHANNEL, true);
    //     }

    //     create_drive_straight(0);
    //     go::arm->moveGripperTo(90);
    //     go::arm->awaitGripperDone();

    //     go::arm->moveWristToRelativeAngle(30);
    //     go::arm->moveEllbowTo(50);

    //     msleep(200);

    //     create_drive_straight(-500);
    //     msleep(3000);

    //     create_drive_straight(0);
    //     go::arm->awaitAllDone();

    //     create_stop();
    // }
    // else
    // {
    //     std::cout << "couldn't connect to create";
    // }
    create_disconnect();

    return 0;
}
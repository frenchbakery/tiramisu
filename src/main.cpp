#include "drivers/tiramisu/ball_sorter/ball_sorter.hpp"
#include "drivers/tiramisu/arm/arm.hpp"
#include "drivers/tiramisu/cam_funcs/cam_funcs.hpp"
#include <kipr/wait_for/wait_for.h>
#include <kipr/analog/analog.hpp>
#include <kipr/create/create.h>
#include <kipr/camera/camera.h>
#include <kipr/time/time.h>
#include <iostream>
#include <thread>


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


// void grab_cube(Analog &dist_sens, Servo &grab_serv, Arm &my_arm)
// {
//     grab_serv.setPosition(1630);

//     my_arm.moveEllbowTo(70);
//     my_arm.moveShoulderToAngle(120);

//     std::cout << "set stuff\n";

//     Cam::look_at(YELLOW_CHANNEL);

//     std::cout << "centered\n";

//     // program
//     // get closer

//     grab_serv.setPosition(1000);
//     msleep(500);
//     my_arm.moveEllbowTo(90);
//     create_drive_straight(-100);
//     msleep(1500);
//     create_stop();
// }


/**
 * @brief initial calibration of the creates position
 * 
 */
void reset_position()
{

}


int main()
{
    BallSorter balls(BALL_MOTOR_PORT, BALL_SERVO_PIN, BALL_END_PIN);

    camera_open_device_model_at_res(0, BLACK_2017, Resolution::MED_RES);
    camera_load_config("cubes.conf");

    kipr::digital::Digital dist(DIST_PIN);
    kipr::analog::Analog light(LIGHT_PIN);

    Arm arm(
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
    arm.calibrate();
    std::cout << "calibrated\n";
    arm.park();

    std::cout << "parked\n";

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

    // wait for ball calibration thread to finish
    if (bal_cal_thread.joinable())
        bal_cal_thread.join();

    while (light.value() > ambient_light - light_range) msleep(10);

    arm.unpark();

    arm.moveShoulderToAngle(110);
    arm.moveEllbowTo(30);
    arm.moveWristToRelativeAngle(-20);

    if (!create_connect())
    {
        create_full();
        Cam::look_at(0);
        create_drive_straight(40);
        while (dist.value() < 1800)
        {
            std::cout << "dist: " << dist.value() <<  "\n";
            create_drive_straight(50);
            Cam::look_at(YELLOW_CHANNEL, true);
        }

        create_drive_straight(0);
        arm.moveGripperTo(90);
        arm.awaitGripperDone();

        arm.moveWristToRelativeAngle(30);
        arm.moveEllbowTo(50);

        msleep(200);

        create_drive_straight(-500);
        msleep(3000);

        create_drive_straight(0);
        arm.awaitAllDone();

        create_stop();
    }
    else
    {
        std::cout << "couldn't connect to create";
    }
    create_disconnect();

    return 0;
}
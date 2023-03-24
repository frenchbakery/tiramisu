#include <kipr/time/time.h>
#include "term_colors.h"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include "arm.hpp"


// int Arm::shoulderAngleToServoPos(Arm::angle_t angle)
// {
//     // 0...2047, 90...1024, 180...0
//     return 2047 - 2047 * (angle / 180);
// }


Arm::Arm(
            int motor_pin,
            int shoulder_servo_pin,
            int wrist_servo_pin,
            int grab_servo_pin,
            int end_switch_pin,
            int start_switch_pin
)
    :   ellbow_motor(motor_pin, 512, 20),
        shoulder_servo(shoulder_servo_pin),
        wrist_servo(wrist_servo_pin),
        grab_servo(grab_servo_pin),
        max_switch(end_switch_pin),
        min_switch(start_switch_pin)
{
    // reate arm range from previousely run calibration
    try
    {
        std::ifstream my_stream("/home/access/config/motor_range.tiramisu");
        my_stream >> motor_range;
        my_stream.close();

        std::cout << "read arm clibration\n";
    }
    catch (const std::exception &e)
    {
        std::cout << CLR_RED << "configuration read error: " << e.what() << CLR_RESET << std::endl;
    }


    // enable servos
    shoulder_servo.enable();
    wrist_servo.enable();
    grab_servo.enable();

    // set speeds
    shoulder_servo.setSpeed(256);
    wrist_servo.setSpeed(256);
    grab_servo.setSpeed(1024);
}


void Arm::calibrate()
{
    int wrist_speed = wrist_servo.getSpeed();
    wrist_servo.setSpeed(300);
    // - is up
    wrist_servo.setPosition(300);
    shoulder_servo.setPosition(900);
    ellbow_motor.moveAtVelocity(-calibrate_speed);

    // wait for motor to hit end switch
    bool set_wrist_down = false;
    while (!max_switch.value())
    {
        int shoulder_pos = shoulder_servo.position();

        if (624 < shoulder_pos && shoulder_pos < 1424 && !set_wrist_down)
        {
            set_wrist_down = true;
            moveWristToRelativeAngle(-90);
        }

        msleep(10);
    }
    std::cout << "max\n";

    ellbow_motor.clearPositionCounter();
    ellbow_motor.moveAtVelocity(calibrate_speed);
    moveWristToRelativeAngle(70);

    // wait for motor to hit end switch
    while (!min_switch.value()) { msleep(10); }
    ellbow_motor.off();
    std::cout << "min\n";
    msleep(500);

    motor_range = -ellbow_motor.getPosition();
    std::cout << "motor range: " << motor_range << std::endl;
    ellbow_motor.clearPositionCounter();
    ellbow_motor.enablePositionControl();
    ellbow_motor.setAbsoluteTarget(-100);

    try
    {
        std::ofstream my_stream("/home/access/config/motor_range.tiramisu");
        my_stream << motor_range;
        my_stream.close();

        std::cout << "wrote arm clibration\n";
    }
    catch (const std::exception &e)
    {
        std::cout << CLR_RED << "configuration write error: " << e.what() << CLR_RESET << std::endl;
    }

    // reset wrist servo speed
    awaitWristDone();
    wrist_servo.setSpeed(wrist_speed);
    grab_servo.setPosition(1024);
}

void Arm::moveEllbowTo(Arm::perc_value_t position_perc)
{
    ellbow_motor.enablePositionControl();
    ellbow_motor.setAbsoluteTarget((position_perc / 100) * motor_range);
}

void Arm::moveWristToRelativeAngle(Arm::angle_t angle)
{
    wrist_servo.setPosition((-angle / 90) * 1024 + 1024);
}

void Arm::moveShoulderTo(Arm::perc_value_t position_perc)
{
    shoulder_servo.setPosition(shoulder_min + (position_perc / 100) * (shoulder_max - shoulder_min));
}

void Arm::moveShoulderToAngle(Arm::angle_t angle)
{
    shoulder_servo.setPosition(angle);
    std::cout << CLR_RED << "move by angle not implemented" << CLR_RESET << std::endl;
    // shoulder_servo.setPosition((angle / 180) * 2047);
}

void Arm::moveGripperTo(perc_value_t position_perc)
{
    grab_servo.setPosition(position_perc * 20.47);
}


double getShoulderAngle()
{
    return 0;
}

void Arm::awaitShoulderDone()
{
    shoulder_servo.waitUntilComleted();
}
void Arm::awaitEllbowDone()
{
    ellbow_motor.blockMotorDone();
}
void Arm::awaitWristDone()
{
    wrist_servo.waitUntilComleted();
}
void Arm::awaitGripperDone()
{
    grab_servo.waitUntilComleted();
}
void Arm::awaitAllDone()
{
    awaitShoulderDone();
    awaitGripperDone();
    awaitEllbowDone();
    awaitWristDone();
}

void Arm::setShoulderSpeed(int speed)
{
    shoulder_servo.setSpeed(speed);
}
void Arm::setWristSpeed(int speed)
{
    wrist_servo.setSpeed(speed);
}
void Arm::setGripperSpeed(int speed)
{
    grab_servo.setSpeed(speed);
}


void Arm::park()
{
    // reset speeds
    wrist_servo.setSpeed(256);
    shoulder_servo.setSpeed(256);
    grab_servo.setSpeed(512);

    wrist_servo.setPosition(300);
    shoulder_servo.setPosition(shoulder_min);
    moveEllbowTo(ellbow_parked);

    awaitShoulderDone();
    moveWristToRelativeAngle(wrist_parked);
    awaitWristDone();
}


void Arm::unpark()
{
    // reset speeds
    wrist_servo.setSpeed(256);
    shoulder_servo.setSpeed(256);
    grab_servo.setSpeed(1024);

    moveWristToRelativeAngle(0);
    moveEllbowTo(50);
    shoulder_servo.setPosition(shoulder_90);
    awaitShoulderDone();
}

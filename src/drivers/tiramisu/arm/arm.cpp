#include <kipr/time/time.h>
#include "utils.hpp"
#include <iostream>
#include "arm.hpp"


Arm::shoulder_angle_t Arm::servoAngleToShoulderAngle(Servo::ticks_t servo_pos)
{
    // calculate the servos angle
    angle_t servo_angle = 24 + map(servo_pos, 150, 1900, 13, 166.8);
}


Arm::angle_t Arm::shoulderAngleToServoAngle(Arm::shoulder_angle_t angle)
{
    // calculate first angle and convert the given on into radian
    double alpha = servo_off_angle + (90 - (angle * (M_PI / 180)));

    // first side (connection_pos to servo_pos)
    double a = sqrt(
        pow(servo_off_len, 2)
        - 2 * servo_off_len * shoulder_connector_height
        * cos(alpha) + pow(shoulder_connector_height, 2)
    );

    // first half of servo angle
    double phi1 = acos(
        (pow(a, 2) - pow(shoulder_connector_height, 2)
        + pow(servo_off_len, 2))
        / (2 * servo_off_len * a)
    );

    // second half of servo angle
    double phi2 = acos(
        (- pow(shoulder_connector_length, 2) + pow(shoulder_servo_length, 2)
        + pow(a, 2))
        / (2 * shoulder_servo_length * a)
    );

    // return full angle in degrees
    return (phi1 + phi2) * (180 / M_PI);
}


Arm::ellbow_angle_t motorPercToAngle(Arm::perc_value_t motor_perc)
{

}


Arm::perc_value_t angleToMotorPerc(Arm::ellbow_angle_t angle)
{

}


Arm::Arm(BackEMF &emf, Motor &ellbow, Servo &shoulder, Digital &end_switch, Digital &start_switch)
 : ellbow_emf(emf), ellbow_motor(ellbow), shoulder_servo(shoulder), max_switch(end_switch), min_switch(start_switch)
{
    // -1 means not calibrated
    current_position = -1;
    pos_delta = -1;

    // enable servo
    shoulder.enable();
}


void Arm::calibrate()
{
    ellbow_motor.moveAtVelocity(calibrate_speed);

    // wait for motor to finish
    while (!max_switch.value()) {msleep(10);};
    ellbow_motor.off();

    // move shoulder to 90°
    int diff = shoulder_servo.position() - shoulder_high;
    for (;diff < 0; diff++)
    {
        shoulder_servo.setPosition(shoulder_high + diff);
        msleep(2);
    }
    shoulder_servo.setPosition(shoulder_high);

    ellbow_motor.moveAtVelocity(calibrate_speed);

    // wait for motor to finish
    while (!max_switch.value()) {msleep(10);};
    ellbow_motor.off();

    int times_moved = 0;

    // wait for motor to finish
    ellbow_motor.clearPositionCounter();
    ellbow_motor.moveAtVelocity(-calibrate_speed);
    while (!min_switch.value()) {msleep(10);};
    // {
        // ellbow_motor.clearPositionCounter();
        // ellbow_motor.moveToPosition(-100, -calibrate_accuracy);

        // times_moved++;        
        // while (!ellbow_motor.isMotorDone()) {};
    // };
    ellbow_motor.off();

    // pos_delta = times_moved * calibrate_accuracy;
    pos_delta = -ellbow_emf.value();
    ellbow_motor.clearPositionCounter();

    ellbow_motor.moveToPosition(100, 50);
    msleep(300);

    current_position = 0;
}

void Arm::moveEllbowTo(Arm::perc_value_t position_perc, float speed)
{
    if (pos_delta == 0)
    {
        return;
    }

    float wanted_position = pos_delta * (position_perc / 100);
    float delta = wanted_position - current_position;

    ellbow_motor.clearPositionCounter();
    ellbow_motor.moveToPosition(speed, delta);

    std::cout << "range: " << pos_delta << ", moving to: " << delta << "\n";

    // wait for leaving end switches
    while (max_switch.value() || min_switch.value()) {msleep(10);};
    while (!ellbow_motor.isMotorDone())
    {
        msleep(10);

        // end position switches are triggered
        if (max_switch.value() || min_switch.value())
        {
            std::cout << "end\n";
            ellbow_motor.off();
            for (;;) {msleep(10);};
        }
    }

    std::cout << "done\n";

    // make sure the motor is off
    ellbow_motor.off();
    current_position += delta;
}

void Arm::moveShoulderTo(Arm::perc_value_t position_perc, short speed)
{
    double wanted_pos = (position_perc / 100) * (shoulder_high - shoulder_low) + shoulder_low;

    // move shoulder
    int diff = shoulder_servo.position() - wanted_pos;
    for (;diff < 0; diff++)
    {
        shoulder_servo.setPosition(shoulder_high + diff);
        msleep(10 / speed);

        // check if min / max end switches are touched
        if (max_switch.value() || min_switch.value())
        {
            return;
        }
    }

    // last step
    shoulder_servo.setPosition(wanted_pos);
}


void Arm::moveShoulderToAngle(Arm::shoulder_angle_t angle, short speed)
{
    angle += 5;
    double servo_angle = shoulderAngleToServoAngle(angle);
    Servo::ticks_t servo_pos = map(servo_angle - 13, 13, 167, 150, 1900);

    std::cout << "servo: " << servo_pos << "\n";

    if (servo_pos > 2047)
    {
        servo_pos = 2047;
    }

    // move shoulder
    int initial = shoulder_servo.position();
    int diff = initial - servo_pos;

    for (int i = 0; i < diff; i++)
    {
        shoulder_servo.setPosition(initial - i);
        msleep(map(speed, 0, 100, 10, 0));

        // check if min / max end switches are touched
        if (max_switch.value() || min_switch.value())
        {
            return;
        }
    }
    for (int i = 0; i > diff; i--)
    {
        shoulder_servo.setPosition(initial - i);
        msleep(map(speed, 0, 100, 10, 0));

        // check if min / max end switches are touched
        if (max_switch.value() || min_switch.value())
        {
            return;
        }
    }

    // last step
    shoulder_servo.setPosition(servo_pos);
}


void Arm::grabCube()
{
    return;
}
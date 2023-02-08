#include "arm.hpp"


Arm::shoulder_angle_t Arm::servoToAngle(Servo::ticks_t servo_pos)
{
    
}


Servo::ticks_t Arm::angleToServo(Arm::shoulder_angle_t angle)
{

}


Arm::ellbow_angle_t motorPercToAngle(Arm::perc_value_t motor_perc)
{

}


Arm::perc_value_t angleToMotorPerc(Arm::ellbow_angle_t angle)
{
    
}


Arm::Arm(Motor &ellbow, Servo &shoulder, Digital &end_switch, Digital &start_switch)
 : ellbow_motor(ellbow), shoulder_servo(shoulder), max_switch(end_switch), min_switch(start_switch)
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
    while (!min_switch.value())
    {
        ellbow_motor.clearPositionCounter();
        ellbow_motor.moveToPosition(-100, -calibrate_accuracy);

        times_moved++;        
        while (!ellbow_motor.isMotorDone()) {};
    };
    ellbow_motor.off();

    pos_delta = times_moved * calibrate_accuracy;
    ellbow_motor.clearPositionCounter();

    std::cout << "moved " << times_moved << " times\n";

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
            return calibrate();
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

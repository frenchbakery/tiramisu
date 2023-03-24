/**
 * @file ball_sorter.hpp
 * @author Nilusink
 * @version 1.0
 * @date 2023-03-07
 * 
 * @copyright Copyright frenchbakery (c) 2023
 * 
 */
#include <kipr/time/time.h>
#include "ball_sorter.hpp"


BallSorter::BallSorter(int motor_pin, int servo_pin, int switch_pin)
    : turn_motor(motor_pin, 768, 20), push_servo(servo_pin, initial_servo_pos), end_switch(switch_pin)
{
    push_servo.enable();
    push_servo.setSpeed(65553);
};


void BallSorter::calibrate()
{
    // if on end switch, move back a bit
    if (end_switch.value())
    {
        turn_motor.moveAtVelocity(100);
        msleep(1000);
    }
    // reset servo
    resetPusher();

    // reset motor
    raise();
}


void BallSorter::toDeck()
{
    // adjust motor position
    turn_motor.enablePositionControl();
    turn_motor.setAbsoluteTarget(motor_down);
}

void BallSorter::toHold()
{
    turn_motor.enablePositionControl();
    turn_motor.setAbsoluteTarget(motor_down - 100);
}

void BallSorter::toDropPosition()
{
    // adjust motor position
    turn_motor.enablePositionControl();
    turn_motor.setAbsoluteTarget(motor_drop);
}

void BallSorter::raise()
{
    // motor on until end switch
    turn_motor.moveAtVelocity(-calibrate_speed);

    while (!end_switch.value()) { msleep(10); };
    turn_motor.off();

    turn_motor.clearPositionCounter();
}


void BallSorter::pushBall(bool reset)
{
    push_servo.setPosition(servo_up);

    if (reset)
    {
        push_servo.waitUntilComleted();
        resetPusher();
    }
}

void BallSorter::resetPusher()
{
    push_servo.setPosition(initial_servo_pos);
}


void BallSorter::setServoSpeed(int speed)
{
    push_servo.setSpeed(speed);
}


void BallSorter::waitForMotor()
{
    turn_motor.blockMotorDone();
}


void BallSorter::waitForServo()
{
    push_servo.waitUntilComleted();
}

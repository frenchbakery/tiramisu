/**
 * @file arm.hpp
 * @author Nilusink
 * @brief the driver for tiramisu's arm
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright FrenchBakery (c) 2023
 * 
 */
#pragma once
#include <kipr/digital/digital.hpp>
#include <kiprplus/smooth_servo.hpp>
#include <kiprplus/pid_motor.hpp>
#include <cmath>



using namespace kipr::digital;
/**
 * @brief driver implementing the whole tiramisu arm
 * 
 */
class Arm
{
    public:
        typedef double angle_t;
        typedef float perc_value_t;  // value between 0 and 100
        typedef kp::SmoothServo Servo;
        typedef kp::PIDMotor Motor;
        const int shoulder_max = 1800;
        const int shoulder_90 = 1140;
        const int shoulder_min = 60;

    protected:
        Servo shoulder_servo;
        Servo wrist_servo;
        Servo grab_servo;
        Motor ellbow_motor;
        Digital max_switch;
        Digital min_switch;

        // calibration configuration
        double calibrate_speed = 750;

        // ellbow positional data
        double current_position;

        const int shoulder_parked = 1700;
        const int ellbow_parked = 20;
        const int wrist_parked = -75;
        const int grab_parked = 1024;

        /**
         * @brief calculate the servo position given the desired angle
         * 
         * @param angle the angle to calculate for (0-90)
         * @return double - the servo position to set
         */
        int shoulderAngleToServoPos(angle_t angle);

        int motor_range = 0;

        // ellbow_angle_t motorPercToAngle(perc_value_t motor_perc);
        // perc_value_t angleToMotorPerc(ellbow_angle_t angle);

    public:
        Arm(
            int motor_pin,
            int shoulder_servo_pin,
            int wrist_servo_pin,
            int grab_servo_pin,
            int end_switch_pin,
            int start_switch_pin
        );

        /**
         * @brief calibrate the arms position
         * 
         */
        void calibrate();

        /**
         * @brief move the ellbow joint
         * 
         * @param position_perc value beween 0 and 100
         * @param speed value between 0 and 1024
         */
        void moveEllbowTo(perc_value_t position_perc);

        /**
         * @brief move the shoulder joint
         * 
         * @param position_perc value between 0 and 100
         * @param speed value between 0 and 100 
         */
        void moveShoulderTo(perc_value_t position_perc);

        void moveShoulderToAngle(angle_t angle);

        /**
         * @brief move wrist relative to the shoulder
         * 
         * @param angle 0..straight, 90..up, -90..down
         */
        void moveWristToRelativeAngle(angle_t angle);

        void moveGripperTo(perc_value_t position_perc);

        // properties
        double getShoulderAngle();

        void awaitShoulderDone();
        void awaitEllbowDone();
        void awaitWristDone();
        void awaitGripperDone();
        void awaitAllDone();

        void setShoulderSpeed(int speed);
        void setWristSpeed(int speed);
        void setGripperSpeed(int speed);

        void park();
        void unpark();

        void terminate();
};

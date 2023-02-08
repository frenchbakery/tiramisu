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
// #include <kipr/digital/digital.hpp>
// #include <kipr/motor/motor.hpp>
// #include <kipr/servo/servo.hpp>
// #include <kipr/time/time.h>
#include <kipr/digital.hpp>
#include <kipr/motors.hpp>
#include <kipr/servo.hpp>
#include <kipr/util.h>
#include <iostream>
#include <cmath>

// using namespace kipr::digital;
// using namespace kipr::motor;
// using namespace kipr::servo;
/**
 * @brief driver implementing the whole tiramisu arm
 * 
 */
class Arm
{
    public:
        typedef double shoulder_angle_t;  // value beween 0 and 90
        typedef double ellbow_angle_t;  // value between  0 and 135
        typedef float perc_value_t;  // value between 0 and 100

    private:
        Servo &shoulder_servo;
        Motor &ellbow_motor;
        Digital &max_switch;
        Digital &min_switch;

        // arm lengths in cm
        const short int shoulder_length = 0;
        const short int ellbow_length = 0;

        // shoulder servo max values
        const int shoulder_high = 1710;
        const int shoulder_low = 0;

        // calibration configuration
        double calibrate_accuracy = 2;
        double calibrate_speed = 1024;

        // ellbow positional data
        double current_position;
        double pos_delta;

        /**
         * @brief calculate the shoulder angle from the servo position
         * 
         * @param servo_pos servo position to calculate from (0-2048)
         * @return double - the shoulder angle
         */
        shoulder_angle_t servoToAngle(Servo::ticks_t servo_pos);

        /**
         * @brief calculate the servo position given the desired angle
         * 
         * @param angle the angle to calculate for (0-90)
         * @return double - the servo position to set
         */
        Servo::ticks_t angleToServo(shoulder_angle_t angle);

        ellbow_angle_t motorPercToAngle(perc_value_t motor_perc);
        perc_value_t angleToMotorPerc(ellbow_angle_t angle);

    public:
        Arm(Motor &ellbow, Servo &shoulder, Digital &end_switch, Digital &start_switch);

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
        void moveEllbowTo(perc_value_t position_perc, float speed = 200);

        /**
         * @brief move the shoulder joint
         * 
         * @param position_perc value between 0 and 100
         * @param speed value between 0 and 100 
         */
        void moveShoulderTo(perc_value_t position_perc, short speed);

        // properties
        double getEllbowAngle();
        double getShoulderAngle();
};

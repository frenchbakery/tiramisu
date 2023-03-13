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
#include <kipr/digital/digital.hpp>
#include <kipr/motor/motor.hpp>
#include <kipr/servo/servo.hpp>
#include <cmath>


using namespace kipr::digital;
using namespace kipr::motor;
using namespace kipr::servo;
/**
 * @brief driver implementing the whole tiramisu arm
 * 
 */
class Arm
{
    public:
        typedef double angle_t;
        typedef double shoulder_angle_t;  // value beween 0 and 90
        typedef double ellbow_angle_t;  // value between  0 and 135
        typedef float perc_value_t;  // value between 0 and 100

    private:
        Servo &shoulder_servo;
        Motor &ellbow_motor;
        BackEMF &ellbow_emf;
        Digital &max_switch;
        Digital &min_switch;

        // arm lengths in cm (metal parts spacing: 1.6mm, lego spacing: 0.8mm)
        // shoulder
        const double shoulder_length = 15 * 1.6;
        const double shoulder_servo_length = 4 * 1.6;
        const double shoulder_connector_length = 21 * .8;
        const double shoulder_connector_height = 6 * 1.6;
        const double servo_x_off = 1.6 * 7.3;
        const double servo_y_off = .8 * 7.6;
        // const double servo_off_len = sqrt(pow(servo_x_off, 2) + pow(servo_y_off, 2));
        const double servo_off_len = 13;
        const angle_t servo_off_angle = atan(servo_x_off / servo_y_off);

        // ellbow
        const double ellbow_length = 29 * .8;

        // shoulder servo max values
        const int shoulder_high = 2047;
        const int shoulder_90 = 1760;
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
        shoulder_angle_t servoAngleToShoulderAngle(Servo::ticks_t servo_pos);

        /**
         * @brief calculate the servo position given the desired angle
         * 
         * @param angle the angle to calculate for (0-90)
         * @return double - the servo position to set
         */
        Arm::angle_t shoulderAngleToServoAngle(shoulder_angle_t angle);

        ellbow_angle_t motorPercToAngle(perc_value_t motor_perc);
        perc_value_t angleToMotorPerc(ellbow_angle_t angle);

    public:
        Arm(BackEMF &ellbow_emf, Motor &ellbow, Servo &shoulder, Digital &end_switch, Digital &start_switch);

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
        void moveEllbowTo(perc_value_t position_perc, float speed = 512);

        /**
         * @brief move the shoulder joint
         * 
         * @param position_perc value between 0 and 100
         * @param speed value between 0 and 100 
         */
        void moveShoulderTo(perc_value_t position_perc, short speed = 80);

        void moveShoulderToAngle(shoulder_angle_t angle, short speed = 80);

        void grabCube();

        // properties
        double getEllbowAngle();
        double getShoulderAngle();
};

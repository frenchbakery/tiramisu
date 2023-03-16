/**
 * @file create.hpp
 * @author Nilusink
 * @brief defines a few useful routines for the create
 * @version 0.1
 * @date 2023-03-14
 * 
 * @copyright Copyright Frenchbakery (c) 2023
 * 
 */
#include "global_objects.hpp"
#include <kipr/create/create.h>
#include <kipr/time/time.h>
#include <iostream>


/**
 * @brief drives until the create hits a wall
 * 
 */
void drive_until_bumper(int speed = 50)
{
    // start motors
    go::nav->disablePositionControl();
    go::nav->driveLeftSpeed(speed);
    go::nav->driveRightSpeed(speed);

    // wait until bumped into wall
    for (;;)
    {
        {
            std::lock_guard lock(kp::CreateMotor::create_access_mutex);
            if (get_create_lbump() || get_create_rbump())
                break;
        }
        msleep(10);
    }

    // stop motors
    go::nav->driveLeftSpeed(0);
    go::nav->driveRightSpeed(0);
    go::nav->resetPositionControllers();
    go::nav->enablePositionControl();
}


void align_wall(int accuracy = 2, int speed = 25)
{
    bool l_b, r_b, ll_b, lr_b;
    go::nav->disablePositionControl();

    int i = 0;
    for (;;)
    {
        std::cout << i << std::endl;
        {
            std::lock_guard lock(kp::CreateMotor::create_access_mutex);
            l_b = get_create_lbump();
            r_b = get_create_rbump();
        }

        if (l_b && r_b)
        {
            go::nav->driveLeftSpeed(-speed);
            go::nav->driveRightSpeed(-speed);
        }
        else if (!(l_b || r_b))
        {
            if (ll_b && lr_b)
            {
                i++;
            }
            go::nav->driveLeftSpeed(speed);
            go::nav->driveRightSpeed(speed);
        }
        else if (l_b)
        {
            go::nav->driveLeftSpeed(-speed);
            go::nav->driveRightSpeed(speed);
        }
        else if (r_b)
        {
            go::nav->driveLeftSpeed(speed);
            go::nav->driveRightSpeed(-speed);
        }
        else
        {
            go::nav->driveLeftSpeed(0);
            go::nav->driveRightSpeed(0);
        }

        if (i >= accuracy)
        {
            go::nav->driveLeftSpeed(0);
            go::nav->driveRightSpeed(0);
            go::nav->resetPositionControllers();
            go::nav->enablePositionControl();
            return;
        }
 
        ll_b = l_b;
        lr_b = r_b;
    }
}


/**
 * @brief initial calibration of the creates position
 * 
 */
void reset_position()
{
    std::cout << "resetting position..\n";

    msleep(100);

    drive_until_bumper();
    std::cout << "first wall\n";

    go::nav->driveDistance(-10);
    go::nav->rotateBy(-M_PI_2);
    go::nav->startSequence();
    go::nav->awaitSequenceComplete();
    msleep(800);

    drive_until_bumper();
    std::cout << "second wall\n";

    go::nav->driveDistance(-5);
    go::nav->rotateBy(M_PI_2);
    go::nav->startSequence();
    go::nav->awaitSequenceComplete();
    msleep(800);
}


namespace navigation_sequences
{
    void remove_first_pom()
    {
        go::nav->driveDistance(5);
        go::nav->rotateBy(-M_PI_2);
        go::nav->driveDistance(-20);
        go::nav->rotateBy(M_PI_2);
        go::nav->driveDistance(-50);
        go::nav->rotateBy(-M_PI_2);
        go::nav->driveDistance(-40);
        go::nav->rotateBy(-M_PI_2);
        go::nav->driveDistance(-25);
        go::nav->driveDistance(20);
        go::nav->rotateBy(M_PI / 10);
    }

    namespace yellow_cubes
    {
        /**
         * @brief stacke the cubes
         * 
         * @param n starting at 0, the current stack height
         */
        void stack_cube(int n)
        {
            if (n == 0)
            {
                // hover with cube facing down
                go::arm->moveEllbowTo(50);
                go::arm->moveWristToRelativeAngle(-90);
                go::arm->awaitAllDone();

                // go further down, so the cube just about touches the ground
                go::arm->moveEllbowTo(40);
                go::arm->moveWristToRelativeAngle(-75);
                go::arm->awaitAllDone();

                // release the cube
                go::arm->moveGripperTo(0);
                go::arm->awaitGripperDone();

                // go back up (claw still open)
                go::arm->moveEllbowTo(60);
                go::arm->moveWristToRelativeAngle(-90);
                go::arm->awaitAllDone();
            }
            else
            {
                double stack_floor = 10.5 * n;

                // drve back to macht the stack
                go::nav->driveDistance(-20);
                go::nav->startSequence();
                go::nav->awaitSequenceComplete();

                // move wrist up to not push over the tower
                go::arm->moveWristToRelativeAngle(60);
                go::arm->awaitWristDone();

                // go down
                go::arm->moveEllbowTo(30);
                go::arm->moveWristToRelativeAngle(70);
                go::arm->awaitAllDone();

                // release
                go::arm->moveGripperTo(0);
                go::arm->awaitGripperDone();

                // go back up (claw still open)
                go::arm->moveEllbowTo(80);
                go::arm->moveWristToRelativeAngle(-30);
            }
        }

        void drop_first_cube()
        {
            go::nav->rotateBy(M_PI - (M_PI / 10));
            go::nav->driveDistance(20);
        }

        /**
         * @brief in case the fist cube wasn't found
         * 
         */
        void alternate_drop_first_cube()
        {
            drop_first_cube();
        }

        void goto_second_cube()
        {
            go::nav->rotateBy(-M_PI_2);
            go::nav->driveDistance(38.1);  // actually 38.1cm
            go::nav->rotateBy(-(M_PI / 2.5));
        }

        void drop_second_cube()
        {
            go::nav->rotateBy(M_PI / 2.5);
            go::nav->driveDistance(-38.1);
            go::nav->rotateBy(M_PI_2);
        }
    }
}

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
#include "term_colors.h"
#include <iostream>


#define LINE_ALIGN_N 10
#define LINE_THRESH 3000
#define LINE_CALIB_SPEED 25
#define LINE_CALIB_SPEED_RANGE 10


#define DIST_TUBE_AALIGN 2919
#define DIST_TUBE_BALIGN 2800
#define DIST_TUBE_ALIGN 2500


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
            msleep(5);
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
    for (int j; j < 20; j++)
    {
        {
            std::lock_guard lock(kp::CreateMotor::create_access_mutex);
            get_create_lbump();
            get_create_rbump();
        }
        msleep(10);
    }

    bool l_b, r_b, ll_b, lr_b;
    go::nav->disablePositionControl();

    int i = 0;
    for (;;)
    {
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


/**
 * @brief align at the ballsorte position
 * 
 */
void align_line()
{
    go::nav->disablePositionControl();
    go::nav->driveLeftSpeed(0);
    go::nav->driveRightSpeed(0);

    bool is_black;
    bool last_black = go::line->value() > LINE_THRESH;
    int n = 0;
    int curr_speed = LINE_CALIB_SPEED - LINE_CALIB_SPEED_RANGE * ((float)n / LINE_ALIGN_N);
    for (;;)
    {
        is_black = go::line->value() > LINE_THRESH;

        if (last_black != is_black)
        {
            n++;
            if (n >= LINE_ALIGN_N)
                break;

            curr_speed = LINE_CALIB_SPEED - LINE_CALIB_SPEED_RANGE * ((float)n / LINE_ALIGN_N);
            std::cout << "n: " << n << ", " << curr_speed << std::endl;
        }

        if (is_black)
        {
            go::nav->driveLeftSpeed(-curr_speed);
            go::nav->driveRightSpeed(curr_speed);
        }
        else
        {
            go::nav->driveLeftSpeed(curr_speed);
            go::nav->driveRightSpeed(-curr_speed);
        }

        last_black = is_black;
        msleep(5);
    }

    go::nav->driveLeftSpeed(curr_speed);
    go::nav->driveRightSpeed(-curr_speed);
    while (go::line->value() <= LINE_THRESH) { msleep(10); }

    go::nav->driveLeftSpeed(0);
    go::nav->driveRightSpeed(0);
    go::nav->resetPositionControllers();
    go::nav->enablePositionControl();
}


int align_tube()
{
    std::cout << "aligning tube\n";

    go::nav->disablePositionControl();

    go::nav->driveLeftSpeed(-20);
    go::nav->driveRightSpeed(20);

    bool is_tube = go::dist->value() > DIST_TUBE_ALIGN;
    if (!is_tube)
    {
        std::cout << CLR_RED << "couldn't find tube!!!" << CLR_RESET << std::endl;
        return 1;
    }

    while (go::dist->value() > DIST_TUBE_ALIGN) { msleep(10); }


    go::nav->driveLeftSpeed(0);
    go::nav->driveRightSpeed(0);

    go::nav->resetPositionControllers();
    go::nav->enablePositionControl();

    // go::nav->rotateBy(-M_PI / 25);
    // go::nav->startSequence();
    // go::nav->awaitSequenceComplete();

    return 0;
}


namespace sequences
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

    /**
     * @brief starting after first cube drop, go to the pingpong ball pickup location
     * 
     */
    void balls_pickup_position()
    {
        // align y
        go::arm->moveGripperTo(90);
        go::nav->driveDistance(-18);
        go::nav->rotateBy(-M_PI_2);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        // drive to the tube
        go::nav->driveDistance(50);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
     
        // drop ball sorter
        go::balls->toDeck();
        go::balls->waitForMotor();
        align_line();

        go::nav->rotateBy(M_PI / 15);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();


        // align to the tube
        align_tube();
        std::cout << "0\n";

        go::nav->disablePositionControl();

        go::nav->driveLeftSpeed(40);
        go::nav->driveRightSpeed(40);

        while (go::dist->value() < DIST_TUBE_BALIGN) { msleep(10); }

        go::nav->driveLeftSpeed(0);
        go::nav->driveRightSpeed(0);

        std::cout << "1\n";
        align_tube();
        std::cout << "2\n";

        msleep(2000);
        go::nav->disablePositionControl();

        go::nav->driveLeftSpeed(40);
        go::nav->driveRightSpeed(40);

        bool is_wall;
        for (;;)
        {
            if (go::dist->value() < DIST_TUBE_AALIGN)  // target reached
            {
                std::cout << "target\n";
                break;
            }
            
            {
                std::lock_guard lock(kp::CreateMotor::create_access_mutex);
                is_wall = get_create_lbump() || get_create_rbump();
            }

            if (is_wall)
            {
                std::cout << "wall\n";
                break;
            }
            msleep(20);
        }
 
        go::nav->driveLeftSpeed(0);
        go::nav->driveRightSpeed(0);
        std::cout << "3\n";

        go::nav->driveDistance(13);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        // rotate to fit under the tube
        go::nav->rotateBy(-M_PI / 11);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
    }

    /**
     * @brief stating at the pinpong ball pickup, go to the drop off point
     * 
     */
    void balls_drop_position()
    {
        // back off the tube
        go::nav->rotateBy(M_PI / 10);
        go::nav->driveDistance(-20);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();

        // move ball sorter up
        go::balls->toDropPosition();

        // drive to the drop off
        go::nav->driveDistance(-140);
        go::nav->startSequence();
        go::nav->awaitSequenceComplete();
    }

    /**
     * @brief wiggle to better catch the balls
     * 
     */
    void funktion_punkt_naggl()
    {
        go::nav->disablePositionControl();
        go::nav->driveLeftSpeed(200);
        msleep(50);

        for (int i = 0; i < 10; i++)
        {
            go::nav->driveLeftSpeed(-200);
            msleep(100);
            go::nav->driveLeftSpeed(200);
            msleep(100);
        }

        go::nav->driveLeftSpeed(-200);
        msleep(50);
        go::nav->driveLeftSpeed(0);

        go::nav->enablePositionControl();
        msleep(500);
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
                go::nav->driveDistance(-80);
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

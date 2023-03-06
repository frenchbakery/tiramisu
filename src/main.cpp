#include "drivers/tiramisu/cam_funcs/cam_funcs.hpp"
#include "drivers/tiramisu/grepper/grepper.hpp"
#include "drivers/tiramisu/arm/arm.hpp"
#include <kipr/botball/botball.h>
#include <kipr/analog/analog.hpp>
#include <kipr/create/create.h>
#include <kipr/create/create.hpp>
#include <kipr/camera/camera.h>
#include <kipr/time/time.h>
#include <iostream>
#include <iomanip>
#include "pitches.h"

using namespace kipr::motor;
using namespace kipr::servo;
using namespace kipr::digital;
using namespace kipr::analog;

#define ANGLE_MUlT 1 / 1.3

uint16_t treble_melody[64] = {
    NR_NOTE_E5,
    NR_NOTE_E3,
    NR_NOTE_B4,
    NR_NOTE_C5,
    NR_NOTE_D5,
    NR_NOTE_E5,
    NR_NOTE_D5,
    NR_NOTE_C5,
    NR_NOTE_B4,
    NR_NOTE_A4,
    NR_NOTE_A3,
    NR_NOTE_A4,
    NR_NOTE_C5,
    NR_NOTE_E5,
    NR_NOTE_A3,
    NR_NOTE_D5,
    NR_NOTE_C5,
    NR_NOTE_B4,
    NR_NOTE_E4,
    NR_NOTE_G4,
    NR_NOTE_C5,
    NR_NOTE_D5,
    NR_NOTE_E3,
    NR_NOTE_E5,
    NR_NOTE_E3,
    NR_NOTE_C5,
    NR_NOTE_A3,
    NR_NOTE_A4,
    NR_NOTE_A3,
    NR_NOTE_A4,
    NR_NOTE_A3,
    NR_NOTE_B2,
    NR_NOTE_C3,
    NR_NOTE_D3,
    NR_NOTE_D5,
    NR_NOTE_F5,
    NR_NOTE_A5,
    NR_NOTE_C5,
    NR_NOTE_C5,
    NR_NOTE_G5,
    NR_NOTE_F5,
    NR_NOTE_E5,
    NR_NOTE_C3,
    0,
    NR_NOTE_C5,
    NR_NOTE_E5,
    NR_NOTE_A4,
    NR_NOTE_G4,
    NR_NOTE_D5,
    NR_NOTE_C5,
    NR_NOTE_B4,
    NR_NOTE_E4,
    NR_NOTE_B4,
    NR_NOTE_C5,
    NR_NOTE_D5,
    NR_NOTE_G4,
    NR_NOTE_E5,
    NR_NOTE_G4,
    NR_NOTE_C5,
    NR_NOTE_E4,
    NR_NOTE_A4,
    NR_NOTE_E3,
    NR_NOTE_A4,
    0,
};

uint16_t bass_melody[64] = {
    NR_NOTE_E4,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_C4,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_D4,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_B3,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_C4,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_A3,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_GS3,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_B3,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_E4,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_C4,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_D4,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_B3,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_C4,
    NR_NOTE_E3,
    NR_NOTE_E4,
    NR_NOTE_E3,
    NR_NOTE_A4,
    NR_NOTE_E3,
    NR_NOTE_A2,
    NR_NOTE_E3,
    NR_NOTE_GS4,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
    NR_NOTE_GS2,
    NR_NOTE_E3,
};

// note durations: 4 = quarter note, 8 = eighth note, etc
uint8_t treble_durations[64] = {
    8,
    8,
    8,
    8,
    8,
    16,
    16,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    4,
    8,
    8,
    16,
    16,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    16,
    16,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    4,
    4,
};

uint8_t bass_durations[64] = {
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
};

void grab_cube(Analog &dist_sens, Servo &grab_serv, Arm &my_arm)
{
    grab_serv.setPosition(1630);

    my_arm.moveEllbowTo(48);
    my_arm.moveShoulderToAngle(115);

    std::cout << "set stuff\n";

    Cam::look_at(YELLOW_CHANNEL);

    std::cout << "centered\n";

    // program
    // get closer
    create_drive_straight(20);
    while (dist_sens.value() < 2500) { msleep(10); create_drive_straight(40); };
    create_stop();

    grab_serv.setPosition(1000);
    msleep(500);
    my_arm.moveEllbowTo(80);
    create_drive_straight(-100);
    msleep(1500);
    create_stop();
}


int main()
{

    if (!create_connect())
    {
        float speed = 1.5;
        uint8_t tr1[32]; float t1 = 0;
        for (int i = 0; i < 16; i++)
        {
            tr1[i * 2] = treble_melody[i];
            int t = (64 * speed) / treble_durations[i];
            tr1[(i * 2) + 1] = t;
            t1 += (float)t / 64;
        }
        uint8_t tr2[32]; float t2 = 0;
        for (int i = 0; i < 16; i++)
        {
            tr2[i * 2] = treble_melody[i+16];
            int t = (64 * speed) / treble_durations[i+16];
            tr2[(i * 2) + 1] = t;
            t2 += (float)t / 64;
        }
        uint8_t tr3[32]; float t3 = 0;
        for (int i = 0; i < 16; i++)
        {
            tr3[i * 2] = treble_melody[i+32];
            int t = (64 * speed) / treble_durations[i+32];
            tr3[(i * 2) + 1] = t;
            t3 += (float)t / 64;
        }
        uint8_t tr4[32]; float t4 = 0;
        for (int i = 0; i < 16; i++)
        {
            tr4[i * 2] = treble_melody[i+48];
            int t = (64 * speed) / treble_durations[i+48];
            tr4[(i * 2) + 1] = t;
            t4 += (float)t / 64;
        }
        uint8_t bass_assembled[128];
        for (int i = 0; i < 16; i++)
        {
            bass_assembled[i * 2] = bass_melody[i];
            bass_assembled[(i * 2) + 1] = (64 * speed) / bass_durations[i];
        }
        create_spin_CW(100);
        create_load_song(tr1, 16, 0);
        create_load_song(tr2, 16, 1);
        create_load_song(tr3, 16, 2);
        create_load_song(tr4, 16, 3);
        create_play_song(0);
        msleep(1000);
        while (get_create_song_playing()) msleep(1);
        create_play_song(1);
        msleep(1000);
        while (get_create_song_playing()) msleep(1);
        create_play_song(2);
        msleep(1000);
        while (get_create_song_playing()) msleep(1);
        create_play_song(3);
        msleep(1000);
        while (get_create_song_playing()) msleep(1);
        create_disconnect();
        return 0;
        set_create_distance(0);
        create_drive_direct(200, 200);
        int ms = 0;
        while ( ms < 2000)
        {
            short l, r;
            _create_get_raw_encoders(&l, &r);
            std::cout << "l=" << std::setw(4) << l << " r=" << std::setw(4) << r << std::endl;
            ms += 50;
            msleep(50);
        }
        create_stop();
        create_disconnect();
    }


    return 0;
    // shut_down_in(115);

    // initialize arm
    Motor tm(0);
    BackEMF tmemf(0);
    Servo ss(0);
    Digital mas(8);
    Digital mis(9);

    Arm my_arm(tmemf, tm, ss, mas, mis);

    // Digital end_sensor(0);
    // Motor l_motor(2);
    // Motor r_motor(3);
    // Servo grab_servo(2);

    // Gripper gripper(grab_servo, l_motor, r_motor, end_sensor);

    int c_open = camera_open_device_model_at_res(0, BLACK_2017, Resolution::LOW_RES);
    camera_load_config("cubes.conf");

    Analog dist_sens(0);
    Servo grab_serv(2);
    Servo turn_serv(3);

    grab_serv.enable();
    turn_serv.enable();

    turn_serv.setPosition(1024);
    grab_serv.setPosition(1630);

    get_create_total_angle();

    if (!create_connect() && c_open)
    {
        // ss.setPosition(1900);
        // return 0;

        my_arm.calibrate();
        std::cout << "done calibrating\n";

        // center on cube
        grab_cube(dist_sens, grab_serv, my_arm);

        create_spin_block(100, -180 * ANGLE_MUlT);
        create_stop();

        // drop first cube
        my_arm.moveEllbowTo(10);
        my_arm.moveShoulderToAngle(30);
        msleep(1000);

        grab_serv.setPosition(1630);
        msleep(1000);

        my_arm.moveShoulderToAngle(110);
        grab_serv.setPosition(1100);
        create_spin_block(100, 80 * ANGLE_MUlT);

        // drive to second cube
        set_create_distance(0);
        create_drive_straight(100);
        while (get_create_distance() < 900) { msleep(10); };
        create_stop();
        std::cout << "drove 900: " << get_create_distance() << "\n";

        create_spin_block(100, 90 * ANGLE_MUlT);
        create_stop();

        // grab second cube
        grab_cube(dist_sens, grab_serv, my_arm);

        create_spin_block(100, -90 * ANGLE_MUlT);
        set_create_distance(0);
        create_drive_straight(-100);
        while (get_create_distance() > -900) { msleep(10); };
        create_stop();

        create_spin_block(100, -90 * ANGLE_MUlT);
        create_stop();

        // drop second cube
        my_arm.moveEllbowTo(10);
        my_arm.moveShoulderToAngle(40);
        msleep(1000);

        grab_serv.setPosition(1630);
        msleep(1000);

        my_arm.moveShoulderToAngle(110);
        grab_serv.setPosition(1100);

        msleep(1000);
        // turn around and 
   }
    

    std::cout << "done\n";
    create_disconnect();
    camera_close();
    create_stop();

    return 0;
}
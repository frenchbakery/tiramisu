#include "drivers/tiramisu/cam_funcs/cam_funcs.hpp"
#include "drivers/tiramisu/grepper/grepper.hpp"
#include "drivers/tiramisu/arm/arm.hpp"
#include <kipr/botball/botball.h>
#include <kipr/analog/analog.hpp>
#include <kipr/create/create.h>
#include <kipr/create/create.hpp>
#include <kipr/camera/camera.h>
#include <kipr/time/time.h>
#include <kiprplus/create_motor.hpp>
#include <kiprplus/aggregation_engine.hpp>
#include <iostream>
#include <iomanip>
#include <thread>
#include "pitches.h"

using namespace kipr::motor;
using namespace kipr::servo;
using namespace kipr::digital;
using namespace kipr::analog;

#define ANGLE_MUlT 1 / 1.3

const std::vector<uint8_t> treble_melody
{
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

const std::vector<uint8_t> bass_melody{
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
const std::vector<uint8_t> treble_durations{
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

const std::vector<uint8_t> bass_durations{
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
    while (dist_sens.value() < 2500)
    {
        msleep(10);
        create_drive_straight(40);
    };
    create_stop();

    grab_serv.setPosition(1000);
    msleep(500);
    my_arm.moveEllbowTo(80);
    create_drive_straight(-100);
    msleep(1500);
    create_stop();
}

void playLongSong(const std::vector<uint8_t> &notes, const std::vector<uint8_t> &durations)
{
    float speed = 1.5;

    if (notes.size() != durations.size()) return;
    if (notes.size() == 0) return;
    int sections = std::ceil((float)(notes.size()) / 16.0f);

    while (get_create_song_playing())
                msleep(1);

    uint8_t scsong[32];
    int scnotes;
    int scnumber;

    for (int section = 0; section < sections; section++)
    {
        scnotes = std::min(notes.size() - 16 * section, 16ul);
        scnumber = section % 4;

        for (int i = 0; i < 16; i++)
        {
            scsong[i * 2] = notes[i + section * 16];
            int t = (64 * speed) / durations[i + section * 16];
            scsong[(i * 2) + 1] = t;
        }
        create_load_song(scsong, scnotes, scnumber);
        while (get_create_song_playing())
            msleep(1);
        create_play_song(scnumber);
        msleep(500);
    }
}

std::thread player;
bool player_exit;
void startPlayer()
{
    player_exit = false;
    player = std::thread([](){
        for (;;)
        {
            playLongSong(treble_melody, treble_durations);
            if (player_exit) break;
            playLongSong(bass_melody, bass_durations);
            if (player_exit) break;
        }
    });
}
void stopPlayer()
{
    player_exit = true;
    while (!player.joinable()) msleep(10);
    player.join();
}


int main()
{

    kp::CreateMotor::globalCreateConnect();

    std::shared_ptr<kp::CreateMotor> motorl = std::make_shared<kp::CreateMotor>(0);
    std::shared_ptr<kp::CreateMotor> motorr = std::make_shared<kp::CreateMotor>(1);
    kp::AggregationEngine engine({
        motorl, motorr
    });
    engine.setMovementModifiers({1, 1});

    motorl->clearPositionCounter();
    motorr->clearPositionCounter();

    motorl->setAbsoluteTarget(0);
    motorr->setAbsoluteTarget(0);

    motorl->enablePositionControl();
    motorr->enablePositionControl();



    msleep(3000);
    /*motorl->getPosition();
    motorr->getPosition();
    for (int i = 0; i < 1000; i++)
    {
        motorl->setAbsoluteTarget(i);
        motorr->setAbsoluteTarget(i);
        msleep(3);
    }
    for (int i = 1000; i > 0; i--)
    {
        motorl->setAbsoluteTarget(i);
        motorr->setAbsoluteTarget(i);
        msleep(3);
    }*/
    for (int i = 0; i < 20; i++)
    {
        engine.moveRelativePosition(500, 1000);
        engine.awaitSequenceComplete();
        msleep(1000);
        engine.moveRelativePosition(500, -1000);
        engine.awaitSequenceComplete();
        msleep(1000);
    }

    /*engine.moveRelativePosition(1000, 200);
    engine.awaitSequenceComplete(); msleep(5000);
    engine.moveRelativePosition(-1000, 200);
    engine.awaitSequenceComplete(); msleep(5000);
    engine.moveRelativePosition(1000, 200);
    engine.awaitSequenceComplete(); msleep(5000);
    engine.moveRelativePosition(-1000, 200);
    engine.awaitSequenceComplete(); msleep(5000);*/

    motorl->disablePositionControl();
    motorr->disablePositionControl();

    kp::CreateMotor::globalCreateDisconnect();

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
        while (get_create_distance() < 900)
        {
            msleep(10);
        };
        create_stop();
        std::cout << "drove 900: " << get_create_distance() << "\n";

        create_spin_block(100, 90 * ANGLE_MUlT);
        create_stop();

        // grab second cube
        grab_cube(dist_sens, grab_serv, my_arm);

        create_spin_block(100, -90 * ANGLE_MUlT);
        set_create_distance(0);
        create_drive_straight(-100);
        while (get_create_distance() > -900)
        {
            msleep(10);
        };
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
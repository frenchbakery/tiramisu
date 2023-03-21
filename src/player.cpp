/**
 * @file player.hpp
 * @author melektron
 * @brief music player for the create
 * @version 0.1
 * @date 2023-03-10
 *
 * @copyright Copyright FrenchBakery (c) 2023
 *
 */

#include <vector>
#include <cstdint>
#include <thread>
#include <cmath>
#include <kipr/time/time.h>
#include <kipr/create/create.h>
#include <kiprplus/create_motor.hpp>
#include "player.hpp"
#include "pitches.h"

static const std::vector<uint8_t> treble_melody{
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

static const std::vector<uint8_t> bass_melody{
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
static const std::vector<uint8_t> treble_durations{
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

static const std::vector<uint8_t> bass_durations{
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

void awaitSongDone()
{
    for (;;)
    {
        std::lock_guard lock(kp::CreateMotor::create_access_mutex);
        if (!get_create_song_playing())
            break;
        msleep(10);
    }
}

static void playLongSong(const std::vector<uint8_t> &notes, const std::vector<uint8_t> &durations)
{
    float speed = 1.5;

    if (notes.size() != durations.size())
        return;
    if (notes.size() == 0)
        return;
    int sections = std::ceil((float)(notes.size()) / 16.0f);

    awaitSongDone();

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
        {
            std::lock_guard lock(kp::CreateMotor::create_access_mutex);
            create_load_song(scsong, scnotes, scnumber);
        }
        awaitSongDone();
        {
            std::lock_guard lock(kp::CreateMotor::create_access_mutex);
            create_play_song(scnumber);
        }
        msleep(500);
    }
}

std::thread player;
bool player_exit;
void Player::start()
{
    player_exit = false;
    player = std::thread([]()
                         {
        for (;;)
        {
            playLongSong(treble_melody, treble_durations);
            if (player_exit) break;
            playLongSong(bass_melody, bass_durations);
            if (player_exit) break;
        } });
}
void Player::stop()
{
    player_exit = true;
    while (!player.joinable())
        msleep(10);
    player.join();
}

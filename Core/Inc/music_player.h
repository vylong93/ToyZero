/**
  ******************************************************************************
  * @file           : music_player.h
  * @brief          : Header for music_player.c file.
  *                   This file contains the common defines for playing music
  *                   node by Timer Compare Output channel.
  ******************************************************************************
  * @attention
  *
  * This software component is reference from bobsomers' MSP430 LaunchPad Music
  * https://github.com/bobsomers/msp430-launchpad-music
  *
  ******************************************************************************
  */

#ifndef __MUSIC_PLAYER_H
#define __MUSIC_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/* The number of milliseconds to kill the note before the duration expires.
 * This leaves a nice gap between back-to-back notes. */
#define DEAD_TIME_MS 20

/* Assuming 4/4 time, this is how many ticks we subdivide the beat into. So
 * in other words, 4 ticks per beat gives us 16th note resolution. We then
 * define all note durations in lengths of these 16th note "ticks". */
#define TICKS_PER_BEAT 4

/* Note frequency mapping to Timer Comparing Value (16 bits)
 * Note Value = Timer Clock Speed / (Note frequency * 2)
 * Ex. Note [B3: 246.94Hz] value = 1000000 / (246.94 * 2) = 2024.7833
 */
typedef enum note {
  REST = 0,
  E3 = 3033,
  Fs3 = 2703,
  G3 = 2551,
  Gs3 = 2408,
  A3 = 2273,
  As3 = 2145,
  B3 = 2025,
  C4 = 1911,
  Cs4 = 1804,
  Db4 = 1804,
  D4 = 1703,
  Ds4 = 1607,
  Eb4 = 1607,
  E4 = 1517,
  F4 = 1432,
  Fs4 = 1351,
  Gb4 = 1351,
  G4 = 1276,
  Gs4 = 1204,
  Ab4 = 1204,
  A4 = 1136,
  As4 = 1073,
  Bb4 = 1073,
  B4 = 1012,
  C5 = 956,
  Cs5 = 902,
  Db5 = 902,
  D5 = 851,
  Ds5 = 804,
  Eb5 = 804,
  E5 = 758,
  F5 = 716,
  Fs5 = 676,
  Gb5 = 676,
  G5 = 638,
  Gs5 = 602,
  Ab5 = 602,
  A5 = 568,
} note_t;

typedef struct measure {
  note_t* data;
  unsigned int length;
} measure_t;

extern volatile unsigned int current_note;

typedef enum Song {
  SUPER_MARIO = 0,
  BOBOMB_BATTLEFIELD = 1,
  PRINCESS_SLIDE = 2,
  NU_CUOI_XINH = 3,
  BAD_APPLE = 4,
} song_t;

measure_t* get_mario_measures_list(int *length);
measure_t* get_sevenHabits_measures_list(int *length);

void set_bpm(unsigned int bpm);
void play(note_t note, unsigned int duration_ticks);
void play_song(song_t song, char * p_force_stop);

void play_with_led(note_t note, unsigned int duration_ticks);
int get_button_index_from_node(note_t node);

void beep(void);
void audio_transistion_back(void);
void audio_transition_starting_to_easy(void);
void audio_transition_starting_to_normal(void);
void audio_transition_starting_to_hard(void);
void audio_transition_gameover(void);

void audio_button_1(void);
void audio_button_2(void);
void audio_button_3(void);
void audio_button_4(void);
void audio_button_5(void);
void audio_button_6(void);
void audio_button_7(void);

#ifdef __cplusplus
}
#endif

#endif /* __MUSIC_PLAYER_H */

/***************************************************************END OF FILE****/

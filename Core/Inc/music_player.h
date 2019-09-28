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

#define E3 3033
#define Fs3 2703
#define G3 2551
#define Gs3 2408
#define A3 2273
#define As3 2145
#define B3 2025
#define Cb4
#define C4 1911
#define Cs4 1804
#define Db4 1804
#define D4 1703
#define Ds4 1607
#define Eb4 1607
#define E4 1517
#define F4 1432
#define Fs4 1351
#define Gb4 1351
#define G4 1276
#define Gs4 1204
#define Ab4 1204
#define A4 1136
#define As4 1073
#define Bb4 1073
#define B4 1012
#define C5 956
#define Cs5 902
#define Db5 902
#define D5 851
#define Ds5 804
#define Eb5 804
#define E5 758
#define F5 716
#define Fs5 676
#define Gb5 676
#define G5 638
#define Gs5 602
#define Ab5 602
#define A5 568

extern volatile unsigned int current_note;

typedef enum Song {
	SUPER_MARIO = 0
} song_t;

void play_song(song_t song);

#ifdef __cplusplus
}
#endif

#endif /* __MUSIC_PLAYER_H */

/***************************************************************END OF FILE****/

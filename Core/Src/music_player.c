/**
  ******************************************************************************
  * @file    music_player.c
  * @brief   This file impelment the music component that using Timer Compare
  *          Output channel to output music tone to a piezo disc speaker.
  ******************************************************************************
  * @attention
  *
  * This software component is reference from bobsomers' MSP430 LaunchPad Music
  * https://github.com/bobsomers/msp430-launchpad-music
  *
  ******************************************************************************
  */

#include "music_player.h"
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim4; /* main.c */

static unsigned int ms_per_tick = 0; /* Reference for playing speed */
volatile unsigned int ms_elapsed = 0; /* Keeps track of the number of ms elapsed. */
volatile unsigned int current_note = G4; /* Keeps track of the current note we're playing. */

/**
  * @brief  Sets the tempo in BPM (beats per minute) at which the music will play back.
  * @param  bpm Number of Beat Per Minute
  * @retval None
  */
static void set_bpm(unsigned int bpm) {
  ms_per_tick = 60000 / (TICKS_PER_BEAT * bpm);
}

/**
  * @brief  Plays the passed note for the given duration (in ticks, see TICKS PER BEAT).
  *         Blocks until the note is over.
  * @param  note target note to play
  * @param  duration_ticks number of ticks for this note
  * @retval None
  */
static void play(note_t note, unsigned int duration_ticks) {
  unsigned int duration_ms = 0;

  /* Compute the duration (in ms). */
  duration_ms = duration_ticks * ms_per_tick;

  /* Set the current note. */
  current_note = (unsigned int)note;

  /* Enable the sound ouput. */
  HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_4);

  /* Reset the elapsed counter. */
  ms_elapsed = 0;

  /* Wait for the note duration to expire. */
  while (ms_elapsed < duration_ms - DEAD_TIME_MS);

  /* Disable sound output. */
  HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_4);

  /* Wait for the full duration to expire. */
  while (ms_elapsed < duration_ms);
}

/**
  * @brief  Rests for the given duration. Exactly the same as playing,
  *         except we just don't play a note for the duration.
  * @param  duration_ticks number of silent ticks
  * @retval None
  */
static void rest(unsigned int duration_ticks) {
  unsigned int duration_ms = 0;

  /* Compute the duration (in ms). */
  duration_ms = duration_ticks * ms_per_tick;

  /* Disable sound output. */
  HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_4);

  /* Reset the elapsed counter. */
  ms_elapsed = 0;

  /* Wait for the full duration to expire. */
  while (ms_elapsed < duration_ms);
}

// TODO: switch to array of music sheet
void super_mario_song_measure_1() {
  play(E4, 1);
  play(E4, 1);
  rest(1);
  play(E4, 1);
  rest(1);
  play(C4, 1);
  play(E4, 1);
  rest(1);
  play(G4, 1);
  rest(3);
  play(G3, 1);
  rest(3);
}
void super_mario_song_measure_2() {
  play(C4, 1);
  rest(2);
  play(G3, 1);
  rest(2);
  play(E3, 1);
  rest(2);
  play(A3, 1);
  rest(1);
  play(B3, 1);
  rest(1);
  play(As3, 1);
  play(A3, 1);
  rest(1);
}
void super_mario_song_measure_3() {
  play(G3, 1);
  play(E4, 1);
  rest(1);
  play(G4, 1);
  play(A4, 1);
  rest(1);
  play(F4, 1);
  play(G4, 1);
  rest(1);
  play(E4, 1);
  rest(1);
  play(C4, 1);
  play(D4, 1);
  play(B3, 1);
  rest(2);
}
void super_mario_song_measure_4() {
  rest(2);
  play(G4, 1);
  play(Fs4, 1);
  play(F4,1);
  play(Ds4, 1);
  rest(1);
  play(E4, 1);
  rest(1);
  play(Gs3, 1);
  play(A3, 1);
  play(C4, 1);
  rest(1);
  play(A3, 1);
  play(C4, 1);
  play(D4, 1);
}
void super_mario_song_measure_5() {
  rest(2);
  play(G4, 1);
  play(Fs4, 1);
  play(F4,1);
  play(Ds4, 1);
  rest(1);
  play(E4, 1);
  rest(1);
  play(C5, 1);
  rest(1);
  play(C5, 1);
  play(C5, 1);
  rest(3);
}
void super_mario_song_measure_6() {
  rest(2);
  play(Eb4, 1);
  rest(2);
  play(D4, 1);
  rest(2);
  play(C4, 1);
  rest(7);
}
void super_mario_song_measure_7() {
  play(C4, 1);
  play(C4, 1);
  rest(1);
  play(C4, 1);
  rest(1);
  play(C4, 1);
  play(D4, 1);
  rest(1);
  play(E4, 1);
  play(C4, 1);
  rest(1);
  play(A3, 1);
  play(G3, 1);
  rest(3);
}
void super_mario_song_measure_8() {
  play(C4, 1);
  play(C4, 1);
  rest(1);
  play(C4, 1);
  rest(1);
  play(C4, 1);
  play(D4, 1);
  play(E4,1);
  rest(7);
}
/**
  * @brief  Play super mario measures
  * @retval None
  */
void play_super_mario_song(void) {
  super_mario_song_measure_1();
  super_mario_song_measure_2();
  super_mario_song_measure_3();
  super_mario_song_measure_2();
  super_mario_song_measure_3();
  super_mario_song_measure_4();
  super_mario_song_measure_5();
  super_mario_song_measure_4();
  super_mario_song_measure_6();
  super_mario_song_measure_4();
  super_mario_song_measure_5();
  super_mario_song_measure_4();
  super_mario_song_measure_6();
  super_mario_song_measure_7();
  super_mario_song_measure_8();
  super_mario_song_measure_7();
}

/**
  * @brief  Blocking call function to play measures of a specific song
  * @param  song Target song to play
  *          This parameter can be one of the following values:
  *            @arg SUPER_MARIO: Super Mario song
  * @retval None
  */
void play_song(song_t song) {
  switch (song) {
    case SUPER_MARIO:
      set_bpm(150);
      play_super_mario_song();
    break;

    default:
      /* Unknown song to play */
    break;
  }
}

/***************************************************************END OF FILE****/

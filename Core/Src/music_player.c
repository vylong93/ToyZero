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
  *         Blocks until the note is over. If note is REST, we just don't play a note for the duration.
  * @param  note target note to play
  * @param  duration_ticks number of ticks for this note
  * @retval None
  */
static void play(note_t note, unsigned int duration_ticks) {
  unsigned int duration_ms = 0;

  /* Compute the duration (in ms). */
  duration_ms = duration_ticks * ms_per_tick;

  if (note == REST) {
    /* Disable sound output. */
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_4);
  } else {
    /* Set the current note. */
    current_note = (unsigned int)note;

    /* Enable the sound ouput. */
    HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_4);
  }

  /* Reset the elapsed counter. */
  ms_elapsed = 0;

  if (note != REST) {
    /* Wait for the note duration to expire. */
    while (ms_elapsed < duration_ms - DEAD_TIME_MS);

    /* Disable sound output. */
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_4);
  }

  /* Wait for the full duration to expire. */
  while (ms_elapsed < duration_ms);
}

/**
  * @brief
  * @param  measures_list is the list of measures of a song
  * @param  measure_sequence is the playing order of measure in measures_list
  * @param  sequence_length is the length of measure_sequence
  * @retval None
  * @retval None
  */
void play_measures(measure_t* measures_list, unsigned char* measure_sequence, int sequence_length) {
  for (int i = 0; i < sequence_length; i++) {
    for (int j = 0; j < measures_list[measure_sequence[i]].length; j += 2) {
      play(measures_list[measure_sequence[i]].data[j], measures_list[measure_sequence[i]].data[j + 1]);
    }
  }
}
/**
  * @brief  Play super mario measures
  * @retval None
  */
void play_super_mario_song(void) {
  note_t measure_1[] = { E4, 1, E4, 1, REST, 1, E4, 1, REST, 1, C4, 1, E4, 1, REST, 1, G4, 1, REST, 3, G3, 1, REST, 3 };
  note_t measure_2[] = { C4, 1, REST, 2, G3, 1, REST, 2, E3, 1, REST, 2, A3, 1, REST, 1, B3, 1, REST, 1, As3, 1, A3, 1, REST, 1 };
  note_t measure_3[] = { G3, 1, E4, 1, REST, 1, G4, 1, A4, 1, REST, 1, F4, 1, G4, 1, REST, 1, E4, 1, REST, 1, C4, 1, D4, 1, B3, 1, REST, 2 };
  note_t measure_4[] = { REST, 2, G4, 1, Fs4, 1, F4,1, Ds4, 1, REST, 1, E4, 1, REST, 1, Gs3, 1, A3, 1, C4, 1, REST, 1, A3, 1, C4, 1, D4, 1 };
  note_t measure_5[] = { REST, 2, G4, 1, Fs4, 1, F4, 1, Ds4, 1, REST, 1, E4, 1, REST, 1, C5, 1, REST, 1, C5, 1, C5, 1, REST, 3 };
  note_t measure_6[] = { REST, 2, Eb4, 1, REST, 2, D4, 1, REST, 2, C4, 1, REST, 7 };
  note_t measure_7[] = { C4, 1, C4, 1, REST, 1, C4, 1, REST, 1, C4, 1, D4, 1, REST, 1, E4, 1, C4, 1, REST, 1, A3, 1, G3, 1, REST, 3 };
  note_t measure_8[] = { C4, 1, C4, 1, REST, 1, C4, 1, REST, 1, C4, 1, D4, 1, E4, 1, REST, 7 };

  measure_t measures_list[] = { {.data = NULL, .length = 0},
                                {.data = measure_1, .length = 24},
                                {.data = measure_2, .length = 26},
                                {.data = measure_3, .length = 30},
                                {.data = measure_4, .length = 30},
                                {.data = measure_5, .length = 26},
                                {.data = measure_6, .length = 14},
                                {.data = measure_7, .length = 28},
                                {.data = measure_8, .length = 18}
                              };

  unsigned char super_mario_measure_sequence[] = { 1, 2, 3, 2, 3, 4, 5, 4, 6, 4, 5, 4, 6, 7, 8, 7 };

  play_measures(measures_list, super_mario_measure_sequence, 16);
}

/**
  * @brief  Play Bobomb Battlefield measures
  * @retval None
  */
void play_bobomb_battlefield_song(void) {
  note_t measure_1[] = { C5, 2, A4, 2, C5, 2, D5, 1, C5, 1, REST, 1, E4, 3, F4, 2, Fs4, 2 };
  note_t measure_2[] = { G4, 3, REST, 1, G4, 1, G3, 2, G4, 2, REST, 5, D5, 1, Eb5, 1 };
  note_t measure_3[] = { E5, 1, Eb5, 1, E5, 1, G5, 2, A5, 1, G5, 2, C5, 4, REST, 2, G4, 1, Ab4, 1 };
  note_t measure_4[] = { A4, 1, Ab4, 1, A4, 1, C5, 2, D5, 1, C5, 2, A4, 4, REST, 2, A4, 2 };
  note_t measure_5[] = { G4, 1, E4, 1, G4, 1, G5, 1, REST, 2, G4, 1, E4, 1, G4, 1, G5, 1, REST, 2, G4, 1, E4, 1, G4, 1, G5, 1 };
  note_t measure_6[] = { REST, 2, G5, 2, A5, 1, G5, 2, D5, 5, REST, 2, D4, 1, Eb4, 1 };
  note_t measure_7[] = { E4, 1, Eb4, 1, E4, 1, G4, 2, A4, 1, G4, 2, Bb4, 4, REST, 2, Ab4, 2 };
  note_t measure_8[] = { A4, 1, Ab4, 1, A4, 1, C5, 2, D5, 1, C5, 2, A4, 4, REST, 2, A4, 2 };
  note_t measure_9[] = { G4, 1, Fs4, 1, G4, 1, G3, 1, REST, 2, G4, 1, Fs4, 1, G4, 1, G3, 1, REST, 1, Eb4, 1, E4, 2, E4, 2 };
  note_t measure_10[] = { C4, 12, REST, 4 };

  measure_t measures_list[] = { {.data = NULL, .length = 0},
                                {.data = measure_1, .length = 18},
                                {.data = measure_2, .length = 16},
                                {.data = measure_3, .length = 20},
                                {.data = measure_4, .length = 18},
                                {.data = measure_5, .length = 28},
                                {.data = measure_6, .length = 16},
                                {.data = measure_7, .length = 18},
                                {.data = measure_8, .length = 18},
                                {.data = measure_9, .length = 26},
                                {.data = measure_10, .length = 4},
                              };
  unsigned char super_bobomb_battlefield_sequence[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

  play_measures(measures_list, super_bobomb_battlefield_sequence, 10);
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

    case BOBOMB_BATTLEFIELD:
      set_bpm(110);
      play_bobomb_battlefield_song();
    break;

    default:
      /* Unknown song to play */
    break;
  }
}

/***************************************************************END OF FILE****/

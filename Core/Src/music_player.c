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
  note_t measures_1[] = { E4, 1, E4, 1, REST, 1, E4, 1, REST, 1, C4, 1, E4, 1, REST, 1, G4, 1, REST, 3, G3, 1, REST, 3 };
  note_t measures_2[] = { C4, 1, REST, 2, G3, 1, REST, 2, E3, 1, REST, 2, A3, 1, REST, 1, B3, 1, REST, 1, As3, 1, A3, 1, REST, 1 };
  note_t measures_3[] = { G3, 1, E4, 1, REST, 1, G4, 1, A4, 1, REST, 1, F4, 1, G4, 1, REST, 1, E4, 1, REST, 1, C4, 1, D4, 1, B3, 1, REST, 2 };
  note_t measures_4[] = { REST, 2, G4, 1, Fs4, 1, F4,1, Ds4, 1, REST, 1, E4, 1, REST, 1, Gs3, 1, A3, 1, C4, 1, REST, 1, A3, 1, C4, 1, D4, 1 };
  note_t measures_5[] = { REST, 2, G4, 1, Fs4, 1, F4, 1, Ds4, 1, REST, 1, E4, 1, REST, 1, C5, 1, REST, 1, C5, 1, C5, 1, REST, 3 };
  note_t measures_6[] = { REST, 2, Eb4, 1, REST, 2, D4, 1, REST, 2, C4, 1, REST, 7 };
  note_t measures_7[] = { C4, 1, C4, 1, REST, 1, C4, 1, REST, 1, C4, 1, D4, 1, REST, 1, E4, 1, C4, 1, REST, 1, A3, 1, G3, 1, REST, 3 };
  note_t measures_8[] = { C4, 1, C4, 1, REST, 1, C4, 1, REST, 1, C4, 1, D4, 1, E4, 1, REST, 7 };

  measure_t measures_list[] = { {.data = NULL, .length = 0},
                                {.data = measures_1, .length = 24},
                                {.data = measures_2, .length = 26},
                                {.data = measures_3, .length = 30},
                                {.data = measures_4, .length = 30},
                                {.data = measures_5, .length = 26},
                                {.data = measures_6, .length = 14},
                                {.data = measures_7, .length = 28},
                                {.data = measures_8, .length = 18}
                              };

  unsigned char super_mario_measure_sequence[] = { 1, 2, 3, 2, 3, 4, 5, 4, 6, 4, 5, 4, 6, 7, 8, 7 };

  play_measures(measures_list, super_mario_measure_sequence, 16);
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

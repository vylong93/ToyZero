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
#include "leds_driver.h"
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim4; /* main.c */

static unsigned int ms_per_tick = 0; /* Reference for playing speed */
volatile unsigned int ms_elapsed = 0; /* Keeps track of the number of ms elapsed. */
volatile unsigned int current_note = G4; /* Keeps track of the current note we're playing. */

/* EASY SONG: Super MARIO */
note_t mario_measure_1[] = { E4, 1, E4, 1, REST, 1, E4, 1 }; //8
note_t mario_measure_2[] = { REST, 1, C4, 1, E4, 1, REST, 1, G4, 1, REST, 3, G3, 1, REST, 3 }; //16
note_t mario_measure_3[] = { C4, 1, REST, 2, G3, 1, REST, 2, E3, 1, REST, 2, A3, 1, REST, 1, B3, 1 }; //18
note_t mario_measure_4[] = { REST, 1, As3, 1, A3, 1, REST, 1, G3, 1, E4, 1, REST, 1, G4, 1 }; //16
note_t mario_measure_5[] = { A4, 1, REST, 1, F4, 1, G4, 1, REST, 1, E4, 1, REST, 1, C4, 1 }; // 16
note_t mario_measure_6[] = { D4, 1, B3, 1, REST, 2, REST, 2, G4, 1, Fs4, 1 }; //12
note_t mario_measure_7[] = { F4, 1, Ds4, 1, REST, 1, E4, 1, REST, 1, Gs3, 1, A3, 1, C4, 1 }; //16
note_t mario_measure_8[] = { REST, 1, A3, 1, C4, 1, D4, 1, REST, 2, G4, 1, Fs4, 1 }; // 14
note_t mario_measure_9[] = { F4, 1, Fs4, 1, REST, 1, E4, 1, REST, 1, C5, 1, REST, 1, C5, 1 }; // 16
note_t mario_measure_10[] = { C5, 1, REST, 3, REST, 2, Eb4, 1, REST, 2, D4, 1, REST, 2, C4, 1, REST, 7 }; // 18
note_t mario_measure_11[] = { C4, 1, C4, 1, REST, 1, C4, 1, REST, 1, C4, 1, D4, 1, REST, 1 }; // 16
note_t mario_measure_12[] = { E4, 1, C4, 1, REST, 1, A3, 1, G3, 1, REST, 3, C4, 1, C4, 1, REST, 1, C4, 1 }; // 20
note_t mario_measure_13[] = { REST, 1, C4, 1, D4, 1, E4, 1, REST, 7 }; // 10
measure_t mario_measures_list[] = { {.data = mario_measure_1, .length = 8},
                                    {.data = mario_measure_2, .length = 16},
                                    {.data = mario_measure_3, .length = 18},
                                    {.data = mario_measure_4, .length = 16},
                                    {.data = mario_measure_5, .length = 16},
                                    {.data = mario_measure_6, .length = 12},
                                    {.data = mario_measure_7, .length = 16},
                                    {.data = mario_measure_8, .length = 14},
                                    {.data = mario_measure_9, .length = 16},
                                    {.data = mario_measure_10, .length = 18},
                                    {.data = mario_measure_11, .length = 18},
                                    {.data = mario_measure_12, .length = 20},
                                    {.data = mario_measure_13, .length = 10}};

measure_t* get_mario_measures_list(int *length) {
  *length = 13;
  return mario_measures_list;
}

/**
  * @brief  Sets the tempo in BPM (beats per minute) at which the music will play back.
  * @param  bpm Number of Beat Per Minute
  * @retval None
  */
void set_bpm(unsigned int bpm) {
  ms_per_tick = 60000 / (TICKS_PER_BEAT * bpm);
}

/**
  * @brief  Plays the passed note for the given duration (in ticks, see TICKS PER BEAT).
  *         Blocks until the note is over. If note is REST, we just don't play a note for the duration.
  * @param  note target note to play
  * @param  duration_ticks number of ticks for this note
  * @retval None
  */
void play(note_t note, unsigned int duration_ticks) {
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
  * @brief  Plays the passed note for the given duration (in ticks, see TICKS PER BEAT).
  *         Blocks until the note is over. If note is REST, we just don't play a note for the duration.
  * @param  note target note to play
  * @param  duration_ticks number of ticks for this note
  * @retval None
  */
led_rgb_t _get_led_position_from_note (note_t node) {
  if ((node == C4) || (node == Cs4) || (node == C5) || (node == Cs5)) {
    return led1;
  }
  if ((node == Db4) || (node == D4) || (node == Ds4) || (node == Db5) || (node == D5) || (node == Ds5)) {
    return led2;
  }
  if ((node == E3) || (node == Eb4) || (node == E4) || (node == Eb5) || (node == E5) || (node == Fs3)) {
    return led3;
  }
  if ((node == F4) || (node == Fs4) || (node == F5) || (node == Fs5)) {
    return led4;
  }
  if ((node == G3) || (node == Gs3) || (node == Gb4) || (node == G4) || (node == Gs4) || (node == Gb5) || (node == G5) || (node == Gs5)) {
    return led5;
  }
  if ((node == A3) || (node == As3) || (node == Ab4) || (node == A4) || (node == As4) || (node == Ab5) || (node == A5)) {
    return led6;
  }
  if ((node == B3) || (node == Bb4) || (node == B4)) {
    return led7;
  }
  return led1; /* REST */
}

int get_button_index_from_node (note_t node) {
  if ((node == C4) || (node == Cs4) || (node == C5) || (node == Cs5)) {
    return 0;
  }
  if ((node == Db4) || (node == D4) || (node == Ds4) || (node == Db5) || (node == D5) || (node == Ds5)) {
    return 1;
  }
  if ((node == E3) || (node == Eb4) || (node == E4) || (node == Eb5) || (node == E5) || (node == Fs3)) {
    return 2;
  }
  if ((node == F4) || (node == Fs4) || (node == F5) || (node == Fs5)) {
    return 3;
  }
  if ((node == G3) || (node == Gs3) || (node == Gb4) || (node == G4) || (node == Gs4) || (node == Gb5) || (node == G5) || (node == Gs5)) {
    return 4;
  }
  if ((node == A3) || (node == As3) || (node == Ab4) || (node == A4) || (node == As4) || (node == Ab5) || (node == A5)) {
    return 5;
  }
  if ((node == B3) || (node == Bb4) || (node == B4)) {
    return 6;
  }
  return 0; /* REST */
}

void play_with_led(note_t note, unsigned int duration_ticks) {
  unsigned int duration_ms = 0;
  led_rgb_t led = _get_led_position_from_note(note);

  /* Compute the duration (in ms). */
  duration_ms = duration_ticks * ms_per_tick;

  if (note == REST) {
    /* Disable sound output. */
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_4);
  } else {
    /* Set the current note. */
    current_note = (unsigned int)note;

    /* Light corresponding led */
    set_rgb_color_base_on_led(led);
    turn_led_on(led);

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

    /* Off coresponding led */
    turn_led_off(led);
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
void play_measures(measure_t* measures_list, unsigned char* measure_sequence, int sequence_length, char * p_force_stop) {
  for (int i = 0; (i < sequence_length) && (*p_force_stop == 0); i++) {
    for (int j = 0; (j < measures_list[measure_sequence[i]].length) && (*p_force_stop == 0); j += 2) {
      play_with_led(measures_list[measure_sequence[i]].data[j], measures_list[measure_sequence[i]].data[j + 1]);
    }
  }
}
/**
  * @brief  Play super mario measures
  * @retval None
  */
void play_super_mario_song(char * p_force_stop) {
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

  play_measures(measures_list, super_mario_measure_sequence, 16, p_force_stop);
}

/**
  * @brief  Play Bobomb Battlefield measures
  * @retval None
  */
void play_bobomb_battlefield_song(char * p_force_stop) {
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

  play_measures(measures_list, super_bobomb_battlefield_sequence, 10, p_force_stop);
}

/**
  * @brief  Play Princess Slide measures
  * @retval None
  */
void play_princess_slide_song(char * p_force_stop) {
  note_t measure_1[] = { A5, 4, A3, 4, A5, 4, A3, 4 };
  note_t measure_2[] = { A5, 4, Fs5, 4, E5, 4, Ds5, 4 };
  note_t measure_3[] = { D5, 2, G3, 2, B3, 2, G3, 2, D5, 2, G3, 2, B3, 2, G3, 2 };
  note_t measure_4[] = { D5, 2, G3, 2, B3, 2, G4, 1, B4, 1, D5, 2, G3, 2, E5, 2, G3, 2 };
  note_t measure_5[] = { D5, 2, Fs3, 2, A3, 2, A4, 2, Fs5, 2, Fs3, 2, A5, 2, Fs3, 2 };
  note_t measure_6[] = { Gs5, 2, Fs3, 2, A5, 2, Fs3, 2, Gs5, 1, A5, 1, Gs5, 1, Fs5, 1, D5, 2, A4, 2 };
  note_t measure_7[] = { B4, 2, G3, 1, Cs5, 1, D5, 2, G3, 1, E5, 1, Fs5, 1, F5, 1, Fs5, 1, G5, 1, A5, 1, G5, 1, Fs5, 1, E5, 1 };
  note_t measure_8[] = { D5, 1, Cs5, 1, D5, 1, A5, 1, Cs5, 1, C5, 1, Cs5, 1, A5, 1, Cs5, 1, C5, 1, B4, 1, Bb4, 1, A4, 1, Gs4, 1, G4, 1, E4, 1 };
  note_t measure_9[] = { A4, 2, A3, 2, D4, 2, A3, 2, B4, 2, A3, 2, A4, 2, A3, 2 };
  note_t measure_10[] = { D4, 2, D5, 2, Cs5, 2, D5, 2, Fs5, 2, A3, 2, E5, 2, D5, 2 };
  note_t measure_11[] = { B4, 2, G3, 2, B4, 2, G3, 2, E5, 2, G3, 2, D5, 2, G3, 2 };
  note_t measure_12[] = { B4, 2, B4, 2, Bb4, 2, B4, 2, D5, 2, G3, 2, Cs5, 2, B4, 2 };
  note_t measure_13[] = { A4, 2, Fs3, 2, Fs3, 2, A4, 2, Fs3, 2, Fs3, 2, A5, 2, Fs3, 2 };

  measure_t measures_list[] = { {.data = NULL, .length = 0},
                                {.data = measure_1, .length = 8},
                                {.data = measure_2, .length = 8},
                                {.data = measure_3, .length = 16},
                                {.data = measure_4, .length = 18},
                                {.data = measure_5, .length = 16},
                                {.data = measure_6, .length = 20},
                                {.data = measure_7, .length = 28},
                                {.data = measure_8, .length = 32},
                                {.data = measure_9, .length = 16},
                                {.data = measure_10, .length = 16},
                                {.data = measure_11, .length = 16},
                                {.data = measure_12, .length = 16},
                                {.data = measure_13, .length = 16},
                              };
  unsigned char super_princess_slide_sequence[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

  play_measures(measures_list, super_princess_slide_sequence, 13, p_force_stop);
}

/**
  * @brief  Blocking call function to play measures of a specific song
  * @param  song Target song to play
  *          This parameter can be one of the following values:
  *            @arg SUPER_MARIO: Super Mario song
  * @retval None
  */
void play_song(song_t song, char * p_force_stop) {
  switch (song) {
    case SUPER_MARIO:
      set_bpm(150);
      play_super_mario_song(p_force_stop);
    break;

    case BOBOMB_BATTLEFIELD:
      set_bpm(110);
      play_bobomb_battlefield_song(p_force_stop);
    break;

    case PRINCESS_SLIDE:
      set_bpm(180);
      play_princess_slide_song(p_force_stop);
    break;

    default:
      /* Unknown song to play */
    break;
  }
}


/**
  * @brief  Play a beep sound
  * @retval None
  */
void beep(void) {
  set_bpm(150);
  play(D5, 1);
}

/**
  * @brief  Play a beep sound
  * @retval None
  */
void audio_transistion_back(void) {
  set_bpm(150);
  play(A4, 1);
  play(D4, 1);
}

/**
  * @brief  Play a beep sound
  * @retval None
  */
void audio_transition_starting_to_easy(void) {
  set_bpm(150);
  play(D4, 1);
  play(A4, 1);
}

/**
  * @brief  Play a beep sound
  * @retval None
  */
void audio_transition_starting_to_normal(void) {
  set_bpm(150);
  play(D4, 1);
  play(D5, 1);
}

/**
  * @brief  Play a beep sound
  * @retval None
  */
void audio_transition_starting_to_hard(void) {
  set_bpm(150);
  play(D4, 1);
  play(D4, 1);
  play(D5, 1);
}

/**
  * @brief  Play a beep sound
  * @retval None
  */
void audio_transition_gameover(void) {
  set_bpm(150);
  play(F4, 1);
  play(E4, 1);
  play(D4, 2);
  play(C4, 2);
}

void audio_button_1(void) { set_bpm(150); play(C4, 1); }
void audio_button_2(void) { set_bpm(150); play(D4, 1); }
void audio_button_3(void) { set_bpm(150); play(E4, 1); }
void audio_button_4(void) { set_bpm(150); play(F4, 1); }
void audio_button_5(void) { set_bpm(150); play(G4, 1); }
void audio_button_6(void) { set_bpm(150); play(A4, 1); }
void audio_button_7(void) { set_bpm(150); play(B4, 1); }

/***************************************************************END OF FILE****/

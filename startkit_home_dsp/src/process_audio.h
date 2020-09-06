// Copyright (c) 2016, XMOS Ltd, All rights reserved
#ifndef _AUDIO_EFFECTS_H_
#define _AUDIO_EFFECTS_H_
#include <startkit_gpio.h>
#include <stddef.h>

/** Task to apply audio effects to a sample stream.
 *
 *  \param c_dsp_eq   channel for receiving samples and sending updated samples
 *  \param i_led      LED interface connection to startkit_gpio task
 *  \param i_button   button control connection to the startkit_gpio task
 *  \param c_gain     channel for receiving updates to the gain parameter
 *                    of the effects
 */
void audio_effects(streaming chanend c_dsp, static const size_t num_chans);

void ui_handler(client startkit_led_if i_led, client startkit_button_if i_button,
        client slider_if i_slider_x, client slider_if i_slider_y);

#endif // _AUDIO_EFFECTS_H_

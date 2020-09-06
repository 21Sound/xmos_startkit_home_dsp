// Copyright (c) 2016, XMOS Ltd, All rights reserved
#include <process_audio.h>
#include <stdint.h>
#include "global_defines.h"
#include "cppdsp.h"

void audio_effects(streaming chanend c_dsp, static const size_t numChans) {
    int32_t sampsIn[numChans] = {0};
    int32_t sampsOut[numChans] = {0};

    int32_t cnt = 0;

    while(1) {

        #pragma loop unroll
        for (size_t i = 0; i < numChans; i++) {
            c_dsp :> sampsIn[i];
            c_dsp <: sampsOut[i];
        }

        if (cnt<15*SAMPLE_FREQUENCY) {
            cnt++;
        } else {
            //EQ processing
            cppdsp_process_eq(sampsIn);

            #pragma loop unroll
            for (size_t i = 0; i < numChans; i++) {
                sampsOut[i] = sampsIn[i];
            }
        }

    }
}

void ui_handler(client startkit_led_if i_led, client startkit_button_if i_button,
        client slider_if i_slider_x, client slider_if i_slider_y) {
    return;
}

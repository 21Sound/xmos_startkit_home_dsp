/*
 * cpp_dsp.cpp
 *
 *  Created on: 02.05.2020
 *      Author: hjaeger
 */

#include "cppdsp.h"
#include "eq32.h"
#include "limiter32.h"

static EQ32 pEQ1(PEAKING_EQ, 55.0, SAMPLE_FREQUENCY, 11.0, 1.0);
static EQ32 pEQ2(PEAKING_EQ, 55.0, SAMPLE_FREQUENCY, 11.0, 1.0);
static EQ32 hiPass1(HIGH_PASS_EQ, 40, SAMPLE_FREQUENCY, 0.0, 0.85);
static EQ32 hiPass2(HIGH_PASS_EQ, 40, SAMPLE_FREQUENCY, 0.0, 0.85);
static EQ32 hiShelv(HIGH_SHELF_EQ, 8000, SAMPLE_FREQUENCY, 3.0, 0.71);

static Limiter32 postprocLim(-30.2, 0.001, 0.1, 1.0, NUM_CHANS, SAMPLE_FREQUENCY);

void cppdsp_process_eq(int32_t inSamps[NUM_CHANS]) {

    for (int i = 0; i < NUM_CHANS; ++i) {
        //Damp by ~24dBFS
        inSamps[i] >>= 4;
    }

    //Equalizer processing
    pEQ1.process(inSamps);
    pEQ2.process(inSamps);
    hiPass1.process(inSamps);
    hiPass2.process(inSamps);
    hiShelv.process(inSamps);

    //Postprocess Limiter to force signal amplitudes below -30.2dBFS after EQing
    postprocLim.process(inSamps);

    //Amplify by ~30.1dB
    for (int i = 0; i < NUM_CHANS; ++i) {
        inSamps[i] <<= 5;
    }
}

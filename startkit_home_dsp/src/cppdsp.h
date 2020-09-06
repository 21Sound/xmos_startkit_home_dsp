/*
 * cpp_dsp.h
 *
 *  Created on: 02.05.2020
 *      Author: hjaeger
 */

#ifndef __CPP_DSP_H__ // include guard
#define __CPP_DSP_H__

#include <stdint.h>
#include "global_defines.h"

extern "C" {

void cppdsp_init_eq();

void cppdsp_process_eq(int32_t inSamps[NUM_CHANS]);

}

#endif

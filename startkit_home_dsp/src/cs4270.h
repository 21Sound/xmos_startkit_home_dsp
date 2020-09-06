// Copyright (c) 2016, XMOS Ltd, All rights reserved
#ifndef _CS4270_H_
#define _CS4270_H_
#include <i2c.h>

enum codec_mode_t {
  CODEC_IS_I2S_MASTER,
  CODEC_IS_I2S_SLAVE
};

void cs4270_configure(client i2c_master_if i2c, uint8_t device_addr,
                      unsigned sample_frequency,
                      unsigned master_clock_frequency,
                      enum codec_mode_t codec_mode);

#endif // _CS4270_H_

// Copyright (c) 2016, XMOS Ltd, All rights reserved
/* Top level app for 'dalek' audio effects app.
 *  Uses startKIT & audio slice hardware to provide various audio effects
 *  Use button to select biquad filter type
 *  Use sliders to control 'dalek' effect (one does modulation depth, the other freq)
 *  Uses sawtooth LFO to amplitude modulate audio to make dalek effect
 *  Hint: enable xSCOPE RT to see input/output/LFO waveforms.
 ******************************************************************************/

#include <xs1.h>
#include <platform.h>
#include <startkit_gpio.h>
#include <process_audio.h>
#include <i2s.h>
#include <i2c.h>
#include <gpio.h>
#include <cs4270.h>
#include "global_defines.h"
#include "cppdsp.h"

/* Ports and clocks used by the application */
startkit_gpio_ports gpio_ports =
  on tile[0] : {XS1_PORT_32A, XS1_PORT_4A, XS1_PORT_4B, XS1_CLKBLK_3};

out buffered port:32 p_dout[1] = on tile[0]: {XS1_PORT_1G};

in buffered port:32 p_din[1]   = on tile[0]: {XS1_PORT_1K};

in port p_mclk                 = on tile[0]: XS1_PORT_1E;
out buffered port:32 p_bclk    = on tile[0]: XS1_PORT_1F;
out buffered port:32 p_lrclk   = on tile[0]: XS1_PORT_1I;

port p_i2c                     = on tile[0]: XS1_PORT_4D;

port p_gpio                    = on tile[0]: XS1_PORT_4C;

clock mclk                     = on tile[0]: XS1_CLKBLK_1;
clock bclk                     = on tile[0]: XS1_CLKBLK_2;

// Pin map for GPIO: clock select, codec reset are on pins 1 and 2 of 4C
static char gpio_pin_map[NUM_CHANS] = {1, 2};

[[distributable]]
void i2s_handler(server i2s_callback_if i2s,
                 client i2c_master_if i2c,
                 client output_gpio_if clock_select,
                 client output_gpio_if codec_reset,
                 streaming chanend c_dsp)
{
  int32_t in_samps[NUM_CHANS] = {0};
  int32_t out_samps[NUM_CHANS] = {0};

  while (1) {
    select {
    case i2s.init(i2s_config_t &?i2s_config, tdm_config_t &?tdm_config):
      /* Set CODEC in reset */
      codec_reset.output(0);

      /* Set master clock select appropriately */
      if ((SAMPLE_FREQUENCY % 22050) == 0) {
        clock_select.output(0);
      }else {
        clock_select.output(1);
      }

      /* Hold in reset for 2ms while waiting for MCLK to stabilise */
      delay_milliseconds(2);

      /* CODEC out of reset */
      codec_reset.output(1);

      cs4270_configure(i2c, CODEC_I2C_DEVICE_ADDR,
                       SAMPLE_FREQUENCY, MASTER_CLOCK_FREQUENCY,
                       CODEC_IS_I2S_SLAVE);

      /* Configure the I2S bus */
      i2s_config.mode = I2S_MODE_I2S;
      i2s_config.mclk_bclk_ratio = (MASTER_CLOCK_FREQUENCY/SAMPLE_FREQUENCY)/64;
      break;

    case i2s.restart_check() -> i2s_restart_t restart:
      // This application never restarts the I2S bus
      restart = I2S_NO_RESTART;
      break;

    case i2s.receive(size_t index, int32_t sample):
      if (index == 0) {
        for (size_t i = 0; i < NUM_CHANS; i++) {
          c_dsp <: in_samps[i];
          c_dsp :> out_samps[i];
        }
      }
      in_samps[index] = sample;
      break;

    case i2s.send(size_t index) -> int32_t sample:
      sample = out_samps[index];
      break; // end of select
    }
  }
};


int main(void)
{
  streaming chan c_aud_dsp;
  startkit_led_if i_led;
  startkit_button_if i_button;
  slider_if i_slider_x, i_slider_y;
  i2s_callback_if i_i2s;
  i2c_master_if i_i2c[1];
  output_gpio_if i_gpio[NUM_CHANS];
  par {

    on tile[0]: i2c_master_single_port(i_i2c, 1, p_i2c, 10, 0, 1, 0);

    on tile[0]: output_gpio(i_gpio, NUM_CHANS, p_gpio, gpio_pin_map);

    on tile[0]: startkit_gpio_driver(i_led, i_button, i_slider_x, i_slider_y, gpio_ports);

    on tile[0]: {  configure_clock_src(mclk, p_mclk);
                   start_clock(mclk);
                   i2s_master(i_i2s, p_dout, 1, p_din, 1,
                              p_bclk, p_lrclk, bclk, mclk);
                }

    on tile[0]: i2s_handler(i_i2s, i_i2c[0], i_gpio[0], i_gpio[1], c_aud_dsp);

    on tile[0]: audio_effects(c_aud_dsp, NUM_CHANS);

    on tile[0]: ui_handler(i_led, i_button, i_slider_x, i_slider_y);


  }
  return 0;
}

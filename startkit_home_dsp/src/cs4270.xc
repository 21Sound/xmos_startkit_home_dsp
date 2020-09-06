// Copyright (c) 2016, XMOS Ltd, All rights reserved
#include <cs4270.h>

#define CODEC_DEV_ID_ADDR           0x01
#define CODEC_PWR_CTRL_ADDR         0x02
#define CODEC_MODE_CTRL_ADDR        0x03
#define CODEC_ADC_DAC_CTRL_ADDR     0x04
#define CODEC_TRAN_CTRL_ADDR        0x05
#define CODEC_MUTE_CTRL_ADDR        0x06
#define CODEC_DACA_VOL_ADDR         0x07
#define CODEC_DACB_VOL_ADDR         0x08

void cs4270_configure(client i2c_master_if i2c, uint8_t device_addr,
                      unsigned sample_frequency,
                      unsigned master_clock_frequency,
                      enum codec_mode_t codec_mode)
{
  /* Set power down bit in the CODEC over I2C */
  i2c.write_reg(device_addr, CODEC_PWR_CTRL_ADDR, 0x01);

  /* Now set all registers as we want them */


  if (codec_mode == CODEC_IS_I2S_SLAVE) {
    /* Mode Control Reg:
       Set FM[1:0] as 11. This sets Slave mode.
       Set MCLK_FREQ[2:0] as 010. This sets MCLK to 512Fs in Single,
       256Fs in Double and 128Fs in Quad Speed Modes.
       This means 24.576MHz for 48k and 22.5792MHz for 44.1k.
       Set Popguard Transient Control.
       So, write 0x35. */
    i2c.write_reg(device_addr, CODEC_MODE_CTRL_ADDR, 0x35);
  } else {
    /* In master mode (i.e. Xcore is I2S slave) to avoid contention
       configure one CODEC as master one the other as slave */

    /* Set FM[1:0] Based on Single/Double/Quad mode
       Set MCLK_FREQ[2:0] as 010. This sets MCLK to 512Fs in Single, 256Fs in Double and 128Fs in Quad Speed Modes.
       This means 24.576MHz for 48k and 22.5792MHz for 44.1k.
       Set Popguard Transient Control.*/

    unsigned char val = 0b0101;

    if(sample_frequency < 54000) {
      // | with 0..
    } else if(sample_frequency < 108000) {
      val |= 0b00100000;
    } else  {
      val |= 0b00100000;
    }
    i2c.write_reg(device_addr, CODEC_MODE_CTRL_ADDR, val);
  }

  /* ADC & DAC Control Reg:
     Leave HPF for ADC inputs continuously running.
     Digital Loopback: OFF
     DAC Digital Interface Format: I2S
     ADC Digital Interface Format: I2S
     So, write 0x09. */
  i2c.write_reg(device_addr, CODEC_ADC_DAC_CTRL_ADDR, 0x09);

  /* Transition Control Reg:
     No De-emphasis. Don't invert any channels.
     Independent vol controls. Soft Ramp and Zero Cross enabled.*/
  i2c.write_reg(device_addr, CODEC_TRAN_CTRL_ADDR, 0x60);

  /* Mute Control Reg: Turn off AUTO_MUTE */
  i2c.write_reg(device_addr, CODEC_MUTE_CTRL_ADDR, 0x00);

  /* DAC Chan A Volume Reg:
     We don't require vol control so write 0x00 (0dB) */
  i2c.write_reg(device_addr, CODEC_DACA_VOL_ADDR, 0x00);

  /* DAC Chan B Volume Reg:
     We don't require vol control so write 0x00 (0dB)  */
  i2c.write_reg(device_addr, CODEC_DACB_VOL_ADDR, 0x00);

  /* Clear power down bit in the CODEC over I2C */
  i2c.write_reg(device_addr, CODEC_PWR_CTRL_ADDR, 0x00);
}


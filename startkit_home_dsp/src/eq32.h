/*---------------------------------------------------------------------------*\
|   Declaration of a Fixed-Point Equalizer Class                              |
|                                                                             |
|   Authors:                                                                  |
|       Hagen Jaeger, Uwe Simmer, 17. Oct. 2015.                              |
|       FixedPoint Adaption, Hagen Jaeger, 01.05.2020                         |
|                                                                             |
|   References:                                                               |
|   [1] Robert Bristow-Johnson,                                               |
|       "Cookbook Formulae for Audio EQ Biquad Filter Coefficients",          |
|       www.musicdsp.org/files/Audio-EQ-Cookbook.txt                          |
|                                                                             |
|   [2] CORRECTION: Cookbook Formulae for Audio EQ Biquad Filter Coefficients |
|       music.columbia.edu/pipermail/music-dsp/1998-October/054185.html       |
|                                                                             |
|   [3] Jon Dattorro,                                                         |
|       "The Implementation of Recursive Digital Filters for                  |
|        High-Fidelity Audio", Journal of the Audio Engineering Society,      |
|        Vol. 36, No. 11, Nov. 1988.                                          |
\*---------------------------------------------------------------------------*/

#ifndef EQ32_H
#define EQ32_H

extern "C" {

#ifndef EQ_CHANS
#define EQ_CHANS 2
#endif

#include <stdint.h>

enum {
    GAIN_EQ,
    LOW_PASS_EQ,
    HIGH_PASS_EQ,
    BAND_PASS_EQ,
    NOTCH_EQ,
    PEAKING_EQ,
    LOW_SHELF_EQ,
    HIGH_SHELF_EQ,
    LOW_SHELFQ_EQ,
    HIGH_SHELFQ_EQ,
    ALLPASS_EQ
};

// number of coefficients
#define BIQUAD_COEFFS 5

// number of states
#define BIQUAD_STATES 5

const int integer_bits = 8;
const int fractional_bits = 24;

// 1.0 in fixed-point format = 2^fractional_bits
const int32_t fixed_one = 1 << fractional_bits;

// mask for error feedback
const int32_t error_mask = 0xffffffff >> integer_bits;

#if ( ! __ADSPBLACKFIN__ )
#define expected_true
#define expected_false
#endif

class EQ32
{
protected:
    int type;                               // filter type
    double f0;                              // center/corner frequency (Hz)
    double fs;                              // sampling frequency (Hz)
    double gain;                            // gain (dB)
    double Q;                               // quality factor
    bool   update_filter;                   // flag for coefficient update
    int32_t coefficients[BIQUAD_COEFFS];    // filter coefficients
    int32_t newCoefficients[BIQUAD_COEFFS]; // new filter coefficients
    int32_t states[EQ_CHANS][BIQUAD_STATES];// filter states

public:
    EQ32(void);
    EQ32(int type, double f0, double fs, double gain, double Q);
    ~EQ32(void);
    void setType(int type, int update_coeffs = 1);
    void setCenterFrequency(double f0, int update_coeffs = 1);
    void setSamplingFrequency(double fs, int update_coeffs = 1);
    void setGain(double gain, int update_coeffs = 1);
    void setQfactor(double Q, int update_coeffs = 1);
    void setCoefficients(double double_coeffs[]);
    void getNewCoefficients(double float_coefficients[]);
    void getCoefficients(double float_coefficients[]);
    void resetStates(void);
    void designEQ(void);

    inline void process(int32_t samples[])
    {
        // coefficients[5] = { b0, b1, b2, -a1, -a2 }
        // format : integer_bits.fractional_bits
        int64_t temp64;

        // copy new coefficients if available
        if (expected_false(update_filter))
            for (int i=0; i<BIQUAD_COEFFS; i++)
                coefficients[i] = newCoefficients[i];

        for (int i=0; i<EQ_CHANS; i++)
        {
            // biquad filter
            temp64 =  (int64_t) coefficients[0] * samples[i];
            temp64 += (int64_t) coefficients[1] * states[i][0];
            temp64 += (int64_t) coefficients[2] * states[i][1];
            temp64 += (int64_t) coefficients[3] * states[i][2];
            temp64 += (int64_t) coefficients[4] * states[i][3];
#if (ERROR_FEEDBACK)
            // error feedback
            temp64 += states[i][4];
#endif
            // update of non-recursive states
            states[i][1] = states[i][0];
            states[i][0] = samples[i];

            // quantization of output
            samples[i] = (int32_t) (temp64 >> fractional_bits);

            // update of recursive states
            states[i][3] = states[i][2];
            states[i][2] = samples[i];
#if (ERROR_FEEDBACK)
            // update of quantization error
            states[i][4] = (int32_t) (temp64 & error_mask);
#endif
        }
    }
};

}

#endif  // _EQ32

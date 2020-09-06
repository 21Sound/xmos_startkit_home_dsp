/*---------------------------------------------------------------------------*\
|   Implementation of a Fixed-Point Equalizer Class                           |
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

#include <math.h>
#include "eq32.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

EQ32::EQ32(void)
{
    type = PEAKING_EQ;
    f0 = 1000.;
    fs = 48000.;
    gain = 0.;
    Q = 0.7071;
    update_filter = false;
    coefficients[0] = fixed_one;
    coefficients[1] = 0;
    coefficients[2] = 0;
    coefficients[3] = 0;
    coefficients[4] = 0;
    resetStates();
}

EQ32::EQ32(int type, double f0, double fs, double gain, double Q)
{
    this->type = type;
    this->f0 = f0;
    this->fs = fs;
    this->gain = gain;
    this->Q = Q;
    update_filter = false;
    designEQ();
    resetStates();
}

EQ32::~EQ32(void)
{
}

void EQ32::setType(int type, int update_coeffs)
{
    this->type = type;

    if (update_coeffs)
        designEQ();
}

void EQ32::setCenterFrequency(double f0, int update_coeffs)
{
    if (f0 < 0)
        f0 = 0;

    if (f0 > fs/2)
        f0 = fs/2;

    this->f0 = f0;

    if (update_coeffs)
        designEQ();
}

void EQ32::setSamplingFrequency(double fs, int update_coeffs)
{
    this->fs = fs;

    if (update_coeffs)
        designEQ();
}

void EQ32::setGain(double gain, int update_coeffs)
{
    if (gain < -24)
        gain = -24;

    if (gain > +24)
        gain = +24;

    this->gain = gain;

    if (update_coeffs)
        designEQ();
}

void EQ32::setQfactor(double Q, int update_coeffs)
{
    if (Q < 0)
        Q = 0;

    if (Q > 30)
        Q = 30;

    this->Q = Q;

    if (update_coeffs)
        designEQ();
}

void EQ32::resetStates(void)
{
    for (int i=0; i<EQ_CHANS; i++)
        for (int j=0; j<BIQUAD_STATES; j++)
            states[i][j] = 0;
}

void EQ32::designEQ(void)
{
    double b0, b1, b2, a0, a1, a2;
    double omega, cs, sn, alpha, beta, A, S;
    double double_coeffs[BIQUAD_COEFFS];

    omega = 2.*M_PI*f0/fs;
    cs = cos(omega);
    sn = sin(omega);
    A = pow(10, gain / 40);
    if (Q < 0.1) Q = 0.1;
    if (Q < 1) S = Q; else S = 1;
    alpha = sn*sinh( 1./(2*Q) );
    beta = sqrt( (A*A + 1)/S - (A-1)*(A-1) );

    switch (type)
    {
    case GAIN_EQ:
        b0 = A*A;
        b1 = 0.0;
        b2 = 0.0;
        a0 = 1.0;
        a1 = 0.0;
        a2 = 0.0;
        break;

    case LOW_PASS_EQ:
        b0 = (1 - cs)/2;
        b1 =  1 - cs;
        b2 = (1 - cs)/2;
        a0 =  1 + alpha;
        a1 = -2*cs;
        a2 =  1 - alpha;
        break;

    case HIGH_PASS_EQ:
        b0 = (1 + cs)/2;
        b1 = -1 - cs;
        b2 = (1 + cs)/2;
        a0 =  1 + alpha;
        a1 = -2*cs;
        a2 =  1 - alpha;
        break;

    case BAND_PASS_EQ:
        b0 =  alpha;
        b1 =  0;
        b2 = -alpha;
        a0 =  1 + alpha;
        a1 = -2*cs;
        a2 =  1 - alpha;
        break;

    case NOTCH_EQ:
        b0 =  1;
        b1 = -2*cs;
        b2 =  1;
        a0 =  1 + alpha;
        a1 = -2*cs;
        a2 =  1 - alpha;
        break;

    case PEAKING_EQ:
        b0 =  1 + alpha*A;
        b1 = -2*cs;
        b2 =  1 - alpha*A;
        a0 =  1 + alpha/A;
        a1 = -2*cs;
        a2 =  1 - alpha/A;
        break;

    case LOW_SHELF_EQ:
        b0 =    A*( (A+1) - (A-1)*cs + beta*sn );
        b1 =  2*A*( (A-1) - (A+1)*cs );
        b2 =    A*( (A+1) - (A-1)*cs - beta*sn );
        a0 =        (A+1) + (A-1)*cs + beta*sn;
        a1 =   -2*( (A-1) + (A+1)*cs );
        a2 =        (A+1) + (A-1)*cs - beta*sn;
        break;

    case HIGH_SHELF_EQ:
        b0 =    A*( (A+1) + (A-1)*cs + beta*sn );
        b1 = -2*A*( (A-1) + (A+1)*cs );
        b2 =    A*( (A+1) + (A-1)*cs - beta*sn );
        a0 =        (A+1) - (A-1)*cs + beta*sn;
        a1 =    2*( (A-1) - (A+1)*cs );
        a2 =        (A+1) - (A-1)*cs - beta*sn;
    break;

    case LOW_SHELFQ_EQ:
        b0 =    A*( (A+1) - (A-1)*cs + 2*sqrt(A)*alpha );
        b1 =  2*A*( (A-1) - (A+1)*cs );
        b2 =    A*( (A+1) - (A-1)*cs - 2*sqrt(A)*alpha );
        a0 =        (A+1) + (A-1)*cs + 2*sqrt(A)*alpha;
        a1 =   -2*( (A-1) + (A+1)*cs );
        a2 =        (A+1) + (A-1)*cs - 2*sqrt(A)*alpha;
    break;

    case HIGH_SHELFQ_EQ:
        b0 =    A*( (A+1) + (A-1)*cs + 2*sqrt(A)*alpha );
        b1 = -2*A*( (A-1) + (A+1)*cs );
        b2 =    A*( (A+1) + (A-1)*cs - 2*sqrt(A)*alpha );
        a0 =        (A+1) - (A-1)*cs + 2*sqrt(A)*alpha;
        a1 =    2*( (A-1) - (A+1)*cs );
        a2 =        (A+1) - (A-1)*cs - 2*sqrt(A)*alpha;
    break;

    case ALLPASS_EQ:
        b0 =  1 - alpha;
        b1 = -2*cs;
        b2 =  1 + alpha;
        a0 =  1 + alpha;
        a1 = -2*cs;
        a2 =  1 - alpha;
        break;

    default:
        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a0 = 1.0;
        a1 = 0.0;
        a2 = 0.0;
        break;
    }

    double_coeffs[0] =  b0 / a0;
    double_coeffs[1] =  b1 / a0;
    double_coeffs[2] =  b2 / a0;
    double_coeffs[3] = -a1 / a0;
    double_coeffs[4] = -a2 / a0;

    setCoefficients(double_coeffs);
}

void EQ32::setCoefficients(double double_coeffs[])
{
    for (int i=0; i<BIQUAD_COEFFS; i++)
        newCoefficients[i] = int32_t(double_coeffs[i] * fixed_one);
    update_filter = true;
}

void EQ32::getNewCoefficients(double float_coefficients[])
{
    for (int i=0; i<BIQUAD_COEFFS; i++)
        float_coefficients[i] = newCoefficients[i] / double(fixed_one);
}

void EQ32::getCoefficients(double float_coefficients[])
{
    for (int i=0; i<BIQUAD_COEFFS; i++)
        float_coefficients[i] = coefficients[i] / double(fixed_one);
}

//--------------------- License ------------------------------------------------

// Copyright (c) 2015-16 Hagen Jaeger, Uwe Simmer

// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files 
// (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, 
// publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//------------------------------------------------------------------------------

/*------------------------------------------------------------------*\
Implmentation of a lookahead limiter, very fast.
Public domain. License: end of file.

Hagen Jaeger, 22.09.2014
Version 1.0.1 (debuged and tested, 22.09.2014).

Uwe Simmer, 10.12.2016
Version 1.1.0. (faster and more precise fixed-point division).
Version 1.1.1. (bugfix in comparison before division).
FixedPoint Adaption, Hagen Jaeger, 02.05.2020
\*------------------------------------------------------------------*/

#include <math.h>
#include <stdint.h>
#include "limiter32.h"

#define abs(x) ((x)<0 ? -(x) : (x))
#define max(x,y) ((x)>(y) ? (x) : (y))
#define min(x,y) ((x)<(y) ? (x) : (y))

Limiter32::Limiter32(int32_t channnels)
{
    double tAtt = 0.002, tHold = 0.01, tRel = 1.0;
    int32_t fs = 48000;
    bAtt = (int32_t)(1. / (tAtt * fs) * 0x7FFFFFFF);
    aAtt = 0x7FFFFFFF - bAtt;
    bRel = (int32_t)(1. / (tRel * fs) * 0x7FFFFFFF);
    aRel = 0x7FFFFFFF - bRel;
    nHoldSamps = (int32_t)(tHold * fs);
    nLookaheadSamps = (int32_t)(tAtt * fs);
    if (channnels > MAX_LIMITER_CHANS)
        channnels = MAX_LIMITER_CHANS;
    nChans = channnels;

    for (int i = 0; i < nChans; i++)
    {
        lookaheadMem[i] = new int32_t[nLookaheadSamps];
        for (int j = 0; j < nLookaheadSamps; j++)
            lookaheadMem[i][j] = 0;
    }

    thresholdLin = 0x40000000; // threshold -6 dBFS

    lookaheadCnt = 0;
    holdCnt = nHoldSamps;
    gain = 0x7FFFFFFF;
    relState = 0x7FFFFFFF;
}

Limiter32::Limiter32(double threshold, double tAtt, double tHold, double tRel,
                     int32_t channnels, int32_t fs)
{
    if (channnels > MAX_LIMITER_CHANS)
        channnels = MAX_LIMITER_CHANS;
    nChans = channnels;
    bAtt = (int32_t)(1. / (tAtt * fs) * 0x7FFFFFFF);
    aAtt = 0x7FFFFFFF - bAtt;
    bRel = (int32_t)(1. / (tRel * fs) * 0x7FFFFFFF);
    aRel = 0x7FFFFFFF - bRel;
    nHoldSamps = (int32_t)(tHold * fs);
    nLookaheadSamps = (int32_t)(tAtt * fs);

    for (int i = 0; i < nChans; i++)
    {
        lookaheadMem[i] = new int32_t[nLookaheadSamps];
        for (int j = 0; j < nLookaheadSamps; j++)
            lookaheadMem[i][j] = 0;
    }

    if (threshold >= 0)
        thresholdLin = 0x7FFFFFFF; // threshold 0 dBFS
    else
        thresholdLin = (int32_t)(pow(10., threshold / 20.) * 0x7FFFFFFF);

    lookaheadCnt = 0;
    holdCnt = nHoldSamps;
    gain = 0x7FFFFFFF;
    relState = 0x7FFFFFFF;
}

Limiter32::~Limiter32(void)
{
    for (int i = 0; i < nChans; i++)
    {
        delete[] lookaheadMem[i];
    }
}

void Limiter32::setThreshold(double threshold)
{
    if (threshold >= 0)
        thresholdLin = 0x7FFFFFFF;  // threshold 0 dBFS
    else
        thresholdLin = (int32_t)(pow(10., threshold / 20.) * 0x7FFFFFFF);
}

int32_t Limiter32::process(int32_t inSamps[])
{
    int32_t gainTmp, tmp32, maxVal;

    maxVal = 0;
    for (int i = 0; i < nChans; i++)
        maxVal = max(abs(inSamps[i]), maxVal);

    // gain = thresholdLin / maxVal
    maxVal = maxVal & 0xFFFF0000;
    if (maxVal > thresholdLin)
        gainTmp = (thresholdLin / (maxVal >> 16)) << 15;
    else
        gainTmp = 0x7FFFFFFF;

    tmp32 = (((int64_t)gainTmp * bRel) >> 31) + (((int64_t)relState * aRel) >> 31);

    if (gainTmp < tmp32)
    {
        relState = gainTmp;
        holdCnt = nHoldSamps;
    }
    else
    {
        if (holdCnt > 0)
            holdCnt--;
        else
            relState = tmp32;
    }

    gain = (((int64_t)relState * bAtt) >> 31) + (((int64_t)gain * aAtt) >> 31);

    for (int i = 0; i < nChans; i++)
    {
        tmp32 = ((int64_t)gain * lookaheadMem[i][lookaheadCnt]) >> 31;
        lookaheadMem[i][lookaheadCnt] = inSamps[i];
        inSamps[i] = max(min(tmp32, thresholdLin), -thresholdLin);
    }

    lookaheadCnt++;
    if (lookaheadCnt >= nLookaheadSamps)
        lookaheadCnt = 0;

    return gain;
}

//--------------------- License ------------------------------------------------

// Copyright (c) 2014-2016 Hagen Jaeger, Uwe Simmer

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

/*------------------------------------------------------------------*\
Implmentation of a lookahead limiter, very fast.
public domain

Hagen Jaeger, 22.09.2014
Version 1.0.1 (debuged and tested, 22.09.2014).

Uwe Simmer, 10.12.2016
Version 1.1.0. (faster and more precise fixed-point division).
Version 1.1.1. (bugfix in comparison before division).

FixedPoint Adaption, Hagen Jaeger, 02.05.2020
\*------------------------------------------------------------------*/

#ifndef LIMITER32_H // include guard
#define LIMITER32_H

extern "C" {

#ifndef MAX_LIMITER_CHANS
#define MAX_LIMITER_CHANS 2
#endif

#include <stdint.h>

class Limiter32
{
public:
    Limiter32(int32_t nChans);
    Limiter32(double threshold, double tAtt, double tHold, double tRel, int32_t nChans, int32_t fs);
    ~Limiter32(void);
    void setThreshold(double threshold);
    int32_t process(int32_t inSamps[]);

private:
    int32_t nChans;
    int32_t aAtt;
    int32_t bAtt;
    int32_t aRel;
    int32_t bRel;
    int32_t nHoldSamps;
    int32_t nLookaheadSamps;
    int32_t thresholdLin;
    int32_t *lookaheadMem[MAX_LIMITER_CHANS];
    int32_t lookaheadCnt;
    int32_t holdCnt;
    int32_t gain;
    int32_t relState;
};

}

#endif // end of include guard

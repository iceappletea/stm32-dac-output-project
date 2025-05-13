#ifndef __ALL_H
#define __ALL_H

#include "stm32f10x.h"

typedef enum
{
    MODE_FREQUENCY,
    MODE_PHASE
}ControlMode;

typedef struct
{
    float frequency;
    float phase;

    uint32_t freq_int_part;
    uint32_t freq_frac_part;
    uint16_t phase_int_part;
    uint16_t phase_frac_part;
}FreqPhaseInfo;

extern volatile ControlMode current_mode;
extern uint16_t wave_offset;
extern uint16_t TIM_Period;

#endif

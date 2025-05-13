#ifndef __DAC_H
#define __DAC_H

#include "sys.h"
#include "ALL.h"
#include "Key.h"
#include "Serial.h"
#include "Delay.h"
#include "lcd.h"

#define Point_Num 256
#define DAC_DHR12RD_ADDRESS (DAC_BASE + 0x20)

void DAC_Config(void);
void DAC_TIM_Config(void);
void DAC_DMA_Config(void);
void DAC_UpdateWaveform(void);
void TIM2_UpdatePeriod(void);
void DAC_Mode_Init(void);
void CalcFreqPhase(FreqPhaseInfo *info);
void ST7789_DrawWaveMark(void);
void ST7789_DrawLegend(int16_t dash_x_right, int16_t dash_y_top);

#endif

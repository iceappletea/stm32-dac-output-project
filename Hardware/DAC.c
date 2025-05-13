#include "DAC.h"

const uint16_t Sine12bit[Point_Num] = 
{
    2098, 2148, 2199, 2249, 2299, 2349, 2398, 2448, 2497, 2546,
    2594, 2643, 2690, 2738, 2785, 2832, 2878, 2924, 2969, 3013,
    3057, 3101, 3144, 3186, 3227, 3268, 3308, 3347, 3386, 3423,
    3460, 3496, 3531, 3565, 3599, 3631, 3663, 3693, 3722, 3751,
    3778, 3805, 3830, 3854, 3877, 3899, 3920, 3940, 3959, 3976,
    3993, 4008, 4022, 4035, 4046, 4057, 4066, 4074, 4081, 4086,
    4090, 4094, 4095, 4095, 4095, 4094, 4090, 4086, 4081, 4074,
    4066, 4057, 4046, 4035, 4022, 4008, 3993, 3976, 3959, 3940,
    3920, 3899, 3877, 3854, 3830, 3805, 3778, 3751, 3722, 3693,
    3663, 3631, 3599, 3565, 3531, 3496, 3460, 3423, 3386, 3347,
    3308, 3268, 3227, 3186, 3144, 3101, 3057, 3013, 2969, 2924,
    2878, 2832, 2785, 2738, 2690, 2643, 2594, 2546, 2497, 2448,
    2398, 2349, 2299, 2249, 2199, 2148, 2098, 2048, 1998, 1948,
    1897, 1847, 1797, 1747, 1698, 1648, 1599, 1550, 1502, 1453,
    1406, 1358, 1311, 1264, 1218, 1172, 1127, 1083, 1039, 995,
    952, 910, 869, 828, 788, 749, 710, 673, 636, 600, 565, 531,
    497, 465, 433, 403, 374, 345, 318, 291, 266, 242, 219, 197,
    176, 156, 137, 120, 103, 88, 74, 61, 50, 39, 30, 22, 15, 10,
    6, 2, 1, 0, 1, 2, 6, 10, 15, 22, 30, 39, 50, 61, 74, 88, 103,
    120, 137, 156, 176, 197, 219, 242, 266, 291, 318, 345, 374,
    403, 433, 465, 497, 531, 565, 600, 636, 673, 710, 749, 788,
    828, 869, 910, 952, 995, 1039, 1083, 1127, 1172, 1218, 1264,
    1311, 1358, 1406, 1453, 1502, 1550, 1599, 1648, 1698, 1747,
    1797, 1847, 1897, 1948, 1998, 2048
};

uint32_t DualSine12bit[Point_Num];

static void DAC_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;

    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);

    DAC_DMACmd(DAC_Channel_2, ENABLE);
}

static void DAC_TIM_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = TIM_Period - 1;	 // freqency=72M/(Point_Num*ARR)
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
}

static void DAC_DMA_Config(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;

    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    DMA_InitStructure.DMA_BufferSize = Point_Num;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

void DAC_UpdateWaveform(void)
{
    for (uint16_t i = 0; i < Point_Num; i++)
    {
        uint16_t idx = (i + wave_offset) % Point_Num;
        DualSine12bit[i] = (Sine12bit[idx] << 16) | Sine12bit[i];
    }
}

void TIM2_UpdatePeriod(void)
{
    TIM2->ARR = TIM_Period - 1;
    TIM_GenerateEvent(TIM2, TIM_EventSource_Update);
}

void DAC_Mode_Init(void)
{
    DAC_Config();
    DAC_TIM_Config();
    DAC_DMA_Config();
    DAC_UpdateWaveform();
}

void CalcFreqPhase(FreqPhaseInfo *info)
{
    info->frequency = 72000000.0f / (Point_Num * TIM_Period);
    info->phase = (wave_offset * 360.0f) / Point_Num;

    info->freq_int_part = (uint32_t)(info->frequency);
    info->freq_frac_part = ((uint32_t)(info->frequency * 100)) % 100;

    info->phase_int_part = (uint16_t)(info->phase);
    info->phase_frac_part = ((uint32_t)(info->phase * 10)) % 10;
}

void ST7789_DrawLegend(int16_t dash_x_right, int16_t dash_y_top)
{
    int16_t legend_x = dash_x_right - 70;
    int16_t legend_y = dash_y_top + 10;

    LCD_Fill(legend_x, legend_y, legend_x + 5, legend_y + 5, BLACK);
    LCD_ShowString(legend_x + 10, legend_y - 8, (uint8_t *)"origin", BLACK);

    legend_y += 20;
    LCD_Fill(legend_x, legend_y, legend_x + 5, legend_y + 5, RED);
    LCD_ShowString(legend_x + 10, legend_y - 8, (uint8_t *)"new", RED);
}

void ST7789_DrawWaveMark(void)
{
    static uint16_t prev_TIM_Period = 0;
    static uint16_t prev_wave_offset = 0;
    static ControlMode prev_mode = MODE_FREQUENCY;

    ControlMode mode = Key_GetMode();

    if (prev_TIM_Period == TIM_Period && prev_wave_offset == wave_offset && prev_mode == mode)
    {
        return;
    }

    prev_TIM_Period = TIM_Period;
    prev_wave_offset = wave_offset;
    prev_mode = mode;

    LCD_CS_Clr();
    LCD_Clear(WHITE);

    FreqPhaseInfo info;
    CalcFreqPhase(&info);

    uint8_t str[40];

    sprintf((char *)str, "Frequency: %lu.%02luHz", info.freq_int_part, info.freq_frac_part);
    LCD_ShowString(10, 5, str, GRAYBLUE);

    sprintf((char *)str, "Phase: %d.%01ddegree", info.phase_int_part, info.phase_frac_part);
    LCD_ShowString(10, 25, str, GRAYBLUE);

    if (mode == MODE_FREQUENCY)
    {
        LCD_Fill(2, 10, 5, 13, RED);
    }
    else if (mode == MODE_PHASE)
    {
        LCD_Fill(2, 30, 5, 33, RED);
    }

    const int16_t dash_x_left = 5;
    const int16_t dash_x_right = LCD_W - 5;
    const int16_t dash_y_top = 45;
    const int16_t dash_y_bottom = LCD_H - 5;

    for (int16_t y = dash_y_top; y <= dash_y_bottom; y += 4)
    {
        LCD_DrawPoint(dash_x_left, y, BLACK);

        if (y + 1 <= dash_y_bottom)
        {
            LCD_DrawPoint(dash_x_left, y + 1, BLACK);
        }
    }

    for (int16_t y = dash_y_top; y <= dash_y_bottom; y += 4)
    {
        LCD_DrawPoint(dash_x_right, y, BLACK);

        if (y + 1 <= dash_y_bottom)
        {
            LCD_DrawPoint(dash_x_right, y + 1, BLACK);
        }
    }

    for (int16_t x = dash_x_left; x <= dash_x_right; x += 4)
    {
        LCD_DrawPoint(x, dash_y_top, BLACK);

        if (x + 1 <= dash_x_right)
        {
            LCD_DrawPoint(x + 1, dash_y_top, BLACK);
        }
    }

    for (int16_t x = dash_x_left; x <= dash_x_right; x += 4)
    {
        LCD_DrawPoint(x, dash_y_bottom, BLACK);

        if (x + 1 <= dash_x_right)
        {
            LCD_DrawPoint(x + 1, dash_y_bottom, BLACK);
        }
    }

    uint16_t displayed_points = (Point_Num <= 230) ? Point_Num : 230;
    const int16_t left_margin = 5;
    const int16_t wave_top = 50;
    const int16_t wave_bottom = 149;
    const int16_t wave_height = wave_bottom - wave_top;
    const int16_t offset = 2048;
    const int16_t fullrange = 4096;

    uint16_t wave_offset_static = 0;
    int16_t x_s, y_s;
    int16_t prev_x_s, prev_y_s;

    uint16_t real_index_s = (0 + wave_offset_static) % Point_Num;
    prev_x_s = left_margin;

    int32_t val_s = (int32_t)Sine12bit[real_index_s] - offset;
    int32_t scaled_s = (val_s * wave_height) / fullrange;
    int32_t tmpy_s = wave_bottom - scaled_s;

    if (tmpy_s < 0)
    {
        tmpy_s = 0;
    }
    if (tmpy_s >= LCD_H)
    {
        tmpy_s = LCD_H - 1;
    }

    prev_y_s = (int16_t)tmpy_s;

    for (uint16_t i = 1; i < displayed_points; i++)
    {
        real_index_s = (i + wave_offset_static) % Point_Num;
        x_s = i + left_margin;

        if (x_s >= LCD_W)
        {
            break;
        }

        val_s = (int32_t)Sine12bit[real_index_s] - offset;
        scaled_s = (val_s * wave_height) / fullrange;
        tmpy_s = wave_bottom - scaled_s;

        if (tmpy_s < 0)
        {
            tmpy_s = 0;
        }
        if (tmpy_s >= LCD_H)
        {
            tmpy_s = LCD_H - 1;
        }

        y_s = (int16_t)tmpy_s;

        LCD_DrawLine(prev_x_s, prev_y_s, x_s, y_s, BLACK);

        prev_x_s = x_s;
        prev_y_s = y_s;
    }

    int16_t x_d, y_d;
    int16_t prev_x_d, prev_y_d;

    uint16_t real_index_d = (0 + wave_offset) % Point_Num;
    prev_x_d = left_margin;

    int32_t val_d = (int32_t)Sine12bit[real_index_d] - offset;
    int32_t scaled_d = (val_d * wave_height) / fullrange;
    int32_t tmpy_d = wave_bottom - scaled_d;

    if (tmpy_d < 0)
    {
        tmpy_d = 0;
    }
    if (tmpy_d >= LCD_H)
    {
        tmpy_d = LCD_H - 1;
    }

    prev_y_d = (int16_t)tmpy_d;

    for (uint16_t i = 1; i < displayed_points; i++)
    {
        real_index_d = (i + wave_offset) % Point_Num;
        x_d = i + left_margin;

        if (x_d >= LCD_W)
        {
            break;
        }

        val_d = (int32_t)Sine12bit[real_index_d] - offset;
        scaled_d = (val_d * wave_height) / fullrange;
        tmpy_d = wave_bottom - scaled_d;

        if (tmpy_d < 0)
        {
            tmpy_d = 0;
        }
        if (tmpy_d >= LCD_H)
        {
            tmpy_d = LCD_H - 1;
        }

        y_d = (int16_t)tmpy_d;

        LCD_DrawLine(prev_x_d, prev_y_d, x_d, y_d, RED);

        prev_x_d = x_d;
        prev_y_d = y_d;
    }

    ST7789_DrawLegend(dash_x_right, dash_y_top);
}

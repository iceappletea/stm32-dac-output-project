#include "HC05.h"

uint8_t RxSTA = 1;
char RxData[100] = "None";

void HC05_Init(void)
{
    Serial3_Init();
}

void HC05_GetData(char *Buf)
{
    uint32_t a = 0;
    RxSTA = 1;
    
    while (1)
    {
        if (Serial3_GetRxFlag())
        {
            char c = Serial3_GetRxData();
            Buf[a++] = c;

            if (c == '\n' || a >= 99)
            {
                break;
            }
        }
    }

    Buf[a] = '\0';
    RxSTA = 0;
}

void HC05_SendString(char *Buf)
{
    Serial3_Printf(Buf);
}

void HC05_proc(void)
{
    if (!Serial3_GetRxFlag())
    {
        return;
    }
    else
    {
        HC05_GetData(RxData);
    }

    if (RxSTA == 0)
    {
        Serial3_Printf("Raw RxData: [%s]\n", RxData);

        int len = strlen(RxData);
        while (len > 0 && (RxData[len - 1] == '\n' || RxData[len - 1] == '\r'))
        {
            RxData[len - 1] = '\0';
            len--;
        }

        if (strstr(RxData, "mode freq") != NULL)
        {
            current_mode = MODE_FREQUENCY;
            HC05_SendString("MODE: FREQUENCY\n");
        }
        else if (strstr(RxData, "mode phase") != NULL)
        {
            current_mode = MODE_PHASE;
            HC05_SendString("MODE: PHASE\n");
        }
        else if (current_mode == MODE_FREQUENCY)
        {
            if (strstr(RxData, "arr+1") != NULL)
            {
                TIM_Period += 1;
            }
            else if (strstr(RxData, "arr-1") != NULL)
            {
                TIM_Period -= 1;
            }
            else if (strstr(RxData, "arr+5") != NULL)
            {
                TIM_Period += 5;
            }
            else if (strstr(RxData, "arr-5") != NULL)
            {
                TIM_Period -= 5;
            }
            else if (strstr(RxData, "arr+7") != NULL)
            {
                TIM_Period += 7;
            }
            else if (strstr(RxData, "arr-7") != NULL)
            {
                TIM_Period -= 7;
            }

            if (TIM_Period < 450)
            {
                TIM_Period = 450;
            }

            if (TIM_Period > 1450)
            {
                TIM_Period = 1450;
            }

            TIM2_UpdatePeriod();
        }
        else if (current_mode == MODE_PHASE)
        {
            if (strstr(RxData, "p+1") != NULL)
            {
                wave_offset = (wave_offset + 1) % Point_Num;
            }
            else if (strstr(RxData, "p-1") != NULL)
            {
                wave_offset = (wave_offset + Point_Num - 1) % Point_Num;
            }
            else if (strstr(RxData, "p+5") != NULL)
            {
                wave_offset = (wave_offset + 5) % Point_Num;
            }
            else if (strstr(RxData, "p-5") != NULL)
            {
                wave_offset = (wave_offset + Point_Num - 5) % Point_Num;
            }
            else if (strstr(RxData, "p+20") != NULL)
            {
                wave_offset = (wave_offset + 20) % Point_Num;
            }
            else if (strstr(RxData, "p-20") != NULL)
            {
                wave_offset = (wave_offset + Point_Num - 20) % Point_Num;
            }

            DAC_UpdateWaveform();
        }

        FreqPhaseInfo info;
        CalcFreqPhase(&info);

        if (current_mode == MODE_FREQUENCY)
        {
            Serial3_Printf("Freq: %lu.%02lu Hz\r\n", info.freq_int_part, info.freq_frac_part);
        }
        else if (current_mode == MODE_PHASE)
        {
            Serial3_Printf("Phase: %d.%01d degree\r\n", info.phase_int_part, info.phase_frac_part);
        }

        HC05_SendString("OK\n");
        Serial3_Printf("MODE: %s\n", current_mode == MODE_FREQUENCY ? "FREQUENCY" : "PHASE");

        memset(RxData, 0, sizeof(RxData));
        RxSTA = 1;
    }
}

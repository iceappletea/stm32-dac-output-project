#include "Encoder.h"

void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == 0)
        {
            if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == 1)
            {
                if (current_mode == MODE_PHASE)
                {
                    wave_offset++;
                }
                else if (current_mode == MODE_FREQUENCY)
                {
                    TIM_Period++;
                }
            }
            else
            {
                if (current_mode == MODE_PHASE)
                {
                    wave_offset--;
                }
                else if (current_mode == MODE_FREQUENCY)
                {
                    TIM_Period--;
                }
            }

            wave_offset = (wave_offset + Point_Num) % Point_Num;

            if (TIM_Period < 450)
            {
                TIM_Period = 450;
            }

            if (TIM_Period > 1450)
            {
                TIM_Period = 1450;
            }

            TIM2_UpdatePeriod();

            if (current_mode == MODE_PHASE)
            {
                DAC_UpdateWaveform();
            }
        }

        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

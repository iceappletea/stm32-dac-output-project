#include "Key.h"

uint8_t Key_State = 1;

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

uint8_t Key_GetState(void)
{
    return (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 0) ? 1 : 0;
}

void Key_Loop(void)
{
    static uint8_t last_state = 0;
    uint8_t current_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);

    if (last_state == 1 && current_state == 0)
    {
        Delay_ms(10);

        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 0)
        {
            if (current_mode == MODE_FREQUENCY)
            {
                current_mode = MODE_PHASE;
            }
            else
            {
                current_mode = MODE_FREQUENCY;
            }
        }
    }

    last_state = current_state;
}

ControlMode Key_GetMode(void)
{
    return current_mode;
}

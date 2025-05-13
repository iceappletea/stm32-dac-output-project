#include "stm32f10x.h"
#include "Delay.h"
#include "Serial.h"
#include "HC05.h"
#include "sys.h"
#include "lcd.h"
#include "DAC.h"
#include "Encoder.h"
#include "ALL.h"
#include "Key.h"

volatile ControlMode current_mode = MODE_FREQUENCY;
uint16_t TIM_Period = 703;
uint16_t wave_offset = 0;

int main(void)
{
    NVIC_Configuration();

    Lcd_Init();
    LCD_Clear(WHITE);
    BACK_COLOR = WHITE;

    Serial_Init();
    Key_Init();
    Encoder_Init();
    HC05_Init();
    DAC_Mode_Init();

    while (1)
    {
        Key_Loop();
        HC05_proc();
        ST7789_DrawWaveMark();
        Delay_ms(1000);
    }
}

#ifndef __KEY_H
#define __KEY_H

#include "sys.h"
#include "Delay.h"
#include "ALL.h"

void Key_Init(void);
uint8_t Key_GetState(void);
void Key_Loop(void);
ControlMode Key_GetMode(void);

#endif

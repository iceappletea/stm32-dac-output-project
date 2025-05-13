#ifndef __HC05_H
#define __HC05_H

#include "Serial3.h"
#include "DAC.h"
#include "ALL.h"
#include "Key.h"
#include <string.h>
#include <stdlib.h>

void HC05_Init(void);
void HC05_proc(void);
void HC05_SendString(char *Buf);

#endif

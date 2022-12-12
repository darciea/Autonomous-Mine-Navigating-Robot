#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <xc.h>

#define _XTAL_FREQ 64000000

unsigned int TimerFlag;

//function prototypes
void Interrupts_init(void);
void __interrupt(low_priority) LowISR();

#endif

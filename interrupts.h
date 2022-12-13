#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <xc.h>

#define _XTAL_FREQ 64000000

unsigned int TimerFlag;

//function prototypes
void Interrupts_init(void);
void __interrupt(low_priority) LowISR();

unsigned int response_in_progress=0;
unsigned int card_detected=0;

#endif

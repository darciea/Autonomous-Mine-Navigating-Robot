#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <xc.h>

#define _XTAL_FREQ 64000000

unsigned int TimerFlag; //this is used to communicate between timer interrupt and main while loop.
//Timer interrupt turns it on (every 0.1s), main sees this and increments timer count and turns it off again.

//function prototypes
void Interrupts_init(void);
void __interrupt(low_priority) LowISR();

#endif

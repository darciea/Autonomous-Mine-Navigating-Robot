#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <xc.h>

#define _XTAL_FREQ 64000000

//function prototypes
void Interrupts_init(void);
void __interrupt(high_priority) HighISR();
void enable_color_interrupt(void);
void set_interrupt_threshold(unsigned int AILT, unsigned int AIHT, unsigned int persistence);
void clear_interrupt_flag(void);

#endif

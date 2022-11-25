#ifndef _LEDsOn_H
#define _LEDsOn_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

/********************************************//**
 *  Buggy Lights
 ***********************************************/

#define BRAKE LATDbits.LATD4
#define MBEAM LATDbits.LATD3
#define RIGHT LATHbits.LATH0
#define HLAMPS LATHbits.LATH1
#define LEFT LATFbits.LATF0

/********************************************//**
 *  Tri-colour LEDs
 ***********************************************/

#define RED LATGbits.LATG1
#define BLUE LATFbits.LATF7
#define GREEN LATAbits.LATA4

void LEDSon_init(void);

#endif    
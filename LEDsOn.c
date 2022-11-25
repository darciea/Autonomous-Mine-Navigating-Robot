// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "LEDsOn.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  
/*
 * 
 */
void LEDSon_init(void) {

    //buggy lights set to output
    TRISHbits.TRISH1 = 0; //headlamps
    TRISHbits.TRISH0 = 0; //right
    TRISDbits.TRISD4 = 0; //brake lights
    TRISDbits.TRISD3 = 0; //main beam
    TRISFbits.TRISF0 = 0; //left
    
    //colour click tri-color LED lights set to output
    TRISGbits.TRISG1 = 0;
    TRISFbits.TRISF7 = 0;
    TRISAbits.TRISA4 = 0;
    
    BLUE = 0;
    RED = 0;
    GREEN = 0;
    
    BRAKE = 0;
    HLAMPS = 0;
    LEFT = 0;
    RIGHT = 0;
    MBEAM = 0;
    
}


// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "color.h"
#include "i2c.h"
#include "LEDsOn.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void) {
    
    /********************************************//**
    *  Initialising all components
    ***********************************************/
    LEDSon_init(); //starts with all lights off
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM(); 
    
    
    
    /********************************************//**
    *  Initialising DC motor variables
    ***********************************************/
    
    unsigned char PWMcycle = T2PR;
    
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures 

    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.brakemode=1;						// brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=PWMcycle; 			//store PWMperiod for motor (value of T2PR in this case)

    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.brakemode=1;						// brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP3 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP4 duty high byte
    motorR.PWMperiod=PWMcycle; 			//store PWMperiod for motor (value of T2PR in this case)
    
    setMotorPWM(&motorL);
    setMotorPWM(&motorR);
    
    
    
    
    unsigned int onoff = 0;
    LATDbits.LATD7=1;   //set initial output state of RD7 LED
    TRISDbits.TRISD7=0; //set TRIS value for D7 pin (output)
    while (1) {
        onoff = color_read_Blue();
        if (onoff != 0) {LATDbits.LATD7 = 0;}
        
        
        
        
    }
}

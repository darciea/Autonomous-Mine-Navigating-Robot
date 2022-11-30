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
#include "colour_identify.h"


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
    
    /********************************************//**
    *  Setting up arrays and variables for collecting data
    ***********************************************/
    colour card  = BLACK;
    unsigned char expected_values[8][3];
    unsigned char normalised_values[8][3];
    unsigned char master_closeness[8];
    
    unsigned char red_read = 0;
    unsigned char green_read = 0;
    unsigned char blue_read = 0;
    
    
    
    /********************************************//**
    *  Ideal main function code
    ***********************************************/
    
    /*Calibration sequence
    1. Press button (within for statement(8 iterations) require button push before incrementing)
    2. Read card using collect avg readings function (decide if want to use new variables for these ones)
    3. Store those values in first index of each row of array (assign which colour that index will be)
    4. Press button to increment i and repeat for all 8 colours
    */
    
    /*
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    for(unsigned int i = 0; i<=7; i++){
        collect_avg_readings(&red_read, &green_read, &blue_read);
        expected_values[i][0] = red_read;
        expected_values[i][1] = green_read;
        expected_values[i][2] = blue_read;
        while(PORTFbits.RF2){
            
        }
    }
    */
    /*
    card = 1; //flag to show that a card has been seen
    stop(&motorL, &motorR);
    collect_avg_readings(&red_read, &green_read, &blue_read);
    normalise_readings(&red_read, &green_read, &blue_read, &expected_values, &normalised_values);
    make_master_closeness(&normalised_values,&master_closeness);
    
    card = determine_card();
    */
    
    card = GREEN;
    
    
    
   /********************************************//**
    *  Trying code
    ***********************************************/
    LATDbits.LATD7=1;   //set initial output state of RD7 LED
    TRISDbits.TRISD7=0; //set TRIS value for D7 pin (output)
    while (1) {
        
        BRAKE = 1;
        respond_to_card(card, &motorL, &motorR);
        reverseOneSquare(&motorL, &motorR);
        //card = PINK;
        //respond_to_card(card, &motorL, &motorR);
        __delay_ms(3000);
        LEFT = 1;
        
        
    }
}

// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "dc_motor.h"
#include "color.h"
#include "i2c.h"
#include "LEDsOn.h"
#include "colour_identify.h"
#include "serial.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void) {
    
    /********************************************//**
    *  Initialising all components
    ***********************************************/
    LEDSon_init(); //starts with all lights off
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM();
    initUSART4();
    
    
    
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
    colour card  = BLUE;
    unsigned int expected_values[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    unsigned int normalised_values[3][3];
    unsigned int master_closeness[3];//= {80, 990/*3467*/, 8000/*3533*/};// = {17, 2, 12, 12, 10, 11, 12, 14, 16};
    unsigned int red_read = 0;
    unsigned int green_read = 0;
    unsigned int blue_read = 0;
    
    
    
    /********************************************//**
    *  Calibration sequence
        1. Press button (within for statement(8 iterations) require button push before incrementing)
        2. Read card using collect avg readings function (decide if want to use new variables for these ones)
        3. Store those values in first index of each row of array (assign which colour that index will be)
        4. Press button to increment i and repeat for all 8 colours                                           * 
    ***********************************************/
    /*
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    for(colour i = RED; i<=BLUE; i++){
        while(PORTFbits.RF2){
            BRAKE = 1;
        }
        collect_avg_readings(&red_read, &green_read, &blue_read);
        expected_values[i][RED] = red_read;
        expected_values[i][GREEN] = green_read;
        expected_values[i][BLUE] = blue_read;
        BRAKE = 0;
        __delay_ms(1000);  
    }
    */
    
    /********************************************//**
    *  Ideal main function code
    ***********************************************/ 
    /*  
    card = 1; //flag to show that a card has been seen
    stop(&motorL, &motorR);
    collect_avg_readings(&red_read, &green_read, &blue_read);
    normalise_readings(red_read, green_read, blue_read, expected_values, normalised_values);
    make_master_closeness(normalised_values,master_closeness);
    card = determine_card(master_closeness);
    respond_to_card(card);
    */
    
    
    
   /********************************************//**
    *  Trying code
    ***********************************************/
    char buf[20];
    
    
    while (1) {
        
        
        //__delay_ms(1000);
        collect_avg_readings(&red_read, &green_read, &blue_read);
        
        sprintf(buf, "\n AVG: R %d, G %d, B %d \n", red_read, green_read, blue_read);
        sendStringSerial4(buf);
        __delay_ms(1000);
        sprintf(buf, "\n Expected values: %u \n", expected_values[1][1]);
        sendStringSerial4(buf);
        __delay_ms(1000);
        LEFT = 0;
        
        
        normalise_readings(buf, red_read, green_read, blue_read, expected_values, normalised_values);
        //sprintf(buf, "\n Normalised: R %d, G %d, B %d \n", normalised_values[0][0], normalised_values[0][1], normalised_values[0][2]);
        //sendStringSerial4(buf);
        __delay_ms(1000);
        
        
        make_master_closeness(buf, normalised_values,master_closeness);
        card = determine_card(master_closeness);
        //sprintf(buf, "CARD %d \n", card);
        //sendStringSerial4(buf);
        
       
        LEFT = 1;
        BRAKE = 1;
        __delay_ms(3000);
        __delay_ms(3000);
                
        
        
    }
}

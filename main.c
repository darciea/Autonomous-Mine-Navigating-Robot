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

unsigned int card_seen; //the flag that shows there is a card in front of the buggy (probs will be global variable)

void main(void) {
    
    /********************************************//**
    *  Initialising all components
    ***********************************************/
    LEDSon_init(); //starts with all lights off
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM();
    initUSART4();
    Interrupts_init();

    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin

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
   
    colour card; // variable that holds the colour of the card that has been seen
    char buf[150];

    unsigned int expected_values[3][9];
    unsigned int red_read = 0;
    unsigned int green_read = 0;
    unsigned int blue_read = 0;
    unsigned int clear_read = 0;
    
    unsigned int TimerCount = 0;
    unsigned int CardCount = 0; //count of the cards seen on the route of the buggy
    
    unsigned int ReturnHomeArray[2][30];
 
    /********************************************//**
    *  Calibration sequence
        1. Press button (within for statement(8 iterations) require button push before incrementing)
        2. Read card using collect avg readings function (decide if want to use new variables for these ones)
        3. Store those values in first index of each row of array (assign which colour that index will be)
        4. Press button to increment i and repeat for all 8 colours                                           * 
    ***********************************************/
    
    for(colour i = RED; i<= BLACK; i++){
        while(PORTFbits.RF2){
            BRAKE = 1;
        }
        BRAKE = 0;
        collect_avg_readings(&red_read, &green_read, &blue_read);
        expected_values[RED][i] = red_read;
        expected_values[GREEN][i] = green_read;
        expected_values[BLUE][i] = blue_read; 
    }

    /********************************************//**
    *  Ideal main function code (put into while(1))
    ***********************************************/
    
    /*
    
     fullSpeedAhead(&motorL, &motorR);
      
    while (card_detected = 1){ //flag to show that a card has been seen //CONCERNED THAT IT WONT STOP IF THE FUNCTION IS SOMEWHERE ELSE
        cards_seen++;
        stop(&motorL, &motorR);
        collect_avg_readings(&red_read, &green_read, &blue_read);
        normalise_readings(red_read, green_read, blue_read, expected_values, normalised_values);
        make_master_closeness(normalised_values,master_closeness);
        card = determine_card(master_closeness);
        respond_to_card(card);
    
    //SHOULD PUT IN FLAG SO THAT BUGGY DOESN'T GET INTERRUPTED/RESPOND TO CARDS ON ITS WAY HOME
    
    
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
    motor_response(card);
    */
        
   /********************************************//**
    *  Trying code
    ***********************************************/
    
    
    
    while (1) {
        
        //currently waits for button press before doing the reading for each card - will be replaced once the interrupt is implemented
        while(PORTFbits.RF2){
            BRAKE = 1;
            LEFT = 1;
        }
        LEFT = 0;
           
        card_response(buf, &red_read, &green_read, &blue_read, expected_values, &motorL, &motorR, ReturnHomeArray[2][30]);
        
    }
}

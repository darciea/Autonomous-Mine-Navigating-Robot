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
#include "interrupts.h"


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
    Interrupts_init();
    
    
    
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
    unsigned int expected_values[3][3] = {{13000, 2600, 1800},{8400, 6500, 5000},{4400, 1800, 2800}};
    unsigned int normalised_values[3][3];
    unsigned int master_closeness[3];//= {80, 990/*3467*/, 8000/*3533*/};// = {17, 2, 12, 12, 10, 11, 12, 14, 16};
    
    unsigned int red_read = 0;
    unsigned int green_read = 0;
    unsigned int blue_read = 0;
    unsigned int clear_read = 0;
    
    unsigned int TimerCount = 0;
    unsigned int CardCount = 0;
    unsigned int card_detected = 0;
    
    unsigned int ReturnHomeArray[2][30] = {0};
    
    
    
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
    *  Trying code
    ***********************************************/
    
    
    
    LATHbits.LATH3=0;   //set initial output state of RH3 LED
    TRISHbits.TRISH3=0; //set TRIS value for H3 pin (output)
    
    char buf[20];
        
    LATDbits.LATD7=0;   //set initial output state of RD7 LED
    TRISDbits.TRISD7=0; //set TRIS value for D7 pin (output)
    
    //fullSpeedAhead(DC_motor *mL, DC_motor *mR); //begin moving
    while (1) {
        /*
        if (TimerFlag == 1){ //incrementing the timer counter every ms if the timer overflows. Note this relies on the while loop running more than once every ms - may need to experiment
            TimerCount += 1;
            if (TimerCount == 10){LATHbits.LATH3=!LATHbits.LATH3; TimerCount = 0;}
            TimerFlag = 0;
        }
        */
        
        /*
        if (card_detected == 1){ //defined as global variable in main.c
            response_in_progress = 1; //let the interrupt know not to keep triggering while the buggy is responding to the card. Defined as global variable
            ReturnHomeArray[0][CardCount] = TimerCount; //put current timer value in 10ths of a second into ReturnHomeArray to be used on the way back to determine how far forward the buggy moves between each card
            stop(DC_motor *mL, DC_motor *mR);
            //insert function here that reads colours, averages values, normalises them, determines master closeness, uses that to find which card is there, and respond to it
            ReturnHomeArray[1][CardCount] = card_seen; //log in the array which card has been detected
            CardCount += 1; //indicate that next time a card is detected the timer value should be stored in the next column along
            response_in_progress = 0; //let the buggy know that any future interrupt triggers will be the next card
            card_detected = 0; //at this point the buggy should not be facing the card anymore so it shouldn't have the interrupt triggered again
            TimerCount = 0; //reset the timer once the buggy is about to move again
            fullSpeedAhead(DC_motor *mL, DC_motor *mR); //begin moving
            
        }
        
        */
        
        red_read = color_read_Red();
        blue_read = color_read_Blue();
        green_read = color_read_Green();
        clear_read = color_read_Clear();

        
        sprintf(buf, "Raw %d, %d, %d, %d \n", red_read, green_read, blue_read, clear_read);
        sendStringSerial4(buf);
        __delay_ms(500);
         LATHbits.LATH3=!LATHbits.LATH3;
        
/*
        BRAKE = 1;
        respond_to_card(card, &motorL, &motorR);
        //reverseOneSquare(&motorL, &motorR);
        //card = PINK;

        
        //__delay_ms(1000);
        
        
        collect_avg_readings(&red_read, &green_read, &blue_read);
        normalise_readings(&red_read,&green_read, &blue_read, &expected_values, &normalised_values);
        //make_master_closeness(&normalised_values,&master_closeness);
      

        //respond_to_card(card, &motorL, &motorR);
        //card = PINK;

        LEFT = 1;

                */
        
        
    }
}

// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

// include all relevant header files
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
    
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin

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

    //temporary variables interacted with by functions involved with colour readings
    unsigned int red_read = 0;
    unsigned int green_read = 0;
    unsigned int blue_read = 0;
    unsigned int clear_read = 0;
    
    unsigned int TimerCount = 0; //keeps track of how long the buggy moves for between cards
    unsigned int CardCount = 0; //count of the cards seen on the route of the buggy
    unsigned int clear_read_threshold = 0; //the threshold value which triggers a card being detected when the continuous clear reading is exeeded
    
    unsigned int expected_values[4][9]; //the array of expected CRGB values for each card filled during calibration sequence
    
    unsigned int ReturnHomeTimes[30] = {0}; //Stores the time taken between each card. Max 30 times
    colour ReturnHomeCards[30]; //stores the order of cards seen during the outwards journey. Max 30 times
    
    unsigned int stop_all = 0; //indicates the end of the return home function
    
 
    /********************************************//**
    *  Calibration sequence
        1. Press button (within for statement(9 iterations) require button push before incrementing)
        2. Read card using collect avg readings function (decide if want to use new variables for these ones)
        3. Store those values in first index of each row of array (assign which colour that index will be)
        4. Press button to increment i and repeat for all 9 colours    
        5. Remove any obstructions in front of the card and perform clear_read_calibration function                                                                           * 
    ***********************************************/
    
    BRAKE = 0;
    for(colour i = RED; i<= BLACK; i++){ //iterate through the cards in order defined in README
        while(PORTFbits.RF2){ //place card right up against the front of the car before pressing the button
            BRAKE = 1;
        }
        BRAKE = 0;
        __delay_ms(100);
        stop(&motorL, &motorR);
        __delay_ms(20);
        reverseFullSpeed(&motorL, &motorR); //this moves the buggy back to the location of optimal reading accuracy
        __delay_ms(260);
        stop(&motorL, &motorR);
        collect_avg_readings(&clear_read, &red_read, &green_read, &blue_read); //take readings of CRGB values at optimal distance
        //fill the expected values array
        expected_values[RED][i] = red_read; 
        expected_values[GREEN][i] = green_read;
        expected_values[BLUE][i] = blue_read;
        expected_values[CLEAR_INDEX][i] = clear_read;
    }
    while(PORTFbits.RF2){BRAKE = 1;} // once all colour cards are done clear the space in front of the buggy and let it take one background clear reading
    BRAKE = 0;
    clear_read_calibration(buf, &clear_read, &clear_read_check); // assign the threshold value that will be exceeded in order for it to detect a card
    

    /********************************************//**
    *  Initialise maze sequence
    ***********************************************/
    while(PORTFbits.RF2){LEFT = 1; RIGHT = 1;} //wait for button press to begin sequence
    LEFT = 0; RIGHT = 0; //indicate sequence has begun by turning off lights
    fullSpeedAhead(&motorL, &motorR); //begin moving  
    
    
    while (1) { //main while loop continuously runs
        
        //timer only runs while the buggy is searching for the next card
        if (TimerFlag == 1){ //incrementing the timer counter every 1/10th second when the timer (in interrupts.c)overflows, triggering the TimerFlag
            TimerCount += 1;
            if (TimerCount % 10 == 0){LATHbits.LATH3=!LATHbits.LATH3;} // every second toggle light to indicate it is searching for a card
            TimerFlag = 0; //Reset the TimerFlag
        }
        
        
        clear_read = color_read_Clear(); //continuously check whether there is a card in front of the buggy (causing a peak in the clear reading)
        if (clear_read > clear_read_threshold && stop_all == 0){ //if current clear reading exceeds the threshold and we are not after the end of the return home sequence

            ReturnHomeTimes[CardCount] = TimerCount - 6; //put current timer value in 10ths of a second into ReturnHomeArray to be used on the way back to determine how far forward the buggy moves between each card
            
            //move buggy to distance of optimal detection accuray
            __delay_ms(2);
            stop(&motorL, &motorR);
            __delay_ms(20);
            reverseFullSpeed(&motorL, &motorR);
            __delay_ms(260);
            stop(&motorL, &motorR);
            __delay_ms(2);
                       
            //execute the function that reads the colour and reacts to it appropriately
            card = card_response(buf, &clear_read, &red_read, &green_read, &blue_read, expected_values, card, &motorL, &motorR, ReturnHomeTimes, ReturnHomeCards, &stop_all);    
            __delay_ms(2);
            ReturnHomeCards[CardCount] = card; //log in the relevant array which card has been detected
      
            CardCount += 1; //indicate that next time a card is detected the timer value should be stored in the next column along
            
            TimerCount = 0; //reset the timer once the buggy is about to move again
            if (stop_all == 0){fullSpeedAhead(&motorL, &motorR);} //begin moving unless the final return home command has been executed, changing the stop_all flag
        }
        
        //a function to reset the stop_all function and allow for the buggy to try another maze
        if(!PORTFbits.RF3){
            stop_all = 0;
            fullSpeedAhead(&motorL, &motorR);
        }
        
    }
}

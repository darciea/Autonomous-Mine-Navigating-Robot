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
    
    HomeStored ReturnHomeArray;
 
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
        for (int i = 0; i <= 500; i++){
        clear_read = color_read_Clear();
    } 
    
    for(int i = 0; i <= 2; i++){
        clear_read += color_read_Clear();
        __delay_ms(200);   
    }
    
    clear_read = clear_read/4;
    
    sprintf(buf, "\n Expected clear: %d \n", clear_read);
    sendStringSerial4(buf);
    
    unsigned int clear_read_check = clear_read + 800;

    /********************************************//**
    *  Trying code
    ***********************************************/
    fullSpeedAhead(&motorL, &motorR); //begin moving  
    while (1) {
        
        if (TimerFlag == 1){ //incrementing the timer counter every ms if the timer overflows. Note this relies on the while loop running more than once every ms - may need to experiment
            TimerCount += 1;
            if (TimerCount == 10){LATHbits.LATH3=!LATHbits.LATH3; TimerCount = 0;}
            TimerFlag = 0;
        }
        clear_read = color_read_Clear();
        if (clear_read > clear_read_check){

            ReturnHomeArray.TimerCount[CardCount] = TimerCount; //put current timer value in 10ths of a second into ReturnHomeArray to be used on the way back to determine how far forward the buggy moves between each card
            stop(&motorL, &motorR);
            __delay_ms(2);
                    
            sprintf(buf, "Timercount %d \n", ReturnHomeArray.TimerCount[CardCount]);
            sendStringSerial4(buf);
            __delay_ms(2);
                    
            card = card_response(buf, &red_read, &green_read, &blue_read, expected_values, card, &motorL, &motorR, ReturnHomeArray);    
            __delay_ms(2);
            ReturnHomeArray.card[CardCount] = card; //log in the array which card has been detected
      
            CardCount += 1; //indicate that next time a card is detected the timer value should be stored in the next column along
            
            TimerCount = 0; //reset the timer once the buggy is about to move again
            fullSpeedAhead(&motorL, &motorR); //begin moving  
        }
        
        red_read = color_read_Red();
        blue_read = color_read_Blue();
        green_read = color_read_Green();
        clear_read = color_read_Clear();

        
        sprintf(buf, "Raw %d, %d, %d, %d \n", red_read, green_read, blue_read, clear_read);
        sendStringSerial4(buf);
        __delay_ms(500);
         LATHbits.LATH3=!LATHbits.LATH3;
        
    }
}

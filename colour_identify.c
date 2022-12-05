#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"
#include "color.h"
#include "serial.h"

void Interrupts_init(void)
{
    enable_color_interrupt(); //enable interrupts from color click board
    set_interrupt_threshold(5, 5000, 0b0100); //set low and high threshold and persistence filter
    TRISBbits.TRISB1 = 1; //interrupt pin on the clickerboard
    ANSELBbits.ANSELB1 = 0; //disable analogue input
    INT1PPS=0x09; // which PPS register needs to be open for interrupt
    PIE0bits.INT1IE = 1; //which peripheral interrupt is enabled
    IPR0bits.INT1IP = 1; //high priority (HighISR)
    INTCONbits.INT1EDG = 0; //rising/falling edge
    INTCONbits.IPEN = 1;    // Priority levels enable bit
    INTCONbits.GIEL = 1; // Peripheral Interrupt Enable bit
    INTCONbits.GIEH=1;} 	//turn on interrupts globally (when this is off, all interrupts are deactivated)

void __interrupt(high_priority) HighISR()
{
	//trigger flag that indicates a card has been identified in front of the sensor
    if(PIR0bits.INT1IF == 1) {			// interrupt source is clickerboard bit corresponding to the colorclick interrupt pin
        LATDbits.LATD7=1;
        __delay_ms(50);
        LATDbits.LATD7=0;
        __delay_ms(50);
        clear_interrupt_flag();
        PIR0bits.INT1IF = 0; //need to clear clickerboard interrupt pin separately
    //undecided on how to define flag that indicates card has been detected
    }
}

void collect_avg_readings(unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read)
{   
    //for each measured colour (Red Green Blue) take three readings and average them, and store them in the appropriate location

    for(colour i = RED; i <= BLUE; i++){
        *red_read += color_read_Red();
        __delay_ms(200);
        //sprintf(buf, "Raw red %d \n", *red_read);
        //sendStringSerial4(buf);
        
        //__delay_ms(1000);
    }
    *red_read = *red_read/3;
    //sprintf(buf, "Average red %d \n", *red_read);
    //sendStringSerial4(buf);
    
    for(colour i = RED; i <= BLUE; i++){
        *green_read += color_read_Green();
        __delay_ms(200);
        //sprintf(buf, "Raw green %d \n", *green_read);
        //sendStringSerial4(buf);
        
        //__delay_ms(1000);
    }
    *green_read = *green_read/3;
    //sprintf(buf, "Average green %d \n", *green_read);
    //sendStringSerial4(buf);

    for(colour i = RED; i <= BLUE; i++){
        *blue_read += color_read_Blue();
        __delay_ms(200);
        //sprintf(buf, "Raw blue %d \n", *blue_read);
        //sendStringSerial4(buf);
        
        //__delay_ms(1000);
    }
    *blue_read = *blue_read/3;
    //sprintf(buf, "Average blue %d \n", *blue_read);
    //sendStringSerial4(buf);
        
}

void normalise_readings(unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read, unsigned int expected_values[][3], unsigned int normalised_values[][3]){   
        //for each colourcard: 
            //for each measured colour (Red, Green, Blue):
                //calculation done is (absolute value of the difference between measured and expected value)/expected value
                //this is then stored in the full array of all normalised differences
 

    for(colour i = RED; i<= BLACK; i++){
        
        //check the red readings against the expected readings and express as a number between 0 and 1
        normalised_values[0][0] = (abs(*red_read - expected_values[0][0])) / (expected_values[0][0]);
        //sprintf(buf, "Read %d, expected %d, normalised %d \n", red_read, expected_values[0][1], normalised_values[0][0]);
        //sendStringSerial4(buf);
        
        //check the green readings against the expected readings and express as a number between 0 and 1
        normalised_values[i][GREEN] = (abs(*green_read - expected_values[i][1])) / (expected_values[i][1]);
        
        //check the blue readings against the expected readings and express as a number between 0 and 1
        normalised_values[i][BLUE] = (abs(*blue_read - expected_values[i][2])) / (expected_values[i][2]);
        
    }
        
}
 
void make_master_closeness(unsigned int normalised_values[][3], unsigned int master_closeness[]){
    for(colour i = RED; i<=BLACK; i++){
        master_closeness[i] = (normalised_values[i][0] + normalised_values[i][1] + normalised_values[i][2])/3;
    }
}
 
colour determine_card(unsigned int master_closeness[]){
    colour predicted_colour = RED;
    unsigned int current_smallest = master_closeness[RED];
    for(colour i = GREEN; i<=BLACK; i++){
        if(master_closeness[i] < current_smallest){
            current_smallest = master_closeness[i];
            predicted_colour = i;
        }
    }
    return predicted_colour;
}

 
void respond_to_card(colour card, DC_motor *mL, DC_motor *mR){
    //reverseFullSpeed(mL,mR);
    //        __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
    switch (card){
        case RED:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case GREEN:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            turnLeft45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case BLUE:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case YELLOW:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            reverseFullSpeed(mL,mR);
            __delay_ms(500); //adjust according to what 'one square' means
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case PINK:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            reverseFullSpeed(mL,mR);
            __delay_ms(500); //adjust according to what 'one square' means
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case ORANGE:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case LIGHT_BLUE:
            reverseFullSpeed(mL,mR);
            __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
            turnLeft45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case WHITE:
            //CODE FOR CHANGING A FLAG TO START THE RETURN HOME SEQUENCE
            break;
        case BLACK:
            //CODE FOR THE EVENTUALITY IT RUNS INTO A WALL?
            break;    
        default:
            break;
    }
    
} 
 
#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"
#include "color.h"
#include "serial.h"
/*
void Interrupts_init(void)
{
    enable_color_interrupt(); //enable interrupts from color click board
    set_interrupt_threshold(0b00000010,0b00000100,0b0011); //set low threshold to 1024 and high to 2048 (most significant bits only) and persistence filter to 3 consecutive values out of range (may need adjusting)
    INTCONbits.GIEH=1; 	//turn on interrupts globally (when this is off, all interrupts are deactivated)
    INTCONbits.GIEL = 1;} // Peripheral Interrupt Enable bit

void __interrupt(high_priority) HighISR()
{
	//trigger flag that indicates a card has been identified in front of the sensor
    if(read_interrupt_status()) {LATDbits.LATD7 = 1; color_writetoaddr(0x13, 0x01);		//check the interrupt source (color click board status bit)
    //undecided on how to define flag that indicates card has been detected
    }
}
 */
void collect_avg_readings(/*char *buf, */unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read)
{   
    //for each measured colour (Red Green Blue) take three readings and average them, and store them in the appropriate location

    for(colour i = 0; i <= 2; i++){
        *red_read += color_read_Red();
        __delay_ms(200);   
    }
    *red_read = *red_read/3;
    
    //sprintf(buf, "Average red %d \n", *red_read);
    //sendStringSerial4(buf);
    
    for(colour i = 0; i <= 2; i++){
        *green_read += color_read_Green();
        __delay_ms(200);
    }
    *green_read = *green_read/3;
    
    //sprintf(buf, "Average green %d \n", *green_read);
    //sendStringSerial4(buf);

    for(colour i = 0; i <= 2; i++){
        *blue_read += color_read_Blue();
        __delay_ms(200);
    }
    *blue_read = *blue_read/3;
    
    //sprintf(buf, "Average blue %d \n", *blue_read);
    //sendStringSerial4(buf);
        
}

void normalise_readings(char *buf, unsigned int red_read, unsigned int green_read, unsigned int blue_read, unsigned int expected_values[][3], unsigned int normalised_values[][3]){   
        //for each colourcard: 
            //for each measured colour (Red, Green, Blue):
                //calculation done is (absolute value of the difference between measured and expected value)/expected value
                //this is then stored in the full array of all normalised differences
 

    for(colour i = RED; i<= BLUE; i++){
        
        //check the red readings against the expected readings and express as a number between 0 and 1
        unsigned int differenceR = abs(red_read - expected_values[i][RED]);
        unsigned int normalR = (differenceR*100) / expected_values[i][RED];
        //normalised_values[i][RED] = (difference*100) / expected_values[i][RED];
        sprintf(buf, "RED: Read %d, expected %d, difference %d, normalised %d \n", red_read, expected_values[i][RED], differenceR, normalR);
        sendStringSerial4(buf);
        __delay_ms(1000);
        
        //check the green readings against the expected readings and express as a number between 0 and 1
        unsigned int differenceG = abs(green_read - expected_values[i][GREEN]);
        unsigned int normalG = (differenceG*100) / expected_values[i][GREEN];
        sprintf(buf, "GREEN: Read %d, expected %d, difference %d, normalised %d \n", green_read, expected_values[i][GREEN], differenceG, normalG);
        sendStringSerial4(buf);
        __delay_ms(1000);
        
        //check the blue readings against the expected readings and express as a number between 0 and 1
        unsigned int differenceB = abs(blue_read - expected_values[i][BLUE]);
        unsigned int normalB = (differenceB*100) / expected_values[i][BLUE];
        sprintf(buf, "BLUE: Read %d, expected %d, difference %d, normalised %d \n", blue_read, expected_values[i][BLUE], differenceB, normalB);
        sendStringSerial4(buf);
        __delay_ms(1000);
    }
        
}
 
void make_master_closeness(char *buf, unsigned int normalised_values[][3], unsigned int master_closeness[]){
    for(colour i = RED; i<=BLACK; i++){
        master_closeness[i] = (normalised_values[i][RED] + normalised_values[i][GREEN] + normalised_values[i][BLUE])/3;
        sprintf(buf, "MC Avg: normRED %d, normGREEN %d, normBLUE %d, master %d \n", normalised_values[i][RED], normalised_values[i][GREEN],normalised_values[i][BLUE], master_closeness[i]);
        __delay_ms(1000);
        sendStringSerial4(buf);
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
            break;/*
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
            break; */   
        default:
            break;
    }
    
} 
 
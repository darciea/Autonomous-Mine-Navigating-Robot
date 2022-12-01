#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"
#include "color.h"
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
    if(read_interrupt_status()) {			//check the interrupt source (color click board status bit)
    //undecided on how to define flag that indicates card has been detected
    }
}
*/ 
void collect_avg_readings(char *buf, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read)
{   
    //for each measured colour (Red Green Blue) take three readings and average them, and store them in the appropriate location

    for(colour i = RED; i <= BLUE; i++){
        *red_read += color_read_Red();
        sprintf(buf, "Raw %d, %d, %d \n", red_read, green_read, blue_read);
        sendStringSerial4(buf);
        __delay_ms(10);
    }
    *red_read = *red_read/3;
    
    for(colour i = RED; i <= BLUE; i++){
        *green_read += color_read_Green();
        __delay_ms(10);
    }
    *green_read = *green_read/3;

    for(colour i = RED; i <= BLUE; i++){
        *blue_read += color_read_Blue();
        __delay_ms(10);
    }
    *blue_read = *blue_read/3;
        
}
/*
void normalise_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read, unsigned char *expected_values, unsigned char *normalised_values)
{   
        //for each colourcard: 
            //for each measured colour (Red, Green, Blue):
                //calculation done is (absolute value of the difference between measured and expected value)/expected value
                //this is then stored in the full array of all normalised differences
 

    for(unsigned int i = 0; i<=7; i++){
        
        //check the red readings against the expected readings and express as a number between 0 and 1
        *normalised_values[i][0] = (abs(*red_read - *expected_values[i][0])) / (*expected_values[i][0]);
        
        //check the green readings against the expected readings and express as a number between 0 and 1
        *normalised_values[i][1] = (abs(*green_read - *expected_values[i][1])) / (*expected_values[i][1]);
        
        //check the blue readings against the expected readings and express as a number between 0 and 1
        *normalised_values[i][2] = (abs(*blue_read - *expected_values[i][2])) / (*expected_values[i][2]);
    }
        
}
 
void make_master_closeness(unsigned char *normalised_values, unsigned char *master_closeness){
    for(int i = 0; i<=7; i++){
        *master_closeness[i] = (*normalised_values[i][0] + *normalised_values[i][1] + *normalised_values[i][2])/3;
    }
}
 
colour determine_card(unsigned char *master_closeness){
    colour smallest = 0;
    for(unsigned int i = 0; i<=7; i++){
        if(master_closeness[i] < master_closeness[i-1]){smallest = i;}
        }
    return smallest;
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
 */
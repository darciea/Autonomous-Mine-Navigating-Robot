#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"

/*
void collect_avg_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read)
{   
    for(int i = 0; i = 2; i++){
        *red_read += color_read_Red;
    }
    *red_read = *red_read/3;
    
    for(int i = 0; i = 2; i++){
        *green_read += color_read_Green;
    }
    *green_read = *green_read/3;

    for(int i = 0; i = 2; i++){
        *blue_read += color_read_Blue;
    }
    *blue_read = *blue_read/3;
        
}

void normalise_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read, unsigned char *expected_values, unsigned char *normalised_values)
{   
    for(unsigned int i = 0; i<=7; i++){
        //temporary values for calculating the normalised reading
        unsigned int difference = 0;
        unsigned int normal = 0; 
        
        //check the red readings against the expected readings and express as a number between 0 and 1
        difference = abs(*red_read - *expected_values[i][0]);
        normal = difference / *expected_values[i][0];
        *normalised_values[i][0] = normal;
        
        //check the green readings against the expected readings and express as a number between 0 and 1
        difference = abs(*green_read - *expected_values[i][1]);
        normal = difference / *expected_values[i][1];
        *normalised_values[i][1] = normal;
        
        //check the blue readings against the expected readings and express as a number between 0 and 1
        difference = abs(*blue_read - *expected_values[i][2]);
        normal = difference / *expected_values[i][2];
        *normalised_values[i][2] = normal;
    }
        
}

void make_master_closeness(unsigned char *normalised_values, *master_closeness){
    for(int i = 0; i<=7; i++){
        unsigned char average = 0;
        average = (*normalised_values[i][0] + *normalised_values[i][1] + *normalised_values[i][2])/3
        *master_closeness[i] = average;
    }
}

colour determine_card(unsigned char *master_closeness){
    colour smallest = 0;
    for(unsigned int i = 0; i<=7; i++){
        if(master_closeness[i] < master_closeness[i-1]){smallest = i;}
        }
    return smallest;
}
*/
 
void respond_to_card(colour card, DC_motor *mL, DC_motor *mR){
    //reverseFullSpeed(mL,mR);
    //        __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
    switch (card){
        case RED:
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case GREEN:
            turnLeft45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case BLUE:
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
            __delay_ms(500); //adjust according to what 'one square' means
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case PINK:
            reverseFullSpeed(mL,mR);
            __delay_ms(500); //adjust according to what 'one square' means
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case ORANGE:
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case LIGHT_BLUE:
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


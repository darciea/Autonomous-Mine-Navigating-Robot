#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"
#include "LEDsOn.h"

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
*/

colour determine_card(unsigned char *master_closeness){
    colour predicted_colour = RED;
    unsigned char current_smallest = master_closeness[RED];
    for(colour i = GREEN; i<=BLACK; i++){
        if(master_closeness[i] < current_smallest){
            current_smallest = master_closeness[i];
            predicted_colour = i;
        }
    }
    return predicted_colour;
}

 
void respond_to_card(colour card, DC_motor *mL, DC_motor *mR){
    //PROBABLY WANT TO REVERSE A LITTLE BEFORE EVERYTHING TO HAVE SPACE TO TURN
    switch (card){
        case RED:
            LEFT = 1;
            /*
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
             * */
            break;
        case GREEN:
            RIGHT = 1;
            /*
            turnLeft45(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
             */
            break;
        case BLUE:
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case YELLOW:
            MBEAM = 1;
            /*
            reverseOneSquare(mL,mR);
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
             */
            break;
        case PINK:
            BRAKE = 1;
            /*
            reverseOneSquare(mL,mR);
            turnLeft45(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
             */
            break;
        case ORANGE:
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case LIGHT_BLUE:
            turnLeft45(mL,mR);
            turnLeft45(mL,mR);
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


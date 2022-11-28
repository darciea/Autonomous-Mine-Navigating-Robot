#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"


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

unsigned int determine_card(unsigned char *master_closeness){
    for(unsigned int i = 0; i<=7; i++){
        unsigned char smallest = 0;
        if(master_closeness[i] < master_closeness[i-1]){smallest = i;}
        }
    return smallest;
}
/*
void respond_to_card(unsigned int card,DC_motor *mL, DC_motor *mR ){
    //PROBABLY WANT TO REVERSE A LITTLE BEFORE EVERYTHING TO HAVE SPACE TO TURN
    if (card = RED){turnRight(&motorL,&motorR);}
    else if(card = GREEN){turnLeft(&motorL,&motorR);}
    else if (card = BLUE){
        turnRight(&motorL,&motorR);
        turnRight(&motorL,&motorR);
    }
    else if (card = YELLOW){
        //CODE FOR REVERSING 1 SQUARE
        turnRight(&motorL,&motorR);
    }
    else if (card = PINK){
        //CODE FOR REVERSING 1 SQUARE
        turnLeft(&motorL,&motorR);
    }
    else if (card = ORANGE){
        //TURN RIGHT 135
    }
    else if (card = LIGHT BLUE){
        //TURN LEFT 135
    }
    else if (card = WHITE){
        //CODE FOR CHANGING A FLAG TO START THE RETURN HOME SEQUENCE
    }
    else if (card = BLACK){
        //CODE FOR THE EVENTUALITY IT RUNS INTO A WALL?
    }
    //else MAYBE HAVE CODE HERE TO CHANGE A FLAG AND DO THE READING AGAIN?
    
}*/


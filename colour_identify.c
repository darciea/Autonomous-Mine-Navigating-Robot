#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"
#include "color.h"
#include "serial.h"
#include "LEDsOn.h"

//function that takes a clear reading, averages it, and assigns a threshold value which will be exceeded when the buggy detects a card
void clear_read_calibration(char *buf, unsigned int *clear_read, unsigned int *clear_read_check){
    
    for (int i = 0; i <= 500; i++){ //500 unsaved readings ensure the final 4 that are saved are as consistent as possible
        *clear_read = color_read_Clear();
    } 
    
    for(int i = 0; i <= 2; i++){ //clear read already has one value assigned, therefore add 3 more...
        *clear_read += color_read_Clear();
        __delay_ms(200);   
    }
    
    *clear_read = *clear_read/4; ///... and divide by 4 to average

    *clear_read_check = *clear_read + 800; //threshold value for when a card is in front. +800 determined experimentally
}

//function that takes 4 readings of each of clear, red, green and blue sensor, and averages them
void collect_avg_readings(unsigned int *clear_read, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read)
{   
    //take ~500 readings to allow colour readings to stabilise before starting to take the average
    
    for (int i = 0; i <= 500; i++){
        *clear_read = color_read_Clear();
        *red_read = color_read_Red();
        *green_read = color_read_Green();
        *blue_read = color_read_Blue();
    } 

    //for each color sensor take three readings and average them, and store them in the appropriate location
    
    for(int i = 0; i <= 2; i++){
        *clear_read += color_read_Clear();
        __delay_ms(200);   
    }
    *clear_read = *clear_read/4; //divide by 4 as one value is left in the reading from the initial 500 check

    
    //repeat for Red
    for(int i = 0; i <= 2; i++){
        *red_read += color_read_Red();
        __delay_ms(200);   
    }
    *red_read = *red_read/4;
    
    
    //Repeat for Green
    for(int i = 0; i <= 2; i++){
        *green_read += color_read_Green();
        __delay_ms(200);
    }
    *green_read = *green_read/4;

    
    //Repeat for Blue
    for(int i = 0; i <= 2; i++){
        *blue_read += color_read_Blue();
        __delay_ms(200);
    }
    *blue_read = *blue_read/4;
        
}

//takes values from collect_avg_readings and normalises them
void normalise_readings(char *buf, unsigned int clear_read, unsigned int red_read, unsigned int green_read, unsigned int blue_read, unsigned int expected_values[][9], unsigned int normalised_values[][9]){   
    //for each colourcard: (comparing our collected values against each expected reading for each colour)
        //for each reading (Clear, Red, Green, Blue):
            //calculation done is (absolute value of the difference between measured and expected value)/expected value
            //this is then stored in the full array of all normalised differences per colour read per colour cards
 

    for(colour i = RED; i<= BLACK; i++){
        //difference is just a temporary variable for performing the calculations
        
              
        //check the red readings against the expected readings and express as a percentage
        unsigned int difference = abs(red_read - expected_values[RED][i]);
        normalised_values[RED][i] = (difference*100) / expected_values[RED][i];
        
        //check the green readings against the expected readings and express as a percentage
        difference = abs(green_read - expected_values[GREEN][i]);
        normalised_values[GREEN][i] = (difference*100) / expected_values[GREEN][i];
        
        //check the blue readings against the expected readings and express as a percentage
        difference = abs(blue_read - expected_values[BLUE][i]);
        normalised_values[BLUE][i] = (difference*100) / expected_values[BLUE][i];
        
        //check the clear readings against the expected readings and express as a percentage
        difference = abs(clear_read - expected_values[3][i]);
        normalised_values[CLEAR_INDEX][i] = (difference*100) / expected_values[3][i];
        
    }
        
}
 
//determines how much each reading resembles each of the expected readings from the coloured cards
void make_master_closeness(char *buf, unsigned int normalised_values[][9], unsigned int master_closeness[]){
    for(colour i = RED; i<=BLACK; i++){
        //for each colour card, find the average difference per colourcard and store it in the array master_closeness
        master_closeness[i] = (normalised_values[RED][i] + normalised_values[GREEN][i] + normalised_values[BLUE][i] + normalised_values[CLEAR_INDEX][i])/4;


    }
}
 
//Uses the master_closeness array to determine which card the detected card is most likely to be
colour determine_card(unsigned int master_closeness[]){
    //this function will iterate through the master_closeness array to find the smallest difference, and hence the colour card that we think it is
    colour predicted_colour = RED; //assume the colour is red at first
    unsigned int current_smallest = master_closeness[RED]; //taking this value as the first comparison
    for(colour i = GREEN; i<=BLACK; i++){
        if(master_closeness[i] < current_smallest){
            current_smallest = master_closeness[i];
            predicted_colour = i;
        }
    }
    return predicted_colour;
}

//Responds appropriately given the predicted card. Response is determined by README
void motor_response(char *buf, colour card, DC_motor *mL, DC_motor *mR, unsigned int ReturnHomeTimes[], colour ReturnHomeCards[], unsigned int *stop_all){
    switch (card){
        case RED:
            reverseFullSpeed(mL,mR);
            __delay_ms(300); //adjust to get the car to the centre of the maze path
            turnRight45(mL,mR);
            stop(mL,mR); //stop in between turns increases accuracy of turn angle
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case GREEN:
            reverseFullSpeed(mL,mR);
            __delay_ms(300); //adjust to get the car to the centre of the maze path
            turnLeft45(mL,mR);
            stop(mL,mR); 
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case BLUE:
            reverseFullSpeed(mL,mR);
            __delay_ms(300); //adjust to get the car to the centre of the maze path
            turnRight45(mL,mR);
            stop(mL,mR); 
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case YELLOW:
            reverseFullSpeed(mL,mR);
            __delay_ms(1700); //one square plus space to get to the centre
            stop(mL,mR); 
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case PINK:
            reverseFullSpeed(mL,mR);
            __delay_ms(1700); //one square plus space to get to the centre
            stop(mL,mR); 
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case ORANGE:
            reverseFullSpeed(mL,mR);
            __delay_ms(300); //adjust to get the car to the centre of the maze path
            turnRight45(mL,mR);
            stop(mL,mR); 
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case LIGHT_BLUE:
            reverseFullSpeed(mL,mR);
            __delay_ms(300); //adjust to get the car to the centre of the maze path
            turnLeft45(mL,mR);
            stop(mL,mR); 
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case WHITE: //this initialises the return home sequence
            reverseFullSpeed(mL,mR);
            __delay_ms(300); //get buggy to centre of maze path
            stop(mL,mR);
            turnLeft45(mL,mR);//turn around 180 degrees
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            __delay_ms(100);
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
        
            for(int i = 28; i >= 0; i--){ //iterate backwards through the ReturnHomeTimes array:
                if (ReturnHomeTimes[i+1] != 0){ //any 0 values will be ignored so it only begins reading at the most recent input
                    fullSpeedAhead(mL,mR);
                    for (int j=0; j<= ReturnHomeTimes[i+1]; j++){ // ReturnHomeTimes stores number of 1/10th seconds the buggy needs to move
                        __delay_ms(100); //hence move for 100ms per unit stored
                    }
                    stop(mL,mR);
                    home_response(ReturnHomeCards[i], mL, mR); //execute the appropriate command given the next card in the array
                }
            }
            //after the last card has been read
            fullSpeedAhead(mL,mR);//move forward
            for (int j=0; j<= ReturnHomeTimes[0]; j++){ //by the final (ie the first) value in the ReturnHomeTimes array
                __delay_ms(100);}
            stop(mL,mR);
            *stop_all = 1; //indicate to the main loop that the return home sequence is complete and the buggy should cease action
            break;
        case BLACK: //if BLACK is read try read the card again
            break;    
        default: //if no card at all is read, try read the card again
            break;
    }
    
} 

//When white card is detected, the opposite command to the outward journey must be executed, which is done by this function
void home_response(colour card, DC_motor *mL, DC_motor *mR){
    switch(card){
        case RED: //turn left 90 degrees
            turnLeft45(mL,mR);
            stop(mL,mR); 
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case GREEN: //turn right 90 degrees
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case BLUE: //turn around 180 degrees
            turnRight45(mL,mR);
            stop(mL,mR); 
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case YELLOW: //turn right 90 degrees, then move forward one square, then turn around 180 degrees
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            fullSpeedAhead(mL,mR);
            __delay_ms(1700); 
            turnRight45(mL,mR);
            stop(mL,mR); 
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case PINK: //turn left 90 degrees, then move forward one square, then turn around 180 degrees
            stop(mL,mR); 
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            fullSpeedAhead(mL,mR);
            __delay_ms(1700);
            turnRight45(mL,mR);
            stop(mL,mR); 
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case ORANGE: //turn left 135 degrees
            turnLeft45(mL,mR);
            stop(mL,mR); 
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case LIGHT_BLUE: //turn right 135 degrees
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        case WHITE: // end of return_home sequence
            stop(mL,mR);
        default:
            break;
    }    
}

//this function combines the workflow of the functions needed to determine and respond to the different coloured cards
colour card_response(char *buf, unsigned int *clear_read, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read, unsigned int expected_values[][9], colour card, DC_motor *mL, DC_motor *mR, unsigned int ReturnHomeTimes[], colour ReturnHomeCards[], unsigned int *stop_all) {
    card  = RED; //default to CARd = 0 which happens to be RED
    unsigned int normalised_values[4][9]; //define temporary variables to assist calculations
    unsigned int master_closeness[9];
        
    collect_avg_readings(clear_read, red_read, green_read, blue_read);//
    normalise_readings(buf, *clear_read, *red_read, *green_read, *blue_read, expected_values, normalised_values);
    make_master_closeness(buf, normalised_values, master_closeness);
    card = determine_card(master_closeness);
    motor_response(buf, card, mL, mR, ReturnHomeTimes, ReturnHomeCards, stop_all);
    return card;
}

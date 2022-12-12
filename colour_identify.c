#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "colour_identify.h"
#include "dc_motor.h"
#include "color.h"
#include "serial.h"

void collect_avg_readings(/*char *buf, */unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read)
{   
    //take ~500 readings to allow colour readings to stabilise before starting to take the average
    
    for (int i = 0; i <= 500; i++){
        *red_read = color_read_Red();
        *green_read = color_read_Green();
        *blue_read = color_read_Blue();
    } 

    //for each measured colour (Red Green Blue) take three readings and average them, and store them in the appropriate location

    for(int i = 0; i <= 2; i++){
        *red_read += color_read_Red();
        __delay_ms(200);   
    }
    *red_read = *red_read/3;
    
    for(int i = 0; i <= 2; i++){
        *green_read += color_read_Green();
        __delay_ms(200);
    }
    *green_read = *green_read/3;

    for(int i = 0; i <= 2; i++){
        *blue_read += color_read_Blue();
        __delay_ms(200);
    }
    *blue_read = *blue_read/3;
        
}

void normalise_readings(char *buf, unsigned int red_read, unsigned int green_read, unsigned int blue_read, unsigned int expected_values[][9], unsigned int normalised_values[][9]){   
    //for each colourcard: (comparing our collected values against each expected reading for each colour)
        //for each reading (Red, Green, Blue):
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
    }
        
}
 
void make_master_closeness(char *buf, unsigned int normalised_values[][9], unsigned int master_closeness[]){
    for(colour i = RED; i<=BLACK; i++){
        //for each colour card, find the average difference per colourcard and store it in the array master_closeness
        master_closeness[i] = (normalised_values[RED][i] + normalised_values[GREEN][i] + normalised_values[BLUE][i])/3;
        //sprintf(buf, "MC Avg: normRED %d, normGREEN %d, normBLUE %d, master %d \n", normalised_values[RED][i], normalised_values[GREEN][i],normalised_values[BLUE][i], master_closeness[i]);
        //sendStringSerial4(buf);
        //__delay_ms(200);

    }
}
 
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

void motor_response(colour card, DC_motor *mL, DC_motor *mR, HomeStored ReturnHomeArray){
    //this function takes in the colour of the card we have found and performs the motor function as directed.
    //reverseFullSpeed(mL,mR);
    //        __delay_ms(50); //adjust to give car enough clearance from the wall to turn freely
    //stop
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
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            for(int i = 29; i >= 0; i--){
                if (ReturnHomeArray.TimerCount[i] != 0){
                    fullSpeedAhead(mL,mR);
                    for (int j=0; j<= ReturnHomeArray.TimerCount[i]; i++){
                        __delay_ms(100);
                    }
                    stop(mL,mR);
                    home_response(ReturnHomeArray.card[i], mL, mR);
                }
            }
            break;
        case BLACK:
            //CODE FOR THE EVENTUALITY IT RUNS INTO A WALL?
            break;    
        default:
            break;
    }
    
} 

void home_response(colour card, DC_motor *mL, DC_motor *mR){
    switch(card){
        case RED:
            turnLeft45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case GREEN:
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
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
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            fullSpeedAhead(mL,mR);
            __delay_ms(500); //adjust according to what 'one square' means
            break;
        case PINK:
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            fullSpeedAhead(mL,mR);
            __delay_ms(500); //adjust according to what 'one square' means
            break;
        case ORANGE:
            turnLeft45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnLeft45(mL,mR);
            stop(mL,mR);
            turnLeft45(mL,mR);
            stop(mL,mR);
            break;
        case LIGHT_BLUE:
            turnRight45(mL,mR);
            stop(mL,mR); //not strictly necessary but may help with consistency to stop drifting further than intended
            turnRight45(mL,mR);
            stop(mL,mR);
            turnRight45(mL,mR);
            stop(mL,mR);
            break;
        default:
            break;
    }
    
}

colour card_response(char *buf, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read, unsigned int expected_values[][9], colour card, DC_motor *mL, DC_motor *mR, HomeStored ReturnHomeArray) {
    //this function combines the workflow of the functions needed to determine and respond to the different coloured cards
    
    card  = RED;
    unsigned int normalised_values[3][9];
    unsigned int master_closeness[9];
        
    collect_avg_readings(red_read, green_read, blue_read);
    sprintf(buf, "\n AVG: R %d, G %d, B %d \n", *red_read, *green_read, *blue_read);
    sendStringSerial4(buf);  
    
    normalise_readings(buf, *red_read, *green_read, *blue_read, expected_values, normalised_values);
    
    make_master_closeness(buf, normalised_values,master_closeness);
    
    card = determine_card(master_closeness);
    sprintf(buf, "CARD %d \n", card);
    sendStringSerial4(buf);
    
    motor_response(card, mL, mR, ReturnHomeArray);
    
    return card;
}
 

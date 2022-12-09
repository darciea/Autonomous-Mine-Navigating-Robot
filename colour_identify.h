#ifndef COLOUR_IDENTIFY_H
#define	COLOUR_IDENTIFY_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000



typedef enum colour{RED, GREEN, BLUE, YELLOW, PINK, ORANGE, LIGHT_BLUE, WHITE, BLACK} colour; 

void collect_avg_readings(/*char *buf, */unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read);
void normalise_readings(char *buf, unsigned int red_read, unsigned int green_read, unsigned int blue_read, unsigned int expected_values[][9], unsigned int normalised_values[][9]);
void make_master_closeness(char *buf, unsigned int normalised_values[][9], unsigned int master_closeness[]);
colour determine_card(unsigned int master_closeness[]);

void respond_to_card(colour card, DC_motor *mL, DC_motor *mR);
void home_response(colour card, DC_motor *mL, DC_motor *mR);

typedef union HomeStored { //union allows both types relating to the Return_Home response to be kept in one array
    unsigned int TimerCount[30];         
    colour card[30];     
} HomeStored;

void motor_response(colour card, DC_motor *mL, DC_motor *mR, HomeStored ReturnHomeArray);

colour card_response(char *buf, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read, unsigned int expected_values[][9], colour card, DC_motor *mL, DC_motor *mR, HomeStored ReturnHomeArray);

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

#endif	



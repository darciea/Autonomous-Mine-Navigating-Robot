#ifndef COLOUR_IDENTIFY_H
#define	COLOUR_IDENTIFY_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000
#define CLEAR_INDEX 3




typedef enum colour{RED, GREEN, BLUE, YELLOW, PINK, ORANGE, LIGHT_BLUE, WHITE, BLACK} colour; 

void clear_read_calibration(char *buf, unsigned int *clear_read, unsigned int *clear_read_check);
void collect_avg_readings(unsigned int *clear_read, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read);
void normalise_readings(char *buf, unsigned int clear_read, unsigned int red_read, unsigned int green_read, unsigned int blue_read, unsigned int expected_values[][9], unsigned int normalised_values[][9]);
void make_master_closeness(char *buf, unsigned int normalised_values[][9], unsigned int master_closeness[]);
colour determine_card(unsigned int master_closeness[]);
void respond_to_card(colour card, DC_motor *mL, DC_motor *mR);
void home_response(colour card, DC_motor *mL, DC_motor *mR);
void motor_response(char *buf, colour card, DC_motor *mL, DC_motor *mR, unsigned int ReturnHomeTimes[], colour ReturnHomeCards[], unsigned int *stop_all);
colour card_response(char *buf, unsigned int *clear_read, unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read, unsigned int expected_values[][9], colour card, DC_motor *mL, DC_motor *mR, unsigned int ReturnHomeTimes[], colour ReturnHomeCards[], unsigned int *stop_all);



#endif

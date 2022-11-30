#ifndef COLOUR_IDENTIFY_H
#define	COLOUR_IDENTIFY_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000



typedef enum colour{RED, GREEN, BLUE, YELLOW, PINK, ORANGE, LIGHT_BLUE, WHITE, BLACK} colour;

void collect_avg_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read);
void normalise_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read, unsigned char *expected_values);
void make_master_closeness(unsigned char *normalised_values, unsigned char *master_closeness);
colour determine_card(unsigned char *master_closeness);

void respond_to_card(colour card, DC_motor *mL, DC_motor *mR);

#endif	


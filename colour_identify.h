#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#ifndef COLOUR_IDENTIFY_H
#define	COLOUR_IDENTIFY_H

enum colour{RED, GREEN, BLUE, YELLOW, PINK, ORANGE, LIGHT BLUE, WHITE, BLACK};

void collect_avg_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read);
void normalise_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read, unsigned char *expected_values);
void make_master_closeness(unsigned char *normalised_values, *master_closeness);
void determine_card(unsigned char *master_closeness);

void respond_to_card(unsigned int card);

#endif	


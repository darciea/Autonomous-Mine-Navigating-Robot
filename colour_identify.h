#ifndef COLOUR_IDENTIFY_H
#define	COLOUR_IDENTIFY_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000



typedef enum colour{RED, GREEN, BLUE, YELLOW, PINK, ORANGE, LIGHT_BLUE, WHITE, BLACK} colour; 
//is it bad that variable and type are called the same name? May cause confusion if the compiler is unsure whether you are referring to variable or type

void collect_avg_readings(unsigned int *red_read, unsigned int *green_read, unsigned int *blue_read);
void normalise_readings(unsigned char *red_read, unsigned char *green_read, unsigned char *blue_read, unsigned char *expected_values, unsigned char *normalised_values);
void make_master_closeness(unsigned char *normalised_values, unsigned char *master_closeness);
colour determine_card(unsigned char *master_closeness);

void respond_to_card(colour card, DC_motor *mL, DC_motor *mR);

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

#endif	



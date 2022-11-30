#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(void); // function to setup PWM
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft45(DC_motor *mL, DC_motor *mR);
void turnRight45(DC_motor *mL, DC_motor *mR);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR);
void reverseOneSquare(DC_motor *mL, DC_motor *mR);

#endif

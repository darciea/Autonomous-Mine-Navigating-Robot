#include <xc.h>
#include <math.h>
#include "interrupts.h"
#include "color.h"
#include "i2c.h"

void Interrupts_init(void)
{
    //timer related commands
    TMR0IE=1; //enable interrupts from timer
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b0101; // 1:32 PS
    T0CON0bits.T016BIT=1;	//8bit mode		
    IPR0bits.TMR0IP = 0; //Low priority
    TMR0H=0b00111100;            //write High reg first, update happens when low reg is written to
    TMR0L=0b10101111;
    T0CON0bits.T0EN=1;	//start the timer
    //then general commands
    INTCONbits.GIEL = 1; // Peripheral Interrupt Enable bit
    INTCONbits.GIEH=1;} 	//turn on interrupts globally (when this is off, all interrupts are deactivated)

//second timer related interrupts
void __interrupt(low_priority) LowISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if(TMR0IF){ 					//check the interrupt source (timer)
    TMR0H=0b00111100;            //write High reg first, update happens when low reg is written to
    TMR0L=0b10101111;
        // at PS = 1, timer increments every 62.5ns, therefore at PS = 32 timer increments every 2us
        //in 16 bit mode timer overflows every 0.13107s
        //reset timer to value 15535 every overflow to remove that 0.03107s error
        
        TimerFlag=1; //defined as global variable in interrupts.h
        
       TMR0IF=0; 						//clear the interrupt flag from comparator
    }
}
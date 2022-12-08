#include <xc.h>
#include <math.h>
#include "interrupts.h"
#include "color.h"
#include "i2c.h"

void Interrupts_init(void)
{/*
    //first color click related commands
    enable_color_interrupt(); //enable interrupts from color click board
    set_interrupt_threshold(1, 2000, 0b0100); //set low and high threshold and persistence filter
    TRISBbits.TRISB1 = 1; //interrupt pin on the clickerboard
    ANSELBbits.ANSELB1 = 0; //disable analogue input
    INT1PPS=0x09; // which PPS register needs to be open for interrupt
    PIE0bits.INT1IE = 1; //which peripheral interrupt is enabled
    IPR0bits.INT1IP = 1; //high priority (HighISR)
    INTCONbits.INT1EDG = 0; //rising/falling edge
    INTCONbits.IPEN = 1;    // Priority levels enable bit
    */
    //then timer related commands
    TMR0IE=1; //enable interrupts from timer
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b0110; // 1:64 PS
    T0CON0bits.T016BIT=0;	//8bit mode		
    TMR0L = 0b00000111;         //because we are in 8bit mode, only the TMR0L register matters.
    T0CON0bits.T0EN=1;	//start the timer
    count = 0; //defined in header file as global variable
    
    //then general commands
    INTCONbits.GIEL = 1; // Peripheral Interrupt Enable bit
    INTCONbits.GIEH=1;} 	//turn on interrupts globally (when this is off, all interrupts are deactivated)

/*
//first color click related functions
void __interrupt(high_priority) HighISR()
{
	//trigger flag that indicates a card has been identified in front of the sensor
    if(PIR0bits.INT1IF == 1 & response_in_progress == 0) {// interrupt source is clickerboard bit corresponding to the colorclick interrupt pin, and the buggy is not currently responding to an interrupt just triggered
        card_detected = 1; //defined as global variable in main.c
        LATDbits.LATD7=1;
        __delay_ms(50);
        LATDbits.LATD7=0;
        __delay_ms(50);
        clear_interrupt_flag();
        PIR0bits.INT1IF = 0; //need to clear clickerboard interrupt pin separately
    }
}

void enable_color_interrupt(void){
    clear_interrupt_flag(); //make sure interrupt flag is cleared to start with
	color_writetoaddr(0x00, 0b10011); //address for ENABLE bit is 0x00 and we want to turn it on, and enable interrupts
}

void set_interrupt_threshold(char AILT, char AIHT, char persistence){
    
    color_writetoaddr(0x0C, persistence); // set the persistence filter
    color_writetoaddr(0x05, AILT && 0b1111111100000000); // set most significant bit of the low threshold register
    color_writetoaddr(0x04, AILT && 0b0000000011111111); // set least significant bit of the low threshold register
    color_writetoaddr(0x07, AIHT && 0b1111111100000000); // set most significant bit of the high threshold register
    color_writetoaddr(0x06, AIHT && 0b0000000011111111); // set least significant bit of the high threshold register
}

void clear_interrupt_flag(void){
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00); // write mode
    I2C_2_Master_Write(0b11100110);  // Command + Register address  
    I2C_2_Master_Stop();             // Send stop bit
}
*/
//second timer related interrupts
void __interrupt(low_priority) LowISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if(TMR0IF){ 					//check the interrupt source (timer)
        TMR0L = 0b00000111; //because we are in 8bit mode, only the TMR0L register matters.
        
        // at PS = 1, timer increments every 62.5ns, therefore at PS = 8 timer increments every 4us
        //in 8 bit mode timer overflows every 1.02ms
        //reset timer to value 5 every overflow to remove that 0.02ms error
        //however - this interrupt code takes time to run. To take this into consideration, set timer to 7 instead of 5 (by experimentation).
        
        TimerFlag=1; //defined as global variable in interrupts.h
        
       TMR0IF=0; 						//clear the interrupt flag from comparator
    }
}
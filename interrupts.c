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
    //LATBbits.LATB1 =0; //interrupt pin for clickerboard
    TRISBbits.TRISB1 = 0; //interrupt pin on the clickerboard
    ANSELBbits.ANSELB1 = 0; //disable analogue input
    INT1PPS=0x09; // which PPS register needs to be open for interrupt
    PIE0bits.INT1IE = 1; //which peripheral interrupt is enabled
    IPR0bits.INT1IP = 1; //high priority (HighISR)
    INTCONbits.INT1EDG = 0; //rising/falling edge
    INTCONbits.PEIE=1; 
    INTCONbits.IPEN = 1;    // Priority levels enable bit
    */
    //then timer related commands
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

/*
//first color click related functions
void __interrupt(high_priority) HighISR()
{
	//trigger flag that indicates a card has been identified in front of the sensor
    if(PIR0bits.INT1IF == 1 && response_in_progress == 0) {// interrupt source is clickerboard bit corresponding to the colorclick interrupt pin, and the buggy is not currently responding to an interrupt just triggered
        card_detected = 1; //defined as global variable in main.c
        LATDbits.LATD7=1;
        __delay_ms(500);
        LATDbits.LATD7=0;
        __delay_ms(500);
        clear_interrupt_flag();
        PIR0bits.INT1IF = 0; //need to clear clickerboard interrupt pin separately
    }
}

void enable_color_interrupt(void){
    clear_interrupt_flag(); //make sure interrupt flag is cleared to start with
	color_writetoaddr(0x00, 0b10011); //address for ENABLE bit is 0x00 and we want to turn it on, and enable interrupts
}

void set_interrupt_threshold(unsigned int AILT, unsigned int AIHT, unsigned int persistence){
    
    color_writetoaddr(0x0C, persistence); // set the persistence filter
    color_writetoaddr(0x05, AILT >> 8); // set most significant bit of the low threshold register
    color_writetoaddr(0x04, AILT && 0b0000000011111111); // set least significant bit of the low threshold register
    color_writetoaddr(0x07, AIHT >> 8); // set most significant bit of the high threshold register
    color_writetoaddr(0x06, AIHT && 0b0000000011111111); // set least significant bit of the high threshold register
}

void clear_interrupt_flag(void){
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00); // write mode
    I2C_2_Master_Write(0b11100110);  // Command + Register address  
    I2C_2_Master_Stop();             // Send stop bit
    set_interrupt_threshold(1, 2000, 0b0100);
}
*/
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
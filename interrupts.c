#include <xc.h>
#include <math.h>
#include "interrupts.h"
#include "color.h"
#include "i2c.h"

void Interrupts_init(void)
{
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
    INTCONbits.GIEL = 1; // Peripheral Interrupt Enable bit
    INTCONbits.GIEH=1;} 	//turn on interrupts globally (when this is off, all interrupts are deactivated)

void __interrupt(high_priority) HighISR()
{
	//trigger flag that indicates a card has been identified in front of the sensor
    if(PIR0bits.INT1IF == 1&& response_in_progress == 0) {// interrupt source is clickerboard bit corresponding to the colorclick interrupt pin, and the buggy is not currently responding to an interrupt just triggered
        card_detected = 1; //defined as global variable in main.c
        LATDbits.LATD7=1;
        __delay_ms(50);
        LATDbits.LATD7=0;
        __delay_ms(50);
        clear_interrupt_flag();
        PIR0bits.INT1IF = 0; //need to clear clickerboard interrupt pin separately
    //undecided on how to define flag that indicates card has been detected
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
}
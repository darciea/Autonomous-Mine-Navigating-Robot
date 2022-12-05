#include <xc.h>
#include <math.h>
#include "interrupts.h"
#include "color.h"
#include "i2c.h"

void Interrupts_init(void)
{
    enable_color_interrupt(); //enable interrupts from color click board
    set_interrupt_threshold(5, 5000, 0b0100); //set low and high threshold and persistence filter
    TRISBbits.TRISB1 = 1; //interrupt pin on the clickerboard
    ANSELBbits.ANSELB1 = 0; //disable analogue input
    INT1PPS=0x09; // which PPS register needs to be open for interrupt
    PIE0bits.INT1IE = 1; //which peripheral interrupt is enabled
    IPR0bits.INT1IP = 1; //high priority (HighISR)
    INTCONbits.INT1EDG = 0; //rising/falling edge
    INTCONbits.IPEN = 1;    // Priority levels enable bit
    INTCONbits.GIEL = 1; // Peripheral Interrupt Enable bit
    INTCONbits.GIEH=1;} 	//turn on interrupts globally (when this is off, all interrupts are deactivated)

void __interrupt(high_priority) HighISR()
{
	//trigger flag that indicates a card has been identified in front of the sensor
    if(PIR0bits.INT1IF == 1) {			// interrupt source is clickerboard bit corresponding to the colorclick interrupt pin
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
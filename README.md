# Darciea + Matti's Final Project

## Explanation of our work

The program contains a calibration routine where it fills an 4 by 9 array with the expected readings for each colour sensor (RGBC) for each colour card. Prompted by a button press, the buggy collects 3 data values for each colour sensor and calculates the average for each. The coloured cards follow the calibration sequence: red, green, blue, yellow, pink, orange, light blue, white and black, and the RF2 button input is used to trigger the next card being read. After all of the cards is read there is one background reading of the clear sensor; this is used to determine the threshold value above which the card-reading sequence is initialised. 

To start the program within the maze, the RF2 button should be pressed when the buggy is in the appropriate position and is ready to start the program. Once the button is pressed, the buggy will move forward, recording the time taken, continuously reading the clear sensor reading until the clear sensor reads a reading above a certain threshold. This triggers the sequence of commands that stores the time taken to reach the card, reads the card in front of the buggy and responds appropriately, and then storing the card read in the aforementioned array. The buggy then repeats the steps until the white card is read. Then it enters the ReturnHome sequence, whithin which it turns around and iterates through the array, moving forward for the time stored, and executing the opposite commands to those corresponding to the stored cards.

### Colour detection and response methodology

To start, variables are created for each of the colour sensors, named red_read, green_read and blue_read, and these are often used to temporarily store the readings of the ColorClick before they are stored elsewhere. An array called expected_values is also created, and this is filled up with the values obtained from each of the colour sensors during the calibration sequence. For example, the first column of the array contains the expected red, green and blue readings for a red card. Another two arrays, ReturnHomeCards and ReturnHomeTimes track the sequence of cards the buggy has encountered, as well as the times between each card respectively.

The colour detection and response is all contained within the function card_response. This function contains multiple functions that carries out each step of the colour detection process. 
1. collect_avg_readings(red_read, green_read, blue_read)

	This function reads 3 readings and calculates the average for each colour sensor and returns it back to the main. 

2. normalise_readings(red_read, green_read, blue_read, expected_values, normalised_values)

	This function takes the collected readings, finds the difference between these readings and the expected for each colour of each possible card, divides by the expected value and stores it in the array normalised_values. This allows us to find the difference between the current unknown card and what we know for all of the colour cards, for each colour sensor.

3. make_master_closeness(normalised_values, master_closeness)

	This function takes the normalised_values for each colour card and finds the average difference across all the colour sensors for each colour card and puts in into the array master_closeness. This allows us to see the average difference from our current unknown card to each colourcard.

4. card = determine_card(master_closeness)

	This function checks through the master_closeness array to find which colourcard has the smallest difference from our collected values and returns that colour, which is the predicted colour of the card.

5. motor_response(card, mL, mR)

	This function takes the predicted colour and carries out the appropriate response as stated in the project brief. 


### Returning home methodology

The main function introduces a union variable HomeStored, that contains two arrays, one that holds the time counted by the timer interrupt between cards, and the other that holds the cards seen on the journey of the buggy. 
A timer is written that overflows every 1/10 second, and the number of times it overflows is counted in the main (where the while loop iterates constantly and therefore is incrementing the timercount every time it detects an overflow), until the buggy detects a card in front of it, therefore counting the time that the buggy moved forward. The buggy then responds to the colour of the card and records the card in the array. As the buggy moves through the maze, both arrays are filled with the time taken to get to each card, and what card it saw. 
Once the buggy detects a white card, this initiates the return home sequence. The buggy turns around 180 degrees and iterates through the array backwards (skipping past any empty elements). The buggy starts moving forward for the time dictated in the last filled element of the array, and when it stops it carries out the appropriate home response function (i.e. the inverse of the original commands). It then moves to the penultimate element, moves forward for that time and repeats this process until the buggy returns to the start.

## Key Program Files

### Main.c
In the main, the initiation functions are called, the DC motor is initialised, and key variables are created and it contains the calibration sequence that stores the expected values for each colour card. The main also takes in the flags from the timer interrupt and the colour click interrupt, and these flags determine whether the card_response function is called.

### i2c.c/h
Here are the key functions required for the I2C communication.

### Color.c/h
This file contains the I2C communication necessary to receive values from the RGBC sensors. 

### LEDsOn.c/h
This file initiates all the lights on the buggy, namely the car lights(e.g. brake lights and headlamps), and the colour click tri-color illumination.

### dc_motor.c/h
Here the DC motor functions for basic movements (forward, backwards, left and right) are defined, with the turns done in 45 degree increments.

### colour_identify.c/h
This file contains all the functions detailed in the colour detection and response methodology section, i.e. collect_avg_readings, normalise_readings, make_master_closeness, determine_card, motor_response, and the overarching function, card_response. It also contains the function home_response which contains the appropriate actions to return home.

### serial.c/h
This file contains the necessary functions to utilise the serial communication to view the colour sensor readings and for debugging purposes.


# Course project - Mine navigation search and rescue

## Challenge brief

Your task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## "Mine" environment specification

A "mine" is contstructed from black plywood walls 100mm high with some walls having coloured cards located on the sides of the maze to assist with navigation. The following colour code is to be used for navigation:

Colour | Instruction
---------|---------
Red | Turn Right 90
Green | Turn Left 90
Blue | Turn 180
Yellow | Reverse 1 square and turn right 90
Pink | Reverse 1 square and turn left 90
Orange | Turn Right 135
Light blue | Turn Left 135 
White | Finish (return home)
Black | Maze wall colour

Mine courses will vary in difficulty, with the simplest requiring 4 basic moves to navigate. More advanced courses may require 10 or moves to navigate. The mines may have features such as dead ends but colour cards will always direct you to the end of the maze. Once the end of the maze has been reached, you must return to the starting position. An example course to navigate is shown below. You do not know in advance which colours will be in the course or how many.

![Navi Diagram](gifs/maze.gif)

## Resources and project submission

To develop your solution you have your Clicker 2 board, buggy and colour click add on board. You must not use any hardware that is not provided as part of this course and you must use the XC8 compiler to compile your C code. 

Please use this GitHub repo to manage your software development and submit your project code. 

Final testing will take place in the CAGB foyer and testing areas will be provided around the mechatronics lab. You will not know the exact layout of the maze(s) in advance. You will also be asked to give a short presentation on the testing day.

## Supplementary technical information

### Additional buggy features

In addition to the motor drives we explored in lab 6, the buggy contains some additional features that may be of use during the project. The first feature is additional LEDs, controlled through the pins labelled **H.LAMPS**, **M.BEAM**, **BRAKE**, **TURN-L** and **TURN-R**. H.LAMPS turns on the front white LEDs and rear red LEDs, at a reduced brightness. M.BEAM and BRAKE enable you to turn these LEDs on at full brightness. The turn signals have not hardware based brightness control. These LEDs give you a method to provide feedback for debugging, in addition of the LEDs on the clicker board.

![Buggy pinout](gifs/buggy_pins.png)

A further feature of the buggy is **BAT-VSENSE** pin which allows you to monitor the batter voltage via an analogue input pin. The battery is connected across a voltage divider, as shown in the diagram below:

![Bat sense](gifs/bat_vsense.png)

The voltage at BAT-VSENSE will always be one third of that at the battery. Measuring this value can be useful for determining if your battery needs charging. You could also use it to calibrate your robot to perform well at different charge levels. 

### Colour click

The Colour click board contains 2 devices to help with navigation, a tri-colour LED for illumination and a 4 channel RGBC photodiode sensor. This combination of two devices (an illumination source and a sensor) allow you a make measurements of the reflected colour of objects near the sensor. The circuit diagram for the Colour click is shown below:

![Color Cick](gifs/color_click.png)

The tri-colour LED is the simpler of the two devices to control. Three separate pins control the red, green and blue LEDs individually, despite them being in a single package. Control via these pins is simple digital on/off control and if any brightness control was required, the user would need program the microcontroller to generate a PWM signal to achieve this.  

The second device on the Colour click is the TCS3471 colour light-to-digital converter. The sensor contains a grid of 4x4 photodiodes, 4 are sensitive to red light, 4 green light, 4 blue light and 4 "clear" light (that is, a range of wavelengths, see datasheet for exact spectral response). When light falls on the photodiode the photons are absorbed and current is generated. This signal is then integrated over time using integrators and sampled by 16 bit on board ADCs. Communication with the device is achieved using an I2C interface. This enables configuration of the device to customise sampling of the data (i.e. integration time, gain, etc.) and to read the 16 bit digital values for each of the RGBC channels. The relative magnitude of these values gives you information about the colour of light that is falling on the sensor. The device can also be configured to send an interrupt signal to the PIC when signal reaches a preset value.

### I2C

The I2C interface widely used in industry for communication between microcontrollers and peripheral integrated circuits (other chips) over short distances. I2C is serial communication bus that enables communication between many devices over a simple 2 wire interface. One wire is the data line (SDA) and is used for both transmission and receiving. The second wire (SCL) is used for a clock signal to ensure all devices are synchronous. To ensure communication of data occurs without problem a message protocol must be followed and understood by all devices on the bus. Devices are termed master and slave devices, with master devices initiation communication to a slave device via unique address for that device. The general sequence of communication between a master/slave over the I2C interface is a follows:

1. Send a Start bit
1. Send the slave address, usually 7 bits
1. Send a Read (1) or Write (0) bit to define what type of transaction it is
1. Wait for an Acknowledge bit
1. Send a data or command byte (8 bits)
1. Wait for Acknowledge bit
1. Send the Stop bit

This is shown pictorial in the figure below:

![I2C](gifs/i2c.png)

Although it is possible to program an entirely software based I2C interface, luckily for us our PIC chip has a module dedicated to generating and receiving I2C signals: the Master Synchronous Serial Port Module, or MSSP (see chapter 28 of the PIC datasheet). This module provides methods for sending start/stop/acknowledge bits and allows us to focus on sending/receiving data.

The included i2c.c/h files contain functions to help you get started with I2C communication. The first function below sets up the MSSP module as an I2C master device and configures the necessary pins.

	void I2C_2_Master_Init(void)
	{
		//i2c config  
		SSP2CON1bits.SSPM= 0b1000;    // i2c master mode
		SSP2CON1bits.SSPEN = 1;       //enable i2c
		SSP2ADD = (_XTAL_FREQ/(4*_I2C_CLOCK))-1; //Baud rate divider bits (in master mode)
  
		//pin configuration for i2c  
		TRISDbits.TRISD5 = 1;                   //Disable output driver
		TRISDbits.TRISD6 = 1;                   //Disable output driver
		ANSELDbits.ANSELD5=0;					// disable analogue on pins
		ANSELDbits.ANSELD6=0;					// disable analogue on pins
		SSP2DATPPS=0x1D;      //pin RD5
		SSP2CLKPPS=0x1E;      //pin RD6
		RD5PPS=0x1C;      // data output
		RD6PPS=0x1B;      //clock output
	}
	
Bits with the SSP2CON2 register are set to send the individual start/stop/acknowledge bits used in the protocol. These must only be set when the bus is idle (nothing being sent/received). The I2C_2_Master_Start(), I2C_2_Master_Stop() and I2C_2_Master_RepStart() functions allow you add the necessary bits as defined in the protocol above. Data is sent on the bus using the SSP2BUF register:

	void I2C_2_Master_Write(unsigned char data_byte)
	{
		I2C_2_Master_Idle();
		SSP2BUF = data_byte;         //Write data to SSPBUF
	}

Data is also read using the same SSP2BUF registers. However, to receive data we first need to switch the module into receiver mode. We also need to start the acknowledge sequence to let the slave device know what we have received the data OK. The following function achieves this:

	unsigned char I2C_2_Master_Read(unsigned char ack)
	{
		unsigned char tmp;
		I2C_2_Master_Idle();
		SSP2CON2bits.RCEN = 1;        // put the module into receive mode
		I2C_2_Master_Idle();
		tmp = SSP2BUF;                //Read data from SS2PBUF
		I2C_2_Master_Idle();
		SSP2CON2bits.ACKDT = !ack;     // 0 turns on acknowledge data bit
		SSP2CON2bits.ACKEN = 1;        //start acknowledge sequence
		return tmp;
	}

The functions described so form the basics required for I2C communication with the PIC. To communicate with the TCS3471 onboard the Colour click we first need to know its address. This is listed in the data sheet as 0x29. To send our first byte over the I2C we need to send this address combined with the transaction type (read or write) as defined in the protocol diagram above. This lets the TCS3471 know the message is intended for it and not some other device on the interface. Next we send a byte which is a combination of command type and the register address in the TCS3471 that we want to write to. Finally we the value that we want to write to that register. This sequence is shown in the function below:

	void color_writetoaddr(char address, char value){
		I2C_2_Master_Start();         		//Start condition
		I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write (0) mode (note 0x52=0x29<<1)
		I2C_2_Master_Write(0x80 | address);    //command + register address
		I2C_2_Master_Write(value);    			//value to store in the register
		I2C_2_Master_Stop();          //Stop condition
	}

We then call the function to, for example, turn the device on:

	color_writetoaddr(0x00, 0x01); // write 1 to the PON bit in the device enable register
	
There are additional commands that must be set to initialise the device and many registers that be configured to obtain optimal performance for the sensor in your conditions. It is up to you to carefully read the TCS3471 datasheet and experiment with this.

To read values from the TCS3471 you need to a similar sequence to above but you first need to tell the device which register you want to deal with, before telling the device you want read from it. The example below uses the combined read format to read multiple bytes in sequence. The auto-increment is set so that instead of reading the same register over and over again, it automatically advances to the next one. The example starts at the Red channel low byte address and then automatically advances and reads the associated high byte.

	unsigned int color_read_Red(void)
	{
		unsigned int tmp;
		I2C_2_Master_Start();         //Start condition
		I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
		I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
		I2C_2_Master_RepStart();
		I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
		tmp=I2C_2_Master_Read(1);			// read the Red LSB
		tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
		I2C_2_Master_Stop();          //Stop condition
		return tmp;
	}

Instead of separate functions for each channel you may want to create a structure to store all the values together, and pass a pointer to the function so that all values in the structure can be updated in one operation. An example structure might look like this:

	//definition of RGB structure
	struct RGB_val { 
		unsigned int R;
		unsigned int G;
		unsigned int B;
	};

This concludes the basics of I2C and communication with the colour sensor. Best of luck! 

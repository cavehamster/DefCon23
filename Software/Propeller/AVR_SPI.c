/*
 
 This is a brief example of how to communicate with the AVR to get button status
 
 Basically, the propeller is in master mode.  It sends a command to the AVR to request status, and 
 then reads in 12 bytes of data that represent the current status of the hardware.
 
 You'll want to put this in a loop somewhere in your code to poll the hardware for updates.
 
 This is a simple bit-banging SPI interface
 
 To request data, you send the the AVR an 0x52.  It responds with 12 bytes, as such:
 
 D0 -  Top 4 bits are button states, low = pressed:

	  Left button			bit 0
	  Right button			bit 1
	  Left joy button		bit 2
	  Right joy button	bit 3
   
      Bottom 4 bits are likely the charging status: bit 0 is 'charged' and bit 1 is 'charging'.  Just guessing.  Unsure
      if bits 2 and 3 report anything.

 D1 - 0-255, status of the rotary encoder dial

 D2 - D5 -  Location of the left stick, left-right.  
            D2-D3 and D4-D5 seem to be the same.  It's a 10 bit value, 0 to 1023.
			     Zero is all the way left, 512 is the center, and 1023 is full right.

 D6 - D7 -	Location of the right stick up-down.  10 bit value
			     All up is zero, center is 512, and down is 1023

 D8 - D9	-	Always 03 FF

 D10 - D11 - Reading something, toggling around 03 64
 
 NOTE that the joysticks are very sensitive near the center and don't change much at full extension!
 
 Also note that the up-down of the left stick and the left-right of the right stick are not being sent I guess.
 
 -hamster
 
*/

#include "simpletools.h"
#include "AVR_SPI.h"       

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif           


// Structure to hold the pin configuration
typedef struct{
  uint8_t CS;
  uint8_t CLK;
  uint8_t MISO;
  uint8_t MOSI;
} PinStruct;

PinStruct pins;
volatile Inputs inputs;
int *avrISP_cog = 0;

// Initialize and start the cog
int avrSPI_Init(uint8_t CS, uint8_t CLK, uint8_t MISO, uint8_t MOSI){
  pins.CS = CS;
  pins.CLK = CLK;
  pins.MISO = MISO;
  pins.MOSI = MOSI;
  
  inputs.rotary = 0;
  
  avrISP_cog = cog_run(avrISP_Run, 128);
}    

// Stop the cog
void avrSPI_Stop(){
  cog_end(avrISP_cog); 
}  

// The cog.
void avrISP_Run(){
  
  unsigned int value[12];
  
  // Chip Select is asserted when it is low, so we make sure to put it high to start with
  high(pins.CS);
  // Set the Clock pin low
  low(pins.CLK);
  
  while(1){
    low(pins.CS);  // Assert the CS
    
    // Ask the AVR for data
    // These first two is what the original code was asking for, but I'm not sure what it does.
    // Just need the third one to get the data
    //shift_out(MOSI, CLK, MSBFIRST, 8, 0b01010111);
    //shift_out(MOSI, CLK, MSBFIRST, 8, 0b00000000);
    shift_out(pins.MOSI, pins.CLK, MSBFIRST, 8, 0x52);
    
    // Save the returned data into an array
    for(int i = 0; i < 12; i++){
      value[i] = shift_in(pins.MISO, pins.CLK, MSBPRE, 8);
    }      
    
    high(pins.CS); // De-assert the CS    
    
    // Get the data and stick it into the data struct
    // Buttons are 'corrected' so a high means the button is down
    inputs.leftButton = !((value[0] >> 4) & 0x01);
    inputs.rightButton = !((value[0] >> 5) & 0x01);
    inputs.leftJoystickButton = !((value[0] >> 6) & 0x01);
    inputs.rightJoystickButton = !((value[0] >> 7) & 0x01);
    
    inputs.charged = !((value[0] >> 1) & 0x01);
    inputs.charging = !(value[0] & 0x01);
    
    inputs.rotary = value[1];
    inputs.LRStick = (value[2] << 8) | value[3];
    inputs.UDStick = (value[6] << 8) | value[7];
    inputs.chargeLevel = (value[10] << 8) | value[11];

    // Pause for a bit
    pause(20);
  }    
 
}

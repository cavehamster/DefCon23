/*
 
 This is a brief example of how to communicate with the AVR to get button status
 
 Basically, the propeller is in master mode.  It sends a command to the AVR to request status, and 
 then reads in 12 bytes of data that represent the current status of the hardware.
 
 You'll want to put this in a loop somewhere in your code to poll the hardware for updates.
 
 This is a simple bit-banging SPI interface
 
 To request data, you send the the AVR a single byte 'R' (0x52).  It responds with 12 bytes, as such:
 
 D0 -  Binary state data:
    
    BattStat1         bit 0     
    BattStat2         bit 1
    Unused (0)        bit 2
    Unused (1)        bit 3
    Button A (left)	  bit 4
    Button B (right)  bit 5
    Left joy button	  bit 6
    Right joy button  bit 7

 D1 - 0-255, status of the rotary encoder dial, 4 counts per click

 D2 - D5 -  Location of the left stick.  Centered is about 512
 
    Left Stick X Axis D2-D3 10 bits (Seems to be the same as the Y axis)
    Left Stick Y Axis D4-D5 10 bits

 D6 - D9 -  Location of the right stick.  Centered is about 512
 
    Right Stick X Axis D2-D3 10 bits
    Right Stick Y Axis D4-D5 10 bits  (Seems to be a random value instead of a position)

 D10 - D11 - Battery voltage, 10 bits
 
 NOTE that the joysticks are very sensitive near the center and don't change much at full extension!
 
 To set the backlight, you send the AVR a single byte 'W' (0x57) and a second byte with the backlight level 0-255
 
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
volatile uint8_t LCD_Backlight;
int *avrISP_cog = 0;

// Initialize and start the cog
int avrSPI_Init(uint8_t CS, uint8_t CLK, uint8_t MISO, uint8_t MOSI){
  pins.CS = CS;
  pins.CLK = CLK;
  pins.MISO = MISO;
  pins.MOSI = MOSI;
  
  inputs.rotary = 0;
  
  avrISP_cog = cog_run((void *)avrISP_Run, 128);
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
    
    // Set the LCD backlight level
    shift_out(pins.MOSI, pins.CLK, MSBFIRST, 8, 0x57);  // 'W'
    shift_out(pins.MOSI, pins.CLK, MSBFIRST, 8, LCD_Backlight);
    
    // Ask the AVR for data 
    shift_out(pins.MOSI, pins.CLK, MSBFIRST, 8, 0x52);  // 'R'
    
    // Save the returned data into an array
    for(int i = 0; i < 12; i++){
      value[i] = shift_in(pins.MISO, pins.CLK, MSBPRE, 8);
    }      
    
    high(pins.CS); // De-assert the CS    
    
    // Get the data and stick it into the data struct
    // Buttons are 'corrected' so a high means the button is down
    inputs.buttonA = !((value[0] >> 4) & 0x01);
    inputs.buttonB = !((value[0] >> 5) & 0x01);
    inputs.leftStick.button = !((value[0] >> 6) & 0x01);
    inputs.rightStick.button = !((value[0] >> 7) & 0x01);
    
    inputs.battStat2 = !((value[0] >> 1) & 0x01);
    inputs.battStat1 = !(value[0] & 0x01);
    inputs.battVolts = (value[10] << 8) | value[11];
    
    inputs.rotary = value[1];
    
    inputs.leftStick.X = (value[2] << 8) | value[3];
    inputs.leftStick.Y = (value[4] << 8) | value[5];
    inputs.rightStick.X = (value[6] << 8) | value[7];
    inputs.rightStick.Y = (value[8] << 8) | value[9];
    

    // Pause for a bit
    pause(20);
  }    
 
}

/*
  This is some basic code that will run your Defcon 23 DC801 badge.  This is not the original firmware,
  but a firmware that works about the same that was reverse engineered from what the badge was already kind of doing.
  
  Basically, there are two microcontrollers on your badge: a Parallax Propeller, and an Atmel AVR.  They handle
  different hardware, and communicate over an SPI interface.  The Propeller is the master, and the AVR is the slave.
  
  The AVR handles analog inputs: Joystick positions, button presses, encoder rotations, and the battery status.
  
  The Propeller handles the inputs from the wifi, xbee, display and NeoPixel LEDS.
  
  Enjoy!
  -hamster
  
*/
#include "simpletools.h"
#include "AVR_SPI.h"
#include "LED.h"

#define AVR_SPI_CS    19  // Chip Select
#define AVR_SPI_CLK   16  // Data Clock
#define AVR_SPI_MISO  17  // Master in, Slave out
#define AVR_SPI_MOSI  18  // Master out, Slave in

int main(){
  
  Inputs oldInputs;
  
  // Initialize the cogs
  avrSPI_Init(AVR_SPI_CS, AVR_SPI_CLK, AVR_SPI_MISO, AVR_SPI_MOSI);
  LED_Init();
 
  // Give things a tick to get the vars primed
  pause(100);
 
  while(1){
    
    // Print the data out to the terminal
    // This takes a while, so it might not update as quickly as you can smack the keys
    // You'll probably want to comment this out at some point.
    print("%c", HOME);
    print("Buttons - l:%d r:%d lj:%d rj:%d\n",  inputs.leftButton, inputs.rightButton,
              inputs.leftJoystickButton, inputs.rightJoystickButton); 
    print("Charging? %d Charged? %d Level 0x%x\n", inputs.charging, inputs.charged, inputs.chargeLevel);
    print("Rotary 0x%x UDJoy %x LRJoy %x\n",  inputs.rotary, inputs.UDStick, inputs.LRStick);
    print("LED Pattern %d", LED_Pattern);
    print("%c", CLREOL);
    
    pause(500);
    
  }  
}

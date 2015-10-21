/*
  This is some basic code that will run your Defcon 23 DC801 badge.  This is not the original firmware,
  but a firmware that works about the same that was reverse engineered from what the badge was already kind of doing.
  
  Basically, there are two microcontrollers on your badge: a Parallax Propeller, and an Atmel AVR.  They handle
  different hardware, and communicate over an SPI interface.  The Propeller is the master, and the AVR is the slave.
  
  The AVR handles analog inputs: Joystick positions, button presses, encoder rotations, and the battery status.
  
  The Propeller handles the inputs from the wifi, xbee, display, SD card and NeoPixel LEDS.
  
  Enjoy!
  -hamster
  
*/
#include "simpletools.h"
#include "AVR_SPI.h"
#include "LED.h"
#include "LCD.h"       

int main(){
    
  uint8_t byte = 0;
  
  // Set the LCD backlight to the middle level
  LCD_Backlight = 64;
  
  // Initialize the cogs
  avrSPI_Init(AVR_SPI_CS, AVR_SPI_CLK, AVR_SPI_MISO, AVR_SPI_MOSI);
  LED_Init(LED_PIN, LED_COUNT);
  LCD_Start();
 
  // Give things a tick to get the vars primed
  pause(100);
  
  while(1){
    
    // Print the data out to the terminal
    // This takes a while, so it might not update as quickly as you can smack the keys
    // You'll probably want to comment this out at some point.
    
    print("%c", HOME);
    print("ButtonA:%d ButtonB:%d Rotary:%d\n",  inputs.buttonA, inputs.buttonB, inputs.rotary); 
    print("BattStat1:%d BattStat2:%d Volts %d\n", inputs.battStat1, inputs.battStat2, inputs.battVolts);
    print("LeftStick X:%d Y:%d B:%d\n", inputs.leftStick.X, inputs.leftStick.Y, inputs.leftStick.button);
    print("RightStick X:%d Y:%d B:%d\n", inputs.rightStick.X, inputs.rightStick.Y, inputs.rightStick.button);
    print("LED Pattern %d", LED_Pattern);
    print("%c", CLREOL);    
    
    pause(500);
    
  }  
}

// Written by Compukidmike July 2015
// Parts of code by Nick Gammon April 2011 http://www.gammon.com.au/forum/?id=10892
//
// SPI Protocol:
// Arduino is in slave mode, master device must request data
//
// Send 'R' for Read, arduino will reply with 12 bytes...
// Byte0: Button status - MSB: RightJoystickButton, LeftJoystickButton,ButtonB,ButtonA,0,0,BatteryStat2,BatteryStat1
// Byte1: Encoder - starts at 128, 4 counts per click, master must account for rollover
// Byte2-3: Left Joystick X Axis - Sent as 0-1023
// Byte4-5: Left Joystick Y Axis - Sent as 0-1023
// Byte6-7: Right Joystick X Axis - Sent as 0-1023
// Byte8-9: Right Joystick Y Axis - Sent as 0-1023
// Byte10-11: Battery Voltage - Sent as 0-1023
//
// Send 'W' for Write, send 1 byte...
// Byte0: Backlight value 0-255




#include "Encoder.h"
#include "TimerOne.h"

const int buttonAPin = 4;
const int buttonBPin = 5;
const int joyLeftButtonPin = 6;
const int joyRightButtonPin = 7;
const int stat1Pin = 8;
const int stat2Pin = 9;
const int backlightPin = A4;
const int joyLeftXPin = A0;
const int joyLeftYPin = A1;
const int joyRightXPin = A2;
const int joyRightYPin = A3;
const int batteryPin = A7;

//volatile byte incomingByte;
 byte command = 0;
 byte byteCounter = 0;
 byte encoder = 0;
 byte backlight = 0;
 byte buttonStatus = 0;
volatile int joyLeftX = 0;
volatile int joyLeftY = 0;
volatile int joyRightX = 0;
volatile int joyRightY = 0;
int timerCounter = 0;
int battery = 0;


Encoder myEnc(2, 3);

void setup (void)
{
  //Serial.begin(115200);
  // set up pin modes
  pinMode(buttonAPin, INPUT_PULLUP);
  pinMode(buttonBPin, INPUT_PULLUP);
  pinMode(joyLeftButtonPin, INPUT_PULLUP);
  pinMode(joyRightButtonPin, INPUT_PULLUP);
  pinMode(stat1Pin, INPUT_PULLUP);
  pinMode(stat2Pin, INPUT_PULLUP);
  //pinMode(batteryPin, INPUT);
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH); //Turns off backlight

  Timer1.initialize(20); //Fire timer every 50uS
  Timer1.attachInterrupt(UpdateBacklight);

  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{

  byte incomingByte = SPDR;
  //if(incomingByte == 'R') command = 'R';
  //if(incomingByte == 'W') command = 'W';
  
  switch (command)
  {
    // no command? then this is the command byte
  case 0:
    command = incomingByte;
    SPDR = 0;
    byteCounter = 0;
    if(command == 'R'){
      SPDR = buttonStatus;
      //byteCounter = 1;
    }
  break;

    // Read command, send back 12 bytes
  case 'R':
    if(byteCounter == 0) {
      SPDR = buttonStatus;
    }
    if(byteCounter == 1) {
      SPDR = encoder; 
    }
    if(byteCounter == 2) {
      SPDR = joyLeftX >> 8; 
    }
    if(byteCounter == 3){
      SPDR = joyLeftX;
    }
    if(byteCounter == 4) {
      SPDR = joyLeftY >> 8;
    }
    if(byteCounter == 5) {
      SPDR = joyLeftY; 
    }
    if(byteCounter == 6) {
      SPDR = joyRightX >> 8;
    }
    if(byteCounter == 7) {
      SPDR = joyRightX; 
    }
    if(byteCounter == 8) {
      SPDR = joyRightY >> 8;
    }
    if(byteCounter == 9) {
      SPDR = joyRightY;
    }
    if(byteCounter == 10) {
      SPDR = battery >> 8;
    }
    if(byteCounter == 11) {
      SPDR = battery;
      byteCounter = 0;
      command = 0;
    }
  break;

    // Write command, read one more byte for backlight value
  case 'W': 
    if(byteCounter == 0) {
      SPDR = 0;
    }
    if(byteCounter == 1) {
      backlight = incomingByte;
      SPDR = 1;
      byteCounter = 0;
      command=0;
    }
  break;
  
  } // end of switch
  byteCounter++;
//SPDR = byteCounter;
}  // end of interrupt service routine (ISR) SPI_STC_vect


void UpdateBacklight (void) //Called by timer every 50uS
{
  timerCounter++; //increment counter
  if(timerCounter > 255){ //rollover to zero at 255
    timerCounter = 0;
    PORTC &= B11101111; //turn backlight on (inverted)
  }
  if(timerCounter == backlight) PORTC |= B00010000; //turn backlight off when its value is reached
}

void loop (void)
{
  // if SPI not active, clear current command
  if (digitalRead(10) == HIGH){ //Should change to direct port access
    //command = 0;
    //byteCounter=0; 
  }

  //buttonStatus = 0;
  buttonStatus = (PIND & B11110000);
  buttonStatus |= (PINB & B00000011); //Update status of all buttons
  //Serial.print(PIND, BIN);
  //Serial.print(" ");
  //Serial.print(PINB, BIN);
  //Serial.print(" ");
  //Serial.println(buttonStatus, BIN);
  joyLeftX = analogRead(joyLeftXPin);
  joyLeftY = analogRead(joyLeftYPin);
  joyRightX = analogRead(joyRightXPin);
  joyRightY = analogRead(joyRightYPin);
  battery = analogRead(batteryPin);

  long encoderPosition = myEnc.read(); //Read encoder value
  if(encoderPosition > 127){ //Handle rollover to fit value in one byte
    encoderPosition -= 255;
    myEnc.write(encoderPosition);
  }
  if(encoderPosition < -127){ //Handle rollover to fit value in one byte
    encoderPosition += 255;
    myEnc.write(encoderPosition);
  }
  
  encoder = encoderPosition + 128; //add 128 for conversion to unsigned byte
  //Serial.print(encoderPosition);
  //Serial.print(" ");
  //Serial.println(encoder);
}  // end of loop


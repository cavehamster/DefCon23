/*
 *  This is the header file for the SPI data from the AVR
 *
 */

typedef struct{
  uint16_t  X : 10;
  uint16_t  Y : 10;
  uint8_t   button : 1;
} JoyStick;  

typedef struct{
  JoyStick leftStick;
  JoyStick rightStick;

  uint8_t buttonA : 1;
  uint8_t buttonB : 1;
  uint8_t battStat1 : 1;
  uint8_t battStat2 : 1;

  uint8_t rotary;
  uint16_t battVolts;
} Inputs;

extern volatile Inputs inputs;  
extern volatile uint8_t LCD_Backlight;

int avrSPI_Init(uint8_t CS, uint8_t CLK, uint8_t MISO, uint8_t MOSI);
void avrSPI_Stop(void);
void avrISP_Run(void);
      

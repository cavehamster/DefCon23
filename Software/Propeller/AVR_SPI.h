/*
 *  This is the header file for the SPI data from the AVR
 *
 */

#define AVR_SPI_CS    19  // Chip Select
#define AVR_SPI_CLK   16  // Data Clock
#define AVR_SPI_MISO  17  // Master in, Slave out
#define AVR_SPI_MOSI  18  // Master out, Slave in

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
      

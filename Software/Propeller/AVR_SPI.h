/*
 *  This is the header file for the SPI data from the AVR
 *
 */

typedef struct{
  uint8_t leftJoystickButton : 1;
  uint8_t rightJoystickButton : 1;
  uint8_t leftButton : 1;
  uint8_t rightButton : 1;
  uint8_t charging : 1;
  uint8_t charged : 1;
  uint8_t rotary;
  uint16_t LRStick;
  uint16_t UDStick;
  uint16_t chargeLevel;
} Inputs;

extern volatile Inputs inputs;  

int avrSPI_Init(uint8_t CS, uint8_t CLK, uint8_t MISO, uint8_t MOSI);
void avrSPI_Stop(void);
void avrISP_Run(void);
      

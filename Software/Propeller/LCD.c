/*
 
  Driver interface for the R61509 interface
  
  This driver is connected in so-called 8 bit mode
 
  This means that data is transferred to it over an 8 bit bus
  There are a couple of control pins used to tell the display what it should be doing:
  
  Pin descriptions:
  
  LCD_CS - Chip selection.  Low = selected
  LCD_RS - Register selection.  Low - index register, high - control register
  LCD_WR - Write strobe.  Low - write data
  LCD_RD - Read strobe.  Low - read data
  LCD_RST - Reset when low
 
  Register selection:
  
  WR  RD  RS  Function
  0   1   0   Write index to the IR (Instruction Register)
  1   0   0   Setting disabled
  0   1   1   Write to control register or internal GRAM via WDR (write data register)
  1   0   1   Read from internal GRAM and register via RDR (read data register)
  
  -hamster
  
 */

#include "simpletools.h"
#include "LCD.h"
#include "sdcard.h"

int *LCD_cog = 0;

void LCD_Start(void){
  LCD_cog = cog_run((void *)LCD_Run, 512);
}                      

/**
 *  Initialize the LCD interface.  The LCD uses negative logic, so we need
 *  to set various pins high.
 *  
 *  TODO document the config options better and optimize
 */
void LCD_Init(void){

  // Set pins as output
  set_direction(LCD_RST, 1);
  set_direction(LCD_CS, 1);
  set_direction(LCD_RS, 1);
  set_direction(LCD_WR, 1);
  set_direction(LCD_RD, 1);
  
  // Reset the display
  set_output(LCD_RST, 1);
  pause(5);
  set_output(LCD_RST, 0);
  pause(10);
  set_output(LCD_RST, 1);
  
  // Set the pins high
  set_output(LCD_CS, 1);
  set_output(LCD_RS, 1);
  set_output(LCD_WR, 1);
  set_output(LCD_RD, 1);
  
  set_directions(7, 0, 0xFF);
    
  LCD_SyncTransfer();
  
  // Soft reset
  LCD_Write(LCD_SOFTWARE_RESET,           0x0001);
  pause(1);
  LCD_Write(LCD_SOFTWARE_RESET,           0x0000);

  
  LCD_Write(LCD_BASE_IMAGE_NB_LINE,       0x6200);
  LCD_Write(LCD_DISPLAY_CONTROL_2,        0x0808);
  
  // Gamma correction
  LCD_Write(LCD_GAMMA_CONTROL_1,          0x0C00);
  LCD_Write(LCD_GAMMA_CONTROL_2,          0x5A0B);
  LCD_Write(LCD_GAMMA_CONTROL_3,          0x0906);
  LCD_Write(LCD_GAMMA_CONTROL_4,          0x1017);
  LCD_Write(LCD_GAMMA_CONTROL_5,          0x2300);
  LCD_Write(LCD_GAMMA_CONTROL_6,          0x1700);
  LCD_Write(LCD_GAMMA_CONTROL_7,          0x6309);
  LCD_Write(LCD_GAMMA_CONTROL_8,          0x0C09);
  LCD_Write(LCD_GAMMA_CONTROL_9,          0x100C);
  LCD_Write(LCD_GAMMA_CONTROL_10,         0x2232);
  
 /*  
  LCD_Write(LCD_PANEL_IF_CONTROL_1,       0x0016);    // Clocks per line, osc divisor
  LCD_Write(LCD_PANEL_IF_CONTROL_2,       0x0101);
  LCD_Write(LCD_PANEL_IF_CONTROL_3,       0x0000);
  LCD_Write(LCD_PANEL_IF_CONTROL_4,       0x0001);
 */
  
  // Power control
  LCD_Write(LCD_POWER_CONTROL_1,          0x0330);      //BT,AP
  LCD_Write(LCD_POWER_CONTROL_2,          0x0237);      //DC0,DC1,VC
  LCD_Write(LCD_POWER_CONTROL_4,          0x0D00);      //VDV
  LCD_Write(LCD_NVM_DATA_RW,              0x6100);      //VCM
  LCD_Write(LCD_POWER_CONTROL_3,          0xC1B0);      //VRH,VCMR,PSON,PON
  pause(50); 
  
//  LCD_Write(LCD_DRIVER_OUTPUT_CONTROL,    0x0100);
  LCD_Write(LCD_LCD_DRIVE_WAVE_CONTROL,   0x0100);       // B[8]    Line inversion
  LCD_Write(LCD_ENTRY_MODE,               0x5030);       // B[3]    Address counter goes horizontal
                                                         // B[5:4]  Horizontal decrement, vertical increment
                                                         // B[7]    Orgin not moved
                                                         // B[12]   Write data in BGR mode
                                                         // B[14]   16bpp
                                                         // B[15]   Two transfers per 16 bits 
//  LCD_Write(LCD_EXT_DISPLAY_IF_CONTROL_1, 0x0001);       // B[0]    16 bit color
                                                         // B[5:4]  Internal clock
                                                         // B[8]    RAM can be accessed while doing display
                                                         // B[14:12]RAM write cycle is 1 frame
  LCD_Write(LCD_FRAME_MARKER_CONTROL,     0x8000);
//  LCD_Write(LCD_EXT_DISPLAY_IF_CONTROL_2, 0x0000);        

  LCD_Write(LCD_WIN_H_RAM_ADRR_START,     0x0000);
  LCD_Write(LCD_WIN_H_RAM_ADRR_END,       0x00EF);
  LCD_Write(LCD_WIN_V_RAM_ADRR_START,     0x0000);
  LCD_Write(LCD_WIN_V_RAM_ADRR_END,       0x018F);       //432=01AF,400=018F
  LCD_Write(0x0500, 0x0000);
  LCD_Write(0x0501, 0x0000);
  LCD_Write(0x0502, 0x005F);
  LCD_Write(LCD_BASE_IMAGE_DISPLAY_CTRL,  0x0001);
  LCD_Write(LCD_BASE_IMAGE_V_SCROLL_CTRL, 0x0000);
  pause(50); 

  LCD_Write(LCD_DISPLAY_CONTROL_1,        0x0100);         //BASEE
  pause(50); 

  LCD_Write(LCD_RAM_ADRR_SET_H,           0x0000);
  LCD_Write(LCD_RAM_ADRR_SET_V,           0x0000);
  LCD_Write_Command(LCD_RAM_RW);

  

}  

/**
 *  A simple cog for painting the screen
 */
void LCD_Run(void){
  
  FILE *fp;
  uint8_t buffer[500];
  uint16_t x, y, i, j;
  int dataOffset;
  int size;
  int counter = 0;
  
  LCD_Init();
  
  // Initialize the interface to the SD card
  sd_mount(SD_DO, SD_SCK, SD_DI, SD_SS);
  
  while(1){
//    paint(0xF800);
//    pause(100);
//    paint(0x07E0);
//    pause(100);
//    paint(0x001F);
//    pause(100);
    
    // Open the first image
    if(counter == 0){
      fp = fopen("1.bmp", "r");
      counter++;
    }
    else{
    fp = fopen("2.bmp", "r");
    counter = 0;
    }            
   
    // Read in the header
    fread(buffer, 1, 54, fp);
    // Get the size of the image
    // 16 bit numbers, do some bit shifting
    x = buffer[18] + (buffer[19] << 8);
    y = buffer[22] + (buffer[23] << 8);
    //size = buffer[2] | (buffer[3] << 8) | (buffer[4] << 16) | (buffer[5] << 24);
    dataOffset = buffer[10] | (buffer[11] << 8) | (buffer[12] << 16) | (buffer[13] << 24);
   
    // Seek the file pointer to the start of the pixel data
    fseek(fp, dataOffset, SEEK_SET);
   
   
    LCD_SetWindow(0,0,239,400);

    for(i = 0; i < y; i++){
    
      // Read a line in
      fread(buffer, 1, x * 2, fp);
     
      for(j = 0; j < x * 2; j += 2){
        // Write the pixel data out to the display
        // Note that the pixel data is the Red, Green, Blue data in the 565 format, ie:
        //
        // Bit 0  1  2  3  4  5  6  7    8  9  10 11 12 13 14 15
        // Use B1 B2 B3 B4 B5 G0 G1 G2   G3 G4 G5 R1 R2 R3 R4 R5
        //
        // Notice that we only have 5 bits of Blue and Red?  The human eye is more sensitive
        // to the color green, so to make the RGB fit into 16 bits it is given priority
        LCD_Write_Data((buffer[j]) + (buffer[j+1] << 8));
      }
    }
      
    // Close and prepare for the next file
    fclose(fp);              
        
  }
}  

/**
 *  A routine that very slowly fills the screen with a color
 */
void paint(unsigned int color){
  int i,j;
  LCD_SetWindow(0,0,239,400);
  
  for(i = 0; i < 400; i++){
    for(j = 0; j < 240; j++){
      LCD_Write_Data(color);
    }
  } 
}   

/**
 *  Data transfer with the display is synchronized by sending a zero
 *  byte 4 times consecutively
 *  You should send this from time to time to keep the data
 *  transfer in sync  
 */
void LCD_SyncTransfer(void){
  pause(10);
  LCD_Write(0x0000, 0x0000);
  LCD_Write(0x0000, 0x0000);
  LCD_Write(0x0000, 0x0000);
  LCD_Write(0x0000, 0x0000);
  pause(15);
}  

/**
 *  Writes a byte out to the pins connected to the LCD
 *  It also strobes the WRX to latch in the data
 */
void LCD_Out(uint8_t byte){
  
  // Since we know what the hardware looks like, and the physical byte is rotated
  // 2 bits, we'll try to do this a little faster.
  // Set the pins to be output
  
//  DIRA |= 0xFF;
  
  // Rotate the byte
  byte = (byte >> 2) | (byte << (8 - 2));
  
  // Set the byte
  OUTA = (OUTA & ~0xFF) | byte;

  // A slower, but more flexible solution
  
/*  
  // Set all the output pins to be output
  set_direction(LCD_D0, 1);
  set_direction(LCD_D1, 1);
  set_direction(LCD_D2, 1);
  set_direction(LCD_D3, 1);
  set_direction(LCD_D4, 1);
  set_direction(LCD_D5, 1);
  set_direction(LCD_D6, 1);
  set_direction(LCD_D7, 1);
  
  // Now write the byte out to the pins
  // Since we don't know if the pins are contiguous, we 
  // set each pin individually
  set_output(LCD_D0, (byte & 0x01));
  set_output(LCD_D1, ((byte >> 1) & 0x01));
  set_output(LCD_D2, ((byte >> 2) & 0x01));
  set_output(LCD_D3, ((byte >> 3) & 0x01));
  set_output(LCD_D4, ((byte >> 4) & 0x01));
  set_output(LCD_D5, ((byte >> 5) & 0x01));
  set_output(LCD_D6, ((byte >> 6) & 0x01));
  set_output(LCD_D7, ((byte >> 7) & 0x01));
  */
}  

uint8_t LCD_In(){
  uint8_t byte;

  // Since we know what the hardware looks like, and the physical byte is rotated
  // 2 bits, we'll try to do this a little faster.
  // Set the pins to be output
  set_directions(7, 0, 0x00);
  
  // Get the byte
  byte = get_outputs(7, 0);
  
  // Rotate the byte
  byte = (byte << 2) | (byte >> (8 - 2));

/*
  // Set all the output pins to be input
  set_direction(LCD_D0, 0);
  set_direction(LCD_D1, 0);
  set_direction(LCD_D2, 0);
  set_direction(LCD_D3, 0);
  set_direction(LCD_D4, 0);
  set_direction(LCD_D5, 0);
  set_direction(LCD_D6, 0);
  set_direction(LCD_D7, 0);
  
  // Now read the inputs
  byte = get_output(LCD_D7);
  byte = (byte << 1) | (get_output(LCD_D6) & 0x01);
  byte = (byte << 1) | (get_output(LCD_D5) & 0x01);
  byte = (byte << 1) | (get_output(LCD_D4) & 0x01);
  byte = (byte << 1) | (get_output(LCD_D3) & 0x01);
  byte = (byte << 1) | (get_output(LCD_D2) & 0x01);
  byte = (byte << 1) | (get_output(LCD_D1) & 0x01);
  byte = (byte << 1) | (get_output(LCD_D0) & 0x01);
*/
  
  // Return the bytes
  return byte;
}       

/**
 *  Set the command
 */
void LCD_Write_Command(uint16_t command){
  // Set command mode
  // Enable the display 
  OUTA &= ~((1 << LCD_RS) | (1 << LCD_CS));  
  // Set the data, high byte first
  LCD_Out(command >> 8); 
  OUTA ^= 1 << LCD_WR;
  OUTA ^= 1 << LCD_WR;
  LCD_Out(command);
  OUTA ^= 1 << LCD_WR;
  OUTA ^= 1 << LCD_WR;
  // Disable the LCD
  OUTA |= (1 << LCD_CS);
}   

/**
 *  Set the data
 */
void LCD_Write_Data(uint16_t data){
  // Set data mode
  // Enable the display
  OUTA |= 1 << LCD_RS;
  OUTA &= ~(1 << LCD_CS);
  // Set the data, high byte first
  LCD_Out(data >> 8); 
  OUTA ^= 1 << LCD_WR;
  OUTA ^= 1 << LCD_WR;
  LCD_Out(data);
  OUTA ^= 1 << LCD_WR;
  OUTA ^= 1 << LCD_WR;
  // Disable the LCD
  OUTA |= (1 << LCD_CS);
} 

/**
 *  Send a command and data out to the display
 */
void LCD_Write(uint16_t command, uint16_t data){
  LCD_Write_Command(command);
  LCD_Write_Data(data);
}  

/**
 *  Set the window to paint in
 */
void LCD_SetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
  LCD_Write(LCD_WIN_H_RAM_ADRR_START, x1);
  LCD_Write(LCD_WIN_H_RAM_ADRR_END,   x2);
  LCD_Write(LCD_WIN_V_RAM_ADRR_START, y1);
  LCD_Write(LCD_WIN_V_RAM_ADRR_END,   y2);
  
  LCD_Write(LCD_RAM_ADRR_SET_H, x1);
  LCD_Write(LCD_RAM_ADRR_SET_V, y1);
  
  LCD_Write_Command(LCD_RAM_RW);
}  

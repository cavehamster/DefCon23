/*
 
 Header for the LCD driver
 
 */


#define LCD_D0  6
#define LCD_D1  7
#define LCD_D2  0
#define LCD_D3  1
#define LCD_D4  2
#define LCD_D5  3
#define LCD_D6  4
#define LCD_D7  5

#define LCD_RST 11
#define LCD_CS  12
#define LCD_RS  13
#define LCD_WR  14
#define LCD_RD  15

#define LCD_DEVICE_CODE_READ          0x000
#define LCD_DRIVER_OUTPUT_CONTROL     0x001
#define LCD_LCD_DRIVE_WAVE_CONTROL    0x002
#define LCD_ENTRY_MODE                0x003
#define LCD_DISPLAY_CONTROL_1         0x007
#define LCD_DISPLAY_CONTROL_2         0x008
#define LCD_DISPLAY_CONTROL_3         0x009
#define LCD_8_COLOR_CONTROL				   0x00B
#define LCD_EXT_DISPLAY_IF_CONTROL_1  0x00C
#define LCD_EXT_DISPLAY_IF_CONTROL_2  0x00F
#define LCD_PANEL_IF_CONTROL_1        0x010
#define LCD_PANEL_IF_CONTROL_2        0x011
#define LCD_PANEL_IF_CONTROL_3        0x012
#define LCD_PANEL_IF_CONTROL_4        0x013
#define LCD_PANEL_IF_CONTROL_5        0x014
#define LCD_PANEL_IF_CONTROL_6        0x020
#define LCD_PANEL_IF_CONTROL_7        0x021
#define LCD_PANEL_IF_CONTROL_8        0x022
#define LCD_PANEL_IF_CONTROL_9        0x023
#define LCD_FRAME_MARKER_CONTROL      0x090
#define LCD_POWER_CONTROL_1           0x100
#define LCD_POWER_CONTROL_2           0x101
#define LCD_POWER_CONTROL_3           0x102
#define LCD_POWER_CONTROL_4           0x103
#define LCD_RAM_ADRR_SET_H            0x200
#define LCD_RAM_ADRR_SET_V            0x201
#define LCD_RAM_RW                    0x202
#define LCD_WIN_H_RAM_ADRR_START      0x210
#define LCD_WIN_H_RAM_ADRR_END        0x211
#define LCD_WIN_V_RAM_ADRR_START      0x212
#define LCD_WIN_V_RAM_ADRR_END        0x213
#define LCD_NVM_DATA_RW               0x280
#define LCD_GAMMA_CONTROL_1           0x300
#define LCD_GAMMA_CONTROL_2           0x301
#define LCD_GAMMA_CONTROL_3           0x302
#define LCD_GAMMA_CONTROL_4           0x303
#define LCD_GAMMA_CONTROL_5           0x304
#define LCD_GAMMA_CONTROL_6           0x305
#define LCD_GAMMA_CONTROL_7           0x306
#define LCD_GAMMA_CONTROL_8           0x307
#define LCD_GAMMA_CONTROL_9           0x308
#define LCD_GAMMA_CONTROL_10          0x309
#define LCD_BASE_IMAGE_NB_LINE        0x400
#define LCD_BASE_IMAGE_DISPLAY_CTRL   0x401
#define LCD_BASE_IMAGE_V_SCROLL_CTRL  0x404
#define LCD_SOFTWARE_RESET            0x600

void LCD_Init(void);
void LCD_SyncTransfer(void);
void LCD_Out(uint8_t byte);
uint8_t LCD_In(void);
void LCD_Write_Command(uint16_t command);
void LCD_Write_Data(uint16_t data);
void LCD_Write(uint16_t command, uint16_t data);
void LCD_SetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Run(void);
void paint(unsigned int color);

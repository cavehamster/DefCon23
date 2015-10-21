
/*
 
 SD Card routines
 
 This interface allows access to the data on the SD card
 
 The interface is pretty simple, it's just a SPI device.  You need to define the
 SS, DI, DO, and SCK pins.  We're just using the simpletools utilities to handle
 the IO to the drive, so once you init, you can just use fopen, fwrite, and fclose
 like you would otherwise to read and write to the data on the card.
 
 Your SD card should be formatted as FAT.  As long as you don't open anything in
 write mode, you can eject at any time.  However, inserting it while the system is
 powered on will result in a reset, beware.
 
 -hamster
 
 */

#include "sdcard.h"
#include "simpletools.h"


void SD_Init(void){
 
 sd_mount(SD_DO, SD_SCK, SD_DI, SD_SS);
  
}  

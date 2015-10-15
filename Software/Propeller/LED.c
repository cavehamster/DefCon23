/*

  This handles the cog for the LED patterns.  The LEDs are NeoPixel units.  I'm using a library I found at 
  http://forums.parallax.com/discussion/152504/c-code-to-work-with-ws2812-neopixel-leds since the data format 
  takes a little doing.
  
  -hamster

 */
#include "simpletools.h"
#include "ws2812.h"
#include "LED.h"
#include "AVR_SPI.h"

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

uint32_t ledColors[LED_COUNT];

// LED driver state
ws2812_t LED_Driver;

// pattern for chase
uint32_t pattern[] = {
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_WHITE,
  COLOR_YELLOW,
};
#define pattern_count  (sizeof(pattern) / sizeof(pattern[0]))

int *LED_cog = 0;
int LED_Pattern = 0;

// Initialize the driver and start the cog
void LED_Init(){
  ws2812b_init(&LED_Driver);
  
  LED_cog = cog_run(LED_Run, 128);
}  

// The cog.  It scans for a button press, and changes the LED pattern as needed
void LED_Run(){
    
  while(1){
    
    LED_CheckPattern();
    switch(LED_Pattern){
      case 0:              
        LED_AllSame(1);
        break;
      case 1:
        LED_Chase(1);
        break;
      case 2:
        LED_SingleFill(1);
        break;
    }        
  }    
}  


// Colors chase around the badge
// Borrowed from http://forums.parallax.com/discussion/152504/c-code-to-work-with-ws2812-neopixel-leds
void LED_Chase(int count){
    static int base = 0;
    int idx, i;
    
    // repeat count times or forever if count < 0
    while (count < 0 || --count >= 0) {
    
        // fill the chain with the pattern
        idx = base;                             // start at base
        for (i = 0; i < LED_COUNT; ++i) {       // loop through connected leds
            ledColors[i] = pattern[idx];        // update channel color
            if (++idx >= pattern_count)              // past end of list?
                idx = 0;                        // yes, reset
        }
        if (++base >= pattern_count)            // update the base for the next time
            base = 0;
    
        // update the LED chain
        ws2812_refresh(&LED_Driver, LED_PIN, ledColors, LED_COUNT);
            
        // Delay until next frame, checking every 5ms if there has been a state change
        for(int d = 0; d < (256 - inputs.rotary); d += 5){
          if(LED_CheckPattern()){
            return;
          } 
          pause(5);
        }
    }
}  

// Set all the LEDs to the same color
void LED_AllSame(int count){
    int j, k;
  
  // repeat count times or forever if count < 0
  while (count < 0 || --count >= 0) {
    for(j = 0; j < pattern_count; j++){
      for(k = 0; k < LED_COUNT; k++){
        ledColors[k] = pattern[j];
      }
      ws2812_refresh(&LED_Driver, LED_PIN, ledColors, LED_COUNT); 
      
      // Delay until next frame, checking every 5ms if there has been a state change
      for(int d = 0; d < (256 - inputs.rotary); d += 5){
        if(LED_CheckPattern()){
          return;
        } 
        pause(5);
      }                 
    }
  }  
}    

// Set all the LEDs to the same color, one by one, like a color snake
void LED_SingleFill(int count){
  int j, k;
  
  // repeat count times or forever if count < 0
  while (count < 0 || --count >= 0) {
    for(j = 0; j < pattern_count; j++){
      for(k = 0; k < LED_COUNT; k++){
        ledColors[k] = pattern[j];
        ws2812_refresh(&LED_Driver, LED_PIN, ledColors, LED_COUNT); 
      
        // Delay until next frame, checking every 5ms if there has been a state change
        for(int d = 0; d < (256 - inputs.rotary); d += 5){
          if(LED_CheckPattern()){
            return;
          } 
          pause(5);
        } 
      }         
    }
  }   
}

 
/*
 * This gets called often to check if a button has been pressed
 *
 * We have to debounce presses, so if we see a button being pressed, we set a 
 * debounce flag.  Once the button is released, the button is debounced and
 * we latch in the state change
 *
 */
int LED_CheckPattern(){
  
  static int debounceBack = FALSE, debounceForward = FALSE;
  
  if(inputs.leftButton){ 
    debounceBack = TRUE;
  }
  if(debounceBack && !inputs.leftButton){
    debounceBack = FALSE;
    if(LED_Pattern > 0){
      LED_Pattern--;
      return TRUE;
    }
  }
  
  if(inputs.rightButton){
    debounceForward = TRUE;
  }
  if(debounceForward && !inputs.rightButton){
    debounceForward = FALSE;
    if(LED_Pattern < 2){
      LED_Pattern++;
      return TRUE;
    }
  } 
  
  return FALSE;
}     
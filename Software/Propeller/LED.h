/*
 *
 * Header for the LED driver - pretty simple stuff here.
 *
 */

// LED drive pin and count
#define LED_PIN     26
#define LED_COUNT   10  

void LED_Chase(int count);
void LED_AllSame(int count);
void LED_SingleFill(int count);
void LED_Init(int pin, int count);
void LED_Run(void);
int LED_CheckPattern(void);
void LED_Off(void);

extern int LED_Pattern;


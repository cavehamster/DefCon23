/*
 *
 * Header for the LED driver - pretty simple stuff here.
 *
 */

void LED_Chase(int count);
void LED_AllSame(int count);
void LED_SingleFill(int count);
void LED_Init(void);
void LED_Run(void);
int LED_CheckPattern(void);

extern int LED_Pattern;

// LED drive pin and count
#define LED_PIN     26
#define LED_COUNT   10

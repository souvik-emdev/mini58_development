#ifndef PHASECUT_H_INCLUDED
#define PHASECUT_H_INCLUDED

#define MAX_NUMBER_LED 4

// //Phasecut enabled bitmasks
// #define PC_ENABLE_FOR_LED1 0x1
// #define PC_ENABLE_FOR_LED2 0x2
// #define PC_ENABLE_FOR_LED3 0x4
// #define PC_ENABLE_FOR_LED4 0x8

//Smoothdim enabled bitmasks
#define SM_ENABLE_FOR_LED1 0x1
#define SM_ENABLE_FOR_LED2 0x2
#define SM_ENABLE_FOR_LED3 0x4
#define SM_ENABLE_FOR_LED4 0x8

#define SM_LEVEL 58


#define MIN_ALLOWED_DIMMING 1 //1% OF 255
#define MAX_ALLOWED_DIMMING 244 //95% OF 255

//#define MIN_ALLOWED_CMPVALUE 300
//#define MAX_ALLOWED_CMPVALUE 14500

#define PHASE_CUT_RESET 14800 //must be higher than MAX_ALLOWED_CMPVALUE

//#define PC_ENABLE_FOR_LED(x) (1 << (x))
#define SM_ENABLE_FOR_LED(x) (1 << (x))

#define LEDID1 0
#define LEDID2 1
#define LEDID3 2
#define LEDID4 3

#define LED_HIGH 1
#define LED_LOW 0

#define LED1 P22
#define LED2 P23
#define LED3 P24
#define LED4 P25

volatile uint8_t smoothDimActive = 0;
//volatile uint8_t phaseCutEnable = 0;
uint32_t allSwitchTiming[MAX_NUMBER_LED+2];
volatile uint8_t SwitchTimingIndex = 0;

//uint16_t map(uint16_t x, uint8_t in_min, uint8_t in_max, uint16_t out_min, uint16_t out_max);
void sortTimings(void);
void setPhaseCut(uint8_t ledno, uint8_t state);
//uint16_t calculateCmpValue(uint16_t timingMicroSec);
uint16_t calculateCmpValue(uint8_t dim_level);
void updateAllTimings (void);
void phaseCutInit(void);
void setLed (uint16_t timingValue);
void smoothDimHandler(void);

extern struct led
{
    uint8_t state;
    uint8_t config;
    uint32_t phaseCutTime;
    volatile uint32_t smoothDimTime;
};

extern struct led arr_led[MAX_NUMBER_LED];


#endif
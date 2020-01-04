#ifndef PHASECUT_H_INCLUDED
#define PHASECUT_H_INCLUDED

#define MAX_NUMBER_LED 4

//Phasecut enabled bitmasks
#define PC_ENABLE_FOR_LED1 0x1
#define PC_ENABLE_FOR_LED2 0x2
#define PC_ENABLE_FOR_LED3 0x4
#define PC_ENABLE_FOR_LED4 0x8

#define PC_ENABLE_FOR_LED(x) (1 << (x))

#define LEDID1 1
#define LEDID2 2
#define LEDID3 3
#define LEDID4 4

#define LED_HIGH 0
#define LED_LOW 1

#define LED1 P22
#define LED2 P23
#define LED3 P24
#define LED4 P25

volatile uint8_t phaseCutEnable = 0;
uint32_t allSwitchTiming[MAX_NUMBER_LED+2];
volatile uint8_t SwitchTimingIndex = 0;

uint16_t map(uint16_t x, uint8_t in_min, uint8_t in_max, uint16_t out_min, uint16_t out_max);
void sortTimings(void);
void setPhaseCut(uint8_t ledno, uint8_t state);
uint16_t calculateCmpValue(uint16_t timingMicroSec);
void updateAllTimings (void);
void phaseCutInit(void);
void setLed (uint16_t timingValue);

extern struct led
{
    uint8_t state;
    uint8_t config;
    uint32_t phaseCutTime;
};

extern struct led arr_led[MAX_NUMBER_LED];


#endif
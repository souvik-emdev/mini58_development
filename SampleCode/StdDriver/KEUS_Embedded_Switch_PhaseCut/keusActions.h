#ifndef KEUSACTIONS_H_INCLUDED
#define KEUSACTIONS_H_INCLUDED

/* ^^ these are the include guards */

/* Prototypes for the functions */
#include "EmbeddedSwitchConstants.h"

uint8_t commandId, txnId, uartDataBuffer[MAX_UART_DATA_BUFFER], retryBuffer[UARTRETRYBUFFER], retryDataLen,
    retryGettingAckEnabled = 0;

volatile uint8_t retryTimerCount = 0;

extern volatile uint8_t g0_u8RecData[];
extern volatile uint32_t keusAppEvents;

extern void UART_tx(uint8_t arr[], uint8_t datalen);
extern void setPhaseCut(uint8_t ledno, uint8_t state);
extern void updateAllTimings(void);
extern volatile uint8_t phaseCutEnable;
extern volatile uint8_t smoothDimActive;

void parseUart(void);
void taskHandler(void);
void configSwitch(void);
//void getSwitchState(void);
//void getConfig(void);
void TMR0_IRQHandler(void);
void retryGettingAck(uint8_t arr[], uint8_t dataLen);
void uartAck(void);
void executeSwitchState(void);
void updateTxnId(void);
void keus_config_switch_init(void);
void setSwitch (uint8_t ledno, uint8_t state);

struct led
{
    uint8_t state;
    uint8_t config;
    uint32_t phaseCutTime;
    volatile uint32_t smoothDimTime;
};

struct led arr_led[MAX_NUMBER_LED];

void toggleLed(uint8_t ledno);
void sendSwitchPressInfoAgain(void);

#endif
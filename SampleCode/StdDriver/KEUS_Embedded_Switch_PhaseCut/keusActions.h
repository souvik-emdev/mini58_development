#ifndef KEUSACTIONS_H_INCLUDED
#define KEUSACTIONS_H_INCLUDED

/* ^^ these are the include guards */

/* Prototypes for the functions */

#define LED1 P22
#define LED2 P23
#define LED3 P24
#define LED4 P25

//Bit masks for phasecut
#define PC_ENABLE_FOR_LED1 0x1
#define PC_ENABLE_FOR_LED2 0x2
#define PC_ENABLE_FOR_LED3 0x4
#define PC_ENABLE_FOR_LED4 0x8

//Smoothdim enabled bitmasks
#define SM_ENABLE_FOR_LED1 0x1
#define SM_ENABLE_FOR_LED2 0x2
#define SM_ENABLE_FOR_LED3 0x4
#define SM_ENABLE_FOR_LED4 0x8

#define CONFIGNORMAL 1
#define CONFIGDIM 3
#define CONFIGFAN 4

#define LEDID1 0
#define LEDID2 1
#define LEDID3 2
#define LEDID4 3

#define LED_HIGH 1
#define LED_LOW 0

//UART
#define TASK_CONFIG_SWITCH 1
//#define TASK_GET_SWITCH_STATE 2
//#define TASK_GET_CONFIG 3
#define TASK_EXECUTE_SWITCH_STATE 4
#define TASK_UART_RETRY_ACK 5

//UART Reply Ids
#define TASK_UART_ACK 5 //for configSwitch and executeSwitchState
//#define GETSWITCHSTATEREPLY 0x06
//#define GETCONFIGREPLY 0x07
#define REPORTSWITCHSTATE 0x01

#define MAX_UART_DATA_BUFFER 10
#define UARTRETRYBUFFER 6

#define MAX_NUMBER_LED 4

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

struct led
{
    uint8_t state;
    uint8_t config;
    uint32_t phaseCutTime;
    volatile uint32_t smoothDimTime;
};

struct led arr_led[MAX_NUMBER_LED];

void toggleLed(uint8_t);

#endif
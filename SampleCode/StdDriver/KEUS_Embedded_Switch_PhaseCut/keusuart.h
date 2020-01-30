#ifndef KEUSUART_H_INCLUDED
#define KEUSUART_H_INCLUDED

/* ^^ these are the include guards */

/* Prototypes for the functions */
#include "EmbeddedSwitchConstants.h"

extern volatile uint32_t keusAppEvents;;

void UART_tx(uint8_t arr[], uint8_t datalen);
void keus_uart_init (void);

volatile uint8_t g0_u8RecData[KEUS_UART_BUFFER] = {0};
volatile uint8_t g0_bufferCounter = 0;
volatile uint8_t g0_shouldSaveToBuffer = 0;
volatile uint8_t g0_endMessageIndex = 0;

#endif
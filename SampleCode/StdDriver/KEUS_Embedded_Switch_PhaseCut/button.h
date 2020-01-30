#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

/* ^^ these are the include guards */

/* Prototypes for the functions */
volatile uint8_t ignoreZC = 0;

extern volatile uint32_t keusAppEvents;

void GPIO234_IRQHandler(void);
void EINT1_IRQHandler(void);
void EINT0_IRQHandler(void);
void keus_button_init(void);

#endif
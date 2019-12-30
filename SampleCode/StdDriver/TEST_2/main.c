/******************************************************************************
* @file     main.c
* @version  V1.00
* $Revision: 3 $
* $Date: 15/05/28 10:37a $
* @brief    Use pin P2.4 to as switch input and 2.3 as LED output.
*           Pressing Switch changes LED state.
*           LED state can be changed from UART msg.
*
* @note
* Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

#define PLL_CLOCK 50000000



uint8_t ir_out2[11] = {0x4A, 0x75, 0xC3, 0x64, 0x9B, 0xFF, 0x0, 0xFF, 0x0, 0x63, 0x9C}; //final decoded array which contain both LSB8 and MSB

void setAsGpio(void)
{
  SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
  SYS->P2_MFP |= SYS_MFP_P25_GPIO;
  GPIO_SetMode(P2, BIT5, GPIO_MODE_OUTPUT);
  P25 = 0;
}

void setAsPwm(void)
{
  SYS->P2_MFP = SYS_MFP_P25_PWM0_CH3;
}

void SYS_Init(void)
{
  /*---------------------------------------------------------------------------------------------------------*/
  /* Init System Clock                                                                                       */
  /*---------------------------------------------------------------------------------------------------------*/

  /* Unlock protected registers */
  SYS_UnlockReg();

  /* Set P5 multi-function pins for crystal output/input */
  SYS->P5_MFP &= ~(SYS_MFP_P50_Msk | SYS_MFP_P51_Msk);
  SYS->P5_MFP |= (SYS_MFP_P50_XT1_IN | SYS_MFP_P51_XT1_OUT);

  /*
  Peower control and main clock
  */
  // Clearing all bits of power control register
  CLK->PWRCTL &= ~(0x2ff);
  /* Enable External XTAL (4~24 MHz) */
  CLK->PWRCTL |= (CLK_PWRCTL_XTL12M << CLK_PWRCTL_XTLEN_Pos);
  //CLK->PWRCTL |= (CLK_PWRCTL_LIRCEN_Msk);

  /* Waiting for 12MHz clock ready */
  CLK_WaitClockReady(CLK_STATUS_XTLSTB_Msk);

  /* 
  Switch HCLK and Sys tick clock source to XTAL 
  */
  CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLKSEL_Msk;
  CLK->CLKSEL0 |= (CLK_CLKSEL0_HCLKSEL_XTAL << CLK_CLKSEL0_HCLKSEL_Pos);
  CLK->CLKSEL0 &= ~CLK_CLKSEL0_STCLKSEL_Msk;
  CLK->CLKSEL0 |= (CLK_CLKSEL0_STCLKSEL_XTAL << CLK_CLKSEL0_STCLKSEL_Pos);
  /*
  Peripheral clock select
  */
  // clearing APBCLK unreserved register
  CLK->APBCLK &= ~(0xd073134d);
  /* Enable peripheral clocks */
  //    CLK->APBCLK |= CLK_APBCLK_WDTCKEN_Msk;
  CLK->APBCLK |= CLK_APBCLK_TMR0CKEN_Msk;
  //CLK->APBCLK |= CLK_APBCLK_TMR1CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_CLKOCKEN_Msk; // clock divider
  //    CLK->APBCLK |= CLK_APBCLK_I2C0CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_I2C1CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_SPICKEN_Msk;
  //CLK->APBCLK |= CLK_APBCLK_UART0CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_UART1CKEN_Msk;
  CLK->APBCLK |= CLK_APBCLK_PWMCH01CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_PWMCH23CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_PWMCH45CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_ADCCKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_ACMPCKEN_Msk;

  /*
  Peripheral clock source 1
  */
  // clearing peripheral clock sources
  CLK->CLKSEL1 &= ~(0xf300773f);
  // Select peripheral clock sources
  CLK->CLKSEL1 |= CLK_CLKSEL1_UARTSEL_XTAL;
  CLK->CLKSEL1 |= CLK_CLKSEL1_TMR0SEL_XTAL;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_TMR1SEL_XTAL;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_TMR1SEL_HCLK;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_WDTSEL_LIRC;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_PWMCH23SEL_HCLK;
  //    CLK->CLKSEL1 |= CLK_CLKSEL1_SPISEL_XTAL;

  /*
  Peripheral clock source 2
  */
  //CLK->CLKSEL2 &= ~CLK_CLKSEL2_PWMCH45SEL_Msk;
  //CLK->CLKSEL2 |= CLK_CLKSEL2_PWMCH45SEL_HCLK;

  /*
  Peripheral clock divider
  */
  // clearing peripheral clock dividers
  CLK->CLKDIV &= ~(0x00ff0f0f);
  //  Select peripheral clock divider
  //    CLK->CLKDIV |=CLK_CLKDIV_ADC(x);
  //    CLK->CLKDIV |=CLK_CLKDIV_UART(1);
  //    CLK->CLKDIV |=CLK_CLKDIV_HCLK(x);

  /* Set P0 multi-function pins for UART RXD and TXD */
  //SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
  //SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);

  /* Setup SPI multi-function pin */
  //SYS->P0_MFP |= SYS_MFP_P04_SPI0_SS | SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO | SYS_MFP_P07_SPI0_CLK;

  /* Set core clock as PLL_CLOCK from PLL */
  //CLK_SetCoreClock(PLL_CLOCK);

  /* Update System Core Clock */
  /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CycylesPerUs automatically. */
  SystemCoreClockUpdate();

  /* Set P2 multi-function pin for PWM Channel 0  */
    // Start pwm
  PWM_ConfigOutputChannel(PWM, 3, 38000, 50);
  // Enable output of all PWM channel 0
  PWM_EnableOutput(PWM, 8);
  PWM_Start(PWM, 0x8);
  //SYS->P2_MFP = SYS_MFP_P22_PWM0_CH0;

  /* Lock protected registers */
  SYS_LockReg();
}

void tx1(){
  //ir_pin = 1;
  setAsPwm();
  TIMER_Delay(TIMER0, 394);
  //ir_pin = 0;
  setAsGpio();
  TIMER_Delay(TIMER0, 1180);
    
}

void tx0(){
  //ir_pin = 1;
  setAsPwm();
  TIMER_Delay(TIMER0, 406);
  //ir_pin = 0;
  setAsGpio();
  TIMER_Delay(TIMER0, 382);  
}

void startbit(){
  //ir_pin = 1;
  setAsPwm();
  TIMER_Delay(TIMER0, 3168); 
  //ir_pin = 0;
  setAsGpio();
  TIMER_Delay(TIMER0, 1568);
}

void stopbit(){
  //ir_pin = 1;
  setAsPwm();
  TIMER_Delay(TIMER0, 406);
  //ir_pin = 0;
  setAsGpio();  
}


void transmit(){
  startbit();
  bool bitvalue = 0;
  for (int i=0; i<11; i++){
    for (int j=7; j>=0; j--){
      bitvalue = bitRead(ir_out2[i], j);
      //printf("%d", bitvalue); 
      if(bitvalue){tx1();}
      else {tx0();}     
    }
    //printf(" ");
  }
  stopbit();
}



int main(void)
{

  /* Init System, IP clock and multi-function I/O
  In the end of SYS_Init() will issue SYS_LockReg()
  to lock protected register. If user want to write
  protected register, please issue SYS_UnlockReg()
  to unlock protected register if necessary */
  SYS_Init();



  //uint8_t uartReply[] = {'D', 'o', 'n', 'e', '!', 0x0A};

  /* Enable interrupt de-bounce function and select de-bounce sampling cycle time */
  // GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_HCLK, GPIO_DBCTL_DBCLKSEL_512);
  // GPIO_ENABLE_DEBOUNCE(P2, BIT4);

  // Set timer 1 shot with 10HZ
//  TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 10);

  // Update prescale value
  //TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
  // Start Timer 0
//  TIMER_Start(TIMER0);
  // Enable timer interrupt
//  TIMER_EnableInt(TIMER0);
//  NVIC_EnableIRQ(TMR0_IRQn);

  while (1)
  {

    // if (P24 == 0) {

    TIMER_Delay(TIMER0, 1000000);
    TIMER_Delay(TIMER0, 1000000);
    TIMER_Delay(TIMER0, 1000000);
    TIMER_Delay(TIMER0, 1000000);
    TIMER_Delay(TIMER0, 1000000);
    // }

    transmit();
  }
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/

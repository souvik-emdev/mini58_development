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
#include "stdbool.h"
#include "Mini58Series.h"

#define PLL_CLOCK 50000000

#define LED_HIGH 0
#define LED_LOW 1

volatile uint8_t timer0Counter = 0;

uint16_t dummyData[4] = {2000, 5000, 6000, 8000};

uint16_t cmpValue[5] = {0};

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
  //    CLK->APBCLK |= CLK_APBCLK_UART0CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_UART1CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_PWMCH01CKEN_Msk;
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
  //CLK->CLKSEL1 |= CLK_CLKSEL1_UARTSEL_XTAL;
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

  /* Lock protected registers */
  SYS_LockReg();
}




void TMR0_IRQHandler(void)
{
  //Fire zero cross here
  timer0Counter++;
  if (timer0Counter >= 80)
  {
    P23 = ~P23;
    timer0Counter = 0;
  }

  // clear timer interrupt flag
  TIMER_ClearIntFlag(TIMER0);
}

void keusZeroCrossInit(void)
{
  TIMER0->CTL = TIMER_PERIODIC_MODE | 7;
  TIMER0->CMP = 0x3A98;
  // TIMER_SET_PRESCALE_VALUE(TIMER0, 0x07);
  // TIMER_SET_CMP_VALUE(TIMER0, 0x3A98);
  // Enable timer interrupt
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);

  // Start Timer 0
  TIMER_Start(TIMER0);
}

uint16_t calculateCmpValue(uint16_t timingMicroSec)
{
  return (1.5 * timingMicroSec);
}

void fillCmpValue(void)
{
  cmpValue[0] = calculateCmpValue(dummyData[0]);
  cmpValue[1] = calculateCmpValue(dummyData[1]);
  cmpValue[2] = calculateCmpValue(dummyData[2]);
  cmpValue[3] = calculateCmpValue(dummyData[3]);
  cmpValue[4] = calculateCmpValue(9600);
}

int main(void)
{

  /* Init System, IP clock and multi-function I/O
  In the end of SYS_Init() will issue SYS_LockReg()
  to lock protected register. If user want to write
  protected register, please issue SYS_UnlockReg()
  to unlock protected register if necessary */
  SYS_Init();

  SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
  SYS->P2_MFP |= SYS_MFP_P22_GPIO;
  GPIO_SetMode(P2, BIT2, GPIO_MODE_OUTPUT);
  P22 = LED_LOW;

  SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
  SYS->P2_MFP |= SYS_MFP_P24_GPIO;
  GPIO_SetMode(P2, BIT4, GPIO_MODE_OUTPUT);
  P24 = LED_LOW;

  SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
  SYS->P2_MFP |= SYS_MFP_P25_GPIO;
  GPIO_SetMode(P2, BIT5, GPIO_MODE_OUTPUT);
  P25 = LED_LOW;

  SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
  SYS->P2_MFP |= SYS_MFP_P23_GPIO;
  GPIO_SetMode(P2, BIT3, GPIO_MODE_OUTPUT);
  P23 = LED_LOW;

  keusZeroCrossInit();

  fillCmpValue();

  while (1)
  {
  }
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/

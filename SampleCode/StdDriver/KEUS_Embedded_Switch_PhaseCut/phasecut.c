#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "phasecut.h"



/**
 * @brief       Mapping values to a different range
 *
 * @param       value to map, min value of input
 *              range, max value of input range, min value of o/p range
 *              max value of o/p range
 *
 * @return      mapped value
 *
 * @details     Required for mapping range 0-255 to 0-9500
 */
uint16_t map(uint16_t x, uint8_t in_min, uint8_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Sorts the all_timing array
void sortTimings(void)
{
    int i, j, temp;
    for (i = 0; i < MAX_NUMBER_LED; i++)
    {
        for (j = i + 1; j < MAX_NUMBER_LED; j++)
        {
            if (allSwitchTiming[i] > allSwitchTiming[j])
            {
                temp = allSwitchTiming[i];
                allSwitchTiming[i] = allSwitchTiming[j];
                allSwitchTiming[j] = temp;
            }
        }
    }
}

uint16_t calculateCmpValue(uint16_t timingMicroSec)
{
    return (1.5 * timingMicroSec);
}

//Picks up the timing values from led structs and call sorting function
void updateAllTimings(void)
{
    for (int i = 0; i < MAX_NUMBER_LED; i++)
    {
        allSwitchTiming[i] = arr_led[i].phaseCutTime;
    }
    sortTimings();
}

void setPhaseCut(uint8_t ledno, uint8_t state)
{
    if (state < 26)
    {
        state = 26;
    } //Less than 10% is not allowed
    else if (state > 243)
    {
        state = 243;
    } //greter than 95% is not allowed
    state = 255 - state;
    arr_led[ledno - 1].phaseCutTime = calculateCmpValue(map(state, 0, 255, 0, 9500));
    updateAllTimings(); //fill allSwitchTiming and sort it
}

void phaseCutInit(void)
{
    TIMER1->CTL = TIMER_CONTINUOUS_MODE | 7;
    TIMER1->CMP = 14400; //9.6ms
    SwitchTimingIndex = 4;
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    allSwitchTiming[MAX_NUMBER_LED] = 14400;     //9.6ms
    allSwitchTiming[MAX_NUMBER_LED + 1] = 15000; //10ms
    TIMER_Start(TIMER1);
}

void TMR1_IRQHandler(void)
{
    //   if (timer1Counter == 1 && allSwitchTiming[LEDID1])
    //   {
    //     //P22 = LED_HIGH;
    //     //call led update function
    //     TIMER1->CMP = allSwitchTiming[LEDID1];
    //   }

    //   else if (timer1Counter == 2)
    //   {
    //     P23 = LED_HIGH;
    //     TIMER1->CMP = cmpValue[2];
    //   }

    //   else if (timer1Counter == 3)
    //   {
    //     P24 = LED_HIGH;
    //     TIMER1->CMP = cmpValue[3];
    //   }

    //   else if (timer1Counter == 4)
    //   {
    //     P25 = LED_HIGH;
    //     TIMER1->CMP = cmpValue[4];
    //   }

    //   else if (timer1Counter == 5)
    //   {
    //     P23 = LED_LOW;
    //     P24 = LED_LOW;
    //     P22 = LED_LOW;
    //     P25 = LED_LOW;
    //     TIMER1->CMP = cmpValue[5];
    //   }
    //   else if (timer1Counter == 6)
    //   {
    //     timer1Counter = 0;
    //     //clear counter
    //     TIMER1->CTL |= TIMER_CTL_RSTCNT_Msk;
    //     TIMER1->CTL |= TIMER_CTL_CNTEN_Msk; //Prescalar mask
    //     //TIMER1->CTL = TIMER_CONTINUOUS_MODE | 7;
    //     TIMER1->CMP = cmpValue[0];
    //   }

    //   timer1Counter++;
    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER1);

    if (SwitchTimingIndex == 4)
    {
        if (phaseCutEnable & PC_ENABLE_FOR_LED1)
        {
            LED1 = LED_LOW;
        }
        if (phaseCutEnable & PC_ENABLE_FOR_LED2)
        {
            LED2 = LED_LOW;
        }

        if (phaseCutEnable & PC_ENABLE_FOR_LED3)
        {
            LED3 = LED_LOW;
        }
        if (phaseCutEnable & PC_ENABLE_FOR_LED4)
        {
            LED4 = LED_LOW;
        }

        //find a non-zero value from allSwitchTiming array
        //for the next cmp value to be filled
        for (int i = 0; i < MAX_NUMBER_LED; i++)
        {
            if (allSwitchTiming[i])
            {
                SwitchTimingIndex = i;
                TIMER1->CMP = allSwitchTiming[SwitchTimingIndex];
                break;
            }
        }

        //if any non-zero element not found, reload with 9.6ms
        if (SwitchTimingIndex == MAX_NUMBER_LED)
        {
            SwitchTimingIndex = MAX_NUMBER_LED;
            TIMER1->CMP = allSwitchTiming[MAX_NUMBER_LED]; //reload to 9.6
        }
    }

    else
        setLed(allSwitchTiming[SwitchTimingIndex]);
}

/*ak*/
uint8_t keus_ports[4] = {2, 2, 2, 2};
uint8_t keus_bits[4] = {2, 3, 4, 5};

void setLed(uint16_t timingValue)
{
    uint8_t duplicate = 0;
    for (int i = 0; i < MAX_NUMBER_LED; i++)
    {
        if (arr_led[i].phaseCutTime == timingValue)
        {
            duplicate++;

             if (phaseCutEnable & PC_ENABLE_FOR_LED(i))
             {
                 GPIO_PIN_ADDR(keus_ports[i], keus_bits[i]) = LED_HIGH;
             }

            //////////OLD METHOD FOR SETTING THE OUTPUT
            // switch (i)
            // {
            // case 0:
            //     if (phaseCutEnable & PC_ENABLE_FOR_LED1)
            //     {
            //         LED1 = LED_HIGH;
            //     }
            //     break;
            // case 1:
            //     if (phaseCutEnable & PC_ENABLE_FOR_LED2)
            //     {
            //         LED2 = LED_HIGH;
            //     }
            //     break;
            // case 2:
            //     if (phaseCutEnable & PC_ENABLE_FOR_LED3)
            //     {
            //         LED3 = LED_HIGH;
            //     }
            //     break;
            // case 3:
            //     if (phaseCutEnable & PC_ENABLE_FOR_LED4)
            //     {
            //         LED4 = LED_HIGH;
            //     }
            //     break;

            // default:
            //     break;
            // }
        }
    }

    if (duplicate) //if found update nxt cmpvalue
    {
        SwitchTimingIndex += duplicate;
        if (SwitchTimingIndex > 4)
            SwitchTimingIndex = 4;
        TIMER1->CMP = allSwitchTiming[SwitchTimingIndex];
    }
    else
    { //reset to 9.6 ms
        TIMER1->CMP = allSwitchTiming[MAX_NUMBER_LED];
        SwitchTimingIndex = MAX_NUMBER_LED;
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "keusActions.h"

/**
 * @brief       Toggle output state on buttonpress
 *
 * @param       ledID
 *
 * @return      Nothing
 *
 * @details     Whenever a button is pressed it toggles the output state and
 *              sends the info via UART. Also stops the phasecut if it was enabled.
 */
void toggleLed(uint8_t ledno)
{
    uint8_t sendReply[5], datalen = 0;
    sendReply[datalen++] = REPORTSWITCHSTATE;
    updateTxnId();
    sendReply[datalen++] = txnId;

    switch (ledno)
    {
    case LEDID1:

        if (arr_led[0].state < 1)
        {
            arr_led[0].state = 255;

            LED1 = LED_HIGH;
        }
        else
        {
            arr_led[0].state = 0;
            LED1 = LED_LOW;
        }
        if (arr_led[0].phaseCutTime)
        {
            //clear phasecut enable bit
            smoothDimActive &= ~SM_ENABLE_FOR_LED1;
            arr_led[0].phaseCutTime = 0;
            //Update timing array and sort it
            updateAllTimings();
        }

        break;
    case LEDID2:

        if (arr_led[1].state < 1)
        {
            arr_led[1].state = 255;

            LED2 = LED_HIGH;
        }
        else
        {
            arr_led[1].state = 0;
            LED2 = LED_LOW;
        }
        if (arr_led[1].phaseCutTime)
        {
            smoothDimActive &= ~SM_ENABLE_FOR_LED2;
            arr_led[1].phaseCutTime = 0;
            updateAllTimings();
        }

        break;
    case LEDID3:

        if (arr_led[2].state < 1)
        {
            arr_led[2].state = 255;

            LED3 = LED_HIGH;
        }
        else
        {
            arr_led[2].state = 0;
            LED3 = LED_LOW;
        }
        if (arr_led[2].phaseCutTime)
        {
            smoothDimActive &= ~SM_ENABLE_FOR_LED3;
            arr_led[2].phaseCutTime = 0;
            updateAllTimings();
        }

        break;
    case LEDID4:

        if (arr_led[3].state < 1)
        {
            arr_led[3].state = 255;

            LED4 = LED_HIGH;
        }
        else
        {
            arr_led[3].state = 0;
            LED3 = LED_LOW;
        }
        if (arr_led[3].phaseCutTime)
        {
            smoothDimActive &= ~SM_ENABLE_FOR_LED4;
            arr_led[3].phaseCutTime = 0;
            updateAllTimings();
        }

        break;

    default:
        break;
    }

    sendReply[datalen++] = ledno;
    sendReply[datalen++] = arr_led[ledno - 1].state;
    UART_tx(sendReply, datalen);
    retryGettingAck(sendReply, datalen);
}

/**
 * @brief       Sets the output as state received from UART
 *
 * @param       Nothing
 *
 * @return      Nothing
 *
 */
void executeSwitchState(void)
{
    //executeSwitchState: 28<size><commandid 4><txn><noof switch><id><state>..<29>
    //<noof switch><id><state><id><state>
    int noOfSwitch = uartDataBuffer[0];
    uint8_t position = 0, switchId, state, sendReply[5], datalen = 0;

    for (int i = 0; i < noOfSwitch; i++)
    {
        switchId = uartDataBuffer[position + 1];
        state = uartDataBuffer[position + 2];

        switch (switchId)
        {
        case LEDID1:
            //Clear phasecut enable bit
            smoothDimActive &= ~SM_ENABLE_FOR_LED1;
            if (arr_led[0].config == CONFIGNORMAL)
            {
                if (state > 0)
                {
                    arr_led[0].state = 255;
                    LED1 = LED_HIGH;
                }
                else
                {
                    arr_led[0].state = 0;
                    LED1 = LED_LOW;
                }
            }
            else //for config other than normal: dim & fan
            {

                if (state == 0 || state == 255)
                {
                    arr_led[0].phaseCutTime = 0;
                    //Update timing array and sort it
                    updateAllTimings();
                    if (state == 0)
                    {
                        LED1 = LED_LOW;
                    }
                    else if (state == 255)
                    {
                        LED1 = LED_HIGH;
                    }
                }
                else
                {
                    //phaseCutEnable |= (1 << (LEDID1 - 1));
                    //Set phasecut bit mask
                    smoothDimActive |= SM_ENABLE_FOR_LED1;
                    setPhaseCut(LEDID1, state);
                }
                arr_led[0].state = state;

                //setPwmDuty(LEDID1, arr_led[0].state);
            }

            break;
        case LEDID2:
            smoothDimActive &= ~SM_ENABLE_FOR_LED2;
            if (arr_led[1].config == CONFIGNORMAL)
            {
                if (state > 0)
                {
                    arr_led[1].state = 255;
                    LED2 = LED_HIGH;
                }
                else
                {
                    arr_led[1].state = 0;
                    LED2 = LED_LOW;
                }
            }
            else
            {
                if (state == 0 || state == 255)
                {
                    arr_led[1].phaseCutTime = 0;
                    //Update timing array and sort it
                    updateAllTimings();

                    if (state == 0)
                    {
                        LED2 = LED_LOW;
                    }
                    else if (state == 255)
                    {
                        LED2 = LED_HIGH;
                    }
                }
                else
                {
                    //phaseCutEnable |= (1 << (LEDID2 - 1));
                    smoothDimActive |= SM_ENABLE_FOR_LED2;
                    setPhaseCut(LEDID2, state);
                }
                arr_led[1].state = state;
                //setPwmDuty(LEDID2, arr_led[1].state);
            }

            break;
        case LEDID3:
            smoothDimActive &= ~SM_ENABLE_FOR_LED3;
            if (arr_led[2].config == CONFIGNORMAL)
            {
                if (state > 0)
                {
                    arr_led[2].state = 255;
                    LED3 = LED_HIGH;
                }
                else
                {
                    arr_led[2].state = 0;
                    LED3 = LED_LOW;
                }
            }
            else
            {
                if (state == 0 || state == 255)
                {
                    arr_led[2].phaseCutTime = 0;
                    //Update timing array and sort it
                    updateAllTimings();
                    if (state == 0)
                    {
                        LED3 = LED_LOW;
                    }
                    else if (state == 255)
                    {
                        LED3 = LED_HIGH;
                    }
                }
                else
                {
                    //phaseCutEnable |= (1 << (LEDID2 - 1));
                    smoothDimActive |= SM_ENABLE_FOR_LED3;
                    setPhaseCut(LEDID3, state);
                }
                arr_led[2].state = state;
                //setPwmDuty(LEDID3, arr_led[2].state);
            }

            break;
        case LEDID4:
            smoothDimActive &= ~SM_ENABLE_FOR_LED4;
            if (arr_led[3].config == 1)
            {
                if (state > 0)
                {
                    arr_led[3].state = 255;
                    LED4 = LED_HIGH;
                }
                else
                {
                    arr_led[3].state = 0;
                    LED4 = LED_LOW;
                }
            }
            else
            {
                if (state == 0 || state == 255)
                {
                    arr_led[3].phaseCutTime = 0;
                    //Update timing array and sort it
                    updateAllTimings();
                    if (state == 0)
                    {
                        LED4 = LED_LOW;
                    }
                    else if (state == 255)
                    {
                        LED4 = LED_HIGH;
                    }
                }
                else
                {
                    //phaseCutEnable |= (1 << (LEDID2 - 1));
                    smoothDimActive |= SM_ENABLE_FOR_LED4;
                    setPhaseCut(LEDID4, state);
                }
                arr_led[3].state = state;
                //setPwmDuty(LEDID4, arr_led[3].state);
            }
            break;
        default:
            break;
        }

        position += 2;
    }
    sendReply[datalen++] = TASK_UART_ACK;
    sendReply[datalen++] = txnId;

    UART_tx(sendReply, datalen);
}

/**
 * @brief       Sets timer to send the switch press information message periodically
 *              until ack received 
 *
 * @param       Message, length of the message
 *
 * @return      Nothing
 *
 */
void retryGettingAck(uint8_t arr[], uint8_t dataLen)
{
    for (int i = 0; i < dataLen; i++)
    {
        retryBuffer[i] = arr[i]; //copies to seperate buffer
    }

    retryDataLen = dataLen; // copies to seperate length var

    retryGettingAckEnabled = 1;
    // Set timer 0 working 1Hz in periodic mode
    // TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);

    // // Enable timer interrupt
    // TIMER_EnableInt(TIMER0);
    // NVIC_EnableIRQ(TMR0_IRQn);

    // // Start Timer 0
    // TIMER_Start(TIMER0);
}

//Sends the switch press info message via uart
void TMR0_IRQHandler(void)
{
    // UART_tx(retryBuffer, retryDataLen);

    // // clear timer interrupt flag
    // TIMER_ClearIntFlag(TIMER0);
    //       timer0Counter++;
    //   if (timer0Counter >= 80)
    //   {
    //     P23 = ~P23;
    //     timer0Counter = 0;
    //   }

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);
    //reset timer 1
    TIMER1->CTL |= TIMER_CTL_RSTCNT_Msk;
    TIMER1->CTL |= TIMER_CTL_CNTEN_Msk;
    //Tx info every 800ms
    if (retryGettingAckEnabled)
    {
        retryTimerCount++;
        if (retryTimerCount >= 80)
        {
            retryTimerCount = 0;
            UART_tx(retryBuffer, retryDataLen);
        }
    }
}

//copies the uart received data into seperate buffer and call taskHandler to perform task
//Extracts commandID and txnID
void parseUart(void)
{
    int i = 0, j = 2;
    commandId = g0_u8RecData[0];
    txnId = g0_u8RecData[1];
    for (i = 0; i < MAX_UART_DATA_BUFFER; i++)
    {
        uartDataBuffer[i] = g0_u8RecData[j];
        j++;
    }

    taskHandler();
}

//Calls the required function as needed by UART received message
void taskHandler(void)
{
    switch (commandId)
    {
    case TASK_CONFIG_SWITCH:
        configSwitch();
        break;
    // case TASK_GET_SWITCH_STATE:
    //     getSwitchState();
    //     break;

    // case TASK_GET_CONFIG:
    //     getConfig();
    //     break;
    case TASK_EXECUTE_SWITCH_STATE:
        executeSwitchState();
        break;

    case TASK_UART_RETRY_ACK:
        uartAck();
        break;

    default:
        break;
    }
}

//At start up configures all switches as normal off/on type
void keus_config_switch_init(void)
{
    arr_led[0].config = CONFIGNORMAL;
    arr_led[1].config = CONFIGNORMAL;
    arr_led[2].config = CONFIGNORMAL;
    arr_led[3].config = CONFIGNORMAL;

    SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
    SYS->P2_MFP |= SYS_MFP_P22_GPIO;
    GPIO_SetMode(P2, BIT2, GPIO_MODE_OUTPUT);
    P22 = LED_LOW;

    SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
    SYS->P2_MFP |= SYS_MFP_P23_GPIO;
    GPIO_SetMode(P2, BIT3, GPIO_MODE_OUTPUT);
    P23 = LED_LOW;

    SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
    SYS->P2_MFP |= SYS_MFP_P24_GPIO;
    GPIO_SetMode(P2, BIT4, GPIO_MODE_OUTPUT);
    P24 = LED_LOW;

    SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
    SYS->P2_MFP |= SYS_MFP_P25_GPIO;
    GPIO_SetMode(P2, BIT5, GPIO_MODE_OUTPUT);
    P25 = LED_LOW;
}

//Sets output state as on-off/dimming/fan
void configSwitch(void)
{
    //Example msg: <no of switch><switchid><config><switchid><config>....
    //Send UART:
    //configSwitch: 28<size><commandid 1><txnid><no of switch><switchid><config><switchid><config>29

    int noOfSwitch = uartDataBuffer[0];
    uint8_t position = 0, switchId, config, sendReply[5], datalen = 0;

    for (int i = 0; i < noOfSwitch; i++)
    {
        switchId = uartDataBuffer[position + 1];
        config = uartDataBuffer[position + 2];

        switch (switchId)
        {
        case LEDID1:
            if (arr_led[0].phaseCutTime && config == 1)
            {
                smoothDimActive &= ~SM_ENABLE_FOR_LED1;
                arr_led[0].phaseCutTime = 0;
                updateAllTimings();
                LED1 = LED_LOW;
            }
            arr_led[0].config = config;
            //setOutputPins(LEDID1, config);
            break;
        case LEDID2:
            if (arr_led[1].phaseCutTime && config == 1)
            {
                smoothDimActive &= ~SM_ENABLE_FOR_LED2;
                arr_led[1].phaseCutTime = 0;
                updateAllTimings();
                LED2 = LED_LOW;
            }
            arr_led[1].config = config;
            //setOutputPins(LEDID2, config);
            break;
        case LEDID3:
            if (arr_led[2].phaseCutTime && config == 1)
            {
                smoothDimActive &= ~SM_ENABLE_FOR_LED3;
                arr_led[2].phaseCutTime = 0;
                updateAllTimings();
                LED3 = LED_LOW;
            }
            arr_led[2].config = config;
            //setOutputPins(LEDID3, config);
            break;
        case LEDID4:
            if (arr_led[3].phaseCutTime && config == 1)
            {
                smoothDimActive &= ~SM_ENABLE_FOR_LED4;
                arr_led[3].phaseCutTime = 0;
                updateAllTimings();
                LED4 = LED_LOW;
            }
            arr_led[3].config = config;
            //setOutputPins(LEDID4, config);
            break;
        default:
            break;
        }

        position += 2;
    }
    sendReply[datalen++] = TASK_UART_ACK;
    sendReply[datalen++] = txnId;

    UART_tx(sendReply, datalen);
}

// //Sends all switch state info via uart
// void getSwitchState(void)
// {
//     //Example msg: <replyID><txnID><no of switch><switchid><state><switchid><state>....
//     //Send UART: 28<size><commandid 2><txnid>29
//     //28 02 02 04 29

//     uint8_t sendState[15], position = 3, datalen = 0;
//     sendState[0] = GETSWITCHSTATEREPLY;
//     datalen++;
//     sendState[1] = txnId;
//     datalen++;
//     sendState[2] = MAX_NUMBER_LED;
//     datalen++;
//     for (int i = 0; i < MAX_NUMBER_LED; i++)
//     {
//         sendState[position] = i + 1; //switch ID
//         datalen++;
//         sendState[position + 1] = arr_led[i].state;
//         datalen++;
//         position += 2;
//     }

//     UART_tx(sendState, datalen);
// }

// //Sends all switch config info via uart
// void getConfig(void)
// {
//     //Example msg: <replyID><txnID><no of switch><switchid><config><switchid><config>....
//     //Send UART: 28<size><commandid 2><txnid>29
//     //28 02 03 04 29

//     uint8_t sendState[15], position = 3, datalen = 0;
//     sendState[datalen++] = GETCONFIGREPLY;
//     sendState[datalen++] = txnId;
//     sendState[datalen++] = MAX_NUMBER_LED;
//     for (int i = 0; i < MAX_NUMBER_LED; i++)
//     {
//         sendState[position] = i + 1; //switch ID
//         datalen++;
//         sendState[position + 1] = arr_led[i].config;
//         datalen++;
//         position += 2;
//     }

//     UART_tx(sendState, datalen);
// }

//Stopes the timer counter which sends switch press info periodically on getting ack
void uartAck(void)
{
    if (txnId == retryBuffer[1])
    {
        //TIMER_Close(TIMER0);
        retryGettingAckEnabled = 0;
        retryTimerCount = 0;
    }
}

void updateTxnId(void)
{
    // if (txnId == 255)
    //     txnId = 0;
    // txnId++;

    //As requested by Monisha, txn fixed to 1
    txnId = 1;
}
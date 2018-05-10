/***************************************************************************//**
* \file Serial_SPI_UART_INT.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in SPI and UART modes.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Serial_PVT.h"
#include "Serial_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (Serial_SCB_IRQ_INTERNAL)
/*******************************************************************************
* Function Name: Serial_SPI_UART_ISR
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the SCB SPI or UART modes.
*
*******************************************************************************/
CY_ISR(Serial_SPI_UART_ISR)
{
#if (Serial_INTERNAL_RX_SW_BUFFER_CONST)
    uint32 locHead;
#endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

#if (Serial_INTERNAL_TX_SW_BUFFER_CONST)
    uint32 locTail;
#endif /* (Serial_INTERNAL_TX_SW_BUFFER_CONST) */

#ifdef Serial_SPI_UART_ISR_ENTRY_CALLBACK
    Serial_SPI_UART_ISR_EntryCallback();
#endif /* Serial_SPI_UART_ISR_ENTRY_CALLBACK */

    if (NULL != Serial_customIntrHandler)
    {
        Serial_customIntrHandler();
    }

    #if(Serial_CHECK_SPI_WAKE_ENABLE)
    {
        /* Clear SPI wakeup source */
        Serial_ClearSpiExtClkInterruptSource(Serial_INTR_SPI_EC_WAKE_UP);
    }
    #endif

    #if (Serial_CHECK_RX_SW_BUFFER)
    {
        if (Serial_CHECK_INTR_RX_MASKED(Serial_INTR_RX_NOT_EMPTY))
        {
            do
            {
                /* Move local head index */
                locHead = (Serial_rxBufferHead + 1u);

                /* Adjust local head index */
                if (Serial_INTERNAL_RX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                if (locHead == Serial_rxBufferTail)
                {
                    #if (Serial_CHECK_UART_RTS_CONTROL_FLOW)
                    {
                        /* There is no space in the software buffer - disable the
                        * RX Not Empty interrupt source. The data elements are
                        * still being received into the RX FIFO until the RTS signal
                        * stops the transmitter. After the data element is read from the
                        * buffer, the RX Not Empty interrupt source is enabled to
                        * move the next data element in the software buffer.
                        */
                        Serial_INTR_RX_MASK_REG &= ~Serial_INTR_RX_NOT_EMPTY;
                        break;
                    }
                    #else
                    {
                        /* Overflow: through away received data element */
                        (void) Serial_RX_FIFO_RD_REG;
                        Serial_rxBufferOverflow = (uint8) Serial_INTR_RX_OVERFLOW;
                    }
                    #endif
                }
                else
                {
                    /* Store received data */
                    Serial_PutWordInRxBuffer(locHead, Serial_RX_FIFO_RD_REG);

                    /* Move head index */
                    Serial_rxBufferHead = locHead;
                }
            }
            while(0u != Serial_GET_RX_FIFO_ENTRIES);

            Serial_ClearRxInterruptSource(Serial_INTR_RX_NOT_EMPTY);
        }
    }
    #endif


    #if (Serial_CHECK_TX_SW_BUFFER)
    {
        if (Serial_CHECK_INTR_TX_MASKED(Serial_INTR_TX_NOT_FULL))
        {
            do
            {
                /* Check for room in TX software buffer */
                if (Serial_txBufferHead != Serial_txBufferTail)
                {
                    /* Move local tail index */
                    locTail = (Serial_txBufferTail + 1u);

                    /* Adjust local tail index */
                    if (Serial_TX_BUFFER_SIZE == locTail)
                    {
                        locTail = 0u;
                    }

                    /* Put data into TX FIFO */
                    Serial_TX_FIFO_WR_REG = Serial_GetWordFromTxBuffer(locTail);

                    /* Move tail index */
                    Serial_txBufferTail = locTail;
                }
                else
                {
                    /* TX software buffer is empty: complete transfer */
                    Serial_DISABLE_INTR_TX(Serial_INTR_TX_NOT_FULL);
                    break;
                }
            }
            while (Serial_SPI_UART_FIFO_SIZE != Serial_GET_TX_FIFO_ENTRIES);

            Serial_ClearTxInterruptSource(Serial_INTR_TX_NOT_FULL);
        }
    }
    #endif

#ifdef Serial_SPI_UART_ISR_EXIT_CALLBACK
    Serial_SPI_UART_ISR_ExitCallback();
#endif /* Serial_SPI_UART_ISR_EXIT_CALLBACK */

}

#endif /* (Serial_SCB_IRQ_INTERNAL) */


/* [] END OF FILE */

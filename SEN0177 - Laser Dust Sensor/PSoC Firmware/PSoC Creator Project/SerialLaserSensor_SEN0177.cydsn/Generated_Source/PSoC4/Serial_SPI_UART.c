/*******************************************************************************
* File Name: Serial_SPI_UART.c
* Version 3.10
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  SPI and UART modes.
*
* Note:
*
*******************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Serial_PVT.h"
#include "Serial_SPI_UART_PVT.h"

/***************************************
*        SPI/UART Private Vars
***************************************/

#if(Serial_INTERNAL_RX_SW_BUFFER_CONST)
    volatile uint32 Serial_rxBufferHead;
    volatile uint32 Serial_rxBufferTail;
    volatile uint8  Serial_rxBufferOverflow;
#endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

#if(Serial_INTERNAL_TX_SW_BUFFER_CONST)
    volatile uint32 Serial_txBufferHead;
    volatile uint32 Serial_txBufferTail;
#endif /* (Serial_INTERNAL_TX_SW_BUFFER_CONST) */

#if(Serial_INTERNAL_RX_SW_BUFFER)
    /* Add one element to the buffer to receive full packet. One byte in receive buffer is always empty */
    volatile uint8 Serial_rxBufferInternal[Serial_INTERNAL_RX_BUFFER_SIZE];
#endif /* (Serial_INTERNAL_RX_SW_BUFFER) */

#if(Serial_INTERNAL_TX_SW_BUFFER)
    volatile uint8 Serial_txBufferInternal[Serial_TX_BUFFER_SIZE];
#endif /* (Serial_INTERNAL_TX_SW_BUFFER) */


#if(Serial_RX_DIRECTION)
    /*******************************************************************************
    * Function Name: Serial_SpiUartReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Retrieves the next data element from the receive buffer.
    *   - RX software buffer is disabled: Returns data element retrieved from
    *     RX FIFO. Undefined data will be returned if the RX FIFO is empty.
    *   - RX software buffer is enabled: Returns data element from the software
    *     receive buffer. Zero value is returned if the software receive buffer
    *     is empty.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Next data element from the receive buffer.
    *
    * Global Variables:
    *  Look into Serial_SpiInit for description.
    *
    *******************************************************************************/
    uint32 Serial_SpiUartReadRxData(void)
    {
        uint32 rxData = 0u;

    #if (Serial_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (Serial_CHECK_RX_SW_BUFFER)
        {
            if (Serial_rxBufferHead != Serial_rxBufferTail)
            {
                /* There is data in RX software buffer */

                /* Calculate index to read from */
                locTail = (Serial_rxBufferTail + 1u);

                if (Serial_INTERNAL_RX_BUFFER_SIZE == locTail)
                {
                    locTail = 0u;
                }

                /* Get data from RX software buffer */
                rxData = Serial_GetWordFromRxBuffer(locTail);

                /* Change index in the buffer */
                Serial_rxBufferTail = locTail;

                #if (Serial_CHECK_UART_RTS_CONTROL_FLOW)
                {
                    /* Check if RX Not Empty is disabled in the interrupt */
                    if (0u == (Serial_INTR_RX_MASK_REG & Serial_INTR_RX_NOT_EMPTY))
                    {
                        /* Enable RX Not Empty interrupt source to continue
                        * receiving data into software buffer.
                        */
                        Serial_INTR_RX_MASK_REG |= Serial_INTR_RX_NOT_EMPTY;
                    }
                }
                #endif

            }
        }
        #else
        {
            /* Read data from RX FIFO */
            rxData = Serial_RX_FIFO_RD_REG;
        }
        #endif

        return (rxData);
    }


    /*******************************************************************************
    * Function Name: Serial_SpiUartGetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received data elements in the receive buffer.
    *   - RX software buffer disabled: returns the number of used entries in
    *     RX FIFO.
    *   - RX software buffer enabled: returns the number of elements which were
    *     placed in the receive buffer. This does not include the hardware RX FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Number of received data elements
    *
    *******************************************************************************/
    uint32 Serial_SpiUartGetRxBufferSize(void)
    {
        uint32 size;
    #if (Serial_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (Serial_CHECK_RX_SW_BUFFER)
        {
            locHead = Serial_rxBufferHead;

            if(locHead >= Serial_rxBufferTail)
            {
                size = (locHead - Serial_rxBufferTail);
            }
            else
            {
                size = (locHead + (Serial_INTERNAL_RX_BUFFER_SIZE - Serial_rxBufferTail));
            }
        }
        #else
        {
            size = Serial_GET_RX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: Serial_SpiUartClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receive buffer and RX FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_SpiUartClearRxBuffer(void)
    {
        #if (Serial_CHECK_RX_SW_BUFFER)
        {
            /* Lock from component interruption */
            Serial_DisableInt();

            /* Flush RX software buffer */
            Serial_rxBufferHead = Serial_rxBufferTail;
            Serial_rxBufferOverflow = 0u;

            Serial_CLEAR_RX_FIFO;
            Serial_ClearRxInterruptSource(Serial_INTR_RX_ALL);

            #if (Serial_CHECK_UART_RTS_CONTROL_FLOW)
            {
                /* Enable RX Not Empty interrupt source to continue receiving
                * data into software buffer.
                */
                Serial_INTR_RX_MASK_REG |= Serial_INTR_RX_NOT_EMPTY;
            }
            #endif

            /* Release lock */
            Serial_EnableInt();
        }
        #else
        {
            Serial_CLEAR_RX_FIFO;
        }
        #endif
    }

#endif /* (Serial_RX_DIRECTION) */


#if(Serial_TX_DIRECTION)
    /*******************************************************************************
    * Function Name: Serial_SpiUartWriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a data entry into the transmit buffer to be sent at the next available
    *  bus time.
    *  This function is blocking and waits until there is space available to put the
    *  requested data in the transmit buffer.
    *
    * Parameters:
    *  txDataByte: the data to be transmitted.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_SpiUartWriteTxData(uint32 txData)
    {
    #if (Serial_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (Serial_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (Serial_CHECK_TX_SW_BUFFER)
        {
            /* Put data directly into the TX FIFO */
            if ((Serial_txBufferHead == Serial_txBufferTail) &&
                (Serial_SPI_UART_FIFO_SIZE != Serial_GET_TX_FIFO_ENTRIES))
            {
                /* TX software buffer is empty: put data directly in TX FIFO */
                Serial_TX_FIFO_WR_REG = txData;
            }
            /* Put data into TX software buffer */
            else
            {
                /* Head index to put data */
                locHead = (Serial_txBufferHead + 1u);

                /* Adjust TX software buffer index */
                if (Serial_TX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                /* Wait for space in TX software buffer */
                while (locHead == Serial_txBufferTail)
                {
                }

                /* TX software buffer has at least one room */

                /* Clear old status of INTR_TX_NOT_FULL. It sets at the end of transfer when TX FIFO is empty. */
                Serial_ClearTxInterruptSource(Serial_INTR_TX_NOT_FULL);

                Serial_PutWordInTxBuffer(locHead, txData);

                Serial_txBufferHead = locHead;

                /* Check if TX Not Full is disabled in interrupt */
                if (0u == (Serial_INTR_TX_MASK_REG & Serial_INTR_TX_NOT_FULL))
                {
                    /* Enable TX Not Full interrupt source to transmit from software buffer */
                    Serial_INTR_TX_MASK_REG |= (uint32) Serial_INTR_TX_NOT_FULL;
                }
            }
        }
        #else
        {
            /* Wait until TX FIFO has space to put data element */
            while (Serial_SPI_UART_FIFO_SIZE == Serial_GET_TX_FIFO_ENTRIES)
            {
            }

            Serial_TX_FIFO_WR_REG = txData;
        }
        #endif
    }


    /*******************************************************************************
    * Function Name: Serial_SpiUartPutArray
    ********************************************************************************
    *
    * Summary:
    *  Places an array of data into the transmit buffer to be sent.
    *  This function is blocking and waits until there is a space available to put
    *  all the requested data in the transmit buffer. The array size can be greater
    *  than transmit buffer size.
    *
    * Parameters:
    *  wrBuf:  pointer to an array with data to be placed in transmit buffer.
    *  count:  number of data elements to be placed in the transmit buffer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_SpiUartPutArray(const uint8 wrBuf[], uint32 count)
    {
        uint32 i;

        for (i=0u; i < count; i++)
        {
            Serial_SpiUartWriteTxData((uint32) wrBuf[i]);
        }
    }


    /*******************************************************************************
    * Function Name: Serial_SpiUartGetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    * Returns the number of elements currently in the transmit buffer.
    *  - TX software buffer is disabled: returns the number of used entries in
    *    TX FIFO.
    *  - TX software buffer is enabled: returns the number of elements currently
    *    used in the transmit buffer. This number does not include used entries in
    *    the TX FIFO. The transmit buffer size is zero until the TX FIFO is
    *    not full.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Number of data elements ready to transmit.
    *
    *******************************************************************************/
    uint32 Serial_SpiUartGetTxBufferSize(void)
    {
        uint32 size;
    #if (Serial_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (Serial_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (Serial_CHECK_TX_SW_BUFFER)
        {
            /* Get current Tail index */
            locTail = Serial_txBufferTail;

            if (Serial_txBufferHead >= locTail)
            {
                size = (Serial_txBufferHead - locTail);
            }
            else
            {
                size = (Serial_txBufferHead + (Serial_TX_BUFFER_SIZE - locTail));
            }
        }
        #else
        {
            size = Serial_GET_TX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: Serial_SpiUartClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the transmit buffer and TX FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_SpiUartClearTxBuffer(void)
    {
        #if (Serial_CHECK_TX_SW_BUFFER)
        {
            /* Lock from component interruption */
            Serial_DisableInt();

            /* Flush TX software buffer */
            Serial_txBufferHead = Serial_txBufferTail;

            Serial_INTR_TX_MASK_REG &= (uint32) ~Serial_INTR_TX_NOT_FULL;
            Serial_CLEAR_TX_FIFO;
            Serial_ClearTxInterruptSource(Serial_INTR_TX_ALL);

            /* Release lock */
            Serial_EnableInt();
        }
        #else
        {
            Serial_CLEAR_TX_FIFO;
        }
        #endif
    }

#endif /* (Serial_TX_DIRECTION) */


/*******************************************************************************
* Function Name: Serial_SpiUartDisableIntRx
********************************************************************************
*
* Summary:
*  Disables the RX interrupt sources.
*
* Parameters:
*  None
*
* Return:
*  Returns the RX interrupt sources enabled before the function call.
*
*******************************************************************************/
uint32 Serial_SpiUartDisableIntRx(void)
{
    uint32 intSource;

    intSource = Serial_GetRxInterruptMode();

    Serial_SetRxInterruptMode(Serial_NO_INTR_SOURCES);

    return (intSource);
}


/*******************************************************************************
* Function Name: Serial_SpiUartDisableIntTx
********************************************************************************
*
* Summary:
*  Disables TX interrupt sources.
*
* Parameters:
*  None
*
* Return:
*  Returns TX interrupt sources enabled before function call.
*
*******************************************************************************/
uint32 Serial_SpiUartDisableIntTx(void)
{
    uint32 intSourceMask;

    intSourceMask = Serial_GetTxInterruptMode();

    Serial_SetTxInterruptMode(Serial_NO_INTR_SOURCES);

    return (intSourceMask);
}


#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: Serial_PutWordInRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Stores a byte/word into the RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    * Parameters:
    *  index:      index to store data byte/word in the RX buffer.
    *  rxDataByte: byte/word to store.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_PutWordInRxBuffer(uint32 idx, uint32 rxDataByte)
    {
        /* Put data in buffer */
        if (Serial_ONE_BYTE_WIDTH == Serial_rxDataBits)
        {
            Serial_rxBuffer[idx] = ((uint8) rxDataByte);
        }
        else
        {
            Serial_rxBuffer[(uint32)(idx << 1u)]      = LO8(LO16(rxDataByte));
            Serial_rxBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(rxDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: Serial_GetWordFromRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Reads byte/word from RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Returns byte/word read from RX buffer.
    *
    *******************************************************************************/
    uint32 Serial_GetWordFromRxBuffer(uint32 idx)
    {
        uint32 value;

        if (Serial_ONE_BYTE_WIDTH == Serial_rxDataBits)
        {
            value = Serial_rxBuffer[idx];
        }
        else
        {
            value  = (uint32) Serial_rxBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32)Serial_rxBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }


    /*******************************************************************************
    * Function Name: Serial_PutWordInTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Stores byte/word into the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    * Parameters:
    *  idx:        index to store data byte/word in the TX buffer.
    *  txDataByte: byte/word to store.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_PutWordInTxBuffer(uint32 idx, uint32 txDataByte)
    {
        /* Put data in buffer */
        if (Serial_ONE_BYTE_WIDTH == Serial_txDataBits)
        {
            Serial_txBuffer[idx] = ((uint8) txDataByte);
        }
        else
        {
            Serial_txBuffer[(uint32)(idx << 1u)]      = LO8(LO16(txDataByte));
            Serial_txBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(txDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: Serial_GetWordFromTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Reads byte/word from the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    * Parameters:
    *  idx: index to get data byte/word from the TX buffer.
    *
    * Return:
    *  Returns byte/word read from the TX buffer.
    *
    *******************************************************************************/
    uint32 Serial_GetWordFromTxBuffer(uint32 idx)
    {
        uint32 value;

        if (Serial_ONE_BYTE_WIDTH == Serial_txDataBits)
        {
            value = (uint32) Serial_txBuffer[idx];
        }
        else
        {
            value  = (uint32) Serial_txBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32) Serial_txBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */


/* [] END OF FILE */

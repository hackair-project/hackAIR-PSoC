/***************************************************************************//**
* \file Serial_SPI_UART.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  SPI and UART modes.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
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
    /* Start index to put data into the software receive buffer.*/
    volatile uint32 Serial_rxBufferHead;
    /* Start index to get data from the software receive buffer.*/
    volatile uint32 Serial_rxBufferTail;
    /**
    * \addtogroup group_globals
    * \{
    */
    /** Sets when internal software receive buffer overflow
    *  was occurred.
    */
    volatile uint8  Serial_rxBufferOverflow;
    /** \} globals */
#endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

#if(Serial_INTERNAL_TX_SW_BUFFER_CONST)
    /* Start index to put data into the software transmit buffer.*/
    volatile uint32 Serial_txBufferHead;
    /* Start index to get data from the software transmit buffer.*/
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
    ****************************************************************************//**
    *
    *  Retrieves the next data element from the receive buffer.
    *   - RX software buffer is disabled: Returns data element retrieved from
    *     RX FIFO. Undefined data will be returned if the RX FIFO is empty.
    *   - RX software buffer is enabled: Returns data element from the software
    *     receive buffer. Zero value is returned if the software receive buffer
    *     is empty.
    *
    * \return
    *  Next data element from the receive buffer. 
    *  The amount of data bits to be received depends on RX data bits selection 
    *  (the data bit counting starts from LSB of return value).
    *
    * \globalvars
    *  Serial_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  Serial_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
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
    ****************************************************************************//**
    *
    *  Returns the number of received data elements in the receive buffer.
    *   - RX software buffer disabled: returns the number of used entries in
    *     RX FIFO.
    *   - RX software buffer enabled: returns the number of elements which were
    *     placed in the receive buffer. This does not include the hardware RX FIFO.
    *
    * \return
    *  Number of received data elements.
    *
    * \globalvars
    *  Serial_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  Serial_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
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
    ****************************************************************************//**
    *
    *  Clears the receive buffer and RX FIFO.
    *
    * \globalvars
    *  Serial_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  Serial_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
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
    ****************************************************************************//**
    *
    *  Places a data entry into the transmit buffer to be sent at the next available
    *  bus time.
    *  This function is blocking and waits until there is space available to put the
    *  requested data in the transmit buffer.
    *
    *  \param txDataByte: the data to be transmitted.
    *   The amount of data bits to be transmitted depends on TX data bits selection 
    *   (the data bit counting starts from LSB of txDataByte).
    *
    * \globalvars
    *  Serial_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Serial_txBufferTail - start index to get data from the software
    *  transmit buffer.
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
    ****************************************************************************//**
    *
    *  Places an array of data into the transmit buffer to be sent.
    *  This function is blocking and waits until there is a space available to put
    *  all the requested data in the transmit buffer. The array size can be greater
    *  than transmit buffer size.
    *
    * \param wrBuf: pointer to an array of data to be placed in transmit buffer. 
    *  The width of the data to be transmitted depends on TX data width selection 
    *  (the data bit counting starts from LSB for each array element).
    * \param count: number of data elements to be placed in the transmit buffer.
    *
    * \globalvars
    *  Serial_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Serial_txBufferTail - start index to get data from the software
    *  transmit buffer.
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
    ****************************************************************************//**
    *
    *  Returns the number of elements currently in the transmit buffer.
    *   - TX software buffer is disabled: returns the number of used entries in
    *     TX FIFO.
    *   - TX software buffer is enabled: returns the number of elements currently
    *     used in the transmit buffer. This number does not include used entries in
    *     the TX FIFO. The transmit buffer size is zero until the TX FIFO is
    *     not full.
    *
    * \return
    *  Number of data elements ready to transmit.
    *
    * \globalvars
    *  Serial_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Serial_txBufferTail - start index to get data from the software
    *  transmit buffer.
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
    ****************************************************************************//**
    *
    *  Clears the transmit buffer and TX FIFO.
    *
    * \globalvars
    *  Serial_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Serial_txBufferTail - start index to get data from the software
    *  transmit buffer.
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
****************************************************************************//**
*
*  Disables the RX interrupt sources.
*
*  \return
*   Returns the RX interrupt sources enabled before the function call.
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
****************************************************************************//**
*
*  Disables TX interrupt sources.
*
*  \return
*   Returns TX interrupt sources enabled before function call.
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
    ****************************************************************************//**
    *
    *  Stores a byte/word into the RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param index:      index to store data byte/word in the RX buffer.
    *  \param rxDataByte: byte/word to store.
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
    ****************************************************************************//**
    *
    *  Reads byte/word from RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \return
    *   Returns byte/word read from RX buffer.
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
    ****************************************************************************//**
    *
    *  Stores byte/word into the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx:        index to store data byte/word in the TX buffer.
    *  \param txDataByte: byte/word to store.
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
    ****************************************************************************//**
    *
    *  Reads byte/word from the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx: index to get data byte/word from the TX buffer.
    *
    *  \return
    *   Returns byte/word read from the TX buffer.
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

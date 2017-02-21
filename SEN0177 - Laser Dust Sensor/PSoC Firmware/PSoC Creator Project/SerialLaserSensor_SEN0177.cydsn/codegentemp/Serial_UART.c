/*******************************************************************************
* File Name: Serial_UART.c
* Version 3.10
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  UART mode.
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
#include "cyapicallbacks.h"

#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    const Serial_UART_INIT_STRUCT Serial_configUart =
    {
        Serial_UART_SUB_MODE,
        Serial_UART_DIRECTION,
        Serial_UART_DATA_BITS_NUM,
        Serial_UART_PARITY_TYPE,
        Serial_UART_STOP_BITS_NUM,
        Serial_UART_OVS_FACTOR,
        Serial_UART_IRDA_LOW_POWER,
        Serial_UART_MEDIAN_FILTER_ENABLE,
        Serial_UART_RETRY_ON_NACK,
        Serial_UART_IRDA_POLARITY,
        Serial_UART_DROP_ON_PARITY_ERR,
        Serial_UART_DROP_ON_FRAME_ERR,
        Serial_UART_WAKE_ENABLE,
        0u,
        NULL,
        0u,
        NULL,
        Serial_UART_MP_MODE_ENABLE,
        Serial_UART_MP_ACCEPT_ADDRESS,
        Serial_UART_MP_RX_ADDRESS,
        Serial_UART_MP_RX_ADDRESS_MASK,
        (uint32) Serial_SCB_IRQ_INTERNAL,
        Serial_UART_INTR_RX_MASK,
        Serial_UART_RX_TRIGGER_LEVEL,
        Serial_UART_INTR_TX_MASK,
        Serial_UART_TX_TRIGGER_LEVEL,
        (uint8) Serial_UART_BYTE_MODE_ENABLE,
        (uint8) Serial_UART_CTS_ENABLE,
        (uint8) Serial_UART_CTS_POLARITY,
        (uint8) Serial_UART_RTS_POLARITY,
        (uint8) Serial_UART_RTS_FIFO_LEVEL
    };


    /*******************************************************************************
    * Function Name: Serial_UartInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the UART operation.
    *
    * Parameters:
    *  config:  Pointer to a structure that contains the following ordered list of
    *           fields. These fields match the selections available in the
    *           customizer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_UartInit(const Serial_UART_INIT_STRUCT *config)
    {
        uint32 pinsConfig;

        if (NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Get direction to configure UART pins: TX, RX or TX+RX */
            pinsConfig  = config->direction;

        #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
            /* Add RTS and CTS pins to configure */
            pinsConfig |= (0u != config->rtsRxFifoLevel) ? (Serial_UART_RTS_PIN_ENABLE) : (0u);
            pinsConfig |= (0u != config->enableCts)      ? (Serial_UART_CTS_PIN_ENABLE) : (0u);
        #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

            /* Configure pins */
            Serial_SetPins(Serial_SCB_MODE_UART, config->mode, pinsConfig);

            /* Store internal configuration */
            Serial_scbMode       = (uint8) Serial_SCB_MODE_UART;
            Serial_scbEnableWake = (uint8) config->enableWake;
            Serial_scbEnableIntr = (uint8) config->enableInterrupt;

            /* Set RX direction internal variables */
            Serial_rxBuffer      =         config->rxBuffer;
            Serial_rxDataBits    = (uint8) config->dataBits;
            Serial_rxBufferSize  = (uint8) config->rxBufferSize;

            /* Set TX direction internal variables */
            Serial_txBuffer      =         config->txBuffer;
            Serial_txDataBits    = (uint8) config->dataBits;
            Serial_txBufferSize  = (uint8) config->txBufferSize;

            /* Configure UART interface */
            if(Serial_UART_MODE_IRDA == config->mode)
            {
                /* OVS settings: IrDA */
                Serial_CTRL_REG  = ((0u != config->enableIrdaLowPower) ?
                                                (Serial_UART_GET_CTRL_OVS_IRDA_LP(config->oversample)) :
                                                (Serial_CTRL_OVS_IRDA_OVS16));
            }
            else
            {
                /* OVS settings: UART and SmartCard */
                Serial_CTRL_REG  = Serial_GET_CTRL_OVS(config->oversample);
            }

            Serial_CTRL_REG     |= Serial_GET_CTRL_BYTE_MODE  (config->enableByteMode)      |
                                             Serial_GET_CTRL_ADDR_ACCEPT(config->multiprocAcceptAddr) |
                                             Serial_CTRL_UART;

            /* Configure sub-mode: UART, SmartCard or IrDA */
            Serial_UART_CTRL_REG = Serial_GET_UART_CTRL_MODE(config->mode);

            /* Configure RX direction */
            Serial_UART_RX_CTRL_REG = Serial_GET_UART_RX_CTRL_MODE(config->stopBits)              |
                                        Serial_GET_UART_RX_CTRL_POLARITY(config->enableInvertedRx)          |
                                        Serial_GET_UART_RX_CTRL_MP_MODE(config->enableMultiproc)            |
                                        Serial_GET_UART_RX_CTRL_DROP_ON_PARITY_ERR(config->dropOnParityErr) |
                                        Serial_GET_UART_RX_CTRL_DROP_ON_FRAME_ERR(config->dropOnFrameErr);

            if(Serial_UART_PARITY_NONE != config->parity)
            {
               Serial_UART_RX_CTRL_REG |= Serial_GET_UART_RX_CTRL_PARITY(config->parity) |
                                                    Serial_UART_RX_CTRL_PARITY_ENABLED;
            }

            Serial_RX_CTRL_REG      = Serial_GET_RX_CTRL_DATA_WIDTH(config->dataBits)       |
                                                Serial_GET_RX_CTRL_MEDIAN(config->enableMedianFilter) |
                                                Serial_GET_UART_RX_CTRL_ENABLED(config->direction);

            Serial_RX_FIFO_CTRL_REG = Serial_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(config->rxTriggerLevel);

            /* Configure MP address */
            Serial_RX_MATCH_REG     = Serial_GET_RX_MATCH_ADDR(config->multiprocAddr) |
                                                Serial_GET_RX_MATCH_MASK(config->multiprocAddrMask);

            /* Configure RX direction */
            Serial_UART_TX_CTRL_REG = Serial_GET_UART_TX_CTRL_MODE(config->stopBits) |
                                                Serial_GET_UART_TX_CTRL_RETRY_NACK(config->enableRetryNack);

            if(Serial_UART_PARITY_NONE != config->parity)
            {
               Serial_UART_TX_CTRL_REG |= Serial_GET_UART_TX_CTRL_PARITY(config->parity) |
                                                    Serial_UART_TX_CTRL_PARITY_ENABLED;
            }

            Serial_TX_CTRL_REG      = Serial_GET_TX_CTRL_DATA_WIDTH(config->dataBits)    |
                                                Serial_GET_UART_TX_CTRL_ENABLED(config->direction);

            Serial_TX_FIFO_CTRL_REG = Serial_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(config->txTriggerLevel);

        #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
            Serial_UART_FLOW_CTRL_REG = Serial_GET_UART_FLOW_CTRL_CTS_ENABLE(config->enableCts) | \
                                            Serial_GET_UART_FLOW_CTRL_CTS_POLARITY (config->ctsPolarity)  | \
                                            Serial_GET_UART_FLOW_CTRL_RTS_POLARITY (config->rtsPolarity)  | \
                                            Serial_GET_UART_FLOW_CTRL_TRIGGER_LEVEL(config->rtsRxFifoLevel);
        #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

            /* Configure interrupt with UART handler but do not enable it */
            CyIntDisable    (Serial_ISR_NUMBER);
            CyIntSetPriority(Serial_ISR_NUMBER, Serial_ISR_PRIORITY);
            (void) CyIntSetVector(Serial_ISR_NUMBER, &Serial_SPI_UART_ISR);

            /* Configure WAKE interrupt */
        #if(Serial_UART_RX_WAKEUP_IRQ)
            CyIntDisable    (Serial_RX_WAKE_ISR_NUMBER);
            CyIntSetPriority(Serial_RX_WAKE_ISR_NUMBER, Serial_RX_WAKE_ISR_PRIORITY);
            (void) CyIntSetVector(Serial_RX_WAKE_ISR_NUMBER, &Serial_UART_WAKEUP_ISR);
        #endif /* (Serial_UART_RX_WAKEUP_IRQ) */

            /* Configure interrupt sources */
            Serial_INTR_I2C_EC_MASK_REG = Serial_NO_INTR_SOURCES;
            Serial_INTR_SPI_EC_MASK_REG = Serial_NO_INTR_SOURCES;
            Serial_INTR_SLAVE_MASK_REG  = Serial_NO_INTR_SOURCES;
            Serial_INTR_MASTER_MASK_REG = Serial_NO_INTR_SOURCES;
            Serial_INTR_RX_MASK_REG     = config->rxInterruptMask;
            Serial_INTR_TX_MASK_REG     = config->txInterruptMask;

            /* Clear RX buffer indexes */
            Serial_rxBufferHead     = 0u;
            Serial_rxBufferTail     = 0u;
            Serial_rxBufferOverflow = 0u;

            /* Clear TX buffer indexes */
            Serial_txBufferHead = 0u;
            Serial_txBufferTail = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: Serial_UartInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the UART operation.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_UartInit(void)
    {
        /* Configure UART interface */
        Serial_CTRL_REG = Serial_UART_DEFAULT_CTRL;

        /* Configure sub-mode: UART, SmartCard or IrDA */
        Serial_UART_CTRL_REG = Serial_UART_DEFAULT_UART_CTRL;

        /* Configure RX direction */
        Serial_UART_RX_CTRL_REG = Serial_UART_DEFAULT_UART_RX_CTRL;
        Serial_RX_CTRL_REG      = Serial_UART_DEFAULT_RX_CTRL;
        Serial_RX_FIFO_CTRL_REG = Serial_UART_DEFAULT_RX_FIFO_CTRL;
        Serial_RX_MATCH_REG     = Serial_UART_DEFAULT_RX_MATCH_REG;

        /* Configure TX direction */
        Serial_UART_TX_CTRL_REG = Serial_UART_DEFAULT_UART_TX_CTRL;
        Serial_TX_CTRL_REG      = Serial_UART_DEFAULT_TX_CTRL;
        Serial_TX_FIFO_CTRL_REG = Serial_UART_DEFAULT_TX_FIFO_CTRL;

    #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
        Serial_UART_FLOW_CTRL_REG = Serial_UART_DEFAULT_FLOW_CTRL;
    #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

        /* Configure interrupt with UART handler but do not enable it */
    #if(Serial_SCB_IRQ_INTERNAL)
        CyIntDisable    (Serial_ISR_NUMBER);
        CyIntSetPriority(Serial_ISR_NUMBER, Serial_ISR_PRIORITY);
        (void) CyIntSetVector(Serial_ISR_NUMBER, &Serial_SPI_UART_ISR);
    #endif /* (Serial_SCB_IRQ_INTERNAL) */

        /* Configure WAKE interrupt */
    #if(Serial_UART_RX_WAKEUP_IRQ)
        CyIntDisable    (Serial_RX_WAKE_ISR_NUMBER);
        CyIntSetPriority(Serial_RX_WAKE_ISR_NUMBER, Serial_RX_WAKE_ISR_PRIORITY);
        (void) CyIntSetVector(Serial_RX_WAKE_ISR_NUMBER, &Serial_UART_WAKEUP_ISR);
    #endif /* (Serial_UART_RX_WAKEUP_IRQ) */

        /* Configure interrupt sources */
        Serial_INTR_I2C_EC_MASK_REG = Serial_UART_DEFAULT_INTR_I2C_EC_MASK;
        Serial_INTR_SPI_EC_MASK_REG = Serial_UART_DEFAULT_INTR_SPI_EC_MASK;
        Serial_INTR_SLAVE_MASK_REG  = Serial_UART_DEFAULT_INTR_SLAVE_MASK;
        Serial_INTR_MASTER_MASK_REG = Serial_UART_DEFAULT_INTR_MASTER_MASK;
        Serial_INTR_RX_MASK_REG     = Serial_UART_DEFAULT_INTR_RX_MASK;
        Serial_INTR_TX_MASK_REG     = Serial_UART_DEFAULT_INTR_TX_MASK;

    #if(Serial_INTERNAL_RX_SW_BUFFER_CONST)
        Serial_rxBufferHead     = 0u;
        Serial_rxBufferTail     = 0u;
        Serial_rxBufferOverflow = 0u;
    #endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

    #if(Serial_INTERNAL_TX_SW_BUFFER_CONST)
        Serial_txBufferHead = 0u;
        Serial_txBufferTail = 0u;
    #endif /* (Serial_INTERNAL_TX_SW_BUFFER_CONST) */
    }
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: Serial_UartPostEnable
********************************************************************************
*
* Summary:
*  Restores HSIOM settings for the UART output pins (TX and/or RTS) to be 
*  controlled by the SCB UART.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Serial_UartPostEnable(void)
{
#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)

#if (Serial_TX_SCL_MISO_PIN)
    if (Serial_CHECK_TX_SCL_MISO_PIN_USED)
    {
        /* Set SCB UART to drive the output pin */
        Serial_SET_HSIOM_SEL(Serial_TX_SCL_MISO_HSIOM_REG, Serial_TX_SCL_MISO_HSIOM_MASK,
                                       Serial_TX_SCL_MISO_HSIOM_POS, Serial_HSIOM_UART_SEL);
    }
#endif /* (Serial_TX_SCL_MISO_PIN_PIN) */

#if (Serial_RTS_SS0_PIN)
    if (Serial_CHECK_RTS_SS0_PIN_USED)
    {
        /* Set SCB UART to drive the output pin */
        Serial_SET_HSIOM_SEL(Serial_RTS_SS0_HSIOM_REG, Serial_RTS_SS0_HSIOM_MASK,
                                       Serial_RTS_SS0_HSIOM_POS, Serial_HSIOM_UART_SEL);
    }
#endif /* (Serial_RTS_SS0_PIN) */

#else
#if (Serial_UART_TX_PIN)
     /* Set SCB UART to drive the output pin */
    Serial_SET_HSIOM_SEL(Serial_TX_HSIOM_REG, Serial_TX_HSIOM_MASK,
                                   Serial_TX_HSIOM_POS, Serial_HSIOM_UART_SEL);
#endif /* (Serial_UART_TX_PIN) */

#if (Serial_UART_RTS_PIN)
    /* Set SCB UART to drive the output pin */
    Serial_SET_HSIOM_SEL(Serial_RTS_HSIOM_REG, Serial_RTS_HSIOM_MASK,
                                   Serial_RTS_HSIOM_POS, Serial_HSIOM_UART_SEL);
#endif /* (Serial_UART_RTS_PIN) */

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Serial_UartStop
********************************************************************************
*
* Summary:
*  Changes the HSIOM settings for the UART output pins (TX and/or RTS) to keep
*  them inactive after the block is disabled. The output pins are controlled by
*  the GPIO data register. Also, the function disables the skip start feature to
*  not cause it to trigger after the component is enabled.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Serial_UartStop(void)
{
#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    #if (Serial_TX_SCL_MISO_PIN)
        if (Serial_CHECK_TX_SCL_MISO_PIN_USED)
        {
            /* Set GPIO to drive output pin */
            Serial_SET_HSIOM_SEL(Serial_TX_SCL_MISO_HSIOM_REG, Serial_TX_SCL_MISO_HSIOM_MASK,
                                           Serial_TX_SCL_MISO_HSIOM_POS, Serial_HSIOM_GPIO_SEL);
        }
    #endif /* (Serial_TX_SCL_MISO_PIN_PIN) */

    #if (Serial_RTS_SS0_PIN)
        if (Serial_CHECK_RTS_SS0_PIN_USED)
        {
            /* Set output pin state after block is disabled */
            Serial_uart_rts_spi_ss0_Write(Serial_GET_UART_RTS_INACTIVE);

            /* Set GPIO to drive output pin */
            Serial_SET_HSIOM_SEL(Serial_RTS_SS0_HSIOM_REG, Serial_RTS_SS0_HSIOM_MASK,
                                           Serial_RTS_SS0_HSIOM_POS, Serial_HSIOM_GPIO_SEL);
        }
    #endif /* (Serial_SS0_PIN) */

#else
    #if (Serial_UART_TX_PIN)
        /* Set GPIO to drive output pin */
        Serial_SET_HSIOM_SEL(Serial_TX_HSIOM_REG, Serial_TX_HSIOM_MASK,
                                       Serial_TX_HSIOM_POS, Serial_HSIOM_GPIO_SEL);
    #endif /* (Serial_UART_TX_PIN) */

    #if (Serial_UART_RTS_PIN)
        /* Set output pin state after block is disabled */
        Serial_rts_Write(Serial_GET_UART_RTS_INACTIVE);

        /* Set GPIO to drive output pin */
        Serial_SET_HSIOM_SEL(Serial_RTS_HSIOM_REG, Serial_RTS_HSIOM_MASK,
                                       Serial_RTS_HSIOM_POS, Serial_HSIOM_GPIO_SEL);
    #endif /* (Serial_UART_RTS_PIN) */

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (Serial_UART_WAKE_ENABLE_CONST)
    /* Disable skip start feature used for wakeup */
    Serial_UART_RX_CTRL_REG &= (uint32) ~Serial_UART_RX_CTRL_SKIP_START;
#endif /* (Serial_UART_WAKE_ENABLE_CONST) */
}


/*******************************************************************************
* Function Name: Serial_UartSetRxAddress
********************************************************************************
*
* Summary:
*  Sets the hardware detectable receiver address for the UART in the
*  Multiprocessor mode.
*
* Parameters:
*  address: Address for hardware address detection.
*
* Return:
*  None
*
*******************************************************************************/
void Serial_UartSetRxAddress(uint32 address)
{
     uint32 matchReg;

    matchReg = Serial_RX_MATCH_REG;

    matchReg &= ((uint32) ~Serial_RX_MATCH_ADDR_MASK); /* Clear address bits */
    matchReg |= ((uint32)  (address & Serial_RX_MATCH_ADDR_MASK)); /* Set address  */

    Serial_RX_MATCH_REG = matchReg;
}


/*******************************************************************************
* Function Name: Serial_UartSetRxAddressMask
********************************************************************************
*
* Summary:
*  Sets the hardware address mask for the UART in the Multiprocessor mode.
*
* Parameters:
*  addressMask: Address mask.
*   0 - address bit does not care while comparison.
*   1 - address bit is significant while comparison.
*
* Return:
*  None
*
*******************************************************************************/
void Serial_UartSetRxAddressMask(uint32 addressMask)
{
    uint32 matchReg;

    matchReg = Serial_RX_MATCH_REG;

    matchReg &= ((uint32) ~Serial_RX_MATCH_MASK_MASK); /* Clear address mask bits */
    matchReg |= ((uint32) (addressMask << Serial_RX_MATCH_MASK_POS));

    Serial_RX_MATCH_REG = matchReg;
}


#if(Serial_UART_RX_DIRECTION)
    /*******************************************************************************
    * Function Name: Serial_UartGetChar
    ********************************************************************************
    *
    * Summary:
    *  Retrieves the next data element from the receive buffer.
    *  This function is designed for ASCII characters and returns a char
    *  where 1 to 255 are valid characters and 0 indicates an error occurred or
    *  no data present.
    *  - The RX software buffer is disabled: returns the data element
    *    retrieved from the RX FIFO.
    *    Undefined data will be returned if the RX FIFO is empty.
    *  - The RX software buffer is enabled: returns the data element from
    *    the software receive buffer.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The next data element from the receive buffer.
    *  ASCII character values from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Side Effects:
    *  The errors bits may not correspond with reading characters due to RX FIFO
    *  and software buffer usage.
    *  RX software buffer is enabled: The internal software buffer overflow
    *  does not treat as an error condition.
    *  Check SCB_rxBufferOverflow to capture that error condition.
    *
    *******************************************************************************/
    uint32 Serial_UartGetChar(void)
    {
        uint32 rxData = 0u;

        /* Reads data only if there is data to read */
        if (0u != Serial_SpiUartGetRxBufferSize())
        {
            rxData = Serial_SpiUartReadRxData();
        }

        if (Serial_CHECK_INTR_RX(Serial_INTR_RX_ERR))
        {
            rxData = 0u; /* Error occurred: returns zero */
            Serial_ClearRxInterruptSource(Serial_INTR_RX_ERR);
        }

        return (rxData);
    }


    /*******************************************************************************
    * Function Name: Serial_UartGetByte
    ********************************************************************************
    *
    * Summary:
    *  Retrieves the next data element from the receive buffer, returns the
    *  received byte and error condition.
    *   - The RX software buffer is disabled: returns the data element retrieved
    *     from the RX FIFO. Undefined data will be returned if the RX FIFO is
    *     empty.
    *   - The RX software buffer is enabled: returns data element from the
    *     software receive buffer.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Bits 7-0 contain the next data element from the receive buffer and
    *  other bits contain the error condition.
    *
    * Side Effects:
    *  The errors bits may not correspond with reading characters due to RX FIFO
    *  and software buffer usage.
    *  RX software buffer is disabled: The internal software buffer overflow
    *  is not returned as status by this function.
    *  Check SCB_rxBufferOverflow to capture that error condition.
    *
    *******************************************************************************/
    uint32 Serial_UartGetByte(void)
    {
        uint32 rxData;
        uint32 tmpStatus;

        #if (Serial_CHECK_RX_SW_BUFFER)
        {
            Serial_DisableInt();
        }
        #endif

        if (0u != Serial_SpiUartGetRxBufferSize())
        {
            /* Enables interrupt to receive more bytes: at least one byte is in
            * buffer.
            */
            #if (Serial_CHECK_RX_SW_BUFFER)
            {            
                Serial_EnableInt();
            }
            #endif

            /* Get received byte */
            rxData = Serial_SpiUartReadRxData();
        }
        else
        {
            /* Reads a byte directly from RX FIFO: underflow is raised in the case
            * of empty. Otherwise the first received byte will be read.
            */
            rxData = Serial_RX_FIFO_RD_REG;

            /* Enables interrupt to receive more bytes.
            * The RX_NOT_EMPTY interrupt is cleared by the interrupt routine
            * in case the byte was received and read by code above.
            */
            #if (Serial_CHECK_RX_SW_BUFFER)
            {
                Serial_EnableInt();
            }
            #endif
        }

        /* Get and clear RX error mask */
        tmpStatus = (Serial_GetRxInterruptSource() & Serial_INTR_RX_ERR);
        Serial_ClearRxInterruptSource(Serial_INTR_RX_ERR);

        /* Puts together data and error status:
        * MP mode and accept address: 9th bit is set to notify mark.
        */
        rxData |= ((uint32) (tmpStatus << 8u));

        return (rxData);
    }


    #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
        /*******************************************************************************
        * Function Name: Serial_UartSetRtsPolarity
        ********************************************************************************
        *
        * Summary:
        *  Sets active polarity of RTS output signal.
        *
        * Parameters:
        *  polarity: Active polarity of RTS output signal.
        *   Serial_UART_RTS_ACTIVE_LOW  - RTS signal is active low.
        *   Serial_UART_RTS_ACTIVE_HIGH - RTS signal is active high.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void Serial_UartSetRtsPolarity(uint32 polarity)
        {
            if(0u != polarity)
            {
                Serial_UART_FLOW_CTRL_REG |= (uint32)  Serial_UART_FLOW_CTRL_RTS_POLARITY;
            }
            else
            {
                Serial_UART_FLOW_CTRL_REG &= (uint32) ~Serial_UART_FLOW_CTRL_RTS_POLARITY;
            }
        }


        /*******************************************************************************
        * Function Name: Serial_UartSetRtsFifoLevel
        ********************************************************************************
        *
        * Summary:
        *  Sets level in the RX FIFO for RTS signal activation.
        *  While the RX FIFO has fewer entries than the RX FIFO level the RTS signal
        *  remains active, otherwise the RTS signal becomes inactive.
        *
        * Parameters:
        *  level: Level in the RX FIFO for RTS signal activation.
        *         The range of valid level values is between 0 and RX FIFO depth - 1.
        *         Setting level value to 0 disables RTS signal activation.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void Serial_UartSetRtsFifoLevel(uint32 level)
        {
            uint32 uartFlowCtrl;

            uartFlowCtrl = Serial_UART_FLOW_CTRL_REG;

            uartFlowCtrl &= ((uint32) ~Serial_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
            uartFlowCtrl |= ((uint32) (Serial_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK & level));

            Serial_UART_FLOW_CTRL_REG = uartFlowCtrl;
        }
    #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#endif /* (Serial_UART_RX_DIRECTION) */


#if(Serial_UART_TX_DIRECTION)
    /*******************************************************************************
    * Function Name: Serial_UartPutString
    ********************************************************************************
    *
    * Summary:
    *  Places a NULL terminated string in the transmit buffer to be sent at the
    *  next available bus time.
    *  This function is blocking and waits until there is space available to put
    *  all the requested data into the  transmit buffer.
    *
    * Parameters:
    *  string: pointer to the null terminated string array to be placed in the
    *          transmit buffer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_UartPutString(const char8 string[])
    {
        uint32 bufIndex;

        bufIndex = 0u;

        /* Blocks the control flow until all data has been sent */
        while(string[bufIndex] != ((char8) 0))
        {
            Serial_UartPutChar((uint32) string[bufIndex]);
            bufIndex++;
        }
    }


    /*******************************************************************************
    * Function Name: Serial_UartPutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data followed by a carriage return (0x0D) and
    *  line feed (0x0A) into the transmit buffer.
    *  This function is blocking and waits until there is space available to put
    *  all the requested data into the  transmit buffer.
    *
    * Parameters:
    *  txDataByte : the data to be transmitted.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_UartPutCRLF(uint32 txDataByte)
    {
        Serial_UartPutChar(txDataByte);  /* Blocks control flow until all data has been sent */
        Serial_UartPutChar(0x0Du);       /* Blocks control flow until all data has been sent */
        Serial_UartPutChar(0x0Au);       /* Blocks control flow until all data has been sent */
    }


    #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
        /*******************************************************************************
        * Function Name: SerialSCB_UartEnableCts
        ********************************************************************************
        *
        * Summary:
        *  Enables usage of CTS input signal by the UART transmitter.
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void Serial_UartEnableCts(void)
        {
            Serial_UART_FLOW_CTRL_REG |= (uint32)  Serial_UART_FLOW_CTRL_CTS_ENABLE;
        }


        /*******************************************************************************
        * Function Name: Serial_UartDisableCts
        ********************************************************************************
        *
        * Summary:
        *  Disables usage of CTS input signal by the UART transmitter.
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void Serial_UartDisableCts(void)
        {
            Serial_UART_FLOW_CTRL_REG &= (uint32) ~Serial_UART_FLOW_CTRL_CTS_ENABLE;
        }


        /*******************************************************************************
        * Function Name: Serial_UartSetCtsPolarity
        ********************************************************************************
        *
        * Summary:
        *  Sets active polarity of CTS input signal.
        *
        * Parameters:
        *  polarity: Active polarity of CTS output signal.
        *   Serial_UART_CTS_ACTIVE_LOW  - CTS signal is active low.
        *   Serial_UART_CTS_ACTIVE_HIGH - CTS signal is active high.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void Serial_UartSetCtsPolarity(uint32 polarity)
        {
            if (0u != polarity)
            {
                Serial_UART_FLOW_CTRL_REG |= (uint32)  Serial_UART_FLOW_CTRL_CTS_POLARITY;
            }
            else
            {
                Serial_UART_FLOW_CTRL_REG &= (uint32) ~Serial_UART_FLOW_CTRL_CTS_POLARITY;
            }
        }
    #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#endif /* (Serial_UART_TX_DIRECTION) */


#if(Serial_UART_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: Serial_UartSaveConfig
    ********************************************************************************
    *
    * Summary:
    *  Clears and enables interrupt on a falling edge of the Rx input. The GPIO
    *  event wakes up the device and SKIP_START feature allows the UART continue
    *  receiving data bytes properly. The GPIO interrupt does not track in the
    *  active mode therefore requires to be cleared by this API.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_UartSaveConfig(void)
    {
        /* Clear interrupt activity:
        *  - set skip start and disable RX. At GPIO wakeup RX will be enabled.
        *  - clear rx_wake interrupt source as it triggers during normal operation.
        *  - clear wake interrupt pending state as it becomes pending in active mode.
        */

        Serial_UART_RX_CTRL_REG |= Serial_UART_RX_CTRL_SKIP_START;

    #if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
        #if(Serial_MOSI_SCL_RX_WAKE_PIN)
            (void) Serial_uart_rx_wake_i2c_sda_spi_mosi_ClearInterrupt();
        #endif /* (Serial_MOSI_SCL_RX_WAKE_PIN) */
    #else
        #if(Serial_UART_RX_WAKE_PIN)
            (void) Serial_rx_wake_ClearInterrupt();
        #endif /* (Serial_UART_RX_WAKE_PIN) */
    #endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

    #if(Serial_UART_RX_WAKEUP_IRQ)
        Serial_RxWakeClearPendingInt();
        Serial_RxWakeEnableInt();
    #endif /* (Serial_UART_RX_WAKEUP_IRQ) */
    }


    /*******************************************************************************
    * Function Name: Serial_UartRestoreConfig
    ********************************************************************************
    *
    * Summary:
    *  Disables the RX GPIO interrupt. Until this function is called the interrupt
    *  remains active and triggers on every falling edge of the UART RX line.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void Serial_UartRestoreConfig(void)
    {
    /* Disable RX GPIO interrupt: no more triggers in active mode */
    #if(Serial_UART_RX_WAKEUP_IRQ)
        Serial_RxWakeDisableInt();
    #endif /* (Serial_UART_RX_WAKEUP_IRQ) */
    }
#endif /* (Serial_UART_WAKE_ENABLE_CONST) */


#if(Serial_UART_RX_WAKEUP_IRQ)
    /*******************************************************************************
    * Function Name: Serial_UART_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  Handles the Interrupt Service Routine for the SCB UART mode GPIO wakeup
    *  event. This event is configured to trigger on a falling edge of the RX line.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    CY_ISR(Serial_UART_WAKEUP_ISR)
    {
    #ifdef Serial_UART_WAKEUP_ISR_ENTRY_CALLBACK
        Serial_UART_WAKEUP_ISR_EntryCallback();
    #endif /* Serial_UART_WAKEUP_ISR_ENTRY_CALLBACK */

        /* Clear interrupt source: the event becomes multi triggered and is
        * only disabled by Serial_UartRestoreConfig() call.
        */
    #if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
        #if(Serial_MOSI_SCL_RX_WAKE_PIN)
            (void) Serial_uart_rx_wake_i2c_sda_spi_mosi_ClearInterrupt();
        #endif /* (Serial_MOSI_SCL_RX_WAKE_PIN) */
    #else
        #if(Serial_UART_RX_WAKE_PIN)
            (void) Serial_rx_wake_ClearInterrupt();
        #endif /* (Serial_UART_RX_WAKE_PIN) */
    #endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

    #ifdef Serial_UART_WAKEUP_ISR_EXIT_CALLBACK
        Serial_UART_WAKEUP_ISR_ExitCallback();
    #endif /* Serial_UART_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* (Serial_UART_RX_WAKEUP_IRQ) */


/* [] END OF FILE */

/***************************************************************************//**
* \file Serial_UART.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  UART mode.
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
#include "cyapicallbacks.h"

#if (Serial_UART_WAKE_ENABLE_CONST && Serial_UART_RX_WAKEUP_IRQ)
    /**
    * \addtogroup group_globals
    * \{
    */
    /** This global variable determines whether to enable Skip Start
    * functionality when Serial_Sleep() function is called:
    * 0 – disable, other values – enable. Default value is 1.
    * It is only available when Enable wakeup from Deep Sleep Mode is enabled.
    */
    uint8 Serial_skipStart = 1u;
    /** \} globals */
#endif /* (Serial_UART_WAKE_ENABLE_CONST && Serial_UART_RX_WAKEUP_IRQ) */

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
        (uint8) Serial_UART_RTS_FIFO_LEVEL,
        (uint8) Serial_UART_RX_BREAK_WIDTH
    };


    /*******************************************************************************
    * Function Name: Serial_UartInit
    ****************************************************************************//**
    *
    *  Configures the Serial for UART operation.
    *
    *  This function is intended specifically to be used when the Serial
    *  configuration is set to “Unconfigured Serial” in the customizer.
    *  After initializing the Serial in UART mode using this function,
    *  the component can be enabled using the Serial_Start() or
    * Serial_Enable() function.
    *  This function uses a pointer to a structure that provides the configuration
    *  settings. This structure contains the same information that would otherwise
    *  be provided by the customizer settings.
    *
    *  \param config: pointer to a structure that contains the following list of
    *   fields. These fields match the selections available in the customizer.
    *   Refer to the customizer for further description of the settings.
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
            Serial_rxBufferSize  =         config->rxBufferSize;

            /* Set TX direction internal variables */
            Serial_txBuffer      =         config->txBuffer;
            Serial_txDataBits    = (uint8) config->dataBits;
            Serial_txBufferSize  =         config->txBufferSize;

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
                                        Serial_GET_UART_RX_CTRL_DROP_ON_FRAME_ERR(config->dropOnFrameErr)   |
                                        Serial_GET_UART_RX_CTRL_BREAK_WIDTH(config->breakWidth);

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

            /* Configure TX interrupt sources to restore. */
            Serial_IntrTxMask = LO16(Serial_INTR_TX_MASK_REG);

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
    ****************************************************************************//**
    *
    *  Configures the SCB for the UART operation.
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

        /* Configure TX interrupt sources to restore. */
        Serial_IntrTxMask = LO16(Serial_INTR_TX_MASK_REG);

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
****************************************************************************//**
*
*  Restores HSIOM settings for the UART output pins (TX and/or RTS) to be
*  controlled by the SCB UART.
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
                                           Serial_TX_SCL_MISO_HSIOM_POS, Serial_TX_SCL_MISO_HSIOM_SEL_UART);
        }
    #endif /* (Serial_TX_SCL_MISO_PIN_PIN) */

    #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
        #if (Serial_RTS_SS0_PIN)
            if (Serial_CHECK_RTS_SS0_PIN_USED)
            {
                /* Set SCB UART to drive the output pin */
                Serial_SET_HSIOM_SEL(Serial_RTS_SS0_HSIOM_REG, Serial_RTS_SS0_HSIOM_MASK,
                                               Serial_RTS_SS0_HSIOM_POS, Serial_RTS_SS0_HSIOM_SEL_UART);
            }
        #endif /* (Serial_RTS_SS0_PIN) */
    #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#else
    #if (Serial_UART_TX_PIN)
         /* Set SCB UART to drive the output pin */
        Serial_SET_HSIOM_SEL(Serial_TX_HSIOM_REG, Serial_TX_HSIOM_MASK,
                                       Serial_TX_HSIOM_POS, Serial_TX_HSIOM_SEL_UART);
    #endif /* (Serial_UART_TX_PIN) */

    #if (Serial_UART_RTS_PIN)
        /* Set SCB UART to drive the output pin */
        Serial_SET_HSIOM_SEL(Serial_RTS_HSIOM_REG, Serial_RTS_HSIOM_MASK,
                                       Serial_RTS_HSIOM_POS, Serial_RTS_HSIOM_SEL_UART);
    #endif /* (Serial_UART_RTS_PIN) */
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Restore TX interrupt sources. */
    Serial_SetTxInterruptMode(Serial_IntrTxMask);
}


/*******************************************************************************
* Function Name: Serial_UartStop
****************************************************************************//**
*
*  Changes the HSIOM settings for the UART output pins (TX and/or RTS) to keep
*  them inactive after the block is disabled. The output pins are controlled by
*  the GPIO data register. Also, the function disables the skip start feature
*  to not cause it to trigger after the component is enabled.
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
                                           Serial_TX_SCL_MISO_HSIOM_POS, Serial_TX_SCL_MISO_HSIOM_SEL_GPIO);
        }
    #endif /* (Serial_TX_SCL_MISO_PIN_PIN) */

    #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
        #if (Serial_RTS_SS0_PIN)
            if (Serial_CHECK_RTS_SS0_PIN_USED)
            {
                /* Set output pin state after block is disabled */
                Serial_uart_rts_spi_ss0_Write(Serial_GET_UART_RTS_INACTIVE);

                /* Set GPIO to drive output pin */
                Serial_SET_HSIOM_SEL(Serial_RTS_SS0_HSIOM_REG, Serial_RTS_SS0_HSIOM_MASK,
                                               Serial_RTS_SS0_HSIOM_POS, Serial_RTS_SS0_HSIOM_SEL_GPIO);
            }
        #endif /* (Serial_RTS_SS0_PIN) */
    #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#else
    #if (Serial_UART_TX_PIN)
        /* Set GPIO to drive output pin */
        Serial_SET_HSIOM_SEL(Serial_TX_HSIOM_REG, Serial_TX_HSIOM_MASK,
                                       Serial_TX_HSIOM_POS, Serial_TX_HSIOM_SEL_GPIO);
    #endif /* (Serial_UART_TX_PIN) */

    #if (Serial_UART_RTS_PIN)
        /* Set output pin state after block is disabled */
        Serial_rts_Write(Serial_GET_UART_RTS_INACTIVE);

        /* Set GPIO to drive output pin */
        Serial_SET_HSIOM_SEL(Serial_RTS_HSIOM_REG, Serial_RTS_HSIOM_MASK,
                                       Serial_RTS_HSIOM_POS, Serial_RTS_HSIOM_SEL_GPIO);
    #endif /* (Serial_UART_RTS_PIN) */

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (Serial_UART_WAKE_ENABLE_CONST)
    /* Disable skip start feature used for wakeup */
    Serial_UART_RX_CTRL_REG &= (uint32) ~Serial_UART_RX_CTRL_SKIP_START;
#endif /* (Serial_UART_WAKE_ENABLE_CONST) */

    /* Store TX interrupt sources (exclude level triggered). */
    Serial_IntrTxMask = LO16(Serial_GetTxInterruptMode() & Serial_INTR_UART_TX_RESTORE);
}


/*******************************************************************************
* Function Name: Serial_UartSetRxAddress
****************************************************************************//**
*
*  Sets the hardware detectable receiver address for the UART in the
*  Multiprocessor mode.
*
*  \param address: Address for hardware address detection.
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
****************************************************************************//**
*
*  Sets the hardware address mask for the UART in the Multiprocessor mode.
*
*  \param addressMask: Address mask.
*   - Bit value 0 – excludes bit from address comparison.
*   - Bit value 1 – the bit needs to match with the corresponding bit
*     of the address.
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
    ****************************************************************************//**
    *
    *  Retrieves next data element from receive buffer.
    *  This function is designed for ASCII characters and returns a char where
    *  1 to 255 are valid characters and 0 indicates an error occurred or no data
    *  is present.
    *  - RX software buffer is disabled: Returns data element retrieved from RX
    *    FIFO.
    *  - RX software buffer is enabled: Returns data element from the software
    *    receive buffer.
    *
    *  \return
    *   Next data element from the receive buffer. ASCII character values from
    *   1 to 255 are valid. A returned zero signifies an error condition or no
    *   data available.
    *
    *  \sideeffect
    *   The errors bits may not correspond with reading characters due to
    *   RX FIFO and software buffer usage.
    *   RX software buffer is enabled: The internal software buffer overflow
    *   is not treated as an error condition.
    *   Check Serial_rxBufferOverflow to capture that error condition.
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
    ****************************************************************************//**
    *
    *  Retrieves the next data element from the receive buffer, returns the
    *  received byte and error condition.
    *   - The RX software buffer is disabled: returns the data element retrieved
    *     from the RX FIFO. Undefined data will be returned if the RX FIFO is
    *     empty.
    *   - The RX software buffer is enabled: returns data element from the
    *     software receive buffer.
    *
    *  \return
    *   Bits 7-0 contain the next data element from the receive buffer and
    *   other bits contain the error condition.
    *   - Serial_UART_RX_OVERFLOW - Attempt to write to a full
    *     receiver FIFO.
    *   - Serial_UART_RX_UNDERFLOW    Attempt to read from an empty
    *     receiver FIFO.
    *   - Serial_UART_RX_FRAME_ERROR - UART framing error detected.
    *   - Serial_UART_RX_PARITY_ERROR - UART parity error detected.
    *
    *  \sideeffect
    *   The errors bits may not correspond with reading characters due to
    *   RX FIFO and software buffer usage.
    *   RX software buffer is enabled: The internal software buffer overflow
    *   is not treated as an error condition.
    *   Check Serial_rxBufferOverflow to capture that error condition.
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
            /* Reads a byte directly from RX FIFO: underflow is raised in the
            * case of empty. Otherwise the first received byte will be read.
            */
            rxData = Serial_RX_FIFO_RD_REG;


            /* Enables interrupt to receive more bytes. */
            #if (Serial_CHECK_RX_SW_BUFFER)
            {

                /* The byte has been read from RX FIFO. Clear RX interrupt to
                * not involve interrupt handler when RX FIFO is empty.
                */
                Serial_ClearRxInterruptSource(Serial_INTR_RX_NOT_EMPTY);

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
        ****************************************************************************//**
        *
        *  Sets active polarity of RTS output signal.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *  \param polarity: Active polarity of RTS output signal.
        *   - Serial_UART_RTS_ACTIVE_LOW  - RTS signal is active low.
        *   - Serial_UART_RTS_ACTIVE_HIGH - RTS signal is active high.
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
        ****************************************************************************//**
        *
        *  Sets level in the RX FIFO for RTS signal activation.
        *  While the RX FIFO has fewer entries than the RX FIFO level the RTS signal
        *  remains active, otherwise the RTS signal becomes inactive.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *  \param level: Level in the RX FIFO for RTS signal activation.
        *   The range of valid level values is between 0 and RX FIFO depth - 1.
        *   Setting level value to 0 disables RTS signal activation.
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
    ****************************************************************************//**
    *
    *  Places a NULL terminated string in the transmit buffer to be sent at the
    *  next available bus time.
    *  This function is blocking and waits until there is a space available to put
    *  requested data in transmit buffer.
    *
    *  \param string: pointer to the null terminated string array to be placed in the
    *   transmit buffer.
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
    ****************************************************************************//**
    *
    *  Places byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) in the transmit buffer.
    *  This function is blocking and waits until there is a space available to put
    *  all requested data in transmit buffer.
    *
    *  \param txDataByte: the data to be transmitted.
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
        ****************************************************************************//**
        *
        *  Enables usage of CTS input signal by the UART transmitter.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *******************************************************************************/
        void Serial_UartEnableCts(void)
        {
            Serial_UART_FLOW_CTRL_REG |= (uint32)  Serial_UART_FLOW_CTRL_CTS_ENABLE;
        }


        /*******************************************************************************
        * Function Name: Serial_UartDisableCts
        ****************************************************************************//**
        *
        *  Disables usage of CTS input signal by the UART transmitter.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *******************************************************************************/
        void Serial_UartDisableCts(void)
        {
            Serial_UART_FLOW_CTRL_REG &= (uint32) ~Serial_UART_FLOW_CTRL_CTS_ENABLE;
        }


        /*******************************************************************************
        * Function Name: Serial_UartSetCtsPolarity
        ****************************************************************************//**
        *
        *  Sets active polarity of CTS input signal.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        * \param
        * polarity: Active polarity of CTS output signal.
        *   - Serial_UART_CTS_ACTIVE_LOW  - CTS signal is active low.
        *   - Serial_UART_CTS_ACTIVE_HIGH - CTS signal is active high.
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


    /*******************************************************************************
    * Function Name: Serial_UartSendBreakBlocking
    ****************************************************************************//**
    *
    * Sends a break condition (logic low) of specified width on UART TX line.
    * Blocks until break is completed. Only call this function when UART TX FIFO
    * and shifter are empty.
    *
    * \param breakWidth
    * Width of break condition. Valid range is 4 to 16 bits.
    *
    * \note
    * Before sending break all UART TX interrupt sources are disabled. The state
    * of UART TX interrupt sources is restored before function returns.
    *
    * \sideeffect
    * If this function is called while there is data in the TX FIFO or shifter that
    * data will be shifted out in packets the size of breakWidth.
    *
    *******************************************************************************/
    void Serial_UartSendBreakBlocking(uint32 breakWidth)
    {
        uint32 txCtrlReg;
        uint32 txIntrReg;

        /* Disable all UART TX interrupt source and clear UART TX Done history */
        txIntrReg = Serial_GetTxInterruptMode();
        Serial_SetTxInterruptMode(0u);
        Serial_ClearTxInterruptSource(Serial_INTR_TX_UART_DONE);

        /* Store TX CTRL configuration */
        txCtrlReg = Serial_TX_CTRL_REG;

        /* Set break width */
        Serial_TX_CTRL_REG = (Serial_TX_CTRL_REG & (uint32) ~Serial_TX_CTRL_DATA_WIDTH_MASK) |
                                        Serial_GET_TX_CTRL_DATA_WIDTH(breakWidth);

        /* Generate break */
        Serial_TX_FIFO_WR_REG = 0u;

        /* Wait for break completion */
        while (0u == (Serial_GetTxInterruptSource() & Serial_INTR_TX_UART_DONE))
        {
        }

        /* Clear all UART TX interrupt sources to  */
        Serial_ClearTxInterruptSource(Serial_INTR_TX_ALL);

        /* Restore TX interrupt sources and data width */
        Serial_TX_CTRL_REG = txCtrlReg;
        Serial_SetTxInterruptMode(txIntrReg);
    }
#endif /* (Serial_UART_TX_DIRECTION) */


#if (Serial_UART_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: Serial_UartSaveConfig
    ****************************************************************************//**
    *
    *  Clears and enables an interrupt on a falling edge of the Rx input. The GPIO
    *  interrupt does not track in the active mode, therefore requires to be
    *  cleared by this API.
    *
    *******************************************************************************/
    void Serial_UartSaveConfig(void)
    {
    #if (Serial_UART_RX_WAKEUP_IRQ)
        /* Set SKIP_START if requested (set by default). */
        if (0u != Serial_skipStart)
        {
            Serial_UART_RX_CTRL_REG |= (uint32)  Serial_UART_RX_CTRL_SKIP_START;
        }
        else
        {
            Serial_UART_RX_CTRL_REG &= (uint32) ~Serial_UART_RX_CTRL_SKIP_START;
        }

        /* Clear RX GPIO interrupt status and pending interrupt in NVIC because
        * falling edge on RX line occurs while UART communication in active mode.
        * Enable interrupt: next interrupt trigger should wakeup device.
        */
        Serial_CLEAR_UART_RX_WAKE_INTR;
        Serial_RxWakeClearPendingInt();
        Serial_RxWakeEnableInt();
    #endif /* (Serial_UART_RX_WAKEUP_IRQ) */
    }


    /*******************************************************************************
    * Function Name: Serial_UartRestoreConfig
    ****************************************************************************//**
    *
    *  Disables the RX GPIO interrupt. Until this function is called the interrupt
    *  remains active and triggers on every falling edge of the UART RX line.
    *
    *******************************************************************************/
    void Serial_UartRestoreConfig(void)
    {
    #if (Serial_UART_RX_WAKEUP_IRQ)
        /* Disable interrupt: no more triggers in active mode */
        Serial_RxWakeDisableInt();
    #endif /* (Serial_UART_RX_WAKEUP_IRQ) */
    }


    #if (Serial_UART_RX_WAKEUP_IRQ)
        /*******************************************************************************
        * Function Name: Serial_UART_WAKEUP_ISR
        ****************************************************************************//**
        *
        *  Handles the Interrupt Service Routine for the SCB UART mode GPIO wakeup
        *  event. This event is configured to trigger on a falling edge of the RX line.
        *
        *******************************************************************************/
        CY_ISR(Serial_UART_WAKEUP_ISR)
        {
        #ifdef Serial_UART_WAKEUP_ISR_ENTRY_CALLBACK
            Serial_UART_WAKEUP_ISR_EntryCallback();
        #endif /* Serial_UART_WAKEUP_ISR_ENTRY_CALLBACK */

            Serial_CLEAR_UART_RX_WAKE_INTR;

        #ifdef Serial_UART_WAKEUP_ISR_EXIT_CALLBACK
            Serial_UART_WAKEUP_ISR_ExitCallback();
        #endif /* Serial_UART_WAKEUP_ISR_EXIT_CALLBACK */
        }
    #endif /* (Serial_UART_RX_WAKEUP_IRQ) */
#endif /* (Serial_UART_RX_WAKEUP_IRQ) */


/* [] END OF FILE */

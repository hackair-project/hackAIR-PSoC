/***************************************************************************//**
* \file Serial_SPI_UART_PVT.h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component in SPI and UART modes.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_SPI_UART_PVT_Serial_H)
#define CY_SCB_SPI_UART_PVT_Serial_H

#include "Serial_SPI_UART.h"


/***************************************
*     Internal Global Vars
***************************************/

#if (Serial_INTERNAL_RX_SW_BUFFER_CONST)
    extern volatile uint32  Serial_rxBufferHead;
    extern volatile uint32  Serial_rxBufferTail;
    
    /**
    * \addtogroup group_globals
    * @{
    */
    
    /** Sets when internal software receive buffer overflow
     *  was occurred.
    */  
    extern volatile uint8   Serial_rxBufferOverflow;
    /** @} globals */
#endif /* (Serial_INTERNAL_RX_SW_BUFFER_CONST) */

#if (Serial_INTERNAL_TX_SW_BUFFER_CONST)
    extern volatile uint32  Serial_txBufferHead;
    extern volatile uint32  Serial_txBufferTail;
#endif /* (Serial_INTERNAL_TX_SW_BUFFER_CONST) */

#if (Serial_INTERNAL_RX_SW_BUFFER)
    extern volatile uint8 Serial_rxBufferInternal[Serial_INTERNAL_RX_BUFFER_SIZE];
#endif /* (Serial_INTERNAL_RX_SW_BUFFER) */

#if (Serial_INTERNAL_TX_SW_BUFFER)
    extern volatile uint8 Serial_txBufferInternal[Serial_TX_BUFFER_SIZE];
#endif /* (Serial_INTERNAL_TX_SW_BUFFER) */


/***************************************
*     Private Function Prototypes
***************************************/

void Serial_SpiPostEnable(void);
void Serial_SpiStop(void);

#if (Serial_SCB_MODE_SPI_CONST_CFG)
    void Serial_SpiInit(void);
#endif /* (Serial_SCB_MODE_SPI_CONST_CFG) */

#if (Serial_SPI_WAKE_ENABLE_CONST)
    void Serial_SpiSaveConfig(void);
    void Serial_SpiRestoreConfig(void);
#endif /* (Serial_SPI_WAKE_ENABLE_CONST) */

void Serial_UartPostEnable(void);
void Serial_UartStop(void);

#if (Serial_SCB_MODE_UART_CONST_CFG)
    void Serial_UartInit(void);
#endif /* (Serial_SCB_MODE_UART_CONST_CFG) */

#if (Serial_UART_WAKE_ENABLE_CONST)
    void Serial_UartSaveConfig(void);
    void Serial_UartRestoreConfig(void);
#endif /* (Serial_UART_WAKE_ENABLE_CONST) */


/***************************************
*         UART API Constants
***************************************/

/* UART RX and TX position to be used in Serial_SetPins() */
#define Serial_UART_RX_PIN_ENABLE    (Serial_UART_RX)
#define Serial_UART_TX_PIN_ENABLE    (Serial_UART_TX)

/* UART RTS and CTS position to be used in  Serial_SetPins() */
#define Serial_UART_RTS_PIN_ENABLE    (0x10u)
#define Serial_UART_CTS_PIN_ENABLE    (0x20u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Interrupt processing */
#define Serial_SpiUartEnableIntRx(intSourceMask)  Serial_SetRxInterruptMode(intSourceMask)
#define Serial_SpiUartEnableIntTx(intSourceMask)  Serial_SetTxInterruptMode(intSourceMask)
uint32  Serial_SpiUartDisableIntRx(void);
uint32  Serial_SpiUartDisableIntTx(void);


#endif /* (CY_SCB_SPI_UART_PVT_Serial_H) */


/* [] END OF FILE */

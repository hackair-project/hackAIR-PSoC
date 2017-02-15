/*******************************************************************************
* File Name: Serial_BOOT.h
* Version 3.10
*
* Description:
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* Copyright 2014-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_Serial_H)
#define CY_SCB_BOOT_Serial_H

#include "Serial_PVT.h"

#if (Serial_SCB_MODE_I2C_INC)
    #include "Serial_I2C.h"
#endif /* (Serial_SCB_MODE_I2C_INC) */

#if (Serial_SCB_MODE_EZI2C_INC)
    #include "Serial_EZI2C.h"
#endif /* (Serial_SCB_MODE_EZI2C_INC) */

#if (Serial_SCB_MODE_SPI_INC || Serial_SCB_MODE_UART_INC)
    #include "Serial_SPI_UART.h"
#endif /* (Serial_SCB_MODE_SPI_INC || Serial_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define Serial_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Serial) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (Serial_SCB_MODE_I2C_INC)
    #define Serial_I2C_BTLDR_COMM_ENABLED     (Serial_BTLDR_COMM_ENABLED && \
                                                            (Serial_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             Serial_I2C_SLAVE_CONST))
#else
     #define Serial_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (Serial_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (Serial_SCB_MODE_EZI2C_INC)
    #define Serial_EZI2C_BTLDR_COMM_ENABLED   (Serial_BTLDR_COMM_ENABLED && \
                                                         Serial_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define Serial_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (Serial_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (Serial_SCB_MODE_SPI_INC)
    #define Serial_SPI_BTLDR_COMM_ENABLED     (Serial_BTLDR_COMM_ENABLED && \
                                                            (Serial_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             Serial_SPI_SLAVE_CONST))
#else
        #define Serial_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (Serial_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (Serial_SCB_MODE_UART_INC)
       #define Serial_UART_BTLDR_COMM_ENABLED    (Serial_BTLDR_COMM_ENABLED && \
                                                            (Serial_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (Serial_UART_RX_DIRECTION && \
                                                              Serial_UART_TX_DIRECTION)))
#else
     #define Serial_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (Serial_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define Serial_BTLDR_COMM_MODE_ENABLED    (Serial_I2C_BTLDR_COMM_ENABLED   || \
                                                     Serial_SPI_BTLDR_COMM_ENABLED   || \
                                                     Serial_EZI2C_BTLDR_COMM_ENABLED || \
                                                     Serial_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void Serial_I2CCyBtldrCommStart(void);
    void Serial_I2CCyBtldrCommStop (void);
    void Serial_I2CCyBtldrCommReset(void);
    cystatus Serial_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Serial_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (Serial_SCB_MODE_I2C_CONST_CFG)
        #define Serial_CyBtldrCommStart   Serial_I2CCyBtldrCommStart
        #define Serial_CyBtldrCommStop    Serial_I2CCyBtldrCommStop
        #define Serial_CyBtldrCommReset   Serial_I2CCyBtldrCommReset
        #define Serial_CyBtldrCommRead    Serial_I2CCyBtldrCommRead
        #define Serial_CyBtldrCommWrite   Serial_I2CCyBtldrCommWrite
    #endif /* (Serial_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void Serial_EzI2CCyBtldrCommStart(void);
    void Serial_EzI2CCyBtldrCommStop (void);
    void Serial_EzI2CCyBtldrCommReset(void);
    cystatus Serial_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Serial_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (Serial_SCB_MODE_EZI2C_CONST_CFG)
        #define Serial_CyBtldrCommStart   Serial_EzI2CCyBtldrCommStart
        #define Serial_CyBtldrCommStop    Serial_EzI2CCyBtldrCommStop
        #define Serial_CyBtldrCommReset   Serial_EzI2CCyBtldrCommReset
        #define Serial_CyBtldrCommRead    Serial_EzI2CCyBtldrCommRead
        #define Serial_CyBtldrCommWrite   Serial_EzI2CCyBtldrCommWrite
    #endif /* (Serial_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void Serial_SpiCyBtldrCommStart(void);
    void Serial_SpiCyBtldrCommStop (void);
    void Serial_SpiCyBtldrCommReset(void);
    cystatus Serial_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Serial_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (Serial_SCB_MODE_SPI_CONST_CFG)
        #define Serial_CyBtldrCommStart   Serial_SpiCyBtldrCommStart
        #define Serial_CyBtldrCommStop    Serial_SpiCyBtldrCommStop
        #define Serial_CyBtldrCommReset   Serial_SpiCyBtldrCommReset
        #define Serial_CyBtldrCommRead    Serial_SpiCyBtldrCommRead
        #define Serial_CyBtldrCommWrite   Serial_SpiCyBtldrCommWrite
    #endif /* (Serial_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void Serial_UartCyBtldrCommStart(void);
    void Serial_UartCyBtldrCommStop (void);
    void Serial_UartCyBtldrCommReset(void);
    cystatus Serial_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Serial_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (Serial_SCB_MODE_UART_CONST_CFG)
        #define Serial_CyBtldrCommStart   Serial_UartCyBtldrCommStart
        #define Serial_CyBtldrCommStop    Serial_UartCyBtldrCommStop
        #define Serial_CyBtldrCommReset   Serial_UartCyBtldrCommReset
        #define Serial_CyBtldrCommRead    Serial_UartCyBtldrCommRead
        #define Serial_CyBtldrCommWrite   Serial_UartCyBtldrCommWrite
    #endif /* (Serial_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_UART_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_BTLDR_COMM_ENABLED)
    #if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void Serial_CyBtldrCommStart(void);
        void Serial_CyBtldrCommStop (void);
        void Serial_CyBtldrCommReset(void);
        cystatus Serial_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus Serial_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Serial)
        #define CyBtldrCommStart    Serial_CyBtldrCommStart
        #define CyBtldrCommStop     Serial_CyBtldrCommStop
        #define CyBtldrCommReset    Serial_CyBtldrCommReset
        #define CyBtldrCommWrite    Serial_CyBtldrCommWrite
        #define CyBtldrCommRead     Serial_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Serial) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Serial_BTLDR_COMM_ENABLED) */


/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define Serial_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define Serial_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define Serial_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define Serial_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef Serial_SPI_BYTE_TO_BYTE
    #define Serial_SPI_BYTE_TO_BYTE   (160u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef Serial_UART_BYTE_TO_BYTE
    #define Serial_UART_BYTE_TO_BYTE  (2086u)
#endif /* Serial_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_Serial_H) */


/* [] END OF FILE */

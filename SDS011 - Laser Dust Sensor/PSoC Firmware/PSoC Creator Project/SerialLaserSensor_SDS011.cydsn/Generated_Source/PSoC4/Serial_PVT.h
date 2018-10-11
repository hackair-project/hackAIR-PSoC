/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
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

#if !defined(CY_SCB_PVT_Serial_H)
#define CY_SCB_PVT_Serial_H

#include "Serial.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define Serial_SetI2CExtClkInterruptMode(interruptMask) Serial_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define Serial_ClearI2CExtClkInterruptSource(interruptMask) Serial_CLEAR_INTR_I2C_EC(interruptMask)
#define Serial_GetI2CExtClkInterruptSource()                (Serial_INTR_I2C_EC_REG)
#define Serial_GetI2CExtClkInterruptMode()                  (Serial_INTR_I2C_EC_MASK_REG)
#define Serial_GetI2CExtClkInterruptSourceMasked()          (Serial_INTR_I2C_EC_MASKED_REG)

#if (!Serial_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define Serial_SetSpiExtClkInterruptMode(interruptMask) \
                                                                Serial_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define Serial_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                Serial_CLEAR_INTR_SPI_EC(interruptMask)
    #define Serial_GetExtSpiClkInterruptSource()                 (Serial_INTR_SPI_EC_REG)
    #define Serial_GetExtSpiClkInterruptMode()                   (Serial_INTR_SPI_EC_MASK_REG)
    #define Serial_GetExtSpiClkInterruptSourceMasked()           (Serial_INTR_SPI_EC_MASKED_REG)
#endif /* (!Serial_CY_SCBIP_V1) */

#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void Serial_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (Serial_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_Serial_CUSTOM_INTR_HANDLER)
    extern cyisraddress Serial_customIntrHandler;
#endif /* !defined (CY_REMOVE_Serial_CUSTOM_INTR_HANDLER) */
#endif /* (Serial_SCB_IRQ_INTERNAL) */

extern Serial_BACKUP_STRUCT Serial_backup;

#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 Serial_scbMode;
    extern uint8 Serial_scbEnableWake;
    extern uint8 Serial_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 Serial_mode;
    extern uint8 Serial_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * Serial_rxBuffer;
    extern uint8   Serial_rxDataBits;
    extern uint32  Serial_rxBufferSize;

    extern volatile uint8 * Serial_txBuffer;
    extern uint8   Serial_txDataBits;
    extern uint32  Serial_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 Serial_numberOfAddr;
    extern uint8 Serial_subAddrSize;
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (Serial_SCB_MODE_I2C_CONST_CFG || \
        Serial_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 Serial_IntrTxMask;
#endif /* (! (Serial_SCB_MODE_I2C_CONST_CFG || \
              Serial_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define Serial_SCB_MODE_I2C_RUNTM_CFG     (Serial_SCB_MODE_I2C      == Serial_scbMode)
    #define Serial_SCB_MODE_SPI_RUNTM_CFG     (Serial_SCB_MODE_SPI      == Serial_scbMode)
    #define Serial_SCB_MODE_UART_RUNTM_CFG    (Serial_SCB_MODE_UART     == Serial_scbMode)
    #define Serial_SCB_MODE_EZI2C_RUNTM_CFG   (Serial_SCB_MODE_EZI2C    == Serial_scbMode)
    #define Serial_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (Serial_SCB_MODE_UNCONFIG == Serial_scbMode)

    /* Defines wakeup enable */
    #define Serial_SCB_WAKE_ENABLE_CHECK       (0u != Serial_scbEnableWake)
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!Serial_CY_SCBIP_V1)
    #define Serial_SCB_PINS_NUMBER    (7u)
#else
    #define Serial_SCB_PINS_NUMBER    (2u)
#endif /* (!Serial_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_Serial_H) */


/* [] END OF FILE */

/***************************************************************************//**
* \file Serial_PM.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Power Management support for
*  the SCB Component.
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

#include "Serial.h"
#include "Serial_PVT.h"

#if(Serial_SCB_MODE_I2C_INC)
    #include "Serial_I2C_PVT.h"
#endif /* (Serial_SCB_MODE_I2C_INC) */

#if(Serial_SCB_MODE_EZI2C_INC)
    #include "Serial_EZI2C_PVT.h"
#endif /* (Serial_SCB_MODE_EZI2C_INC) */

#if(Serial_SCB_MODE_SPI_INC || Serial_SCB_MODE_UART_INC)
    #include "Serial_SPI_UART_PVT.h"
#endif /* (Serial_SCB_MODE_SPI_INC || Serial_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG || \
   (Serial_SCB_MODE_I2C_CONST_CFG   && (!Serial_I2C_WAKE_ENABLE_CONST))   || \
   (Serial_SCB_MODE_EZI2C_CONST_CFG && (!Serial_EZI2C_WAKE_ENABLE_CONST)) || \
   (Serial_SCB_MODE_SPI_CONST_CFG   && (!Serial_SPI_WAKE_ENABLE_CONST))   || \
   (Serial_SCB_MODE_UART_CONST_CFG  && (!Serial_UART_WAKE_ENABLE_CONST)))

    Serial_BACKUP_STRUCT Serial_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: Serial_Sleep
****************************************************************************//**
*
*  Prepares the Serial component to enter Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has an influence on this 
*  function implementation:
*  - Checked: configures the component to be wakeup source from Deep Sleep.
*  - Unchecked: stores the current component state (enabled or disabled) and 
*    disables the component. See SCB_Stop() function for details about component 
*    disabling.
*
*  Call the Serial_Sleep() function before calling the 
*  CyPmSysDeepSleep() function. 
*  Refer to the PSoC Creator System Reference Guide for more information about 
*  power management functions and Low power section of this document for the 
*  selected mode.
*
*  This function should not be called before entering Sleep.
*
*******************************************************************************/
void Serial_Sleep(void)
{
#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)

    if(Serial_SCB_WAKE_ENABLE_CHECK)
    {
        if(Serial_SCB_MODE_I2C_RUNTM_CFG)
        {
            Serial_I2CSaveConfig();
        }
        else if(Serial_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            Serial_EzI2CSaveConfig();
        }
    #if(!Serial_CY_SCBIP_V1)
        else if(Serial_SCB_MODE_SPI_RUNTM_CFG)
        {
            Serial_SpiSaveConfig();
        }
        else if(Serial_SCB_MODE_UART_RUNTM_CFG)
        {
            Serial_UartSaveConfig();
        }
    #endif /* (!Serial_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        Serial_backup.enableState = (uint8) Serial_GET_CTRL_ENABLED;

        if(0u != Serial_backup.enableState)
        {
            Serial_Stop();
        }
    }

#else

    #if (Serial_SCB_MODE_I2C_CONST_CFG && Serial_I2C_WAKE_ENABLE_CONST)
        Serial_I2CSaveConfig();

    #elif (Serial_SCB_MODE_EZI2C_CONST_CFG && Serial_EZI2C_WAKE_ENABLE_CONST)
        Serial_EzI2CSaveConfig();

    #elif (Serial_SCB_MODE_SPI_CONST_CFG && Serial_SPI_WAKE_ENABLE_CONST)
        Serial_SpiSaveConfig();

    #elif (Serial_SCB_MODE_UART_CONST_CFG && Serial_UART_WAKE_ENABLE_CONST)
        Serial_UartSaveConfig();

    #else

        Serial_backup.enableState = (uint8) Serial_GET_CTRL_ENABLED;

        if(0u != Serial_backup.enableState)
        {
            Serial_Stop();
        }

    #endif /* defined (Serial_SCB_MODE_I2C_CONST_CFG) && (Serial_I2C_WAKE_ENABLE_CONST) */

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Serial_Wakeup
****************************************************************************//**
*
*  Prepares the Serial component for Active mode operation after 
*  Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has influence on this 
*  function implementation:
*  - Checked: restores the component Active mode configuration.
*  - Unchecked: enables the component if it was enabled before enter Deep Sleep.
*
*  This function should not be called after exiting Sleep.
*
*  \sideeffect
*   Calling the Serial_Wakeup() function without first calling the 
*   Serial_Sleep() function may produce unexpected behavior.
*
*******************************************************************************/
void Serial_Wakeup(void)
{
#if(Serial_SCB_MODE_UNCONFIG_CONST_CFG)

    if(Serial_SCB_WAKE_ENABLE_CHECK)
    {
        if(Serial_SCB_MODE_I2C_RUNTM_CFG)
        {
            Serial_I2CRestoreConfig();
        }
        else if(Serial_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            Serial_EzI2CRestoreConfig();
        }
    #if(!Serial_CY_SCBIP_V1)
        else if(Serial_SCB_MODE_SPI_RUNTM_CFG)
        {
            Serial_SpiRestoreConfig();
        }
        else if(Serial_SCB_MODE_UART_RUNTM_CFG)
        {
            Serial_UartRestoreConfig();
        }
    #endif /* (!Serial_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != Serial_backup.enableState)
        {
            Serial_Enable();
        }
    }

#else

    #if (Serial_SCB_MODE_I2C_CONST_CFG  && Serial_I2C_WAKE_ENABLE_CONST)
        Serial_I2CRestoreConfig();

    #elif (Serial_SCB_MODE_EZI2C_CONST_CFG && Serial_EZI2C_WAKE_ENABLE_CONST)
        Serial_EzI2CRestoreConfig();

    #elif (Serial_SCB_MODE_SPI_CONST_CFG && Serial_SPI_WAKE_ENABLE_CONST)
        Serial_SpiRestoreConfig();

    #elif (Serial_SCB_MODE_UART_CONST_CFG && Serial_UART_WAKE_ENABLE_CONST)
        Serial_UartRestoreConfig();

    #else

        if(0u != Serial_backup.enableState)
        {
            Serial_Enable();
        }

    #endif /* (Serial_I2C_WAKE_ENABLE_CONST) */

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */

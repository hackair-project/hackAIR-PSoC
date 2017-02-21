/*******************************************************************************
* File Name: Serial_PM.c
* Version 3.10
*
* Description:
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
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
********************************************************************************
*
* Summary:
*  Prepares the component to enter Deep Sleep.
*  The "Enable wakeup from Sleep Mode" selection has an influence on
*  this function implementation.
*
* Parameters:
*  None
*
* Return:
*  None
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
********************************************************************************
*
* Summary:
*  Prepares the component for the Active mode operation after exiting
*  Deep Sleep. The "Enable wakeup from Sleep Mode" option has an influence
*  on this function implementation.
*  This function should not be called after exiting Sleep.
*
* Parameters:
*  None
*
* Return:
*  None
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

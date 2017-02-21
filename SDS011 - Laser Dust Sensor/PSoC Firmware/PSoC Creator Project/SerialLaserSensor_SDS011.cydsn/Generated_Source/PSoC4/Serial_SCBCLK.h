/*******************************************************************************
* File Name: Serial_SCBCLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Serial_SCBCLK_H)
#define CY_CLOCK_Serial_SCBCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void Serial_SCBCLK_StartEx(uint32 alignClkDiv);
#define Serial_SCBCLK_Start() \
    Serial_SCBCLK_StartEx(Serial_SCBCLK__PA_DIV_ID)

#else

void Serial_SCBCLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void Serial_SCBCLK_Stop(void);

void Serial_SCBCLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 Serial_SCBCLK_GetDividerRegister(void);
uint8  Serial_SCBCLK_GetFractionalDividerRegister(void);

#define Serial_SCBCLK_Enable()                         Serial_SCBCLK_Start()
#define Serial_SCBCLK_Disable()                        Serial_SCBCLK_Stop()
#define Serial_SCBCLK_SetDividerRegister(clkDivider, reset)  \
    Serial_SCBCLK_SetFractionalDividerRegister((clkDivider), 0u)
#define Serial_SCBCLK_SetDivider(clkDivider)           Serial_SCBCLK_SetDividerRegister((clkDivider), 1u)
#define Serial_SCBCLK_SetDividerValue(clkDivider)      Serial_SCBCLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define Serial_SCBCLK_DIV_ID     Serial_SCBCLK__DIV_ID

#define Serial_SCBCLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define Serial_SCBCLK_CTRL_REG   (*(reg32 *)Serial_SCBCLK__CTRL_REGISTER)
#define Serial_SCBCLK_DIV_REG    (*(reg32 *)Serial_SCBCLK__DIV_REGISTER)

#define Serial_SCBCLK_CMD_DIV_SHIFT          (0u)
#define Serial_SCBCLK_CMD_PA_DIV_SHIFT       (8u)
#define Serial_SCBCLK_CMD_DISABLE_SHIFT      (30u)
#define Serial_SCBCLK_CMD_ENABLE_SHIFT       (31u)

#define Serial_SCBCLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << Serial_SCBCLK_CMD_DISABLE_SHIFT))
#define Serial_SCBCLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << Serial_SCBCLK_CMD_ENABLE_SHIFT))

#define Serial_SCBCLK_DIV_FRAC_MASK  (0x000000F8u)
#define Serial_SCBCLK_DIV_FRAC_SHIFT (3u)
#define Serial_SCBCLK_DIV_INT_MASK   (0xFFFFFF00u)
#define Serial_SCBCLK_DIV_INT_SHIFT  (8u)

#else 

#define Serial_SCBCLK_DIV_REG        (*(reg32 *)Serial_SCBCLK__REGISTER)
#define Serial_SCBCLK_ENABLE_REG     Serial_SCBCLK_DIV_REG
#define Serial_SCBCLK_DIV_FRAC_MASK  Serial_SCBCLK__FRAC_MASK
#define Serial_SCBCLK_DIV_FRAC_SHIFT (16u)
#define Serial_SCBCLK_DIV_INT_MASK   Serial_SCBCLK__DIVIDER_MASK
#define Serial_SCBCLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_Serial_SCBCLK_H) */

/* [] END OF FILE */

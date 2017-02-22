/*******************************************************************************
* File Name: Sensor_Power.h  
* Version 2.10
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Sensor_Power_H) /* Pins Sensor_Power_H */
#define CY_PINS_Sensor_Power_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Sensor_Power_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    Sensor_Power_Write(uint8 value) ;
void    Sensor_Power_SetDriveMode(uint8 mode) ;
uint8   Sensor_Power_ReadDataReg(void) ;
uint8   Sensor_Power_Read(void) ;
uint8   Sensor_Power_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define Sensor_Power_DRIVE_MODE_BITS        (3)
#define Sensor_Power_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Sensor_Power_DRIVE_MODE_BITS))

#define Sensor_Power_DM_ALG_HIZ         (0x00u)
#define Sensor_Power_DM_DIG_HIZ         (0x01u)
#define Sensor_Power_DM_RES_UP          (0x02u)
#define Sensor_Power_DM_RES_DWN         (0x03u)
#define Sensor_Power_DM_OD_LO           (0x04u)
#define Sensor_Power_DM_OD_HI           (0x05u)
#define Sensor_Power_DM_STRONG          (0x06u)
#define Sensor_Power_DM_RES_UPDWN       (0x07u)

/* Digital Port Constants */
#define Sensor_Power_MASK               Sensor_Power__MASK
#define Sensor_Power_SHIFT              Sensor_Power__SHIFT
#define Sensor_Power_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Sensor_Power_PS                     (* (reg32 *) Sensor_Power__PS)
/* Port Configuration */
#define Sensor_Power_PC                     (* (reg32 *) Sensor_Power__PC)
/* Data Register */
#define Sensor_Power_DR                     (* (reg32 *) Sensor_Power__DR)
/* Input Buffer Disable Override */
#define Sensor_Power_INP_DIS                (* (reg32 *) Sensor_Power__PC2)


#if defined(Sensor_Power__INTSTAT)  /* Interrupt Registers */

    #define Sensor_Power_INTSTAT                (* (reg32 *) Sensor_Power__INTSTAT)

#endif /* Interrupt Registers */


/***************************************
* The following code is DEPRECATED and 
* must not be used.
***************************************/

#define Sensor_Power_DRIVE_MODE_SHIFT       (0x00u)
#define Sensor_Power_DRIVE_MODE_MASK        (0x07u << Sensor_Power_DRIVE_MODE_SHIFT)


#endif /* End Pins Sensor_Power_H */


/* [] END OF FILE */

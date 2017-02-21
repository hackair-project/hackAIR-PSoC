/*******************************************************************************
* File Name: Serial_rx.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Serial_rx_ALIASES_H) /* Pins Serial_rx_ALIASES_H */
#define CY_PINS_Serial_rx_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Serial_rx_0			(Serial_rx__0__PC)
#define Serial_rx_0_PS		(Serial_rx__0__PS)
#define Serial_rx_0_PC		(Serial_rx__0__PC)
#define Serial_rx_0_DR		(Serial_rx__0__DR)
#define Serial_rx_0_SHIFT	(Serial_rx__0__SHIFT)
#define Serial_rx_0_INTR	((uint16)((uint16)0x0003u << (Serial_rx__0__SHIFT*2u)))

#define Serial_rx_INTR_ALL	 ((uint16)(Serial_rx_0_INTR))


#endif /* End Pins Serial_rx_ALIASES_H */


/* [] END OF FILE */

/*******************************************************************************
* File Name: Adc_IN.h  
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

#if !defined(CY_PINS_Adc_IN_ALIASES_H) /* Pins Adc_IN_ALIASES_H */
#define CY_PINS_Adc_IN_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Adc_IN_0			(Adc_IN__0__PC)
#define Adc_IN_0_PS		(Adc_IN__0__PS)
#define Adc_IN_0_PC		(Adc_IN__0__PC)
#define Adc_IN_0_DR		(Adc_IN__0__DR)
#define Adc_IN_0_SHIFT	(Adc_IN__0__SHIFT)
#define Adc_IN_0_INTR	((uint16)((uint16)0x0003u << (Adc_IN__0__SHIFT*2u)))

#define Adc_IN_INTR_ALL	 ((uint16)(Adc_IN_0_INTR))


#endif /* End Pins Adc_IN_ALIASES_H */


/* [] END OF FILE */

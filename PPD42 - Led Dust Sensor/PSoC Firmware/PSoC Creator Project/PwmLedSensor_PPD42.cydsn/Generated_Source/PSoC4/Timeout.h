/*******************************************************************************
* File Name: Timeout.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Timeout_H)
#define CY_ISR_Timeout_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Timeout_Start(void);
void Timeout_StartEx(cyisraddress address);
void Timeout_Stop(void);

CY_ISR_PROTO(Timeout_Interrupt);

void Timeout_SetVector(cyisraddress address);
cyisraddress Timeout_GetVector(void);

void Timeout_SetPriority(uint8 priority);
uint8 Timeout_GetPriority(void);

void Timeout_Enable(void);
uint8 Timeout_GetState(void);
void Timeout_Disable(void);

void Timeout_SetPending(void);
void Timeout_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Timeout ISR. */
#define Timeout_INTC_VECTOR            ((reg32 *) Timeout__INTC_VECT)

/* Address of the Timeout ISR priority. */
#define Timeout_INTC_PRIOR             ((reg32 *) Timeout__INTC_PRIOR_REG)

/* Priority of the Timeout interrupt. */
#define Timeout_INTC_PRIOR_NUMBER      Timeout__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Timeout interrupt. */
#define Timeout_INTC_SET_EN            ((reg32 *) Timeout__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Timeout interrupt. */
#define Timeout_INTC_CLR_EN            ((reg32 *) Timeout__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Timeout interrupt state to pending. */
#define Timeout_INTC_SET_PD            ((reg32 *) Timeout__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Timeout interrupt. */
#define Timeout_INTC_CLR_PD            ((reg32 *) Timeout__INTC_CLR_PD_REG)



#endif /* CY_ISR_Timeout_H */


/* [] END OF FILE */

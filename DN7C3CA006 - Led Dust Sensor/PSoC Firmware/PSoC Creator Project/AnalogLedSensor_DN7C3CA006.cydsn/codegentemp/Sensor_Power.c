/*******************************************************************************
* File Name: Sensor_Power.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Sensor_Power.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        Sensor_Power_PC =   (Sensor_Power_PC & \
                                (uint32)(~(uint32)(Sensor_Power_DRIVE_MODE_IND_MASK << (Sensor_Power_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (Sensor_Power_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: Sensor_Power_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None 
*  
*******************************************************************************/
void Sensor_Power_Write(uint8 value) 
{
    uint8 drVal = (uint8)(Sensor_Power_DR & (uint8)(~Sensor_Power_MASK));
    drVal = (drVal | ((uint8)(value << Sensor_Power_SHIFT) & Sensor_Power_MASK));
    Sensor_Power_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: Sensor_Power_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  Sensor_Power_DM_STRONG     Strong Drive 
*  Sensor_Power_DM_OD_HI      Open Drain, Drives High 
*  Sensor_Power_DM_OD_LO      Open Drain, Drives Low 
*  Sensor_Power_DM_RES_UP     Resistive Pull Up 
*  Sensor_Power_DM_RES_DWN    Resistive Pull Down 
*  Sensor_Power_DM_RES_UPDWN  Resistive Pull Up/Down 
*  Sensor_Power_DM_DIG_HIZ    High Impedance Digital 
*  Sensor_Power_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void Sensor_Power_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(Sensor_Power__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: Sensor_Power_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro Sensor_Power_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Sensor_Power_Read(void) 
{
    return (uint8)((Sensor_Power_PS & Sensor_Power_MASK) >> Sensor_Power_SHIFT);
}


/*******************************************************************************
* Function Name: Sensor_Power_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 Sensor_Power_ReadDataReg(void) 
{
    return (uint8)((Sensor_Power_DR & Sensor_Power_MASK) >> Sensor_Power_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Sensor_Power_INTSTAT) 

    /*******************************************************************************
    * Function Name: Sensor_Power_ClearInterrupt
    ********************************************************************************
    *
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 Sensor_Power_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(Sensor_Power_INTSTAT & Sensor_Power_MASK);
		Sensor_Power_INTSTAT = maskedStatus;
        return maskedStatus >> Sensor_Power_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */

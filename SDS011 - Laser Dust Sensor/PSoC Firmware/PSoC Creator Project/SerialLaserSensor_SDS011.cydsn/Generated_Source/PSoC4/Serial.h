/***************************************************************************//**
* \file Serial.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values for the SCB Component.
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

#if !defined(CY_SCB_Serial_H)
#define CY_SCB_Serial_H

#include <cydevice_trm.h>
#include <cyfitter.h>
#include <cytypes.h>
#include <CyLib.h>

/* SCB IP block v0 is available in PSoC 4100/PSoC 4200 */
#define Serial_CY_SCBIP_V0    (CYIPBLOCK_m0s8scb_VERSION == 0u)
/* SCB IP block v1 is available in PSoC 4000 */
#define Serial_CY_SCBIP_V1    (CYIPBLOCK_m0s8scb_VERSION == 1u)
/* SCB IP block v2 is available in all other devices */
#define Serial_CY_SCBIP_V2    (CYIPBLOCK_m0s8scb_VERSION >= 2u)

/** Component version major.minor */
#define Serial_COMP_VERSION_MAJOR    (4)
#define Serial_COMP_VERSION_MINOR    (0)
    
#define Serial_SCB_MODE           (4u)

/* SCB modes enum */
#define Serial_SCB_MODE_I2C       (0x01u)
#define Serial_SCB_MODE_SPI       (0x02u)
#define Serial_SCB_MODE_UART      (0x04u)
#define Serial_SCB_MODE_EZI2C     (0x08u)
#define Serial_SCB_MODE_UNCONFIG  (0xFFu)

/* Condition compilation depends on operation mode: Unconfigured implies apply to all modes */
#define Serial_SCB_MODE_I2C_CONST_CFG       (Serial_SCB_MODE_I2C       == Serial_SCB_MODE)
#define Serial_SCB_MODE_SPI_CONST_CFG       (Serial_SCB_MODE_SPI       == Serial_SCB_MODE)
#define Serial_SCB_MODE_UART_CONST_CFG      (Serial_SCB_MODE_UART      == Serial_SCB_MODE)
#define Serial_SCB_MODE_EZI2C_CONST_CFG     (Serial_SCB_MODE_EZI2C     == Serial_SCB_MODE)
#define Serial_SCB_MODE_UNCONFIG_CONST_CFG  (Serial_SCB_MODE_UNCONFIG  == Serial_SCB_MODE)

/* Condition compilation for includes */
#define Serial_SCB_MODE_I2C_INC      (0u !=(Serial_SCB_MODE_I2C   & Serial_SCB_MODE))
#define Serial_SCB_MODE_EZI2C_INC    (0u !=(Serial_SCB_MODE_EZI2C & Serial_SCB_MODE))
#if (!Serial_CY_SCBIP_V1)
    #define Serial_SCB_MODE_SPI_INC  (0u !=(Serial_SCB_MODE_SPI   & Serial_SCB_MODE))
    #define Serial_SCB_MODE_UART_INC (0u !=(Serial_SCB_MODE_UART  & Serial_SCB_MODE))
#else
    #define Serial_SCB_MODE_SPI_INC  (0u)
    #define Serial_SCB_MODE_UART_INC (0u)
#endif /* (!Serial_CY_SCBIP_V1) */

/* Interrupts remove options */
#define Serial_REMOVE_SCB_IRQ             (0u)
#define Serial_SCB_IRQ_INTERNAL           (0u == Serial_REMOVE_SCB_IRQ)

#define Serial_REMOVE_UART_RX_WAKEUP_IRQ  (1u)
#define Serial_UART_RX_WAKEUP_IRQ         (0u == Serial_REMOVE_UART_RX_WAKEUP_IRQ)

/* SCB interrupt enum */
#define Serial_SCB_INTR_MODE_NONE     (0u)
#define Serial_SCB_INTR_MODE_INTERNAL (1u)
#define Serial_SCB_INTR_MODE_EXTERNAL (2u)

/* Internal clock remove option */
#define Serial_REMOVE_SCB_CLK     (0u)
#define Serial_SCB_CLK_INTERNAL   (0u == Serial_REMOVE_SCB_CLK)


/***************************************
*       Includes
****************************************/

#include "Serial_PINS.h"

#if (Serial_SCB_CLK_INTERNAL)
    #include "Serial_SCBCLK.h"
#endif /* (Serial_SCB_CLK_INTERNAL) */


/***************************************
*       Type Definitions
***************************************/

typedef struct
{
    uint8 enableState;
} Serial_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_general
* @{
*/

/* Start and Stop APIs */
void Serial_Init(void);
void Serial_Enable(void);
void Serial_Start(void);
void Serial_Stop(void);

/** @} general */

/**
* \addtogroup group_power
* @{
*/
/* Sleep and Wakeup APis */
void Serial_Sleep(void);
void Serial_Wakeup(void);
/** @} power */ 

/**
* \addtogroup group_interrupt
* @{
*/
#if (Serial_SCB_IRQ_INTERNAL)
    /* Custom interrupt handler */
    void Serial_SetCustomInterruptHandler(void (*func)(void));
#endif /* (Serial_SCB_IRQ_INTERNAL) */
/** @} interrupt */

/* Interface to internal interrupt component */
#if (Serial_SCB_IRQ_INTERNAL)
    /**
    * \addtogroup group_interrupt
    * @{
    */    
    /*******************************************************************************
    * Function Name: Serial_EnableInt
    ****************************************************************************//**
    *
    *  When using an Internal interrupt, this enables the interrupt in the NVIC. 
    *  When using an external interrupt the API for the interrupt component must 
    *  be used to enable the interrupt.
    *
    *******************************************************************************/
    #define Serial_EnableInt()    CyIntEnable(Serial_ISR_NUMBER)
    
    
    /*******************************************************************************
    * Function Name: Serial_DisableInt
    ****************************************************************************//**
    *
    *  When using an Internal interrupt, this disables the interrupt in the NVIC. 
    *  When using an external interrupt the API for the interrupt component must 
    *  be used to disable the interrupt.
    *
    *******************************************************************************/    
    #define Serial_DisableInt()   CyIntDisable(Serial_ISR_NUMBER)
    /** @} interrupt */

    /*******************************************************************************
    * Function Name: Serial_ClearPendingInt
    ****************************************************************************//**
    *
    *  This function clears the interrupt pending status in the NVIC. 
    *
    *******************************************************************************/
    #define Serial_ClearPendingInt()  CyIntClearPending(Serial_ISR_NUMBER)
#endif /* (Serial_SCB_IRQ_INTERNAL) */

#if (Serial_UART_RX_WAKEUP_IRQ)
    /*******************************************************************************
    * Function Name: Serial_RxWakeEnableInt
    ****************************************************************************//**
    *
    *  This function enables the interrupt (RX_WAKE) pending status in the NVIC. 
    *
    *******************************************************************************/    
    #define Serial_RxWakeEnableInt()  CyIntEnable(Serial_RX_WAKE_ISR_NUMBER)
    

    /*******************************************************************************
    * Function Name: Serial_RxWakeDisableInt
    ****************************************************************************//**
    *
    *  This function disables the interrupt (RX_WAKE) pending status in the NVIC.  
    *
    *******************************************************************************/
    #define Serial_RxWakeDisableInt() CyIntDisable(Serial_RX_WAKE_ISR_NUMBER)
    
    
    /*******************************************************************************
    * Function Name: Serial_RxWakeClearPendingInt
    ****************************************************************************//**
    *
    *  This function clears the interrupt (RX_WAKE) pending status in the NVIC. 
    *
    *******************************************************************************/    
    #define Serial_RxWakeClearPendingInt()  CyIntClearPending(Serial_RX_WAKE_ISR_NUMBER)
#endif /* (Serial_UART_RX_WAKEUP_IRQ) */

/**
* \addtogroup group_interrupt
* @{
*/
/* Get interrupt cause */
/*******************************************************************************
* Function Name: Serial_GetInterruptCause
****************************************************************************//**
*
*  Returns a mask of bits showing the source of the current triggered interrupt. 
*  This is useful for modes of operation where an interrupt can be generated by 
*  conditions in multiple interrupt source registers.
*
*  \return
*   Mask with the OR of the following conditions that have been triggered.
*    - Serial_INTR_CAUSE_MASTER - Interrupt from Master
*    - Serial_INTR_CAUSE_SLAVE - Interrupt from Slave
*    - Serial_INTR_CAUSE_TX - Interrupt from TX
*    - Serial_INTR_CAUSE_RX - Interrupt from RX
*
*******************************************************************************/
#define Serial_GetInterruptCause()    (Serial_INTR_CAUSE_REG)


/* APIs to service INTR_RX register */
/*******************************************************************************
* Function Name: Serial_GetRxInterruptSource
****************************************************************************//**
*
*  Returns RX interrupt request register. This register contains current status 
*  of RX interrupt sources.
*
*  \return
*   Current status of RX interrupt sources.
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - Serial_INTR_RX_FIFO_LEVEL - The number of data elements in the 
      RX FIFO is greater than the value of RX FIFO level.
*   - Serial_INTR_RX_NOT_EMPTY - Receiver FIFO is not empty.
*   - Serial_INTR_RX_FULL - Receiver FIFO is full.
*   - Serial_INTR_RX_OVERFLOW - Attempt to write to a full 
*     receiver FIFO.
*   - Serial_INTR_RX_UNDERFLOW - Attempt to read from an empty 
*     receiver FIFO.
*   - Serial_INTR_RX_FRAME_ERROR - UART framing error detected.
*   - Serial_INTR_RX_PARITY_ERROR - UART parity error detected.
*
*******************************************************************************/
#define Serial_GetRxInterruptSource() (Serial_INTR_RX_REG)


/*******************************************************************************
* Function Name: Serial_SetRxInterruptMode
****************************************************************************//**
*
*  Writes RX interrupt mask register. This register configures which bits from 
*  RX interrupt request register will trigger an interrupt event.
*
*  \param interruptMask: RX interrupt sources to be enabled (refer to 
*   Serial_GetRxInterruptSource() function for bit fields values).
*
*******************************************************************************/
#define Serial_SetRxInterruptMode(interruptMask)     Serial_WRITE_INTR_RX_MASK(interruptMask)


/*******************************************************************************
* Function Name: Serial_GetRxInterruptMode
****************************************************************************//**
*
*  Returns RX interrupt mask register This register specifies which bits from 
*  RX interrupt request register will trigger an interrupt event.
*
*  \return 
*   RX interrupt sources to be enabled (refer to 
*   Serial_GetRxInterruptSource() function for bit fields values).
*
*******************************************************************************/
#define Serial_GetRxInterruptMode()   (Serial_INTR_RX_MASK_REG)


/*******************************************************************************
* Function Name: Serial_GetRxInterruptSourceMasked
****************************************************************************//**
*
*  Returns RX interrupt masked request register. This register contains logical
*  AND of corresponding bits from RX interrupt request and mask registers.
*  This function is intended to be used in the interrupt service routine to 
*  identify which of enabled RX interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled RX interrupt sources (refer to 
*   Serial_GetRxInterruptSource() function for bit fields values).
*
*******************************************************************************/
#define Serial_GetRxInterruptSourceMasked()   (Serial_INTR_RX_MASKED_REG)


/*******************************************************************************
* Function Name: Serial_ClearRxInterruptSource
****************************************************************************//**
*
*  Clears RX interrupt sources in the interrupt request register.
*
*  \param interruptMask: RX interrupt sources to be cleared (refer to 
*   Serial_GetRxInterruptSource() function for bit fields values).
*
*  \sideeffects 
*   The side effects are listed in the table below for each 
*   affected interrupt source. Refer to section RX FIFO interrupt sources for 
*   detailed description.
*   - Serial_INTR_RX_FIFO_LEVEL Interrupt source is not cleared when 
*     the receiver FIFO has more entries than level.
*   - Serial_INTR_RX_NOT_EMPTY Interrupt source is not cleared when
*     receiver FIFO is not empty.
*   - Serial_INTR_RX_FULL Interrupt source is not cleared when 
*      receiver FIFO is full.
*
*******************************************************************************/
#define Serial_ClearRxInterruptSource(interruptMask)  Serial_CLEAR_INTR_RX(interruptMask)


/*******************************************************************************
* Function Name: Serial_SetRxInterrupt
****************************************************************************//**
*
*  Sets RX interrupt sources in the interrupt request register.
*
*  \param interruptMask: RX interrupt sources to set in the RX interrupt request 
*   register (refer to Serial_GetRxInterruptSource() function for bit 
*   fields values).
*
*******************************************************************************/
#define Serial_SetRxInterrupt(interruptMask)  Serial_SET_INTR_RX(interruptMask)

void Serial_SetRxFifoLevel(uint32 level);


/* APIs to service INTR_TX register */
/*******************************************************************************
* Function Name: Serial_GetTxInterruptSource
****************************************************************************//**
*
*  Returns TX interrupt request register. This register contains current status 
*  of TX interrupt sources.
* 
*  \return 
*   Current status of TX interrupt sources.
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - Serial_INTR_TX_FIFO_LEVEL - The number of data elements in the 
*     TX FIFO is less than the value of TX FIFO level.
*   - Serial_INTR_TX_NOT_FULL - Transmitter FIFO is not full.
*   - Serial_INTR_TX_EMPTY - Transmitter FIFO is empty.
*   - Serial_INTR_TX_OVERFLOW - Attempt to write to a full 
*     transmitter FIFO.
*   - Serial_INTR_TX_UNDERFLOW - Attempt to read from an empty 
*     transmitter FIFO.
*   - Serial_INTR_TX_UART_NACK - UART received a NACK in SmartCard 
*   mode.
*   - Serial_INTR_TX_UART_DONE - UART transfer is complete. 
*     All data elements from the TX FIFO are sent.
*   - Serial_INTR_TX_UART_ARB_LOST - Value on the TX line of the UART
*     does not match the value on the RX line.
*
*******************************************************************************/
#define Serial_GetTxInterruptSource() (Serial_INTR_TX_REG)


/*******************************************************************************
* Function Name: Serial_SetTxInterruptMode
****************************************************************************//**
*
*  Writes TX interrupt mask register. This register configures which bits from 
*  TX interrupt request register will trigger an interrupt event.
*
*  \param interruptMask: TX interrupt sources to be enabled (refer to 
*   Serial_GetTxInterruptSource() function for bit field values).
*
*******************************************************************************/
#define Serial_SetTxInterruptMode(interruptMask)  Serial_WRITE_INTR_TX_MASK(interruptMask)


/*******************************************************************************
* Function Name: Serial_GetTxInterruptMode
****************************************************************************//**
*
*  Returns TX interrupt mask register This register specifies which bits from 
*  TX interrupt request register will trigger an interrupt event.
*
*  \return 
*   Enabled TX interrupt sources (refer to 
*   Serial_GetTxInterruptSource() function for bit field values).
*   
*******************************************************************************/
#define Serial_GetTxInterruptMode()   (Serial_INTR_TX_MASK_REG)


/*******************************************************************************
* Function Name: Serial_GetTxInterruptSourceMasked
****************************************************************************//**
*
*  Returns TX interrupt masked request register. This register contains logical
*  AND of corresponding bits from TX interrupt request and mask registers.
*  This function is intended to be used in the interrupt service routine to identify 
*  which of enabled TX interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled TX interrupt sources (refer to 
*   Serial_GetTxInterruptSource() function for bit field values).
*
*******************************************************************************/
#define Serial_GetTxInterruptSourceMasked()   (Serial_INTR_TX_MASKED_REG)


/*******************************************************************************
* Function Name: Serial_ClearTxInterruptSource
****************************************************************************//**
*
*  Clears TX interrupt sources in the interrupt request register.
*
*  \param interruptMask: TX interrupt sources to be cleared (refer to 
*   Serial_GetTxInterruptSource() function for bit field values).
*
*  \sideeffects 
*   The side effects are listed in the table below for each affected interrupt 
*   source. Refer to section TX FIFO interrupt sources for detailed description.
*   - Serial_INTR_TX_FIFO_LEVEL - Interrupt source is not cleared when 
*     transmitter FIFO has less entries than level.
*   - Serial_INTR_TX_NOT_FULL - Interrupt source is not cleared when
*     transmitter FIFO has empty entries.
*   - Serial_INTR_TX_EMPTY - Interrupt source is not cleared when 
*     transmitter FIFO is empty.
*   - Serial_INTR_TX_UNDERFLOW - Interrupt source is not cleared when 
*     transmitter FIFO is empty and I2C mode with clock stretching is selected. 
*     Put data into the transmitter FIFO before clearing it. This behavior only 
*     applicable for PSoC 4100/PSoC 4200 devices.
*
*******************************************************************************/
#define Serial_ClearTxInterruptSource(interruptMask)  Serial_CLEAR_INTR_TX(interruptMask)


/*******************************************************************************
* Function Name: Serial_SetTxInterrupt
****************************************************************************//**
*
*  Sets RX interrupt sources in the interrupt request register.
*
*  \param interruptMask: RX interrupt sources to set in the RX interrupt request 
*   register (refer to Serial_GetRxInterruptSource() function for bit 
*   fields values).
*
*******************************************************************************/
#define Serial_SetTxInterrupt(interruptMask)  Serial_SET_INTR_TX(interruptMask)

void Serial_SetTxFifoLevel(uint32 level);


/* APIs to service INTR_MASTER register */
/*******************************************************************************
* Function Name: Serial_GetMasterInterruptSource
****************************************************************************//**
*
*  Returns Master interrupt request register. This register contains current 
*  status of Master interrupt sources.
*
*  \return 
*   Current status of Master interrupt sources. 
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - Serial_INTR_MASTER_SPI_DONE - SPI master transfer is complete.
*     Refer to Interrupt sources section for detailed description.
*   - Serial_INTR_MASTER_I2C_ARB_LOST - I2C master lost arbitration.
*   - Serial_INTR_MASTER_I2C_NACK - I2C master received negative 
*    acknowledgement (NAK).
*   - Serial_INTR_MASTER_I2C_ACK - I2C master received acknowledgement.
*   - Serial_INTR_MASTER_I2C_STOP - I2C master generated STOP.
*   - Serial_INTR_MASTER_I2C_BUS_ERROR - I2C master bus error 
*     (detection of unexpected START or STOP condition).
*
*******************************************************************************/
#define Serial_GetMasterInterruptSource() (Serial_INTR_MASTER_REG)

/*******************************************************************************
* Function Name: Serial_SetMasterInterruptMode
****************************************************************************//**
*
*  Writes Master interrupt mask register. This register configures which bits 
*  from Master interrupt request register will trigger an interrupt event.
*
*  \param interruptMask: Master interrupt sources to be enabled (refer to 
*   Serial_GetMasterInterruptSource() function for bit field values).
*
*******************************************************************************/
#define Serial_SetMasterInterruptMode(interruptMask)  Serial_WRITE_INTR_MASTER_MASK(interruptMask)

/*******************************************************************************
* Function Name: Serial_GetMasterInterruptMode
****************************************************************************//**
*
*  Returns Master interrupt mask register This register specifies which bits 
*  from Master interrupt request register will trigger an interrupt event.
*
*  \return 
*   Enabled Master interrupt sources (refer to 
*   Serial_GetMasterInterruptSource() function for return values).
*
*******************************************************************************/
#define Serial_GetMasterInterruptMode()   (Serial_INTR_MASTER_MASK_REG)

/*******************************************************************************
* Function Name: Serial_GetMasterInterruptSourceMasked
****************************************************************************//**
*
*  Returns Master interrupt masked request register. This register contains 
*  logical AND of corresponding bits from Master interrupt request and mask 
*  registers.
*  This function is intended to be used in the interrupt service routine to 
*  identify which of enabled Master interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled Master interrupt sources (refer to 
*   Serial_GetMasterInterruptSource() function for return values).
*
*******************************************************************************/
#define Serial_GetMasterInterruptSourceMasked()   (Serial_INTR_MASTER_MASKED_REG)

/*******************************************************************************
* Function Name: Serial_ClearMasterInterruptSource
****************************************************************************//**
*
*  Clears Master interrupt sources in the interrupt request register.
*
*  \param interruptMask: Master interrupt sources to be cleared (refer to 
*   Serial_GetMasterInterruptSource() function for bit field values).
*
*******************************************************************************/
#define Serial_ClearMasterInterruptSource(interruptMask)  Serial_CLEAR_INTR_MASTER(interruptMask)

/*******************************************************************************
* Function Name: Serial_SetMasterInterrupt
****************************************************************************//**
*
*  Sets Master interrupt sources in the interrupt request register.
*
*  \param interruptMask: Master interrupt sources to set in the Master interrupt
*   request register (refer to Serial_GetMasterInterruptSource() 
*   function for bit field values).
*
*******************************************************************************/
#define Serial_SetMasterInterrupt(interruptMask)  Serial_SET_INTR_MASTER(interruptMask)


/* APIs to service INTR_SLAVE register */
/*******************************************************************************
* Function Name: Serial_GetSlaveInterruptSource
****************************************************************************//**
*
*  Returns Slave interrupt request register. This register contains current 
*  status of Slave interrupt sources.
*
*  \return 
*   Current status of Slave interrupt sources.
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - Serial_INTR_SLAVE_I2C_ARB_LOST - I2C slave lost arbitration: 
*     the value driven on the SDA line is not the same as the value observed 
*     on the SDA line.
*   - Serial_INTR_SLAVE_I2C_NACK - I2C slave received negative 
*     acknowledgement (NAK).
*   - Serial_INTR_SLAVE_I2C_ACK - I2C slave received 
*     acknowledgement (ACK).
*   - Serial_INTR_SLAVE_I2C_WRITE_STOP - Stop or Repeated Start 
*     event for write transfer intended for this slave (address matching 
*     is performed).
*   - Serial_INTR_SLAVE_I2C_STOP - Stop or Repeated Start event 
*     for (read or write) transfer intended for this slave (address matching 
*     is performed).
*   - Serial_INTR_SLAVE_I2C_START - I2C slave received Start 
*     condition.
*   - Serial_INTR_SLAVE_I2C_ADDR_MATCH - I2C slave received matching 
*     address.
*   - Serial_INTR_SLAVE_I2C_GENERAL - I2C Slave received general 
*     call address.
*   - Serial_INTR_SLAVE_I2C_BUS_ERROR - I2C slave bus error (detection 
*      of unexpected Start or Stop condition).
*   - Serial_INTR_SLAVE_SPI_BUS_ERROR - SPI slave select line is 
*      deselected at an expected time while the SPI transfer.
*
*******************************************************************************/
#define Serial_GetSlaveInterruptSource()  (Serial_INTR_SLAVE_REG)

/*******************************************************************************
* Function Name: Serial_SetSlaveInterruptMode
****************************************************************************//**
*
*  Writes Slave interrupt mask register. 
*  This register configures which bits from Slave interrupt request register 
*  will trigger an interrupt event.
*
*  \param interruptMask: Slave interrupt sources to be enabled (refer to 
*   Serial_GetSlaveInterruptSource() function for bit field values).
*
*******************************************************************************/
#define Serial_SetSlaveInterruptMode(interruptMask)   Serial_WRITE_INTR_SLAVE_MASK(interruptMask)

/*******************************************************************************
* Function Name: Serial_GetSlaveInterruptMode
****************************************************************************//**
*
*  Returns Slave interrupt mask register.
*  This register specifies which bits from Slave interrupt request register 
*  will trigger an interrupt event.
*
*  \return 
*   Enabled Slave interrupt sources(refer to 
*   Serial_GetSlaveInterruptSource() function for bit field values).
*
*******************************************************************************/
#define Serial_GetSlaveInterruptMode()    (Serial_INTR_SLAVE_MASK_REG)

/*******************************************************************************
* Function Name: Serial_GetSlaveInterruptSourceMasked
****************************************************************************//**
*
*  Returns Slave interrupt masked request register. This register contains 
*  logical AND of corresponding bits from Slave interrupt request and mask 
*  registers.
*  This function is intended to be used in the interrupt service routine to 
*  identify which of enabled Slave interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled Slave interrupt sources (refer to 
*   Serial_GetSlaveInterruptSource() function for return values).
*
*******************************************************************************/
#define Serial_GetSlaveInterruptSourceMasked()    (Serial_INTR_SLAVE_MASKED_REG)

/*******************************************************************************
* Function Name: Serial_ClearSlaveInterruptSource
****************************************************************************//**
*
*  Clears Slave interrupt sources in the interrupt request register.
*
*  \param interruptMask: Slave interrupt sources to be cleared (refer to 
*   Serial_GetSlaveInterruptSource() function for return values).
*
*******************************************************************************/
#define Serial_ClearSlaveInterruptSource(interruptMask)   Serial_CLEAR_INTR_SLAVE(interruptMask)

/*******************************************************************************
* Function Name: Serial_SetSlaveInterrupt
****************************************************************************//**
*
*  Sets Slave interrupt sources in the interrupt request register.
*
*  \param interruptMask: Slave interrupt sources to set in the Slave interrupt 
*   request register (refer to Serial_GetSlaveInterruptSource() 
*   function for return values).
*
*******************************************************************************/
#define Serial_SetSlaveInterrupt(interruptMask)   Serial_SET_INTR_SLAVE(interruptMask)

/** @} interrupt */ 


/***************************************
*     Vars with External Linkage
***************************************/

/**
* \addtogroup group_globals
* @{
*/

/** Serial_initVar indicates whether the Serial 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the Serial_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  Serial_Init() function can be called before the 
*  Serial_Start() or Serial_Enable() function.
*/
extern uint8 Serial_initVar;
/** @} globals */

/***************************************
*              Registers
***************************************/

#define Serial_CTRL_REG               (*(reg32 *) Serial_SCB__CTRL)
#define Serial_CTRL_PTR               ( (reg32 *) Serial_SCB__CTRL)

#define Serial_STATUS_REG             (*(reg32 *) Serial_SCB__STATUS)
#define Serial_STATUS_PTR             ( (reg32 *) Serial_SCB__STATUS)

#if (!Serial_CY_SCBIP_V1)
    #define Serial_SPI_CTRL_REG           (*(reg32 *) Serial_SCB__SPI_CTRL)
    #define Serial_SPI_CTRL_PTR           ( (reg32 *) Serial_SCB__SPI_CTRL)

    #define Serial_SPI_STATUS_REG         (*(reg32 *) Serial_SCB__SPI_STATUS)
    #define Serial_SPI_STATUS_PTR         ( (reg32 *) Serial_SCB__SPI_STATUS)

    #define Serial_UART_CTRL_REG          (*(reg32 *) Serial_SCB__UART_CTRL)
    #define Serial_UART_CTRL_PTR          ( (reg32 *) Serial_SCB__UART_CTRL)

    #define Serial_UART_TX_CTRL_REG       (*(reg32 *) Serial_SCB__UART_TX_CTRL)
    #define Serial_UART_TX_CTRL_PTR       ( (reg32 *) Serial_SCB__UART_TX_CTRL)

    #define Serial_UART_RX_CTRL_REG       (*(reg32 *) Serial_SCB__UART_RX_CTRL)
    #define Serial_UART_RX_CTRL_PTR       ( (reg32 *) Serial_SCB__UART_RX_CTRL)

    #define Serial_UART_RX_STATUS_REG     (*(reg32 *) Serial_SCB__UART_RX_STATUS)
    #define Serial_UART_RX_STATUS_PTR     ( (reg32 *) Serial_SCB__UART_RX_STATUS)
#endif /* (!Serial_CY_SCBIP_V1) */

#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_UART_FLOW_CTRL_REG     (*(reg32 *) Serial_SCB__UART_FLOW_CTRL)
    #define Serial_UART_FLOW_CTRL_PTR     ( (reg32 *) Serial_SCB__UART_FLOW_CTRL)
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#define Serial_I2C_CTRL_REG           (*(reg32 *) Serial_SCB__I2C_CTRL)
#define Serial_I2C_CTRL_PTR           ( (reg32 *) Serial_SCB__I2C_CTRL)

#define Serial_I2C_STATUS_REG         (*(reg32 *) Serial_SCB__I2C_STATUS)
#define Serial_I2C_STATUS_PTR         ( (reg32 *) Serial_SCB__I2C_STATUS)

#define Serial_I2C_MASTER_CMD_REG     (*(reg32 *) Serial_SCB__I2C_M_CMD)
#define Serial_I2C_MASTER_CMD_PTR     ( (reg32 *) Serial_SCB__I2C_M_CMD)

#define Serial_I2C_SLAVE_CMD_REG      (*(reg32 *) Serial_SCB__I2C_S_CMD)
#define Serial_I2C_SLAVE_CMD_PTR      ( (reg32 *) Serial_SCB__I2C_S_CMD)

#define Serial_I2C_CFG_REG            (*(reg32 *) Serial_SCB__I2C_CFG)
#define Serial_I2C_CFG_PTR            ( (reg32 *) Serial_SCB__I2C_CFG)

#define Serial_TX_CTRL_REG            (*(reg32 *) Serial_SCB__TX_CTRL)
#define Serial_TX_CTRL_PTR            ( (reg32 *) Serial_SCB__TX_CTRL)

#define Serial_TX_FIFO_CTRL_REG       (*(reg32 *) Serial_SCB__TX_FIFO_CTRL)
#define Serial_TX_FIFO_CTRL_PTR       ( (reg32 *) Serial_SCB__TX_FIFO_CTRL)

#define Serial_TX_FIFO_STATUS_REG     (*(reg32 *) Serial_SCB__TX_FIFO_STATUS)
#define Serial_TX_FIFO_STATUS_PTR     ( (reg32 *) Serial_SCB__TX_FIFO_STATUS)

#define Serial_TX_FIFO_WR_REG         (*(reg32 *) Serial_SCB__TX_FIFO_WR)
#define Serial_TX_FIFO_WR_PTR         ( (reg32 *) Serial_SCB__TX_FIFO_WR)

#define Serial_RX_CTRL_REG            (*(reg32 *) Serial_SCB__RX_CTRL)
#define Serial_RX_CTRL_PTR            ( (reg32 *) Serial_SCB__RX_CTRL)

#define Serial_RX_FIFO_CTRL_REG       (*(reg32 *) Serial_SCB__RX_FIFO_CTRL)
#define Serial_RX_FIFO_CTRL_PTR       ( (reg32 *) Serial_SCB__RX_FIFO_CTRL)

#define Serial_RX_FIFO_STATUS_REG     (*(reg32 *) Serial_SCB__RX_FIFO_STATUS)
#define Serial_RX_FIFO_STATUS_PTR     ( (reg32 *) Serial_SCB__RX_FIFO_STATUS)

#define Serial_RX_MATCH_REG           (*(reg32 *) Serial_SCB__RX_MATCH)
#define Serial_RX_MATCH_PTR           ( (reg32 *) Serial_SCB__RX_MATCH)

#define Serial_RX_FIFO_RD_REG         (*(reg32 *) Serial_SCB__RX_FIFO_RD)
#define Serial_RX_FIFO_RD_PTR         ( (reg32 *) Serial_SCB__RX_FIFO_RD)

#define Serial_RX_FIFO_RD_SILENT_REG  (*(reg32 *) Serial_SCB__RX_FIFO_RD_SILENT)
#define Serial_RX_FIFO_RD_SILENT_PTR  ( (reg32 *) Serial_SCB__RX_FIFO_RD_SILENT)

#ifdef Serial_SCB__EZ_DATA0
    #define Serial_EZBUF_DATA0_REG    (*(reg32 *) Serial_SCB__EZ_DATA0)
    #define Serial_EZBUF_DATA0_PTR    ( (reg32 *) Serial_SCB__EZ_DATA0)
#else
    #define Serial_EZBUF_DATA0_REG    (*(reg32 *) Serial_SCB__EZ_DATA00)
    #define Serial_EZBUF_DATA0_PTR    ( (reg32 *) Serial_SCB__EZ_DATA00)
#endif /* Serial_SCB__EZ_DATA00 */

#define Serial_INTR_CAUSE_REG         (*(reg32 *) Serial_SCB__INTR_CAUSE)
#define Serial_INTR_CAUSE_PTR         ( (reg32 *) Serial_SCB__INTR_CAUSE)

#define Serial_INTR_I2C_EC_REG        (*(reg32 *) Serial_SCB__INTR_I2C_EC)
#define Serial_INTR_I2C_EC_PTR        ( (reg32 *) Serial_SCB__INTR_I2C_EC)

#define Serial_INTR_I2C_EC_MASK_REG   (*(reg32 *) Serial_SCB__INTR_I2C_EC_MASK)
#define Serial_INTR_I2C_EC_MASK_PTR   ( (reg32 *) Serial_SCB__INTR_I2C_EC_MASK)

#define Serial_INTR_I2C_EC_MASKED_REG (*(reg32 *) Serial_SCB__INTR_I2C_EC_MASKED)
#define Serial_INTR_I2C_EC_MASKED_PTR ( (reg32 *) Serial_SCB__INTR_I2C_EC_MASKED)

#if (!Serial_CY_SCBIP_V1)
    #define Serial_INTR_SPI_EC_REG        (*(reg32 *) Serial_SCB__INTR_SPI_EC)
    #define Serial_INTR_SPI_EC_PTR        ( (reg32 *) Serial_SCB__INTR_SPI_EC)

    #define Serial_INTR_SPI_EC_MASK_REG   (*(reg32 *) Serial_SCB__INTR_SPI_EC_MASK)
    #define Serial_INTR_SPI_EC_MASK_PTR   ( (reg32 *) Serial_SCB__INTR_SPI_EC_MASK)

    #define Serial_INTR_SPI_EC_MASKED_REG (*(reg32 *) Serial_SCB__INTR_SPI_EC_MASKED)
    #define Serial_INTR_SPI_EC_MASKED_PTR ( (reg32 *) Serial_SCB__INTR_SPI_EC_MASKED)
#endif /* (!Serial_CY_SCBIP_V1) */

#define Serial_INTR_MASTER_REG        (*(reg32 *) Serial_SCB__INTR_M)
#define Serial_INTR_MASTER_PTR        ( (reg32 *) Serial_SCB__INTR_M)

#define Serial_INTR_MASTER_SET_REG    (*(reg32 *) Serial_SCB__INTR_M_SET)
#define Serial_INTR_MASTER_SET_PTR    ( (reg32 *) Serial_SCB__INTR_M_SET)

#define Serial_INTR_MASTER_MASK_REG   (*(reg32 *) Serial_SCB__INTR_M_MASK)
#define Serial_INTR_MASTER_MASK_PTR   ( (reg32 *) Serial_SCB__INTR_M_MASK)

#define Serial_INTR_MASTER_MASKED_REG (*(reg32 *) Serial_SCB__INTR_M_MASKED)
#define Serial_INTR_MASTER_MASKED_PTR ( (reg32 *) Serial_SCB__INTR_M_MASKED)

#define Serial_INTR_SLAVE_REG         (*(reg32 *) Serial_SCB__INTR_S)
#define Serial_INTR_SLAVE_PTR         ( (reg32 *) Serial_SCB__INTR_S)

#define Serial_INTR_SLAVE_SET_REG     (*(reg32 *) Serial_SCB__INTR_S_SET)
#define Serial_INTR_SLAVE_SET_PTR     ( (reg32 *) Serial_SCB__INTR_S_SET)

#define Serial_INTR_SLAVE_MASK_REG    (*(reg32 *) Serial_SCB__INTR_S_MASK)
#define Serial_INTR_SLAVE_MASK_PTR    ( (reg32 *) Serial_SCB__INTR_S_MASK)

#define Serial_INTR_SLAVE_MASKED_REG  (*(reg32 *) Serial_SCB__INTR_S_MASKED)
#define Serial_INTR_SLAVE_MASKED_PTR  ( (reg32 *) Serial_SCB__INTR_S_MASKED)

#define Serial_INTR_TX_REG            (*(reg32 *) Serial_SCB__INTR_TX)
#define Serial_INTR_TX_PTR            ( (reg32 *) Serial_SCB__INTR_TX)

#define Serial_INTR_TX_SET_REG        (*(reg32 *) Serial_SCB__INTR_TX_SET)
#define Serial_INTR_TX_SET_PTR        ( (reg32 *) Serial_SCB__INTR_TX_SET)

#define Serial_INTR_TX_MASK_REG       (*(reg32 *) Serial_SCB__INTR_TX_MASK)
#define Serial_INTR_TX_MASK_PTR       ( (reg32 *) Serial_SCB__INTR_TX_MASK)

#define Serial_INTR_TX_MASKED_REG     (*(reg32 *) Serial_SCB__INTR_TX_MASKED)
#define Serial_INTR_TX_MASKED_PTR     ( (reg32 *) Serial_SCB__INTR_TX_MASKED)

#define Serial_INTR_RX_REG            (*(reg32 *) Serial_SCB__INTR_RX)
#define Serial_INTR_RX_PTR            ( (reg32 *) Serial_SCB__INTR_RX)

#define Serial_INTR_RX_SET_REG        (*(reg32 *) Serial_SCB__INTR_RX_SET)
#define Serial_INTR_RX_SET_PTR        ( (reg32 *) Serial_SCB__INTR_RX_SET)

#define Serial_INTR_RX_MASK_REG       (*(reg32 *) Serial_SCB__INTR_RX_MASK)
#define Serial_INTR_RX_MASK_PTR       ( (reg32 *) Serial_SCB__INTR_RX_MASK)

#define Serial_INTR_RX_MASKED_REG     (*(reg32 *) Serial_SCB__INTR_RX_MASKED)
#define Serial_INTR_RX_MASKED_PTR     ( (reg32 *) Serial_SCB__INTR_RX_MASKED)

/* Defines get from SCB IP parameters. */
#define Serial_FIFO_SIZE      (8u)  /* TX or RX FIFO size. */
#define Serial_EZ_DATA_NR     (32u)  /* Number of words in EZ memory. */ 
#define Serial_ONE_BYTE_WIDTH (8u)            /* Number of bits in one byte. */
#define Serial_FF_DATA_NR_LOG2_MASK       (0x0Fu)      /* Number of bits to represent a FIFO address. */
#define Serial_FF_DATA_NR_LOG2_PLUS1_MASK (0x1Fu) /* Number of bits to represent #bytes in FIFO. */


/***************************************
*        Registers Constants
***************************************/

#if (Serial_SCB_IRQ_INTERNAL)
    #define Serial_ISR_NUMBER     ((uint8) Serial_SCB_IRQ__INTC_NUMBER)
    #define Serial_ISR_PRIORITY   ((uint8) Serial_SCB_IRQ__INTC_PRIOR_NUM)
#endif /* (Serial_SCB_IRQ_INTERNAL) */

#if (Serial_UART_RX_WAKEUP_IRQ)
    #define Serial_RX_WAKE_ISR_NUMBER     ((uint8) Serial_RX_WAKEUP_IRQ__INTC_NUMBER)
    #define Serial_RX_WAKE_ISR_PRIORITY   ((uint8) Serial_RX_WAKEUP_IRQ__INTC_PRIOR_NUM)
#endif /* (Serial_UART_RX_WAKEUP_IRQ) */

/* Serial_CTRL_REG */
#define Serial_CTRL_OVS_POS           (0u)  /* [3:0]   Oversampling factor                 */
#define Serial_CTRL_EC_AM_MODE_POS    (8u)  /* [8]     Externally clocked address match    */
#define Serial_CTRL_EC_OP_MODE_POS    (9u)  /* [9]     Externally clocked operation mode   */
#define Serial_CTRL_EZBUF_MODE_POS    (10u) /* [10]    EZ buffer is enabled                */
#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_CTRL_BYTE_MODE_POS (11u) /* [11]    Determines the number of bits per FIFO data element */
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */
#define Serial_CTRL_ADDR_ACCEPT_POS   (16u) /* [16]    Put matched address in RX FIFO       */
#define Serial_CTRL_BLOCK_POS         (17u) /* [17]    Ext and Int logic to resolve collide */
#define Serial_CTRL_MODE_POS          (24u) /* [25:24] Operation mode                       */
#define Serial_CTRL_ENABLED_POS       (31u) /* [31]    Enable SCB block                     */
#define Serial_CTRL_OVS_MASK          ((uint32) 0x0Fu)
#define Serial_CTRL_EC_AM_MODE        ((uint32) 0x01u << Serial_CTRL_EC_AM_MODE_POS)
#define Serial_CTRL_EC_OP_MODE        ((uint32) 0x01u << Serial_CTRL_EC_OP_MODE_POS)
#define Serial_CTRL_EZBUF_MODE        ((uint32) 0x01u << Serial_CTRL_EZBUF_MODE_POS)
#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_CTRL_BYTE_MODE ((uint32) 0x01u << Serial_CTRL_BYTE_MODE_POS)
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */
#define Serial_CTRL_ADDR_ACCEPT       ((uint32) 0x01u << Serial_CTRL_ADDR_ACCEPT_POS)
#define Serial_CTRL_BLOCK             ((uint32) 0x01u << Serial_CTRL_BLOCK_POS)
#define Serial_CTRL_MODE_MASK         ((uint32) 0x03u << Serial_CTRL_MODE_POS)
#define Serial_CTRL_MODE_I2C          ((uint32) 0x00u)
#define Serial_CTRL_MODE_SPI          ((uint32) 0x01u << Serial_CTRL_MODE_POS)
#define Serial_CTRL_MODE_UART         ((uint32) 0x02u << Serial_CTRL_MODE_POS)
#define Serial_CTRL_ENABLED           ((uint32) 0x01u << Serial_CTRL_ENABLED_POS)

/* Serial_STATUS_REG */
#define Serial_STATUS_EC_BUSY_POS     (0u)  /* [0] Bus busy. Externally clocked logic access to EZ memory */
#define Serial_STATUS_EC_BUSY         ((uint32) 0x0Fu)

/* Serial_SPI_CTRL_REG  */
#define Serial_SPI_CTRL_CONTINUOUS_POS        (0u)  /* [0]     Continuous or Separated SPI data transfers */
#define Serial_SPI_CTRL_SELECT_PRECEDE_POS    (1u)  /* [1]     Precedes or coincides start of data frame  */
#define Serial_SPI_CTRL_CPHA_POS              (2u)  /* [2]     SCLK phase                                 */
#define Serial_SPI_CTRL_CPOL_POS              (3u)  /* [3]     SCLK polarity                              */
#define Serial_SPI_CTRL_LATE_MISO_SAMPLE_POS  (4u)  /* [4]     Late MISO sample enabled                   */
#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_SPI_CTRL_SCLK_CONTINUOUS_POS   (5u)  /* [5]     Enable continuous SCLK generation */
    #define Serial_SPI_CTRL_SSEL0_POLARITY_POS    (8u)  /* [8]     SS0 polarity                      */
    #define Serial_SPI_CTRL_SSEL1_POLARITY_POS    (9u)  /* [9]     SS1 polarity                      */
    #define Serial_SPI_CTRL_SSEL2_POLARITY_POS    (10u) /* [10]    SS2 polarity                      */
    #define Serial_SPI_CTRL_SSEL3_POLARITY_POS    (11u) /* [11]    SS3 polarity                      */
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */
#define Serial_SPI_CTRL_LOOPBACK_POS          (16u) /* [16]    Local loop-back control enabled            */
#define Serial_SPI_CTRL_MODE_POS              (24u) /* [25:24] Submode of SPI operation                   */
#define Serial_SPI_CTRL_SLAVE_SELECT_POS      (26u) /* [27:26] Selects SPI SS signal                      */
#define Serial_SPI_CTRL_MASTER_MODE_POS       (31u) /* [31]    Master mode enabled                        */
#define Serial_SPI_CTRL_CONTINUOUS            ((uint32) 0x01u)
#define Serial_SPI_CTRL_SELECT_PRECEDE        ((uint32) 0x01u << Serial_SPI_CTRL_SELECT_PRECEDE_POS)
#define Serial_SPI_CTRL_SCLK_MODE_MASK        ((uint32) 0x03u << Serial_SPI_CTRL_CPHA_POS)
#define Serial_SPI_CTRL_CPHA                  ((uint32) 0x01u << Serial_SPI_CTRL_CPHA_POS)
#define Serial_SPI_CTRL_CPOL                  ((uint32) 0x01u << Serial_SPI_CTRL_CPOL_POS)
#define Serial_SPI_CTRL_LATE_MISO_SAMPLE      ((uint32) 0x01u << \
                                                                    Serial_SPI_CTRL_LATE_MISO_SAMPLE_POS)
#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_SPI_CTRL_SCLK_CONTINUOUS  ((uint32) 0x01u << Serial_SPI_CTRL_SCLK_CONTINUOUS_POS)
    #define Serial_SPI_CTRL_SSEL0_POLARITY   ((uint32) 0x01u << Serial_SPI_CTRL_SSEL0_POLARITY_POS)
    #define Serial_SPI_CTRL_SSEL1_POLARITY   ((uint32) 0x01u << Serial_SPI_CTRL_SSEL1_POLARITY_POS)
    #define Serial_SPI_CTRL_SSEL2_POLARITY   ((uint32) 0x01u << Serial_SPI_CTRL_SSEL2_POLARITY_POS)
    #define Serial_SPI_CTRL_SSEL3_POLARITY   ((uint32) 0x01u << Serial_SPI_CTRL_SSEL3_POLARITY_POS)
    #define Serial_SPI_CTRL_SSEL_POLARITY_MASK ((uint32)0x0Fu << Serial_SPI_CTRL_SSEL0_POLARITY_POS)
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#define Serial_SPI_CTRL_LOOPBACK              ((uint32) 0x01u << Serial_SPI_CTRL_LOOPBACK_POS)
#define Serial_SPI_CTRL_MODE_MASK             ((uint32) 0x03u << Serial_SPI_CTRL_MODE_POS)
#define Serial_SPI_CTRL_MODE_MOTOROLA         ((uint32) 0x00u)
#define Serial_SPI_CTRL_MODE_TI               ((uint32) 0x01u << Serial_CTRL_MODE_POS)
#define Serial_SPI_CTRL_MODE_NS               ((uint32) 0x02u << Serial_CTRL_MODE_POS)
#define Serial_SPI_CTRL_SLAVE_SELECT_MASK     ((uint32) 0x03u << Serial_SPI_CTRL_SLAVE_SELECT_POS)
#define Serial_SPI_CTRL_SLAVE_SELECT0         ((uint32) 0x00u)
#define Serial_SPI_CTRL_SLAVE_SELECT1         ((uint32) 0x01u << Serial_SPI_CTRL_SLAVE_SELECT_POS)
#define Serial_SPI_CTRL_SLAVE_SELECT2         ((uint32) 0x02u << Serial_SPI_CTRL_SLAVE_SELECT_POS)
#define Serial_SPI_CTRL_SLAVE_SELECT3         ((uint32) 0x03u << Serial_SPI_CTRL_SLAVE_SELECT_POS)
#define Serial_SPI_CTRL_MASTER                ((uint32) 0x01u << Serial_SPI_CTRL_MASTER_MODE_POS)
#define Serial_SPI_CTRL_SLAVE                 ((uint32) 0x00u)

/* Serial_SPI_STATUS_REG  */
#define Serial_SPI_STATUS_BUS_BUSY_POS    (0u)  /* [0]    Bus busy - slave selected */
#define Serial_SPI_STATUS_EZBUF_ADDR_POS  (8u)  /* [15:8] EzAddress                 */
#define Serial_SPI_STATUS_BUS_BUSY        ((uint32) 0x01u)
#define Serial_SPI_STATUS_EZBUF_ADDR_MASK ((uint32) 0xFFu << Serial_I2C_STATUS_EZBUF_ADDR_POS)

/* Serial_UART_CTRL */
#define Serial_UART_CTRL_LOOPBACK_POS         (16u) /* [16] Loop-back    */
#define Serial_UART_CTRL_MODE_POS             (24u) /* [24] UART subMode */
#define Serial_UART_CTRL_LOOPBACK             ((uint32) 0x01u << Serial_UART_CTRL_LOOPBACK_POS)
#define Serial_UART_CTRL_MODE_UART_STD        ((uint32) 0x00u)
#define Serial_UART_CTRL_MODE_UART_SMARTCARD  ((uint32) 0x01u << Serial_UART_CTRL_MODE_POS)
#define Serial_UART_CTRL_MODE_UART_IRDA       ((uint32) 0x02u << Serial_UART_CTRL_MODE_POS)
#define Serial_UART_CTRL_MODE_MASK            ((uint32) 0x03u << Serial_UART_CTRL_MODE_POS)

/* Serial_UART_TX_CTRL */
#define Serial_UART_TX_CTRL_STOP_BITS_POS         (0u)  /* [2:0] Stop bits: (Stop bits + 1) * 0.5 period */
#define Serial_UART_TX_CTRL_PARITY_POS            (4u)  /* [4]   Parity bit                              */
#define Serial_UART_TX_CTRL_PARITY_ENABLED_POS    (5u)  /* [5]   Parity enable                           */
#define Serial_UART_TX_CTRL_RETRY_ON_NACK_POS     (8u)  /* [8]   Smart Card: re-send frame on NACK       */
#define Serial_UART_TX_CTRL_ONE_STOP_BIT          ((uint32) 0x01u)
#define Serial_UART_TX_CTRL_ONE_HALF_STOP_BITS    ((uint32) 0x02u)
#define Serial_UART_TX_CTRL_TWO_STOP_BITS         ((uint32) 0x03u)
#define Serial_UART_TX_CTRL_STOP_BITS_MASK        ((uint32) 0x07u)
#define Serial_UART_TX_CTRL_PARITY                ((uint32) 0x01u << \
                                                                    Serial_UART_TX_CTRL_PARITY_POS)
#define Serial_UART_TX_CTRL_PARITY_ENABLED        ((uint32) 0x01u << \
                                                                    Serial_UART_TX_CTRL_PARITY_ENABLED_POS)
#define Serial_UART_TX_CTRL_RETRY_ON_NACK         ((uint32) 0x01u << \
                                                                    Serial_UART_TX_CTRL_RETRY_ON_NACK_POS)

/* Serial_UART_RX_CTRL */
#define Serial_UART_RX_CTRL_STOP_BITS_POS             (0u)  /* [2:0] Stop bits: (Stop bits + 1) * 0.5 period*/
#define Serial_UART_RX_CTRL_PARITY_POS                (4u)  /* [4]   Parity bit                             */
#define Serial_UART_RX_CTRL_PARITY_ENABLED_POS        (5u)  /* [5]   Parity enable                          */
#define Serial_UART_RX_CTRL_POLARITY_POS              (6u)  /* [6]   IrDA: inverts polarity of RX signal    */
#define Serial_UART_RX_CTRL_DROP_ON_PARITY_ERR_POS    (8u)  /* [8]   Drop and lost RX FIFO on parity error  */
#define Serial_UART_RX_CTRL_DROP_ON_FRAME_ERR_POS     (9u)  /* [9]   Drop and lost RX FIFO on frame error   */
#define Serial_UART_RX_CTRL_MP_MODE_POS               (10u) /* [10]  Multi-processor mode                   */
#define Serial_UART_RX_CTRL_LIN_MODE_POS              (12u) /* [12]  Lin mode: applicable for UART Standard */
#define Serial_UART_RX_CTRL_SKIP_START_POS            (13u) /* [13]  Skip start not: only for UART Standard */
#define Serial_UART_RX_CTRL_BREAK_WIDTH_POS           (16u) /* [19:16]  Break width: (Break width + 1)      */
#define Serial_UART_TX_CTRL_ONE_STOP_BIT              ((uint32) 0x01u)
#define Serial_UART_TX_CTRL_ONE_HALF_STOP_BITS        ((uint32) 0x02u)
#define Serial_UART_TX_CTRL_TWO_STOP_BITS             ((uint32) 0x03u)
#define Serial_UART_RX_CTRL_STOP_BITS_MASK            ((uint32) 0x07u)
#define Serial_UART_RX_CTRL_PARITY                    ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_PARITY_POS)
#define Serial_UART_RX_CTRL_PARITY_ENABLED            ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_PARITY_ENABLED_POS)
#define Serial_UART_RX_CTRL_POLARITY                  ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_POLARITY_POS)
#define Serial_UART_RX_CTRL_DROP_ON_PARITY_ERR        ((uint32) 0x01u << \
                                                                   Serial_UART_RX_CTRL_DROP_ON_PARITY_ERR_POS)
#define Serial_UART_RX_CTRL_DROP_ON_FRAME_ERR         ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_DROP_ON_FRAME_ERR_POS)
#define Serial_UART_RX_CTRL_MP_MODE                   ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_MP_MODE_POS)
#define Serial_UART_RX_CTRL_LIN_MODE                  ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_LIN_MODE_POS)
#define Serial_UART_RX_CTRL_SKIP_START                ((uint32) 0x01u << \
                                                                    Serial_UART_RX_CTRL_SKIP_START_POS)
#define Serial_UART_RX_CTRL_BREAK_WIDTH_MASK          ((uint32) 0x0Fu << \
                                                                    Serial_UART_RX_CTRL_BREAK_WIDTH_POS)
/* Serial_UART_RX_STATUS_REG */
#define Serial_UART_RX_STATUS_BR_COUNTER_POS     (0u)  /* [11:0] Baud Rate counter */
#define Serial_UART_RX_STATUS_BR_COUNTER_MASK    ((uint32) 0xFFFu)

#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    /* Serial_UART_FLOW_CTRL_REG */
    #define Serial_UART_FLOW_CTRL_TRIGGER_LEVEL_POS    (0u)  /* [7:0] RTS RX FIFO trigger level         */
    #define Serial_UART_FLOW_CTRL_RTS_POLARITY_POS     (16u) /* [16]  Polarity of the RTS output signal */
    #define Serial_UART_FLOW_CTRL_CTS_POLARITY_POS     (24u) /* [24]  Polarity of the CTS input signal  */
    #define Serial_UART_FLOW_CTRL_CTS_ENABLED_POS      (25u) /* [25]  Enable CTS signal                 */
    #define Serial_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK   ((uint32) Serial_FF_DATA_NR_LOG2_MASK)
    #define Serial_UART_FLOW_CTRL_RTS_POLARITY         ((uint32) 0x01u << \
                                                                       Serial_UART_FLOW_CTRL_RTS_POLARITY_POS)
    #define Serial_UART_FLOW_CTRL_CTS_POLARITY         ((uint32) 0x01u << \
                                                                       Serial_UART_FLOW_CTRL_CTS_POLARITY_POS)
    #define Serial_UART_FLOW_CTRL_CTS_ENABLE           ((uint32) 0x01u << \
                                                                       Serial_UART_FLOW_CTRL_CTS_ENABLED_POS)
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

/* Serial_I2C_CTRL */
#define Serial_I2C_CTRL_HIGH_PHASE_OVS_POS           (0u)   /* [3:0] Oversampling factor high: master only */
#define Serial_I2C_CTRL_LOW_PHASE_OVS_POS            (4u)   /* [7:4] Oversampling factor low:  master only */
#define Serial_I2C_CTRL_M_READY_DATA_ACK_POS         (8u)   /* [8]   Master ACKs data while RX FIFO != FULL*/
#define Serial_I2C_CTRL_M_NOT_READY_DATA_NACK_POS    (9u)   /* [9]   Master NACKs data if RX FIFO ==  FULL */
#define Serial_I2C_CTRL_S_GENERAL_IGNORE_POS         (11u)  /* [11]  Slave ignores General call            */
#define Serial_I2C_CTRL_S_READY_ADDR_ACK_POS         (12u)  /* [12]  Slave ACKs Address if RX FIFO != FULL */
#define Serial_I2C_CTRL_S_READY_DATA_ACK_POS         (13u)  /* [13]  Slave ACKs data while RX FIFO == FULL */
#define Serial_I2C_CTRL_S_NOT_READY_ADDR_NACK_POS    (14u)  /* [14]  Slave NACKs address if RX FIFO == FULL*/
#define Serial_I2C_CTRL_S_NOT_READY_DATA_NACK_POS    (15u)  /* [15]  Slave NACKs data if RX FIFO is  FULL  */
#define Serial_I2C_CTRL_LOOPBACK_POS                 (16u)  /* [16]  Loop-back                             */
#define Serial_I2C_CTRL_SLAVE_MODE_POS               (30u)  /* [30]  Slave mode enabled                    */
#define Serial_I2C_CTRL_MASTER_MODE_POS              (31u)  /* [31]  Master mode enabled                   */
#define Serial_I2C_CTRL_HIGH_PHASE_OVS_MASK  ((uint32) 0x0Fu)
#define Serial_I2C_CTRL_LOW_PHASE_OVS_MASK   ((uint32) 0x0Fu << \
                                                                Serial_I2C_CTRL_LOW_PHASE_OVS_POS)
#define Serial_I2C_CTRL_M_READY_DATA_ACK      ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_M_READY_DATA_ACK_POS)
#define Serial_I2C_CTRL_M_NOT_READY_DATA_NACK ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_M_NOT_READY_DATA_NACK_POS)
#define Serial_I2C_CTRL_S_GENERAL_IGNORE      ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_S_GENERAL_IGNORE_POS)
#define Serial_I2C_CTRL_S_READY_ADDR_ACK      ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_S_READY_ADDR_ACK_POS)
#define Serial_I2C_CTRL_S_READY_DATA_ACK      ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_S_READY_DATA_ACK_POS)
#define Serial_I2C_CTRL_S_NOT_READY_ADDR_NACK ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_S_NOT_READY_ADDR_NACK_POS)
#define Serial_I2C_CTRL_S_NOT_READY_DATA_NACK ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_S_NOT_READY_DATA_NACK_POS)
#define Serial_I2C_CTRL_LOOPBACK              ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_LOOPBACK_POS)
#define Serial_I2C_CTRL_SLAVE_MODE            ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_SLAVE_MODE_POS)
#define Serial_I2C_CTRL_MASTER_MODE           ((uint32) 0x01u << \
                                                                Serial_I2C_CTRL_MASTER_MODE_POS)
#define Serial_I2C_CTRL_SLAVE_MASTER_MODE_MASK    ((uint32) 0x03u << \
                                                                Serial_I2C_CTRL_SLAVE_MODE_POS)

/* Serial_I2C_STATUS_REG  */
#define Serial_I2C_STATUS_BUS_BUSY_POS    (0u)  /* [0]    Bus busy: internally clocked */
#define Serial_I2C_STATUS_S_READ_POS      (4u)  /* [4]    Slave is read by master      */
#define Serial_I2C_STATUS_M_READ_POS      (5u)  /* [5]    Master reads Slave           */
#define Serial_I2C_STATUS_EZBUF_ADDR_POS  (8u)  /* [15:8] EZAddress                    */
#define Serial_I2C_STATUS_BUS_BUSY        ((uint32) 0x01u)
#define Serial_I2C_STATUS_S_READ          ((uint32) 0x01u << Serial_I2C_STATUS_S_READ_POS)
#define Serial_I2C_STATUS_M_READ          ((uint32) 0x01u << Serial_I2C_STATUS_M_READ_POS)
#define Serial_I2C_STATUS_EZBUF_ADDR_MASK ((uint32) 0xFFu << Serial_I2C_STATUS_EZBUF_ADDR_POS)

/* Serial_I2C_MASTER_CMD_REG */
#define Serial_I2C_MASTER_CMD_M_START_POS             (0u)  /* [0] Master generate Start                */
#define Serial_I2C_MASTER_CMD_M_START_ON_IDLE_POS     (1u)  /* [1] Master generate Start if bus is free */
#define Serial_I2C_MASTER_CMD_M_ACK_POS               (2u)  /* [2] Master generate ACK                  */
#define Serial_I2C_MASTER_CMD_M_NACK_POS              (3u)  /* [3] Master generate NACK                 */
#define Serial_I2C_MASTER_CMD_M_STOP_POS              (4u)  /* [4] Master generate Stop                 */
#define Serial_I2C_MASTER_CMD_M_START         ((uint32) 0x01u)
#define Serial_I2C_MASTER_CMD_M_START_ON_IDLE ((uint32) 0x01u << \
                                                                   Serial_I2C_MASTER_CMD_M_START_ON_IDLE_POS)
#define Serial_I2C_MASTER_CMD_M_ACK           ((uint32) 0x01u << \
                                                                   Serial_I2C_MASTER_CMD_M_ACK_POS)
#define Serial_I2C_MASTER_CMD_M_NACK          ((uint32) 0x01u << \
                                                                    Serial_I2C_MASTER_CMD_M_NACK_POS)
#define Serial_I2C_MASTER_CMD_M_STOP          ((uint32) 0x01u << \
                                                                    Serial_I2C_MASTER_CMD_M_STOP_POS)

/* Serial_I2C_SLAVE_CMD_REG  */
#define Serial_I2C_SLAVE_CMD_S_ACK_POS    (0u)  /* [0] Slave generate ACK  */
#define Serial_I2C_SLAVE_CMD_S_NACK_POS   (1u)  /* [1] Slave generate NACK */
#define Serial_I2C_SLAVE_CMD_S_ACK        ((uint32) 0x01u)
#define Serial_I2C_SLAVE_CMD_S_NACK       ((uint32) 0x01u << Serial_I2C_SLAVE_CMD_S_NACK_POS)

#define Serial_I2C_SLAVE_CMD_S_ACK_POS    (0u)  /* [0] Slave generate ACK  */
#define Serial_I2C_SLAVE_CMD_S_NACK_POS   (1u)  /* [1] Slave generate NACK */
#define Serial_I2C_SLAVE_CMD_S_ACK        ((uint32) 0x01u)
#define Serial_I2C_SLAVE_CMD_S_NACK       ((uint32) 0x01u << Serial_I2C_SLAVE_CMD_S_NACK_POS)

/* Serial_I2C_CFG_REG */
#if (Serial_CY_SCBIP_V0)
#define Serial_I2C_CFG_SDA_FILT_HYS_POS           (0u)  /* [1:0]   Trim bits for the I2C SDA filter         */
#define Serial_I2C_CFG_SDA_FILT_TRIM_POS          (2u)  /* [3:2]   Trim bits for the I2C SDA filter         */
#define Serial_I2C_CFG_SCL_FILT_HYS_POS           (4u)  /* [5:4]   Trim bits for the I2C SCL filter         */
#define Serial_I2C_CFG_SCL_FILT_TRIM_POS          (6u)  /* [7:6]   Trim bits for the I2C SCL filter         */
#define Serial_I2C_CFG_SDA_FILT_OUT_HYS_POS       (8u)  /* [9:8]   Trim bits for I2C SDA filter output path */
#define Serial_I2C_CFG_SDA_FILT_OUT_TRIM_POS      (10u) /* [11:10] Trim bits for I2C SDA filter output path */
#define Serial_I2C_CFG_SDA_FILT_HS_POS            (16u) /* [16]    '0': 50 ns filter, '1': 10 ns filter     */
#define Serial_I2C_CFG_SDA_FILT_ENABLED_POS       (17u) /* [17]    I2C SDA filter enabled                   */
#define Serial_I2C_CFG_SCL_FILT_HS_POS            (24u) /* [24]    '0': 50 ns filter, '1': 10 ns filter     */
#define Serial_I2C_CFG_SCL_FILT_ENABLED_POS       (25u) /* [25]    I2C SCL filter enabled                   */
#define Serial_I2C_CFG_SDA_FILT_OUT_HS_POS        (26u) /* [26]    '0': 50 ns filter, '1': 10 ns filter     */
#define Serial_I2C_CFG_SDA_FILT_OUT_ENABLED_POS   (27u) /* [27]    I2C SDA output delay filter enabled      */
#define Serial_I2C_CFG_SDA_FILT_HYS_MASK          ((uint32) 0x03u)
#define Serial_I2C_CFG_SDA_FILT_TRIM_MASK         ((uint32) 0x03u << \
                                                                Serial_I2C_CFG_SDA_FILT_TRIM_POS)
#define Serial_I2C_CFG_SCL_FILT_HYS_MASK          ((uint32) 0x03u << \
                                                                Serial_I2C_CFG_SCL_FILT_HYS_POS)
#define Serial_I2C_CFG_SCL_FILT_TRIM_MASK         ((uint32) 0x03u << \
                                                                Serial_I2C_CFG_SCL_FILT_TRIM_POS)
#define Serial_I2C_CFG_SDA_FILT_OUT_HYS_MASK      ((uint32) 0x03u << \
                                                                Serial_I2C_CFG_SDA_FILT_OUT_HYS_POS)
#define Serial_I2C_CFG_SDA_FILT_OUT_TRIM_MASK     ((uint32) 0x03u << \
                                                                Serial_I2C_CFG_SDA_FILT_OUT_TRIM_POS)
#define Serial_I2C_CFG_SDA_FILT_HS                ((uint32) 0x01u << \
                                                                Serial_I2C_CFG_SDA_FILT_HS_POS)
#define Serial_I2C_CFG_SDA_FILT_ENABLED           ((uint32) 0x01u << \
                                                                Serial_I2C_CFG_SDA_FILT_ENABLED_POS)
#define Serial_I2C_CFG_SCL_FILT_HS                ((uint32) 0x01u << \
                                                                Serial_I2C_CFG_SCL_FILT_HS_POS)
#define Serial_I2C_CFG_SCL_FILT_ENABLED           ((uint32) 0x01u << \
                                                                Serial_I2C_CFG_SCL_FILT_ENABLED_POS)
#define Serial_I2C_CFG_SDA_FILT_OUT_HS            ((uint32) 0x01u << \
                                                                Serial_I2C_CFG_SDA_FILT_OUT_HS_POS)
#define Serial_I2C_CFG_SDA_FILT_OUT_ENABLED       ((uint32) 0x01u << \
                                                                Serial_I2C_CFG_SDA_FILT_OUT_ENABLED_POS)
#else
#define Serial_I2C_CFG_SDA_IN_FILT_TRIM_POS   (0u)  /* [1:0] Trim bits for "i2c_sda_in" 50 ns filter */
#define Serial_I2C_CFG_SDA_IN_FILT_SEL_POS    (4u)  /* [4]   "i2c_sda_in" filter delay: 0 ns and 50 ns */
#define Serial_I2C_CFG_SCL_IN_FILT_TRIM_POS   (8u)  /* [9:8] Trim bits for "i2c_scl_in" 50 ns filter */
#define Serial_I2C_CFG_SCL_IN_FILT_SEL_POS    (12u) /* [12]  "i2c_scl_in" filter delay: 0 ns and 50 ns */
#define Serial_I2C_CFG_SDA_OUT_FILT0_TRIM_POS (16u) /* [17:16] Trim bits for "i2c_sda_out" 50 ns filter 0 */
#define Serial_I2C_CFG_SDA_OUT_FILT1_TRIM_POS (18u) /* [19:18] Trim bits for "i2c_sda_out" 50 ns filter 1 */
#define Serial_I2C_CFG_SDA_OUT_FILT2_TRIM_POS (20u) /* [21:20] Trim bits for "i2c_sda_out" 50 ns filter 2 */
#define Serial_I2C_CFG_SDA_OUT_FILT_SEL_POS   (28u) /* [29:28] Cumulative "i2c_sda_out" filter delay: */

#define Serial_I2C_CFG_SDA_IN_FILT_TRIM_MASK  ((uint32) 0x03u)
#define Serial_I2C_CFG_SDA_IN_FILT_SEL        ((uint32) 0x01u << Serial_I2C_CFG_SDA_IN_FILT_SEL_POS)
#define Serial_I2C_CFG_SCL_IN_FILT_TRIM_MASK  ((uint32) 0x03u << \
                                                            Serial_I2C_CFG_SCL_IN_FILT_TRIM_POS)
#define Serial_I2C_CFG_SCL_IN_FILT_SEL        ((uint32) 0x01u << Serial_I2C_CFG_SCL_IN_FILT_SEL_POS)
#define Serial_I2C_CFG_SDA_OUT_FILT0_TRIM_MASK ((uint32) 0x03u << \
                                                            Serial_I2C_CFG_SDA_OUT_FILT0_TRIM_POS)
#define Serial_I2C_CFG_SDA_OUT_FILT1_TRIM_MASK ((uint32) 0x03u << \
                                                            Serial_I2C_CFG_SDA_OUT_FILT1_TRIM_POS)
#define Serial_I2C_CFG_SDA_OUT_FILT2_TRIM_MASK ((uint32) 0x03u << \
                                                            Serial_I2C_CFG_SDA_OUT_FILT2_TRIM_POS)
#define Serial_I2C_CFG_SDA_OUT_FILT_SEL_MASK   ((uint32) 0x03u << \
                                                            Serial_I2C_CFG_SDA_OUT_FILT_SEL_POS)
#endif /* (Serial_CY_SCBIP_V0) */


/* Serial_TX_CTRL_REG */
#define Serial_TX_CTRL_DATA_WIDTH_POS     (0u)  /* [3:0] Data frame width: (Data width - 1) */
#define Serial_TX_CTRL_MSB_FIRST_POS      (8u)  /* [8]   MSB first shifter-out             */
#define Serial_TX_CTRL_ENABLED_POS        (31u) /* [31]  Transmitter enabled               */
#define Serial_TX_CTRL_DATA_WIDTH_MASK    ((uint32) 0x0Fu)
#define Serial_TX_CTRL_MSB_FIRST          ((uint32) 0x01u << Serial_TX_CTRL_MSB_FIRST_POS)
#define Serial_TX_CTRL_LSB_FIRST          ((uint32) 0x00u)
#define Serial_TX_CTRL_ENABLED            ((uint32) 0x01u << Serial_TX_CTRL_ENABLED_POS)

/* Serial_TX_CTRL_FIFO_REG */
#define Serial_TX_FIFO_CTRL_TRIGGER_LEVEL_POS     (0u)  /* [2:0] Trigger level                              */
#define Serial_TX_FIFO_CTRL_CLEAR_POS             (16u) /* [16]  Clear TX FIFO: cleared after set           */
#define Serial_TX_FIFO_CTRL_FREEZE_POS            (17u) /* [17]  Freeze TX FIFO: HW do not inc read pointer */
#define Serial_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK    ((uint32) Serial_FF_DATA_NR_LOG2_MASK)
#define Serial_TX_FIFO_CTRL_CLEAR                 ((uint32) 0x01u << Serial_TX_FIFO_CTRL_CLEAR_POS)
#define Serial_TX_FIFO_CTRL_FREEZE                ((uint32) 0x01u << Serial_TX_FIFO_CTRL_FREEZE_POS)

/* Serial_TX_FIFO_STATUS_REG */
#define Serial_TX_FIFO_STATUS_USED_POS    (0u)  /* [3:0]   Amount of entries in TX FIFO */
#define Serial_TX_FIFO_SR_VALID_POS       (15u) /* [15]    Shifter status of TX FIFO    */
#define Serial_TX_FIFO_STATUS_RD_PTR_POS  (16u) /* [18:16] TX FIFO read pointer         */
#define Serial_TX_FIFO_STATUS_WR_PTR_POS  (24u) /* [26:24] TX FIFO write pointer        */
#define Serial_TX_FIFO_STATUS_USED_MASK   ((uint32) Serial_FF_DATA_NR_LOG2_PLUS1_MASK)
#define Serial_TX_FIFO_SR_VALID           ((uint32) 0x01u << Serial_TX_FIFO_SR_VALID_POS)
#define Serial_TX_FIFO_STATUS_RD_PTR_MASK ((uint32) Serial_FF_DATA_NR_LOG2_MASK << \
                                                                    Serial_TX_FIFO_STATUS_RD_PTR_POS)
#define Serial_TX_FIFO_STATUS_WR_PTR_MASK ((uint32) Serial_FF_DATA_NR_LOG2_MASK << \
                                                                    Serial_TX_FIFO_STATUS_WR_PTR_POS)

/* Serial_TX_FIFO_WR_REG */
#define Serial_TX_FIFO_WR_POS    (0u)  /* [15:0] Data written into TX FIFO */
#define Serial_TX_FIFO_WR_MASK   ((uint32) 0xFFu)

/* Serial_RX_CTRL_REG */
#define Serial_RX_CTRL_DATA_WIDTH_POS     (0u)  /* [3:0] Data frame width: (Data width - 1) */
#define Serial_RX_CTRL_MSB_FIRST_POS      (8u)  /* [8]   MSB first shifter-out             */
#define Serial_RX_CTRL_MEDIAN_POS         (9u)  /* [9]   Median filter                     */
#define Serial_RX_CTRL_ENABLED_POS        (31u) /* [31]  Receiver enabled                  */
#define Serial_RX_CTRL_DATA_WIDTH_MASK    ((uint32) 0x0Fu)
#define Serial_RX_CTRL_MSB_FIRST          ((uint32) 0x01u << Serial_RX_CTRL_MSB_FIRST_POS)
#define Serial_RX_CTRL_LSB_FIRST          ((uint32) 0x00u)
#define Serial_RX_CTRL_MEDIAN             ((uint32) 0x01u << Serial_RX_CTRL_MEDIAN_POS)
#define Serial_RX_CTRL_ENABLED            ((uint32) 0x01u << Serial_RX_CTRL_ENABLED_POS)


/* Serial_RX_FIFO_CTRL_REG */
#define Serial_RX_FIFO_CTRL_TRIGGER_LEVEL_POS     (0u)   /* [2:0] Trigger level                            */
#define Serial_RX_FIFO_CTRL_CLEAR_POS             (16u)  /* [16]  Clear RX FIFO: clear after set           */
#define Serial_RX_FIFO_CTRL_FREEZE_POS            (17u)  /* [17]  Freeze RX FIFO: HW writes has not effect */
#define Serial_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK    ((uint32) Serial_FF_DATA_NR_LOG2_MASK)
#define Serial_RX_FIFO_CTRL_CLEAR                 ((uint32) 0x01u << Serial_RX_FIFO_CTRL_CLEAR_POS)
#define Serial_RX_FIFO_CTRL_FREEZE                ((uint32) 0x01u << Serial_RX_FIFO_CTRL_FREEZE_POS)

/* Serial_RX_FIFO_STATUS_REG */
#define Serial_RX_FIFO_STATUS_USED_POS    (0u)   /* [3:0]   Amount of entries in RX FIFO */
#define Serial_RX_FIFO_SR_VALID_POS       (15u)  /* [15]    Shifter status of RX FIFO    */
#define Serial_RX_FIFO_STATUS_RD_PTR_POS  (16u)  /* [18:16] RX FIFO read pointer         */
#define Serial_RX_FIFO_STATUS_WR_PTR_POS  (24u)  /* [26:24] RX FIFO write pointer        */
#define Serial_RX_FIFO_STATUS_USED_MASK   ((uint32) Serial_FF_DATA_NR_LOG2_PLUS1_MASK)
#define Serial_RX_FIFO_SR_VALID           ((uint32) 0x01u << Serial_RX_FIFO_SR_VALID_POS)
#define Serial_RX_FIFO_STATUS_RD_PTR_MASK ((uint32) Serial_FF_DATA_NR_LOG2_MASK << \
                                                                    Serial_RX_FIFO_STATUS_RD_PTR_POS)
#define Serial_RX_FIFO_STATUS_WR_PTR_MASK ((uint32) Serial_FF_DATA_NR_LOG2_MASK << \
                                                                    Serial_RX_FIFO_STATUS_WR_PTR_POS)

/* Serial_RX_MATCH_REG */
#define Serial_RX_MATCH_ADDR_POS     (0u)  /* [7:0]   Slave address                        */
#define Serial_RX_MATCH_MASK_POS     (16u) /* [23:16] Slave address mask: 0 - doesn't care */
#define Serial_RX_MATCH_ADDR_MASK    ((uint32) 0xFFu)
#define Serial_RX_MATCH_MASK_MASK    ((uint32) 0xFFu << Serial_RX_MATCH_MASK_POS)

/* Serial_RX_FIFO_WR_REG */
#define Serial_RX_FIFO_RD_POS    (0u)  /* [15:0] Data read from RX FIFO */
#define Serial_RX_FIFO_RD_MASK   ((uint32) 0xFFu)

/* Serial_RX_FIFO_RD_SILENT_REG */
#define Serial_RX_FIFO_RD_SILENT_POS     (0u)  /* [15:0] Data read from RX FIFO: not remove data from FIFO */
#define Serial_RX_FIFO_RD_SILENT_MASK    ((uint32) 0xFFu)

/* Serial_RX_FIFO_RD_SILENT_REG */
#define Serial_RX_FIFO_RD_SILENT_POS     (0u)  /* [15:0] Data read from RX FIFO: not remove data from FIFO */
#define Serial_RX_FIFO_RD_SILENT_MASK    ((uint32) 0xFFu)

/* Serial_EZBUF_DATA_REG */
#define Serial_EZBUF_DATA_POS   (0u)  /* [7:0] Data from EZ Memory */
#define Serial_EZBUF_DATA_MASK  ((uint32) 0xFFu)

/*  Serial_INTR_CAUSE_REG */
#define Serial_INTR_CAUSE_MASTER_POS  (0u)  /* [0] Master interrupt active                 */
#define Serial_INTR_CAUSE_SLAVE_POS   (1u)  /* [1] Slave interrupt active                  */
#define Serial_INTR_CAUSE_TX_POS      (2u)  /* [2] Transmitter interrupt active            */
#define Serial_INTR_CAUSE_RX_POS      (3u)  /* [3] Receiver interrupt active               */
#define Serial_INTR_CAUSE_I2C_EC_POS  (4u)  /* [4] Externally clock I2C interrupt active   */
#define Serial_INTR_CAUSE_SPI_EC_POS  (5u)  /* [5] Externally clocked SPI interrupt active */
#define Serial_INTR_CAUSE_MASTER      ((uint32) 0x01u)
#define Serial_INTR_CAUSE_SLAVE       ((uint32) 0x01u << Serial_INTR_CAUSE_SLAVE_POS)
#define Serial_INTR_CAUSE_TX          ((uint32) 0x01u << Serial_INTR_CAUSE_TX_POS)
#define Serial_INTR_CAUSE_RX          ((uint32) 0x01u << Serial_INTR_CAUSE_RX_POS)
#define Serial_INTR_CAUSE_I2C_EC      ((uint32) 0x01u << Serial_INTR_CAUSE_I2C_EC_POS)
#define Serial_INTR_CAUSE_SPI_EC      ((uint32) 0x01u << Serial_INTR_CAUSE_SPI_EC_POS)

/* Serial_INTR_SPI_EC_REG, Serial_INTR_SPI_EC_MASK_REG, Serial_INTR_SPI_EC_MASKED_REG */
#define Serial_INTR_SPI_EC_WAKE_UP_POS          (0u)  /* [0] Address match: triggers wakeup of chip */
#define Serial_INTR_SPI_EC_EZBUF_STOP_POS       (1u)  /* [1] Externally clocked Stop detected       */
#define Serial_INTR_SPI_EC_EZBUF_WRITE_STOP_POS (2u)  /* [2] Externally clocked Write Stop detected */
#define Serial_INTR_SPI_EC_WAKE_UP              ((uint32) 0x01u)
#define Serial_INTR_SPI_EC_EZBUF_STOP           ((uint32) 0x01u << \
                                                                    Serial_INTR_SPI_EC_EZBUF_STOP_POS)
#define Serial_INTR_SPI_EC_EZBUF_WRITE_STOP     ((uint32) 0x01u << \
                                                                    Serial_INTR_SPI_EC_EZBUF_WRITE_STOP_POS)

/* Serial_INTR_I2C_EC, Serial_INTR_I2C_EC_MASK, Serial_INTR_I2C_EC_MASKED */
#define Serial_INTR_I2C_EC_WAKE_UP_POS          (0u)  /* [0] Address match: triggers wakeup of chip */
#define Serial_INTR_I2C_EC_EZBUF_STOP_POS       (1u)  /* [1] Externally clocked Stop detected       */
#define Serial_INTR_I2C_EC_EZBUF_WRITE_STOP_POS (2u)  /* [2] Externally clocked Write Stop detected */
#define Serial_INTR_I2C_EC_WAKE_UP              ((uint32) 0x01u)
#define Serial_INTR_I2C_EC_EZBUF_STOP           ((uint32) 0x01u << \
                                                                    Serial_INTR_I2C_EC_EZBUF_STOP_POS)
#define Serial_INTR_I2C_EC_EZBUF_WRITE_STOP     ((uint32) 0x01u << \
                                                                    Serial_INTR_I2C_EC_EZBUF_WRITE_STOP_POS)

/* Serial_INTR_MASTER, Serial_INTR_MASTER_SET,
   Serial_INTR_MASTER_MASK, Serial_INTR_MASTER_MASKED */
#define Serial_INTR_MASTER_I2C_ARB_LOST_POS   (0u)  /* [0] Master lost arbitration                          */
#define Serial_INTR_MASTER_I2C_NACK_POS       (1u)  /* [1] Master receives NACK: address or write to slave  */
#define Serial_INTR_MASTER_I2C_ACK_POS        (2u)  /* [2] Master receives NACK: address or write to slave  */
#define Serial_INTR_MASTER_I2C_STOP_POS       (4u)  /* [4] Master detects the Stop: only self generated Stop*/
#define Serial_INTR_MASTER_I2C_BUS_ERROR_POS  (8u)  /* [8] Master detects bus error: misplaced Start or Stop*/
#define Serial_INTR_MASTER_SPI_DONE_POS       (9u)  /* [9] Master complete transfer: Only for SPI           */
#define Serial_INTR_MASTER_I2C_ARB_LOST       ((uint32) 0x01u)
#define Serial_INTR_MASTER_I2C_NACK           ((uint32) 0x01u << Serial_INTR_MASTER_I2C_NACK_POS)
#define Serial_INTR_MASTER_I2C_ACK            ((uint32) 0x01u << Serial_INTR_MASTER_I2C_ACK_POS)
#define Serial_INTR_MASTER_I2C_STOP           ((uint32) 0x01u << Serial_INTR_MASTER_I2C_STOP_POS)
#define Serial_INTR_MASTER_I2C_BUS_ERROR      ((uint32) 0x01u << \
                                                                    Serial_INTR_MASTER_I2C_BUS_ERROR_POS)
#define Serial_INTR_MASTER_SPI_DONE           ((uint32) 0x01u << Serial_INTR_MASTER_SPI_DONE_POS)

/*
* Serial_INTR_SLAVE, Serial_INTR_SLAVE_SET,
* Serial_INTR_SLAVE_MASK, Serial_INTR_SLAVE_MASKED
*/
#define Serial_INTR_SLAVE_I2C_ARB_LOST_POS         (0u)  /* [0]  Slave lost arbitration                   */
#define Serial_INTR_SLAVE_I2C_NACK_POS             (1u)  /* [1]  Slave receives NACK: master reads data   */
#define Serial_INTR_SLAVE_I2C_ACK_POS              (2u)  /* [2]  Slave receives ACK: master reads data    */
#define Serial_INTR_SLAVE_I2C_WRITE_STOP_POS       (3u)  /* [3]  Slave detects end of write transaction   */
#define Serial_INTR_SLAVE_I2C_STOP_POS             (4u)  /* [4]  Slave detects end of transaction intended */
#define Serial_INTR_SLAVE_I2C_START_POS            (5u)  /* [5]  Slave detects Start                      */
#define Serial_INTR_SLAVE_I2C_ADDR_MATCH_POS       (6u)  /* [6]  Slave address matches                    */
#define Serial_INTR_SLAVE_I2C_GENERAL_POS          (7u)  /* [7]  General call received                    */
#define Serial_INTR_SLAVE_I2C_BUS_ERROR_POS        (8u)  /* [8]  Slave detects bus error                  */
#define Serial_INTR_SLAVE_SPI_EZBUF_WRITE_STOP_POS (9u)  /* [9]  Slave write complete: Only for SPI       */
#define Serial_INTR_SLAVE_SPI_EZBUF_STOP_POS       (10u) /* [10] Slave end of transaction: Only for SPI   */
#define Serial_INTR_SLAVE_SPI_BUS_ERROR_POS        (11u) /* [11] Slave detects bus error: Only for SPI    */
#define Serial_INTR_SLAVE_I2C_ARB_LOST             ((uint32) 0x01u)
#define Serial_INTR_SLAVE_I2C_NACK                 ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_NACK_POS)
#define Serial_INTR_SLAVE_I2C_ACK                  ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_ACK_POS)
#define Serial_INTR_SLAVE_I2C_WRITE_STOP           ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_WRITE_STOP_POS)
#define Serial_INTR_SLAVE_I2C_STOP                 ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_STOP_POS)
#define Serial_INTR_SLAVE_I2C_START                ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_START_POS)
#define Serial_INTR_SLAVE_I2C_ADDR_MATCH           ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_ADDR_MATCH_POS)
#define Serial_INTR_SLAVE_I2C_GENERAL              ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_GENERAL_POS)
#define Serial_INTR_SLAVE_I2C_BUS_ERROR            ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_I2C_BUS_ERROR_POS)
#define Serial_INTR_SLAVE_SPI_EZBUF_WRITE_STOP     ((uint32) 0x01u << \
                                                                   Serial_INTR_SLAVE_SPI_EZBUF_WRITE_STOP_POS)
#define Serial_INTR_SLAVE_SPI_EZBUF_STOP           ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_SPI_EZBUF_STOP_POS)
#define Serial_INTR_SLAVE_SPI_BUS_ERROR           ((uint32) 0x01u << \
                                                                    Serial_INTR_SLAVE_SPI_BUS_ERROR_POS)

/*
* Serial_INTR_TX, Serial_INTR_TX_SET,
* Serial_INTR_TX_MASK, Serial_INTR_TX_MASKED
*/
#define Serial_INTR_TX_TRIGGER_POS        (0u)  /* [0]  Trigger on TX FIFO entires                       */
#define Serial_INTR_TX_NOT_FULL_POS       (1u)  /* [1]  TX FIFO is not full                              */
#define Serial_INTR_TX_EMPTY_POS          (4u)  /* [4]  TX FIFO is empty                                 */
#define Serial_INTR_TX_OVERFLOW_POS       (5u)  /* [5]  Attempt to write to a full TX FIFO               */
#define Serial_INTR_TX_UNDERFLOW_POS      (6u)  /* [6]  Attempt to read from an empty TX FIFO            */
#define Serial_INTR_TX_BLOCKED_POS        (7u)  /* [7]  No access to the EZ memory                       */
#define Serial_INTR_TX_UART_NACK_POS      (8u)  /* [8]  UART transmitter received a NACK: SmartCard mode */
#define Serial_INTR_TX_UART_DONE_POS      (9u)  /* [9]  UART transmitter done even                       */
#define Serial_INTR_TX_UART_ARB_LOST_POS  (10u) /* [10] UART lost arbitration: LIN or SmartCard          */
#define Serial_INTR_TX_TRIGGER            ((uint32) 0x01u)
#define Serial_INTR_TX_FIFO_LEVEL         (Serial_INTR_TX_TRIGGER)
#define Serial_INTR_TX_NOT_FULL           ((uint32) 0x01u << Serial_INTR_TX_NOT_FULL_POS)
#define Serial_INTR_TX_EMPTY              ((uint32) 0x01u << Serial_INTR_TX_EMPTY_POS)
#define Serial_INTR_TX_OVERFLOW           ((uint32) 0x01u << Serial_INTR_TX_OVERFLOW_POS)
#define Serial_INTR_TX_UNDERFLOW          ((uint32) 0x01u << Serial_INTR_TX_UNDERFLOW_POS)
#define Serial_INTR_TX_BLOCKED            ((uint32) 0x01u << Serial_INTR_TX_BLOCKED_POS)
#define Serial_INTR_TX_UART_NACK          ((uint32) 0x01u << Serial_INTR_TX_UART_NACK_POS)
#define Serial_INTR_TX_UART_DONE          ((uint32) 0x01u << Serial_INTR_TX_UART_DONE_POS)
#define Serial_INTR_TX_UART_ARB_LOST      ((uint32) 0x01u << Serial_INTR_TX_UART_ARB_LOST_POS)

/*
* Serial_INTR_RX, Serial_INTR_RX_SET,
* Serial_INTR_RX_MASK, Serial_INTR_RX_MASKED
*/
#define Serial_INTR_RX_TRIGGER_POS        (0u)   /* [0]  Trigger on RX FIFO entires            */
#define Serial_INTR_RX_NOT_EMPTY_POS      (2u)   /* [2]  RX FIFO is not empty                  */
#define Serial_INTR_RX_FULL_POS           (3u)   /* [3]  RX FIFO is full                       */
#define Serial_INTR_RX_OVERFLOW_POS       (5u)   /* [5]  Attempt to write to a full RX FIFO    */
#define Serial_INTR_RX_UNDERFLOW_POS      (6u)   /* [6]  Attempt to read from an empty RX FIFO */
#define Serial_INTR_RX_BLOCKED_POS        (7u)   /* [7]  No access to the EZ memory            */
#define Serial_INTR_RX_FRAME_ERROR_POS    (8u)   /* [8]  Frame error in received data frame    */
#define Serial_INTR_RX_PARITY_ERROR_POS   (9u)   /* [9]  Parity error in received data frame   */
#define Serial_INTR_RX_BAUD_DETECT_POS    (10u)  /* [10] LIN baud rate detection is completed   */
#define Serial_INTR_RX_BREAK_DETECT_POS   (11u)  /* [11] Break detection is successful         */
#define Serial_INTR_RX_TRIGGER            ((uint32) 0x01u)
#define Serial_INTR_RX_FIFO_LEVEL         (Serial_INTR_RX_TRIGGER)
#define Serial_INTR_RX_NOT_EMPTY          ((uint32) 0x01u << Serial_INTR_RX_NOT_EMPTY_POS)
#define Serial_INTR_RX_FULL               ((uint32) 0x01u << Serial_INTR_RX_FULL_POS)
#define Serial_INTR_RX_OVERFLOW           ((uint32) 0x01u << Serial_INTR_RX_OVERFLOW_POS)
#define Serial_INTR_RX_UNDERFLOW          ((uint32) 0x01u << Serial_INTR_RX_UNDERFLOW_POS)
#define Serial_INTR_RX_BLOCKED            ((uint32) 0x01u << Serial_INTR_RX_BLOCKED_POS)
#define Serial_INTR_RX_FRAME_ERROR        ((uint32) 0x01u << Serial_INTR_RX_FRAME_ERROR_POS)
#define Serial_INTR_RX_PARITY_ERROR       ((uint32) 0x01u << Serial_INTR_RX_PARITY_ERROR_POS)
#define Serial_INTR_RX_BAUD_DETECT        ((uint32) 0x01u << Serial_INTR_RX_BAUD_DETECT_POS)
#define Serial_INTR_RX_BREAK_DETECT       ((uint32) 0x01u << Serial_INTR_RX_BREAK_DETECT_POS)

/* Define all interrupt sources */
#define Serial_INTR_I2C_EC_ALL    (Serial_INTR_I2C_EC_WAKE_UP    | \
                                             Serial_INTR_I2C_EC_EZBUF_STOP | \
                                             Serial_INTR_I2C_EC_EZBUF_WRITE_STOP)

#define Serial_INTR_SPI_EC_ALL    (Serial_INTR_SPI_EC_WAKE_UP    | \
                                             Serial_INTR_SPI_EC_EZBUF_STOP | \
                                             Serial_INTR_SPI_EC_EZBUF_WRITE_STOP)

#define Serial_INTR_MASTER_ALL    (Serial_INTR_MASTER_I2C_ARB_LOST  | \
                                             Serial_INTR_MASTER_I2C_NACK      | \
                                             Serial_INTR_MASTER_I2C_ACK       | \
                                             Serial_INTR_MASTER_I2C_STOP      | \
                                             Serial_INTR_MASTER_I2C_BUS_ERROR | \
                                             Serial_INTR_MASTER_SPI_DONE)

#define Serial_INTR_SLAVE_ALL     (Serial_INTR_SLAVE_I2C_ARB_LOST      | \
                                             Serial_INTR_SLAVE_I2C_NACK          | \
                                             Serial_INTR_SLAVE_I2C_ACK           | \
                                             Serial_INTR_SLAVE_I2C_WRITE_STOP    | \
                                             Serial_INTR_SLAVE_I2C_STOP          | \
                                             Serial_INTR_SLAVE_I2C_START         | \
                                             Serial_INTR_SLAVE_I2C_ADDR_MATCH    | \
                                             Serial_INTR_SLAVE_I2C_GENERAL       | \
                                             Serial_INTR_SLAVE_I2C_BUS_ERROR     | \
                                             Serial_INTR_SLAVE_SPI_EZBUF_WRITE_STOP | \
                                             Serial_INTR_SLAVE_SPI_EZBUF_STOP       | \
                                             Serial_INTR_SLAVE_SPI_BUS_ERROR)

#define Serial_INTR_TX_ALL        (Serial_INTR_TX_TRIGGER   | \
                                             Serial_INTR_TX_NOT_FULL  | \
                                             Serial_INTR_TX_EMPTY     | \
                                             Serial_INTR_TX_OVERFLOW  | \
                                             Serial_INTR_TX_UNDERFLOW | \
                                             Serial_INTR_TX_BLOCKED   | \
                                             Serial_INTR_TX_UART_NACK | \
                                             Serial_INTR_TX_UART_DONE | \
                                             Serial_INTR_TX_UART_ARB_LOST)

#define Serial_INTR_RX_ALL        (Serial_INTR_RX_TRIGGER      | \
                                             Serial_INTR_RX_NOT_EMPTY    | \
                                             Serial_INTR_RX_FULL         | \
                                             Serial_INTR_RX_OVERFLOW     | \
                                             Serial_INTR_RX_UNDERFLOW    | \
                                             Serial_INTR_RX_BLOCKED      | \
                                             Serial_INTR_RX_FRAME_ERROR  | \
                                             Serial_INTR_RX_PARITY_ERROR | \
                                             Serial_INTR_RX_BAUD_DETECT  | \
                                             Serial_INTR_RX_BREAK_DETECT)

/* I2C and EZI2C slave address defines */
#define Serial_I2C_SLAVE_ADDR_POS    (0x01u)    /* 7-bit address shift */
#define Serial_I2C_SLAVE_ADDR_MASK   (0xFEu)    /* 8-bit address mask */

/* OVS constants for IrDA Low Power operation */
#define Serial_CTRL_OVS_IRDA_LP_OVS16     (0x00u)
#define Serial_CTRL_OVS_IRDA_LP_OVS32     (0x01u)
#define Serial_CTRL_OVS_IRDA_LP_OVS48     (0x02u)
#define Serial_CTRL_OVS_IRDA_LP_OVS96     (0x03u)
#define Serial_CTRL_OVS_IRDA_LP_OVS192    (0x04u)
#define Serial_CTRL_OVS_IRDA_LP_OVS768    (0x05u)
#define Serial_CTRL_OVS_IRDA_LP_OVS1536   (0x06u)

/* OVS constant for IrDA */
#define Serial_CTRL_OVS_IRDA_OVS16        (Serial_UART_IRDA_LP_OVS16)


/***************************************
*    Common Macro Definitions
***************************************/

/* Re-enables the SCB IP. A clear enable bit has a different effect
* on the scb IP depending on the version:
*  CY_SCBIP_V0: resets state, status, TX and RX FIFOs.
*  CY_SCBIP_V1 or later: resets state, status, TX and RX FIFOs and interrupt sources.
* Clear I2C command registers are because they are not impacted by re-enable.
*/
#define Serial_SCB_SW_RESET   Serial_I2CFwBlockReset()

/* TX FIFO macro */
#define Serial_CLEAR_TX_FIFO \
                            do{        \
                                Serial_TX_FIFO_CTRL_REG |= ((uint32)  Serial_TX_FIFO_CTRL_CLEAR); \
                                Serial_TX_FIFO_CTRL_REG &= ((uint32) ~Serial_TX_FIFO_CTRL_CLEAR); \
                            }while(0)

#define Serial_GET_TX_FIFO_ENTRIES    (Serial_TX_FIFO_STATUS_REG & \
                                                 Serial_TX_FIFO_STATUS_USED_MASK)

#define Serial_GET_TX_FIFO_SR_VALID   ((0u != (Serial_TX_FIFO_STATUS_REG & \
                                                         Serial_TX_FIFO_SR_VALID)) ? (1u) : (0u))

/* RX FIFO macro */
#define Serial_CLEAR_RX_FIFO \
                            do{        \
                                Serial_RX_FIFO_CTRL_REG |= ((uint32)  Serial_RX_FIFO_CTRL_CLEAR); \
                                Serial_RX_FIFO_CTRL_REG &= ((uint32) ~Serial_RX_FIFO_CTRL_CLEAR); \
                            }while(0)

#define Serial_GET_RX_FIFO_ENTRIES    (Serial_RX_FIFO_STATUS_REG & \
                                                    Serial_RX_FIFO_STATUS_USED_MASK)

#define Serial_GET_RX_FIFO_SR_VALID   ((0u != (Serial_RX_FIFO_STATUS_REG & \
                                                         Serial_RX_FIFO_SR_VALID)) ? (1u) : (0u))

/* Write interrupt source: set sourceMask bits in Serial_INTR_X_MASK_REG */
#define Serial_WRITE_INTR_I2C_EC_MASK(sourceMask) \
                                                do{         \
                                                    Serial_INTR_I2C_EC_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#if (!Serial_CY_SCBIP_V1)
    #define Serial_WRITE_INTR_SPI_EC_MASK(sourceMask) \
                                                do{         \
                                                    Serial_INTR_SPI_EC_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)
#endif /* (!Serial_CY_SCBIP_V1) */

#define Serial_WRITE_INTR_MASTER_MASK(sourceMask) \
                                                do{         \
                                                    Serial_INTR_MASTER_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_WRITE_INTR_SLAVE_MASK(sourceMask)  \
                                                do{         \
                                                    Serial_INTR_SLAVE_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_WRITE_INTR_TX_MASK(sourceMask)     \
                                                do{         \
                                                    Serial_INTR_TX_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_WRITE_INTR_RX_MASK(sourceMask)     \
                                                do{         \
                                                    Serial_INTR_RX_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

/* Enable interrupt source: set sourceMask bits in Serial_INTR_X_MASK_REG */
#define Serial_ENABLE_INTR_I2C_EC(sourceMask) \
                                                do{     \
                                                    Serial_INTR_I2C_EC_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)
#if (!Serial_CY_SCBIP_V1)
    #define Serial_ENABLE_INTR_SPI_EC(sourceMask) \
                                                do{     \
                                                    Serial_INTR_SPI_EC_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)
#endif /* (!Serial_CY_SCBIP_V1) */

#define Serial_ENABLE_INTR_MASTER(sourceMask) \
                                                do{     \
                                                    Serial_INTR_MASTER_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

#define Serial_ENABLE_INTR_SLAVE(sourceMask)  \
                                                do{     \
                                                    Serial_INTR_SLAVE_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

#define Serial_ENABLE_INTR_TX(sourceMask)     \
                                                do{     \
                                                    Serial_INTR_TX_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

#define Serial_ENABLE_INTR_RX(sourceMask)     \
                                                do{     \
                                                    Serial_INTR_RX_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

/* Disable interrupt source: clear sourceMask bits in Serial_INTR_X_MASK_REG */
#define Serial_DISABLE_INTR_I2C_EC(sourceMask) \
                                do{                      \
                                    Serial_INTR_I2C_EC_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

#if (!Serial_CY_SCBIP_V1)
    #define Serial_DISABLE_INTR_SPI_EC(sourceMask) \
                                do{                      \
                                    Serial_INTR_SPI_EC_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                 }while(0)
#endif /* (!Serial_CY_SCBIP_V1) */

#define Serial_DISABLE_INTR_MASTER(sourceMask) \
                                do{                      \
                                Serial_INTR_MASTER_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

#define Serial_DISABLE_INTR_SLAVE(sourceMask) \
                                do{                     \
                                    Serial_INTR_SLAVE_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

#define Serial_DISABLE_INTR_TX(sourceMask)    \
                                do{                     \
                                    Serial_INTR_TX_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                 }while(0)

#define Serial_DISABLE_INTR_RX(sourceMask)    \
                                do{                     \
                                    Serial_INTR_RX_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

/* Set interrupt sources: write sourceMask bits in Serial_INTR_X_SET_REG */
#define Serial_SET_INTR_MASTER(sourceMask)    \
                                                do{     \
                                                    Serial_INTR_MASTER_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_SET_INTR_SLAVE(sourceMask) \
                                                do{ \
                                                    Serial_INTR_SLAVE_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_SET_INTR_TX(sourceMask)    \
                                                do{ \
                                                    Serial_INTR_TX_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_SET_INTR_RX(sourceMask)    \
                                                do{ \
                                                    Serial_INTR_RX_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

/* Clear interrupt sources: write sourceMask bits in Serial_INTR_X_REG */
#define Serial_CLEAR_INTR_I2C_EC(sourceMask)  \
                                                do{     \
                                                    Serial_INTR_I2C_EC_REG = (uint32) (sourceMask); \
                                                }while(0)

#if (!Serial_CY_SCBIP_V1)
    #define Serial_CLEAR_INTR_SPI_EC(sourceMask)  \
                                                do{     \
                                                    Serial_INTR_SPI_EC_REG = (uint32) (sourceMask); \
                                                }while(0)
#endif /* (!Serial_CY_SCBIP_V1) */

#define Serial_CLEAR_INTR_MASTER(sourceMask)  \
                                                do{     \
                                                    Serial_INTR_MASTER_REG = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_CLEAR_INTR_SLAVE(sourceMask)   \
                                                do{     \
                                                    Serial_INTR_SLAVE_REG  = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_CLEAR_INTR_TX(sourceMask)      \
                                                do{     \
                                                    Serial_INTR_TX_REG     = (uint32) (sourceMask); \
                                                }while(0)

#define Serial_CLEAR_INTR_RX(sourceMask)      \
                                                do{     \
                                                    Serial_INTR_RX_REG     = (uint32) (sourceMask); \
                                                }while(0)

/* Return true if sourceMask is set in Serial_INTR_CAUSE_REG */
#define Serial_CHECK_CAUSE_INTR(sourceMask)    (0u != (Serial_INTR_CAUSE_REG & (sourceMask)))

/* Return true if sourceMask is set in INTR_X_MASKED_REG */
#define Serial_CHECK_INTR_I2C_EC(sourceMask)  (0u != (Serial_INTR_I2C_EC_REG & (sourceMask)))
#if (!Serial_CY_SCBIP_V1)
    #define Serial_CHECK_INTR_SPI_EC(sourceMask)  (0u != (Serial_INTR_SPI_EC_REG & (sourceMask)))
#endif /* (!Serial_CY_SCBIP_V1) */
#define Serial_CHECK_INTR_MASTER(sourceMask)  (0u != (Serial_INTR_MASTER_REG & (sourceMask)))
#define Serial_CHECK_INTR_SLAVE(sourceMask)   (0u != (Serial_INTR_SLAVE_REG  & (sourceMask)))
#define Serial_CHECK_INTR_TX(sourceMask)      (0u != (Serial_INTR_TX_REG     & (sourceMask)))
#define Serial_CHECK_INTR_RX(sourceMask)      (0u != (Serial_INTR_RX_REG     & (sourceMask)))

/* Return true if sourceMask is set in Serial_INTR_X_MASKED_REG */
#define Serial_CHECK_INTR_I2C_EC_MASKED(sourceMask)   (0u != (Serial_INTR_I2C_EC_MASKED_REG & \
                                                                       (sourceMask)))
#if (!Serial_CY_SCBIP_V1)
    #define Serial_CHECK_INTR_SPI_EC_MASKED(sourceMask)   (0u != (Serial_INTR_SPI_EC_MASKED_REG & \
                                                                       (sourceMask)))
#endif /* (!Serial_CY_SCBIP_V1) */
#define Serial_CHECK_INTR_MASTER_MASKED(sourceMask)   (0u != (Serial_INTR_MASTER_MASKED_REG & \
                                                                       (sourceMask)))
#define Serial_CHECK_INTR_SLAVE_MASKED(sourceMask)    (0u != (Serial_INTR_SLAVE_MASKED_REG  & \
                                                                       (sourceMask)))
#define Serial_CHECK_INTR_TX_MASKED(sourceMask)       (0u != (Serial_INTR_TX_MASKED_REG     & \
                                                                       (sourceMask)))
#define Serial_CHECK_INTR_RX_MASKED(sourceMask)       (0u != (Serial_INTR_RX_MASKED_REG     & \
                                                                       (sourceMask)))

/* Return true if sourceMask is set in Serial_CTRL_REG: generally is used to check enable bit */
#define Serial_GET_CTRL_ENABLED    (0u != (Serial_CTRL_REG & Serial_CTRL_ENABLED))

#define Serial_CHECK_SLAVE_AUTO_ADDR_NACK     (0u != (Serial_I2C_CTRL_REG & \
                                                                Serial_I2C_CTRL_S_NOT_READY_DATA_NACK))


/***************************************
*      I2C Macro Definitions
***************************************/

/* Enable auto ACK/NACK */
#define Serial_ENABLE_SLAVE_AUTO_ADDR_NACK \
                            do{                      \
                                Serial_I2C_CTRL_REG |= Serial_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define Serial_ENABLE_SLAVE_AUTO_DATA_ACK \
                            do{                     \
                                Serial_I2C_CTRL_REG |= Serial_I2C_CTRL_S_READY_DATA_ACK; \
                            }while(0)

#define Serial_ENABLE_SLAVE_AUTO_DATA_NACK \
                            do{                      \
                                Serial_I2C_CTRL_REG |= Serial_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define Serial_ENABLE_MASTER_AUTO_DATA_ACK \
                            do{                      \
                                Serial_I2C_CTRL_REG |= Serial_I2C_CTRL_M_READY_DATA_ACK; \
                            }while(0)

#define Serial_ENABLE_MASTER_AUTO_DATA_NACK \
                            do{                       \
                                Serial_I2C_CTRL_REG |= Serial_I2C_CTRL_M_NOT_READY_DATA_NACK; \
                            }while(0)

/* Disable auto ACK/NACK */
#define Serial_DISABLE_SLAVE_AUTO_ADDR_NACK \
                            do{                       \
                                Serial_I2C_CTRL_REG &= ~Serial_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define Serial_DISABLE_SLAVE_AUTO_DATA_ACK \
                            do{                      \
                                Serial_I2C_CTRL_REG &= ~Serial_I2C_CTRL_S_READY_DATA_ACK; \
                            }while(0)

#define Serial_DISABLE_SLAVE_AUTO_DATA_NACK \
                            do{                       \
                                Serial_I2C_CTRL_REG &= ~Serial_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define Serial_DISABLE_MASTER_AUTO_DATA_ACK \
                            do{                       \
                                Serial_I2C_CTRL_REG &= ~Serial_I2C_CTRL_M_READY_DATA_ACK; \
                            }while(0)

#define Serial_DISABLE_MASTER_AUTO_DATA_NACK \
                            do{                        \
                                Serial_I2C_CTRL_REG &= ~Serial_I2C_CTRL_M_NOT_READY_DATA_NACK; \
                            }while(0)

/* Enable Slave autoACK/NACK Data */
#define Serial_ENABLE_SLAVE_AUTO_DATA \
                            do{                 \
                                Serial_I2C_CTRL_REG |= (Serial_I2C_CTRL_S_READY_DATA_ACK |      \
                                                                  Serial_I2C_CTRL_S_NOT_READY_DATA_NACK); \
                            }while(0)

/* Disable Slave autoACK/NACK Data */
#define Serial_DISABLE_SLAVE_AUTO_DATA \
                            do{                  \
                                Serial_I2C_CTRL_REG &= ((uint32) \
                                                                  ~(Serial_I2C_CTRL_S_READY_DATA_ACK |       \
                                                                    Serial_I2C_CTRL_S_NOT_READY_DATA_NACK)); \
                            }while(0)

/* Disable Master autoACK/NACK Data */
#define Serial_DISABLE_MASTER_AUTO_DATA \
                            do{                   \
                                Serial_I2C_CTRL_REG &= ((uint32) \
                                                                  ~(Serial_I2C_CTRL_M_READY_DATA_ACK |       \
                                                                    Serial_I2C_CTRL_M_NOT_READY_DATA_NACK)); \
                            }while(0)
/* Disables auto data ACK/NACK bits */
#define Serial_DISABLE_AUTO_DATA \
                do{                        \
                    Serial_I2C_CTRL_REG &= ((uint32) ~(Serial_I2C_CTRL_M_READY_DATA_ACK      |  \
                                                                 Serial_I2C_CTRL_M_NOT_READY_DATA_NACK |  \
                                                                 Serial_I2C_CTRL_S_READY_DATA_ACK      |  \
                                                                 Serial_I2C_CTRL_S_NOT_READY_DATA_NACK)); \
                }while(0)

/* Master commands */
#define Serial_I2C_MASTER_GENERATE_START \
                            do{                    \
                                Serial_I2C_MASTER_CMD_REG = Serial_I2C_MASTER_CMD_M_START_ON_IDLE; \
                            }while(0)

#define Serial_I2C_MASTER_CLEAR_START \
                            do{                 \
                                Serial_I2C_MASTER_CMD_REG =  ((uint32) 0u); \
                            }while(0)

#define Serial_I2C_MASTER_GENERATE_RESTART Serial_I2CReStartGeneration()

#define Serial_I2C_MASTER_GENERATE_STOP \
                            do{                   \
                                Serial_I2C_MASTER_CMD_REG =                                            \
                                    (Serial_I2C_MASTER_CMD_M_STOP |                                    \
                                        (Serial_CHECK_I2C_STATUS(Serial_I2C_STATUS_M_READ) ? \
                                            (Serial_I2C_MASTER_CMD_M_NACK) : (0u)));                   \
                            }while(0)

#define Serial_I2C_MASTER_GENERATE_ACK \
                            do{                  \
                                Serial_I2C_MASTER_CMD_REG = Serial_I2C_MASTER_CMD_M_ACK; \
                            }while(0)

#define Serial_I2C_MASTER_GENERATE_NACK \
                            do{                   \
                                Serial_I2C_MASTER_CMD_REG = Serial_I2C_MASTER_CMD_M_NACK; \
                            }while(0)

/* Slave commands */
#define Serial_I2C_SLAVE_GENERATE_ACK \
                            do{                 \
                                Serial_I2C_SLAVE_CMD_REG = Serial_I2C_SLAVE_CMD_S_ACK; \
                            }while(0)

#if (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    /* Slave NACK generation for EC_AM logic on address phase. Ticket ID #183902 */
    void Serial_I2CSlaveNackGeneration(void);
    #define Serial_I2C_SLAVE_GENERATE_NACK Serial_I2CSlaveNackGeneration()

#else
    #define Serial_I2C_SLAVE_GENERATE_NACK \
                            do{                      \
                                Serial_I2C_SLAVE_CMD_REG = Serial_I2C_SLAVE_CMD_S_NACK; \
                            }while(0)
#endif /* (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#define Serial_I2C_SLAVE_CLEAR_NACK \
                            do{               \
                                Serial_I2C_SLAVE_CMD_REG = 0u; \
                            }while(0)

/* Return 8-bit address. The input address should be 7-bits */
#define Serial_GET_I2C_8BIT_ADDRESS(addr) (((uint32) ((uint32) (addr) << \
                                                                    Serial_I2C_SLAVE_ADDR_POS)) & \
                                                                        Serial_I2C_SLAVE_ADDR_MASK)

#define Serial_GET_I2C_7BIT_ADDRESS(addr) ((uint32) (addr) >> Serial_I2C_SLAVE_ADDR_POS)

/* Adjust SDA filter Trim settings */
#define Serial_DEFAULT_I2C_CFG_SDA_FILT_TRIM  (0x02u)
#define Serial_EC_AM_I2C_CFG_SDA_FILT_TRIM    (0x03u)

#if (Serial_CY_SCBIP_V0)
    #define Serial_SET_I2C_CFG_SDA_FILT_TRIM(sdaTrim) \
        do{                                                 \
            Serial_I2C_CFG_REG =                  \
                            ((Serial_I2C_CFG_REG & (uint32) ~Serial_I2C_CFG_SDA_FILT_TRIM_MASK) | \
                             ((uint32) ((uint32) (sdaTrim) <<Serial_I2C_CFG_SDA_FILT_TRIM_POS)));           \
        }while(0)
#endif /* (Serial_CY_SCBIP_V0) */

/* Enable/Disable analog and digital filter */
#define Serial_DIGITAL_FILTER_DISABLE    (0u)
#define Serial_DIGITAL_FILTER_ENABLE     (1u)
#define Serial_I2C_DATA_RATE_FS_MODE_MAX (400u)
#if (Serial_CY_SCBIP_V0)
    /* Serial_I2C_CFG_SDA_FILT_OUT_ENABLED is disabled by default */
    #define Serial_I2C_CFG_FILT_MASK  (Serial_I2C_CFG_SDA_FILT_ENABLED | \
                                                 Serial_I2C_CFG_SCL_FILT_ENABLED)
#else
    /* Serial_I2C_CFG_SDA_OUT_FILT_SEL_MASK is disabled by default */
    #define Serial_I2C_CFG_FILT_MASK  (Serial_I2C_CFG_SDA_IN_FILT_SEL | \
                                                 Serial_I2C_CFG_SCL_IN_FILT_SEL)
#endif /* (Serial_CY_SCBIP_V0) */

#define Serial_I2C_CFG_ANALOG_FITER_DISABLE \
        do{                                           \
            Serial_I2C_CFG_REG &= (uint32) ~Serial_I2C_CFG_FILT_MASK; \
        }while(0)

#define Serial_I2C_CFG_ANALOG_FITER_ENABLE \
        do{                                          \
            Serial_I2C_CFG_REG |= (uint32)  Serial_I2C_CFG_FILT_MASK; \
        }while(0)

/* Return slave select number from SPI_CTRL register */
#define Serial_GET_SPI_CTRL_SS(activeSelect) (((uint32) ((uint32) (activeSelect) << \
                                                                    Serial_SPI_CTRL_SLAVE_SELECT_POS)) & \
                                                                        Serial_SPI_CTRL_SLAVE_SELECT_MASK)

/* Return true if bit is set in Serial_I2C_STATUS_REG */
#define Serial_CHECK_I2C_STATUS(sourceMask)   (0u != (Serial_I2C_STATUS_REG & (sourceMask)))

/* Return true if bit is set in Serial_SPI_STATUS_REG */
#define Serial_CHECK_SPI_STATUS(sourceMask)   (0u != (Serial_SPI_STATUS_REG & (sourceMask)))

/* Return FIFO size depends on Serial_CTRL_BYTE_MODE bit */
#define Serial_GET_FIFO_SIZE(condition) ((0u != (condition)) ? \
                                                    (2u * Serial_FIFO_SIZE) : (Serial_FIFO_SIZE))


/***************************************
*       Get Macros Definitions
***************************************/

/* Serial_CTRL */
#define Serial_GET_CTRL_OVS(oversample)       (((uint32) (oversample) - 1u) & Serial_CTRL_OVS_MASK)

#define Serial_GET_CTRL_EC_OP_MODE(opMode)        ((0u != (opMode)) ? \
                                                                (Serial_CTRL_EC_OP_MODE)  : (0u))

#define Serial_GET_CTRL_EC_AM_MODE(amMode)        ((0u != (amMode)) ? \
                                                                (Serial_CTRL_EC_AM_MODE)  : (0u))

#define Serial_GET_CTRL_BLOCK(block)              ((0u != (block))  ? \
                                                                (Serial_CTRL_BLOCK)       : (0u))

#define Serial_GET_CTRL_ADDR_ACCEPT(acceptAddr)   ((0u != (acceptAddr)) ? \
                                                                (Serial_CTRL_ADDR_ACCEPT) : (0u))

#if (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_GET_CTRL_BYTE_MODE(mode)   (0u)
#else
    #define Serial_GET_CTRL_BYTE_MODE(mode)   ((0u != (mode)) ? \
                                                            (Serial_CTRL_BYTE_MODE) : (0u))
#endif /* (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

/* Serial_I2C_CTRL */
#define Serial_GET_I2C_CTRL_HIGH_PHASE_OVS(oversampleHigh) (((uint32) (oversampleHigh) - 1u) & \
                                                                        Serial_I2C_CTRL_HIGH_PHASE_OVS_MASK)

#define Serial_GET_I2C_CTRL_LOW_PHASE_OVS(oversampleLow)  ((((uint32) (oversampleLow) - 1u) << \
                                                                    Serial_I2C_CTRL_LOW_PHASE_OVS_POS) &  \
                                                                    Serial_I2C_CTRL_LOW_PHASE_OVS_MASK)

#define Serial_GET_I2C_CTRL_S_NOT_READY_ADDR_NACK(wakeNack) ((0u != (wakeNack)) ? \
                                                            (Serial_I2C_CTRL_S_NOT_READY_ADDR_NACK) : (0u))

#define Serial_GET_I2C_CTRL_S_GENERAL_IGNORE(genCall) ((0u != (genCall)) ? \
                                                                    (Serial_I2C_CTRL_S_GENERAL_IGNORE) : (0u))

#define Serial_GET_I2C_CTRL_SL_MSTR_MODE(mode)    ((uint32)(mode) << Serial_I2C_CTRL_SLAVE_MODE_POS)

/* Serial_SPI_CTRL */
#define Serial_GET_SPI_CTRL_CONTINUOUS(separate)  ((0u != (separate)) ? \
                                                                (Serial_SPI_CTRL_CONTINUOUS) : (0u))

#define Serial_GET_SPI_CTRL_SELECT_PRECEDE(mode)  ((0u != (mode)) ? \
                                                                      (Serial_SPI_CTRL_SELECT_PRECEDE) : (0u))

#define Serial_GET_SPI_CTRL_SCLK_MODE(mode)       (((uint32) (mode) << \
                                                                        Serial_SPI_CTRL_CPHA_POS) & \
                                                                        Serial_SPI_CTRL_SCLK_MODE_MASK)

#define Serial_GET_SPI_CTRL_LATE_MISO_SAMPLE(lateMiso) ((0u != (lateMiso)) ? \
                                                                    (Serial_SPI_CTRL_LATE_MISO_SAMPLE) : (0u))

#if (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_GET_SPI_CTRL_SCLK_CONTINUOUS(sclkType) (0u)
    #define Serial_GET_SPI_CTRL_SSEL_POLARITY(polarity)   (0u)
#else
    #define Serial_GET_SPI_CTRL_SCLK_CONTINUOUS(sclkType) ((0u != (sclkType)) ? \
                                                                    (Serial_SPI_CTRL_SCLK_CONTINUOUS) : (0u))

    #define Serial_GET_SPI_CTRL_SSEL_POLARITY(polarity)   (((uint32) (polarity) << \
                                                                     Serial_SPI_CTRL_SSEL0_POLARITY_POS) & \
                                                                     Serial_SPI_CTRL_SSEL_POLARITY_MASK)
#endif /* ((Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

#define Serial_GET_SPI_CTRL_SUB_MODE(mode)        (((uint32) (mode) << Serial_SPI_CTRL_MODE_POS) & \
                                                                                 Serial_SPI_CTRL_MODE_MASK)

#define Serial_GET_SPI_CTRL_SLAVE_SELECT(select)  (((uint32) (select) << \
                                                                      Serial_SPI_CTRL_SLAVE_SELECT_POS) & \
                                                                      Serial_SPI_CTRL_SLAVE_SELECT_MASK)

#define Serial_GET_SPI_CTRL_MASTER_MODE(mode)     ((0u != (mode)) ? \
                                                                (Serial_SPI_CTRL_MASTER) : (0u))

/* Serial_UART_CTRL */
#define Serial_GET_UART_CTRL_MODE(mode)           (((uint32) (mode) << \
                                                                            Serial_UART_CTRL_MODE_POS) & \
                                                                            Serial_UART_CTRL_MODE_MASK)

/* Serial_UART_RX_CTRL */
#define Serial_GET_UART_RX_CTRL_MODE(stopBits)    (((uint32) (stopBits) - 1u) & \
                                                                        Serial_UART_RX_CTRL_STOP_BITS_MASK)

#define Serial_GET_UART_RX_CTRL_PARITY(parity)    ((0u != (parity)) ? \
                                                                    (Serial_UART_RX_CTRL_PARITY) : (0u))

#define Serial_GET_UART_RX_CTRL_POLARITY(polarity)    ((0u != (polarity)) ? \
                                                                    (Serial_UART_RX_CTRL_POLARITY) : (0u))

#define Serial_GET_UART_RX_CTRL_DROP_ON_PARITY_ERR(dropErr) ((0u != (dropErr)) ? \
                                                        (Serial_UART_RX_CTRL_DROP_ON_PARITY_ERR) : (0u))

#define Serial_GET_UART_RX_CTRL_DROP_ON_FRAME_ERR(dropErr) ((0u != (dropErr)) ? \
                                                        (Serial_UART_RX_CTRL_DROP_ON_FRAME_ERR) : (0u))

#define Serial_GET_UART_RX_CTRL_MP_MODE(mpMode)   ((0u != (mpMode)) ? \
                                                        (Serial_UART_RX_CTRL_MP_MODE) : (0u))

#define Serial_GET_UART_RX_CTRL_BREAK_WIDTH(width)    (((uint32) ((uint32) (width) - 1u) << \
                                                                    Serial_UART_RX_CTRL_BREAK_WIDTH_POS) & \
                                                                    Serial_UART_RX_CTRL_BREAK_WIDTH_MASK)

/* Serial_UART_TX_CTRL */
#define Serial_GET_UART_TX_CTRL_MODE(stopBits)    (((uint32) (stopBits) - 1u) & \
                                                                Serial_UART_RX_CTRL_STOP_BITS_MASK)

#define Serial_GET_UART_TX_CTRL_PARITY(parity)    ((0u != (parity)) ? \
                                                               (Serial_UART_TX_CTRL_PARITY) : (0u))

#define Serial_GET_UART_TX_CTRL_RETRY_NACK(nack)  ((0u != (nack)) ? \
                                                               (Serial_UART_TX_CTRL_RETRY_ON_NACK) : (0u))

/* Serial_UART_FLOW_CTRL */
#if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    #define Serial_GET_UART_FLOW_CTRL_TRIGGER_LEVEL(level)   ( (uint32) (level) & \
                                                                 Serial_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK)

    #define Serial_GET_UART_FLOW_CTRL_RTS_POLARITY(polarity) ((0u != (polarity)) ? \
                                                                (Serial_UART_FLOW_CTRL_RTS_POLARITY) : (0u))

    #define Serial_GET_UART_FLOW_CTRL_CTS_POLARITY(polarity) ((0u != (polarity)) ? \
                                                                (Serial_UART_FLOW_CTRL_CTS_POLARITY) : (0u))

    #define Serial_GET_UART_FLOW_CTRL_CTS_ENABLE(ctsEn)      ((0u != (ctsEn)) ? \
                                                                (Serial_UART_FLOW_CTRL_CTS_ENABLE) : (0u))
#endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

/* Serial_RX_CTRL */
#define Serial_GET_RX_CTRL_DATA_WIDTH(dataWidth)  (((uint32) (dataWidth) - 1u) & \
                                                                Serial_RX_CTRL_DATA_WIDTH_MASK)

#define Serial_GET_RX_CTRL_BIT_ORDER(bitOrder)    ((0u != (bitOrder)) ? \
                                                                (Serial_RX_CTRL_MSB_FIRST) : (0u))

#define Serial_GET_RX_CTRL_MEDIAN(filterEn)       ((0u != (filterEn)) ? \
                                                                (Serial_RX_CTRL_MEDIAN) : (0u))

/* Serial_RX_MATCH */
#define Serial_GET_RX_MATCH_ADDR(addr)    ((uint32) (addr) & Serial_RX_MATCH_ADDR_MASK)
#define Serial_GET_RX_MATCH_MASK(mask)    (((uint32) (mask) << \
                                                            Serial_RX_MATCH_MASK_POS) & \
                                                            Serial_RX_MATCH_MASK_MASK)

/* Serial_RX_FIFO_CTRL */
#define Serial_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(level)  ((uint32) (level) & \
                                                                    Serial_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK)

/* Serial_TX_CTRL */
#define Serial_GET_TX_CTRL_DATA_WIDTH(dataWidth)  (((uint32) (dataWidth) - 1u) & \
                                                                Serial_TX_CTRL_DATA_WIDTH_MASK)

#define Serial_GET_TX_CTRL_BIT_ORDER(bitOrder)    ((0u != (bitOrder)) ? \
                                                                (Serial_TX_CTRL_MSB_FIRST) : (0u))

/* Serial_TX_FIFO_CTRL */
#define Serial_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(level)  ((uint32) (level) & \
                                                                    Serial_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK)

/* Serial_INTR_SLAVE_I2C_GENERAL */
#define Serial_GET_INTR_SLAVE_I2C_GENERAL(genCall)  ((0u != (genCall)) ? \
                                                                (Serial_INTR_SLAVE_I2C_GENERAL) : (0u))

/* Return true if master mode is enabled Serial_SPI_CTRL_REG */
#define Serial_CHECK_SPI_MASTER   (0u != (Serial_SPI_CTRL_REG & Serial_SPI_CTRL_MASTER))

/* Return inactive state of SPI SCLK line depends on CPOL */
#define Serial_GET_SPI_SCLK_INACTIVE \
            ((0u == (Serial_SPI_CTRL_REG & Serial_SPI_CTRL_CPOL)) ? (0u) : (1u))

/* Get output pin inactive state */
#if (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
#define Serial_GET_SPI_SS0_INACTIVE       (1u)
#define Serial_GET_SPI_SS1_INACTIVE       (1u)
#define Serial_GET_SPI_SS2_INACTIVE       (1u)
#define Serial_GET_SPI_SS3_INACTIVE       (1u)
#define Serial_GET_UART_RTS_INACTIVE      (1u)

#else
#define Serial_GET_SPI_SS0_INACTIVE  \
        ((0u != (Serial_SPI_CTRL_REG & Serial_SPI_CTRL_SSEL0_POLARITY)) ? (0u) : (1u))

#define Serial_GET_SPI_SS1_INACTIVE  \
        ((0u != (Serial_SPI_CTRL_REG & Serial_SPI_CTRL_SSEL1_POLARITY)) ? (0u) : (1u))

#define Serial_GET_SPI_SS2_INACTIVE  \
        ((0u != (Serial_SPI_CTRL_REG & Serial_SPI_CTRL_SSEL2_POLARITY)) ? (0u) : (1u))

#define Serial_GET_SPI_SS3_INACTIVE  \
        ((0u != (Serial_SPI_CTRL_REG & Serial_SPI_CTRL_SSEL3_POLARITY)) ? (0u) : (1u))

#define Serial_GET_UART_RTS_INACTIVE \
        ((0u == (Serial_UART_FLOW_CTRL_REG & Serial_UART_FLOW_CTRL_RTS_POLARITY)) ? (0u) : (1u))

#endif /*(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */

/* Clear register constants for configuration and interrupt mask */
#define Serial_CLEAR_REG          ((uint32) (0u))
#define Serial_NO_INTR_SOURCES    ((uint32) (0u))
#define Serial_DUMMY_PARAM        ((uint32) (0u))
#define Serial_SUBMODE_SPI_SLAVE  ((uint32) (0u))

/* Return in case of I2C read error */
#define Serial_I2C_INVALID_BYTE   ((uint32) 0xFFFFFFFFu)
#define Serial_CHECK_VALID_BYTE   ((uint32) 0x80000000u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

#define Serial_CHECK_INTR_EC_I2C(sourceMask)  Serial_CHECK_INTR_I2C_EC(sourceMask)
#if (!Serial_CY_SCBIP_V1)
    #define Serial_CHECK_INTR_EC_SPI(sourceMask)  Serial_CHECK_INTR_SPI_EC(sourceMask)
#endif /* (!Serial_CY_SCBIP_V1) */

#define Serial_CY_SCBIP_V1_I2C_ONLY   (Serial_CY_SCBIP_V1)
#define Serial_EZBUFFER_SIZE          (Serial_EZ_DATA_NR)

#define Serial_EZBUF_DATA00_REG   Serial_EZBUF_DATA0_REG
#define Serial_EZBUF_DATA00_PTR   Serial_EZBUF_DATA0_PTR

#endif /* (CY_SCB_Serial_H) */


/* [] END OF FILE */

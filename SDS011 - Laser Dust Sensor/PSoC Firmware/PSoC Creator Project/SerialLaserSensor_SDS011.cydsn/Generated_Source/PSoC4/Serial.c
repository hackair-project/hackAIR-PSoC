/***************************************************************************//**
* \file Serial.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Serial_PVT.h"

#if (Serial_SCB_MODE_I2C_INC)
    #include "Serial_I2C_PVT.h"
#endif /* (Serial_SCB_MODE_I2C_INC) */

#if (Serial_SCB_MODE_EZI2C_INC)
    #include "Serial_EZI2C_PVT.h"
#endif /* (Serial_SCB_MODE_EZI2C_INC) */

#if (Serial_SCB_MODE_SPI_INC || Serial_SCB_MODE_UART_INC)
    #include "Serial_SPI_UART_PVT.h"
#endif /* (Serial_SCB_MODE_SPI_INC || Serial_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 Serial_scbMode = Serial_SCB_MODE_UNCONFIG;
    uint8 Serial_scbEnableWake;
    uint8 Serial_scbEnableIntr;

    /* I2C configuration variables */
    uint8 Serial_mode;
    uint8 Serial_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * Serial_rxBuffer;
    uint8  Serial_rxDataBits;
    uint32 Serial_rxBufferSize;

    volatile uint8 * Serial_txBuffer;
    uint8  Serial_txDataBits;
    uint32 Serial_txBufferSize;

    /* EZI2C configuration variables */
    uint8 Serial_numberOfAddr;
    uint8 Serial_subAddrSize;
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
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
uint8 Serial_initVar = 0u;


#if (! (Serial_SCB_MODE_I2C_CONST_CFG || \
        Serial_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * Serial_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent Serial_Enable() call.
    */
    uint16 Serial_IntrTxMask = 0u;
#endif /* (! (Serial_SCB_MODE_I2C_CONST_CFG || \
              Serial_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (Serial_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_Serial_CUSTOM_INTR_HANDLER)
    void (*Serial_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_Serial_CUSTOM_INTR_HANDLER) */
#endif /* (Serial_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void Serial_ScbEnableIntr(void);
static void Serial_ScbModeStop(void);
static void Serial_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: Serial_Init
****************************************************************************//**
*
*  Initializes the Serial component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  Serial_I2CInit, Serial_SpiInit, 
*  Serial_UartInit or Serial_EzI2CInit.
*
*******************************************************************************/
void Serial_Init(void)
{
#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    if (Serial_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        Serial_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (Serial_SCB_MODE_I2C_CONST_CFG)
    Serial_I2CInit();

#elif (Serial_SCB_MODE_SPI_CONST_CFG)
    Serial_SpiInit();

#elif (Serial_SCB_MODE_UART_CONST_CFG)
    Serial_UartInit();

#elif (Serial_SCB_MODE_EZI2C_CONST_CFG)
    Serial_EzI2CInit();

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Serial_Enable
****************************************************************************//**
*
*  Enables Serial component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  Serial_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The Serial configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured Serial”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void Serial_Enable(void)
{
#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!Serial_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        Serial_CTRL_REG |= Serial_CTRL_ENABLED;

        Serial_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        Serial_ScbModePostEnable();
    }
#else
    Serial_CTRL_REG |= Serial_CTRL_ENABLED;

    Serial_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    Serial_ScbModePostEnable();
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Serial_Start
****************************************************************************//**
*
*  Invokes Serial_Init() and Serial_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  Serial_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void Serial_Start(void)
{
    if (0u == Serial_initVar)
    {
        Serial_Init();
        Serial_initVar = 1u; /* Component was initialized */
    }

    Serial_Enable();
}


/*******************************************************************************
* Function Name: Serial_Stop
****************************************************************************//**
*
*  Disables the Serial component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function Serial_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void Serial_Stop(void)
{
#if (Serial_SCB_IRQ_INTERNAL)
    Serial_DisableInt();
#endif /* (Serial_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    Serial_ScbModeStop();

    /* Disable SCB IP */
    Serial_CTRL_REG &= (uint32) ~Serial_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    Serial_SetTxInterruptMode(Serial_NO_INTR_SOURCES);

#if (Serial_SCB_IRQ_INTERNAL)
    Serial_ClearPendingInt();
#endif /* (Serial_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: Serial_SetRxFifoLevel
****************************************************************************//**
*
*  Sets level in the RX FIFO to generate a RX level interrupt.
*  When the RX FIFO has more entries than the RX FIFO level an RX level
*  interrupt request is generated.
*
*  \param level: Level in the RX FIFO to generate RX level interrupt.
*   The range of valid level values is between 0 and RX FIFO depth - 1.
*
*******************************************************************************/
void Serial_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = Serial_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~Serial_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (Serial_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    Serial_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: Serial_SetTxFifoLevel
****************************************************************************//**
*
*  Sets level in the TX FIFO to generate a TX level interrupt.
*  When the TX FIFO has less entries than the TX FIFO level an TX level
*  interrupt request is generated.
*
*  \param level: Level in the TX FIFO to generate TX level interrupt.
*   The range of valid level values is between 0 and TX FIFO depth - 1.
*
*******************************************************************************/
void Serial_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = Serial_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~Serial_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (Serial_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    Serial_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (Serial_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: Serial_SetCustomInterruptHandler
    ****************************************************************************//**
    *
    *  Registers a function to be called by the internal interrupt handler.
    *  First the function that is registered is called, then the internal interrupt
    *  handler performs any operation such as software buffer management functions
    *  before the interrupt returns.  It is the user's responsibility not to break
    *  the software buffer operations. Only one custom handler is supported, which
    *  is the function provided by the most recent call.
    *  At the initialization time no custom handler is registered.
    *
    *  \param func: Pointer to the function to register.
    *        The value NULL indicates to remove the current custom interrupt
    *        handler.
    *
    *******************************************************************************/
    void Serial_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_Serial_CUSTOM_INTR_HANDLER)
        Serial_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_Serial_CUSTOM_INTR_HANDLER) */
    }
#endif /* (Serial_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: Serial_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void Serial_ScbEnableIntr(void)
{
#if (Serial_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != Serial_scbEnableIntr)
        {
            Serial_EnableInt();
        }

    #else
        Serial_EnableInt();

    #endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (Serial_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: Serial_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void Serial_ScbModePostEnable(void)
{
#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!Serial_CY_SCBIP_V1)
    if (Serial_SCB_MODE_SPI_RUNTM_CFG)
    {
        Serial_SpiPostEnable();
    }
    else if (Serial_SCB_MODE_UART_RUNTM_CFG)
    {
        Serial_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!Serial_CY_SCBIP_V1) */

#elif (Serial_SCB_MODE_SPI_CONST_CFG)
    Serial_SpiPostEnable();

#elif (Serial_SCB_MODE_UART_CONST_CFG)
    Serial_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Serial_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void Serial_ScbModeStop(void)
{
#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    if (Serial_SCB_MODE_I2C_RUNTM_CFG)
    {
        Serial_I2CStop();
    }
    else if (Serial_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        Serial_EzI2CStop();
    }
#if (!Serial_CY_SCBIP_V1)
    else if (Serial_SCB_MODE_SPI_RUNTM_CFG)
    {
        Serial_SpiStop();
    }
    else if (Serial_SCB_MODE_UART_RUNTM_CFG)
    {
        Serial_UartStop();
    }
#endif /* (!Serial_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (Serial_SCB_MODE_I2C_CONST_CFG)
    Serial_I2CStop();

#elif (Serial_SCB_MODE_EZI2C_CONST_CFG)
    Serial_EzI2CStop();

#elif (Serial_SCB_MODE_SPI_CONST_CFG)
    Serial_SpiStop();

#elif (Serial_SCB_MODE_UART_CONST_CFG)
    Serial_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (Serial_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: Serial_SetPins
    ****************************************************************************//**
    *
    *  Sets the pins settings accordingly to the selected operation mode.
    *  Only available in the Unconfigured operation mode. The mode specific
    *  initialization function calls it.
    *  Pins configuration is set by PSoC Creator when a specific mode of operation
    *  is selected in design time.
    *
    *  \param mode:      Mode of SCB operation.
    *  \param subMode:   Sub-mode of SCB operation. It is only required for SPI and UART
    *             modes.
    *  \param uartEnableMask: enables TX or RX direction and RTS and CTS signals.
    *
    *******************************************************************************/
    void Serial_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[Serial_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!Serial_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!Serial_CY_SCBIP_V1) */
        
        uint32 hsiomSel[Serial_SCB_PINS_NUMBER] = 
        {
            Serial_RX_SDA_MOSI_HSIOM_SEL_GPIO,
            Serial_TX_SCL_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (Serial_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (Serial_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < Serial_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = Serial_PIN_DM_ALG_HIZ;
        }

        if ((Serial_SCB_MODE_I2C   == mode) ||
            (Serial_SCB_MODE_EZI2C == mode))
        {
        #if (Serial_RX_SDA_MOSI_PIN)
            hsiomSel[Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_RX_SDA_MOSI_HSIOM_SEL_I2C;
            pinsDm  [Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_PIN_DM_OD_LO;
        #elif (Serial_RX_WAKE_SDA_MOSI_PIN)
            hsiomSel[Serial_RX_WAKE_SDA_MOSI_PIN_INDEX] = Serial_RX_WAKE_SDA_MOSI_HSIOM_SEL_I2C;
            pinsDm  [Serial_RX_WAKE_SDA_MOSI_PIN_INDEX] = Serial_PIN_DM_OD_LO;
        #else
        #endif /* (Serial_RX_SDA_MOSI_PIN) */
        
        #if (Serial_TX_SCL_MISO_PIN)
            hsiomSel[Serial_TX_SCL_MISO_PIN_INDEX] = Serial_TX_SCL_MISO_HSIOM_SEL_I2C;
            pinsDm  [Serial_TX_SCL_MISO_PIN_INDEX] = Serial_PIN_DM_OD_LO;
        #endif /* (Serial_TX_SCL_MISO_PIN) */
        }
    #if (!Serial_CY_SCBIP_V1)
        else if (Serial_SCB_MODE_SPI == mode)
        {
        #if (Serial_RX_SDA_MOSI_PIN)
            hsiomSel[Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_RX_SDA_MOSI_HSIOM_SEL_SPI;
        #elif (Serial_RX_WAKE_SDA_MOSI_PIN)
            hsiomSel[Serial_RX_WAKE_SDA_MOSI_PIN_INDEX] = Serial_RX_WAKE_SDA_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (Serial_RX_SDA_MOSI_PIN) */
        
        #if (Serial_TX_SCL_MISO_PIN)
            hsiomSel[Serial_TX_SCL_MISO_PIN_INDEX] = Serial_TX_SCL_MISO_HSIOM_SEL_SPI;
        #endif /* (Serial_TX_SCL_MISO_PIN) */
        
        #if (Serial_CTS_SCLK_PIN)
            hsiomSel[Serial_CTS_SCLK_PIN_INDEX] = Serial_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (Serial_CTS_SCLK_PIN) */

            if (Serial_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;
                pinsDm[Serial_TX_SCL_MISO_PIN_INDEX] = Serial_PIN_DM_STRONG;
                pinsDm[Serial_CTS_SCLK_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;

            #if (Serial_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[Serial_RTS_SS0_PIN_INDEX] = Serial_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [Serial_RTS_SS0_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;
            #endif /* (Serial_RTS_SS0_PIN) */

            #if (Serial_TX_SCL_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= Serial_TX_SCL_MISO_PIN_MASK;
            #endif /* (Serial_TX_SCL_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_PIN_DM_STRONG;
                pinsDm[Serial_TX_SCL_MISO_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;
                pinsDm[Serial_CTS_SCLK_PIN_INDEX] = Serial_PIN_DM_STRONG;

            #if (Serial_RTS_SS0_PIN)
                hsiomSel [Serial_RTS_SS0_PIN_INDEX] = Serial_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [Serial_RTS_SS0_PIN_INDEX] = Serial_PIN_DM_STRONG;
                pinsInBuf |= Serial_RTS_SS0_PIN_MASK;
            #endif /* (Serial_RTS_SS0_PIN) */

            #if (Serial_SS1_PIN)
                hsiomSel [Serial_SS1_PIN_INDEX] = Serial_SS1_HSIOM_SEL_SPI;
                pinsDm   [Serial_SS1_PIN_INDEX] = Serial_PIN_DM_STRONG;
                pinsInBuf |= Serial_SS1_PIN_MASK;
            #endif /* (Serial_SS1_PIN) */

            #if (Serial_SS2_PIN)
                hsiomSel [Serial_SS2_PIN_INDEX] = Serial_SS2_HSIOM_SEL_SPI;
                pinsDm   [Serial_SS2_PIN_INDEX] = Serial_PIN_DM_STRONG;
                pinsInBuf |= Serial_SS2_PIN_MASK;
            #endif /* (Serial_SS2_PIN) */

            #if (Serial_SS3_PIN)
                hsiomSel [Serial_SS3_PIN_INDEX] = Serial_SS3_HSIOM_SEL_SPI;
                pinsDm   [Serial_SS3_PIN_INDEX] = Serial_PIN_DM_STRONG;
                pinsInBuf |= Serial_SS3_PIN_MASK;
            #endif /* (Serial_SS3_PIN) */

                /* Disable input buffers */
            #if (Serial_RX_SDA_MOSI_PIN)
                pinsInBuf |= Serial_RX_SDA_MOSI_PIN_MASK;
            #elif (Serial_RX_WAKE_SDA_MOSI_PIN)
                pinsInBuf |= Serial_RX_WAKE_SDA_MOSI_PIN_MASK;
            #else
            #endif /* (Serial_RX_SDA_MOSI_PIN) */

            #if (Serial_CTS_SCLK_PIN)
                pinsInBuf |= Serial_CTS_SCLK_PIN_MASK;
            #endif /* (Serial_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (Serial_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (Serial_TX_SCL_MISO_PIN)
                hsiomSel[Serial_TX_SCL_MISO_PIN_INDEX] = Serial_TX_SCL_MISO_HSIOM_SEL_UART;
                pinsDm  [Serial_TX_SCL_MISO_PIN_INDEX] = Serial_PIN_DM_OD_LO;
            #endif /* (Serial_TX_SCL_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (Serial_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (Serial_RX_SDA_MOSI_PIN)
                    hsiomSel[Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_RX_SDA_MOSI_HSIOM_SEL_UART;
                    pinsDm  [Serial_RX_SDA_MOSI_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;
                #elif (Serial_RX_WAKE_SDA_MOSI_PIN)
                    hsiomSel[Serial_RX_WAKE_SDA_MOSI_PIN_INDEX] = Serial_RX_WAKE_SDA_MOSI_HSIOM_SEL_UART;
                    pinsDm  [Serial_RX_WAKE_SDA_MOSI_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;
                #else
                #endif /* (Serial_RX_SDA_MOSI_PIN) */
                }

                if (0u != (Serial_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (Serial_TX_SCL_MISO_PIN)
                    hsiomSel[Serial_TX_SCL_MISO_PIN_INDEX] = Serial_TX_SCL_MISO_HSIOM_SEL_UART;
                    pinsDm  [Serial_TX_SCL_MISO_PIN_INDEX] = Serial_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= Serial_TX_SCL_MISO_PIN_MASK;
                #endif /* (Serial_TX_SCL_MISO_PIN) */
                }

            #if !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
                if (Serial_UART_MODE_STD == subMode)
                {
                    if (0u != (Serial_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (Serial_CTS_SCLK_PIN)
                        hsiomSel[Serial_CTS_SCLK_PIN_INDEX] = Serial_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [Serial_CTS_SCLK_PIN_INDEX] = Serial_PIN_DM_DIG_HIZ;
                    #endif /* (Serial_CTS_SCLK_PIN) */
                    }

                    if (0u != (Serial_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (Serial_RTS_SS0_PIN)
                        hsiomSel[Serial_RTS_SS0_PIN_INDEX] = Serial_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [Serial_RTS_SS0_PIN_INDEX] = Serial_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= Serial_RTS_SS0_PIN_MASK;
                    #endif /* (Serial_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */
            }
        }
    #endif /* (!Serial_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (Serial_RX_SDA_MOSI_PIN)
        Serial_SET_HSIOM_SEL(Serial_RX_SDA_MOSI_HSIOM_REG,
                                       Serial_RX_SDA_MOSI_HSIOM_MASK,
                                       Serial_RX_SDA_MOSI_HSIOM_POS,
                                        hsiomSel[Serial_RX_SDA_MOSI_PIN_INDEX]);

        Serial_uart_rx_i2c_sda_spi_mosi_SetDriveMode((uint8) pinsDm[Serial_RX_SDA_MOSI_PIN_INDEX]);

        #if (!Serial_CY_SCBIP_V1)
            Serial_SET_INP_DIS(Serial_uart_rx_i2c_sda_spi_mosi_INP_DIS,
                                         Serial_uart_rx_i2c_sda_spi_mosi_MASK,
                                         (0u != (pinsInBuf & Serial_RX_SDA_MOSI_PIN_MASK)));
        #endif /* (!Serial_CY_SCBIP_V1) */
    
    #elif (Serial_RX_WAKE_SDA_MOSI_PIN)
        Serial_SET_HSIOM_SEL(Serial_RX_WAKE_SDA_MOSI_HSIOM_REG,
                                       Serial_RX_WAKE_SDA_MOSI_HSIOM_MASK,
                                       Serial_RX_WAKE_SDA_MOSI_HSIOM_POS,
                                       hsiomSel[Serial_RX_WAKE_SDA_MOSI_PIN_INDEX]);

        Serial_uart_rx_wake_i2c_sda_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[Serial_RX_WAKE_SDA_MOSI_PIN_INDEX]);

        Serial_SET_INP_DIS(Serial_uart_rx_wake_i2c_sda_spi_mosi_INP_DIS,
                                     Serial_uart_rx_wake_i2c_sda_spi_mosi_MASK,
                                     (0u != (pinsInBuf & Serial_RX_WAKE_SDA_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        Serial_SET_INCFG_TYPE(Serial_RX_WAKE_SDA_MOSI_INTCFG_REG,
                                        Serial_RX_WAKE_SDA_MOSI_INTCFG_TYPE_MASK,
                                        Serial_RX_WAKE_SDA_MOSI_INTCFG_TYPE_POS,
                                        Serial_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (Serial_RX_WAKE_SDA_MOSI_PIN) */

    #if (Serial_TX_SCL_MISO_PIN)
        Serial_SET_HSIOM_SEL(Serial_TX_SCL_MISO_HSIOM_REG,
                                       Serial_TX_SCL_MISO_HSIOM_MASK,
                                       Serial_TX_SCL_MISO_HSIOM_POS,
                                        hsiomSel[Serial_TX_SCL_MISO_PIN_INDEX]);

        Serial_uart_tx_i2c_scl_spi_miso_SetDriveMode((uint8) pinsDm[Serial_TX_SCL_MISO_PIN_INDEX]);

    #if (!Serial_CY_SCBIP_V1)
        Serial_SET_INP_DIS(Serial_uart_tx_i2c_scl_spi_miso_INP_DIS,
                                     Serial_uart_tx_i2c_scl_spi_miso_MASK,
                                    (0u != (pinsInBuf & Serial_TX_SCL_MISO_PIN_MASK)));
    #endif /* (!Serial_CY_SCBIP_V1) */
    #endif /* (Serial_RX_SDA_MOSI_PIN) */

    #if (Serial_CTS_SCLK_PIN)
        Serial_SET_HSIOM_SEL(Serial_CTS_SCLK_HSIOM_REG,
                                       Serial_CTS_SCLK_HSIOM_MASK,
                                       Serial_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[Serial_CTS_SCLK_PIN_INDEX]);

        Serial_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[Serial_CTS_SCLK_PIN_INDEX]);

        Serial_SET_INP_DIS(Serial_uart_cts_spi_sclk_INP_DIS,
                                     Serial_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & Serial_CTS_SCLK_PIN_MASK)));
    #endif /* (Serial_CTS_SCLK_PIN) */

    #if (Serial_RTS_SS0_PIN)
        Serial_SET_HSIOM_SEL(Serial_RTS_SS0_HSIOM_REG,
                                       Serial_RTS_SS0_HSIOM_MASK,
                                       Serial_RTS_SS0_HSIOM_POS,
                                       hsiomSel[Serial_RTS_SS0_PIN_INDEX]);

        Serial_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[Serial_RTS_SS0_PIN_INDEX]);

        Serial_SET_INP_DIS(Serial_uart_rts_spi_ss0_INP_DIS,
                                     Serial_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & Serial_RTS_SS0_PIN_MASK)));
    #endif /* (Serial_RTS_SS0_PIN) */

    #if (Serial_SS1_PIN)
        Serial_SET_HSIOM_SEL(Serial_SS1_HSIOM_REG,
                                       Serial_SS1_HSIOM_MASK,
                                       Serial_SS1_HSIOM_POS,
                                       hsiomSel[Serial_SS1_PIN_INDEX]);

        Serial_spi_ss1_SetDriveMode((uint8) pinsDm[Serial_SS1_PIN_INDEX]);

        Serial_SET_INP_DIS(Serial_spi_ss1_INP_DIS,
                                     Serial_spi_ss1_MASK,
                                     (0u != (pinsInBuf & Serial_SS1_PIN_MASK)));
    #endif /* (Serial_SS1_PIN) */

    #if (Serial_SS2_PIN)
        Serial_SET_HSIOM_SEL(Serial_SS2_HSIOM_REG,
                                       Serial_SS2_HSIOM_MASK,
                                       Serial_SS2_HSIOM_POS,
                                       hsiomSel[Serial_SS2_PIN_INDEX]);

        Serial_spi_ss2_SetDriveMode((uint8) pinsDm[Serial_SS2_PIN_INDEX]);

        Serial_SET_INP_DIS(Serial_spi_ss2_INP_DIS,
                                     Serial_spi_ss2_MASK,
                                     (0u != (pinsInBuf & Serial_SS2_PIN_MASK)));
    #endif /* (Serial_SS2_PIN) */

    #if (Serial_SS3_PIN)
        Serial_SET_HSIOM_SEL(Serial_SS3_HSIOM_REG,
                                       Serial_SS3_HSIOM_MASK,
                                       Serial_SS3_HSIOM_POS,
                                       hsiomSel[Serial_SS3_PIN_INDEX]);

        Serial_spi_ss3_SetDriveMode((uint8) pinsDm[Serial_SS3_PIN_INDEX]);

        Serial_SET_INP_DIS(Serial_spi_ss3_INP_DIS,
                                     Serial_spi_ss3_MASK,
                                     (0u != (pinsInBuf & Serial_SS3_PIN_MASK)));
    #endif /* (Serial_SS3_PIN) */
    }

#endif /* (Serial_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: Serial_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void Serial_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (Serial_CTRL_REG & Serial_CTRL_EC_AM_MODE)) &&
            (0u == (Serial_I2C_CTRL_REG & Serial_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            Serial_CTRL_REG &= ~Serial_CTRL_EC_AM_MODE;
            Serial_CTRL_REG |=  Serial_CTRL_EC_AM_MODE;
        }

        Serial_I2C_SLAVE_CMD_REG = Serial_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (Serial_CY_SCBIP_V0 || Serial_CY_SCBIP_V1) */


/* [] END OF FILE */

/* ========================================
 * Air Quality Beacon, part of the hackAIR project.
 * Freely available under CC BY 4.0
 *
 *
 * Author: George Doxastakis gdoxastakis.ee@gmail.com
 * Technology Transfer & Smart Solutions
 * TEI of Athens
 * http://research.ee.teiath.gr/ttss/
 *
 * http://www.hackair.eu/
*/
#include <project.h>
#include <math.h>
/* Function prototypes */  
void StackEventHandler(uint32 event, void* eventParam);
int16 readParticles();

/* ADV payload dta structure */  
extern CYBLE_GAPP_DISC_MODE_INFO_T cyBle_discoveryModeInfo;
#define advPayload   (cyBle_discoveryModeInfo.advData->advData) 


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Start CYBLE component and register the generic event handler */
    CyBle_Start(StackEventHandler);
    for(;;)
    {
        STATUS_Write(1);CyDelay(300);STATUS_Write(0);
        int16 val=readParticles(); //Perform sensor measurement
        
        /* Dynamic payload will be continuously updated */
        advPayload[19] =  0x05; //Sensor ID: 0x05 for PPD42
        advPayload[25] =  val>>8; //High byte of sensor measurement
        advPayload[26] =  val&0xFF; //Low byte of sensor measurement
        CyBle_GapUpdateAdvData(cyBle_discoveryModeInfo.advData, cyBle_discoveryModeInfo.scanRspData); //Update Advertisment Packet
        
        CyBle_ProcessEvents(); 
        
        CyDelay(500); //Measurement delay 
    }
}

void StackEventHandler(uint32 event, void *eventParam)
{
    switch(event)
    {
        
		case CYBLE_EVT_STACK_ON:
			/* BLE stack is on. Start BLE advertisement */   
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		    break;
		
        default:
            break;
    }   	

}

/*******************************************************************
* NAME :            int16 readParticles()
*
* DESCRIPTION :     Read sensor measurement
* OUTPUTS :
*       int16 Dust concentration in pcs/0.01cf
*/
int16 readParticles(){
    int i; 
    int dur=0;
    
    for(i=0;i<2000000;i++){
      CyDelayUs(1);
      dur+=!PWM_IN_Read();//Count low pulse Duration
    }
    int ratio = dur/20000;
    uint16 senDat = (uint16)(1.1 * pow(ratio, 3.0) - 3.8 * pow(ratio, 2.0) + 520 * ratio + 0.62); // Sensor transfer function
    
    return senDat;
}

/* [] END OF FILE */

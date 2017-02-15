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

/* Function prototypes */  
void StackEventHandler(uint32 event, void* eventParam);
char* readParticles();

/* ADV payload dta structure */  
extern CYBLE_GAPP_DISC_MODE_INFO_T cyBle_discoveryModeInfo;
#define advPayload   (cyBle_discoveryModeInfo.advData->advData) 


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    Serial_Start();
    /* Start CYBLE component and register the generic event handler */
    CyBle_Start(StackEventHandler);
    
    for(;;)
    {
        char *val=readParticles(); //Perform sensor measurement (waits for result)
         
        /* Dynamic payload will be continuously updated */
        advPayload[19] =  0x01; //Sensor ID: 0x01 for SEN0177
        advPayload[23] =  val[4]; //High byte of PM1 value
        advPayload[24] =  val[5]; //Low byte of PM1 value
        advPayload[25] =  val[6]; //High byte of PM2.5 value
        advPayload[26] =  val[7]; //Low byte of PM2.5 value
        advPayload[27] =  val[8]; //High byte of PM10 value
        advPayload[28] =  val[9]; //Low byte of PM10 value
        CyBle_GapUpdateAdvData(cyBle_discoveryModeInfo.advData, cyBle_discoveryModeInfo.scanRspData); //Update Advertisment Packet
        CyBle_ProcessEvents();
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
* NAME :            char* readParticles()
*
* DESCRIPTION :     Read sensor measurement
* OUTPUTS :
*       char* Sensor TX packet
*/
char* readParticles(){
    int idx;
    static char senData[32];
    senData[0]=0x42;
    while(Serial_UartGetChar()!= 0x42); // Wait for start character
    for(idx=1;idx<32;idx++){ // Read TX packet
        CyDelay(10); // Wait for data
        senData[idx]=Serial_UartGetChar();// Get next byte
    }
    return senData;
}

/* [] END OF FILE */

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
        STATUS_Write(1);CyDelay(300);STATUS_Write(0);
        char *val=readParticles(); //Perform sensor measurement (waits for result)
        uint16 pmsmall= (val[3]*256 + val[2])/10; //PM2.5 value
        uint16 pmlarge= (val[5]*256 + val[4])/10; //PM10 value
        /* Dynamic payload will be continuously updated */
        advPayload[19] =  0x02; //Sensor ID: 0x02 for SDS011
        advPayload[21] =  0x00; //High byte of PM1 value - Not Available
        advPayload[22] =  0x00; //Low byte of PM1 value - Not Available
        advPayload[23] =  pmsmall>>8; //High byte of PM2.5 value
        advPayload[24] =  pmsmall&0xFF; //Low byte of PM2.5 value
        advPayload[25] =  pmlarge>>8; //High byte of PM10 value
        advPayload[26] =  pmlarge&0xFF; //Low byte of PM10 value
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
    static char senData[10];
    senData[0]=0xAA;
    while(Serial_UartGetChar()!= 0xAA); // Wait for start character
    for(idx=1;idx<10;idx++){ // Read TX packet
        CyDelay(10); // Wait for data
        senData[idx]=Serial_UartGetChar();// Get next byte
    }
    return senData;
}

/* [] END OF FILE */

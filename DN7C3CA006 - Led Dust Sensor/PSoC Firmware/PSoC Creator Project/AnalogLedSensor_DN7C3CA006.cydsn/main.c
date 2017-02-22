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
int16 readParticles();
uint8 getQualityIndex(uint16 totalConcentration);

/* ADV payload dta structure */  
extern CYBLE_GAPP_DISC_MODE_INFO_T cyBle_discoveryModeInfo;
#define advPayload   (cyBle_discoveryModeInfo.advData->advData) 


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Start CYBLE component and register the generic event handler */
    CyBle_Start(StackEventHandler);
    
    ADC_Start();
    ADC_StartConvert();
    for(;;)
    {
        STATUS_Write(1);CyDelay(300);STATUS_Write(0);
        int16 val=readParticles(); //Perform sensor measurement
        
        /* Dynamic payload will be continuously updated */
        advPayload[19] =  0x04; //Sensor ID: 0x04 for DN7C3CA006
        advPayload[23] =  val>>8; //High byte of sensor measurement
        advPayload[24] =  val&0xFF; //Low byte of sensor measurement
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
*       int16 Dust concentration in ug/m^3
*/
int16 readParticles(){
    int i; 
    int sum=0;
    
    for(i=0;i<4;i++){ // Average of 4 measurements
        Sensor_Power_Write(0); // Turn LED on
        CyDelayUs(250); // Specified delay
        sum+=ADC_CountsTo_mVolts(0, ADC_GetResult16(0)); // Get output voltage
        CyDelayUs(100); // Specified delay
        Sensor_Power_Write(1); // Turn LED off
        CyDelay(10); // Cycle delay
    }
    sum/=4; // Get average
    
    uint16 senDat=sum;//(int16)(1000.0f*((0.172f * (sum/1000.0f)) - 0.0999f)); // Sensor transfer function
    
    return senDat;
}

/* [] END OF FILE */

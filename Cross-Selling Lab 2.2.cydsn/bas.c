/*****************************************************************************
* File Name: bas.c
*
* Version 1.0
*
* Description:
*  Does Analog to Digital conversion of battery voltage and converts it to
*  battery percentage. Has definitions for battery service call-back functions
*  which would update the battery characteristic with the current battery 
*  percentage
*
* Hardware Dependency:
* 	Quadcopter 
*
******************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/

#include <bas.h>

/*******************************************************************************
* Function Name: MeasureBattery()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   Battery Service.  
*
* Parameters:
*  None
*
* Return:
*  None.
*
********************************************************************************/

void MeasureBattery()
{
	uint16 adcResult;
	uint32 mvolts;
    uint32 battery_volt;
    uint8 batteryLevel;
	uint8 adcStatus = 0;
    CYBLE_API_RESULT_T apiResult;
    

    ADC_Battery_Monitor_StartConvert();
	
	/* Immediately return ADC conversion result status */
    adcStatus = ADC_Battery_Monitor_IsEndConversion(ADC_Battery_Monitor_RETURN_STATUS);

	if(adcStatus != 0)
    {
		/* Read the ADC data and convert back to mVolt */
	    adcResult = ADC_Battery_Monitor_GetResult16(0);
	   	mvolts= ADC_Battery_Monitor_CountsTo_mVolts(0,adcResult);
		
		/*scaling measured ADC volt to actual battery voltage*/
		battery_volt= mvolts*4;      
	    battery_volt=battery_volt + OFFSET_CORRECTION;
		
		/* Measure the battery percentage */
	    if(battery_volt < MEASURE_BATTERY_MIN)
	    {
	      	batteryLevel = 0;
	    }
		
	    else if(battery_volt < MEASURE_BATTERY_MID)
	    {
	       	batteryLevel = (battery_volt - MEASURE_BATTERY_MIN) * MEASURE_BATTERY_MID_PERCENT / 
	                           (MEASURE_BATTERY_MID - MEASURE_BATTERY_MIN); 
	    }
	    else if(battery_volt < MEASURE_BATTERY_75)
	    {
	        batteryLevel = MEASURE_BATTERY_MID_PERCENT +
	                           (battery_volt- MEASURE_BATTERY_MID) * (100 - MEASURE_BATTERY_MID_PERCENT) / 
	                           (MEASURE_BATTERY_75 - MEASURE_BATTERY_MID); 
	    }
	    else if(battery_volt < MEASURE_BATTERY_MAX)
	    {
	        batteryLevel= MEASURE_BATTERY_75_PERCENT +
	                           (battery_volt - MEASURE_BATTERY_75) * (100 - MEASURE_BATTERY_75_PERCENT) / 
	                           (MEASURE_BATTERY_MAX - MEASURE_BATTERY_75); 
	    }
	    else
	    {
	            batteryLevel = CYBLE_BAS_MAX_BATTERY_LEVEL_VALUE;
	    }
	    
		/* Update the BLE characteristic with the current battery percentage */
		
		apiResult = CyBle_BassSetCharacteristicValue(CYBLE_BATTERY_SERVICE_INDEX,    
		                CYBLE_BAS_BATTERY_LEVEL, sizeof(batteryLevel), &batteryLevel);
		
		if(CYBLE_ERROR_OK == apiResult)
		{
			/* To remove warning for apiResult */
		}
	}	
}

/* [] END OF FILE */

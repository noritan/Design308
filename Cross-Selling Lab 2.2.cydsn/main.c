/*****************************************************************************
* File Name: main.c
* 
* Version 1.0
*
* Description:
*  The project demonstrates the communication of Quadcopter Controller Board
*  with CyFly Mobile Application. It also demonstrates how to receive broadcast
*  information from a beacon, send it to the client and log the data to FRAM
*
* Hardware Dependency:
* 	Quadcopter 
*
* Code Tested With:
* 	Creator 3.3 SP2
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

#include <main.h>

uint8 fram_rd_data[FRAM_BUFFER_SIZE];
uint8 count;
uint8 status;
uint32 addr;

uint32 FRAM_Address=0;
uint8 Temp_Humid[2];

/* To add Beacons address to Quadcopter Whitelist */
CYBLE_GAP_BD_ADDR_T     whitelistdeviceaddress;

/* Beacon Address */
uint8 beaconAddress[]={0x20, 0x1E, 0x19, 0x50, 0xA0, 0x00};

/* Variable that increments everytime beacon data is received */
uint8 beaconDataCount;

int main()
{
	CyGlobalIntEnable;

	/* Initialize and start all the Component used in the project */
	InitializeSystem();	
	
	#if DEBUG_UART_ENABLED
		UART_Debug_PutString("Welcome To X-Sell with CY products: Quadcopter \r\n");
		UART_Debug_PutString("Please Connect to the CyFly App \r\n");
	#endif
	
	/* Adding Beacon address to Quadcopter Whitelist */
	CyBle_ProcessEvents();
	memcpy(&whitelistdeviceaddress.bdAddr,beaconAddress,sizeof(whitelistdeviceaddress.bdAddr));
	CyBle_GapAddDeviceToWhiteList(&whitelistdeviceaddress); 
	CyBle_GapAddDeviceToWhiteList(&whitelistdeviceaddress);
   	CyBle_ProcessEvents();

	while(1)
    {    
		/* CyBle_ProcessEvents() allows BLE stack to process pending events */
		CyBle_ProcessEvents();
		
		/* Check if the device is connected to CyFly */
		if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {
			/* Measure current Battery voltage and update the battery characterisitc */
			MeasureBattery();
		}
		
		if(beaconDataCount>=MAX_BEACON_DATA_COUNT)
		{
			/* Reset beacon data count */
			beaconDataCount=0;
			
			/* Write temperature and humidity to FRAM */

			/* Add Code 4 shown in the Lab Manual here */
//            /* Write temperature and humidity to FRAM */
//            status = SPI_FRAMBurstWrite(FRAM_Address, Temp_Humid, FRAM_BUFFER_SIZE);			
			/* End of Code 4 */
			
			#if DEBUG_UART_ENABLED
				DBG_PRINTF("\r\nFRAM WRITE");
				
				if(status==FRAM_WRITE_SUCCESS)
				{
					DBG_PRINTF("\n\r Data written to the address 0x%04X = \n\r", (uint16)FRAM_Address);
					for(count=0;count<FRAM_BUFFER_SIZE;count++)
		    		{
			    		DBG_PRINTF("%d\t", Temp_Humid[count]);        
		    		}
				}
				
				else
				{
					DBG_PRINTF("\n\r FRAM Write Operation Failed");
				}
			#endif
			
			/* Read back the data written to FRAM */

			/* Add Code 5 shown in the Lab Manual here */
//            /* Read back the data written to FRAM */
//            status = SPI_FRAMBurstRead(FRAM_Address,fram_rd_data,FRAM_BUFFER_SIZE);			
			/* End of Code 5 */
			
			#if DEBUG_UART_ENABLED
				DBG_PRINTF("\r\nFRAM READ");
				
				if(status==FRAM_READ_SUCCESS)
				{
					DBG_PRINTF("\n\r Data read from the address 0x%04X = \n\r", (uint16)FRAM_Address);
		  		 	for(count=0;count<FRAM_BUFFER_SIZE;count++)
		   		 	{
						DBG_PRINTF("%d\t", fram_rd_data[count]);
		  		 	}
				}
				else
				{
					DBG_PRINTF("\n\r FRAM Read Operation Failed");
				}
			#endif
			
			/* Increment the FRAM address by two bytes */
			FRAM_Address+=2;
		}
    }
}

/*******************************************************************************
* Function Name: InitializeSystem
********************************************************************************
*
* Summary:
*  Starts all the components and initializes PID
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/

void InitializeSystem(void)
{
	/* Delay execution by 3s. */
    CyDelay(3000);
	
	CyBle_Start(GeneralEventHandler);

	UART_Debug_Start();
	
	ADC_Battery_Monitor_Start();
	
	SPI_ExtMem_Start();
}

/* [] END OF FILE */

/*****************************************************************************
* File Name: BLEProcess.c
*
* Version 1.0
*
* Description:
*  Contains function definitions for Event callback handler function for BLE 
*  events 
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
#include <BLEProcess.h>

/* Array used to store the received flight commands */
int32 FlightData[FLIGHT_CHAR_DATA_LEN];

/* A firmware counter used to send the data over UART once 10 Write requests are
received from CyFly */
char softwareCounter = 0;

/* Object to read the advertisement data from the beacon */
CYBLE_GAPC_ADV_REPORT_T advReport;

extern uint8 Temp_Humid[2];

/* Variable that increments everytime beacon data is received */
extern uint8 beaconDataCount;

/*******************************************************************************
* Function Name: GeneralEventHandler
********************************************************************************
*
* Summary:
*  Performs the following tasks:
*  - Handles the necessary BLE events
*  - Reads the data which CyFly sends. Also sends the received beacon data to CyFly
* Parameters:
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The
*                     list of event structure is described in the component
*                     datasheet.
*
* Return:
*  None.
*
*******************************************************************************/

void GeneralEventHandler(uint32 event, void * eventParam)
{
	/* Structure to store data written by Client */	
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;    
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    	
	/* Pointer to the Flight Control Attribute handle */ 
	uint8* ptr;

	switch(event)
    {
		case CYBLE_EVT_STACK_ON:
			/* BLE stack is on. Start BLE advertisement */
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;
		
		case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
			/* This event is generated whenever Advertisement starts or stops.
			* The exact state of advertisement is obtained by CyBle_State() */
			if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
			{
				#if DEBUG_UART_ENABLED
                	DBG_PRINTF("Starting adv\r\n");
				#endif
				CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
			}
		break;
			
		case CYBLE_EVT_GAP_DEVICE_CONNECTED:
			#if DEBUG_UART_ENABLED
				DBG_PRINTF("CyFly is now connected with Quadcopter \r\n");
				DBG_PRINTF("Use the flight control options in the App \r\n");
			#endif
			
			/* Change quadcopter role to observer for scanning beacon data */
			
			/* Add Code 1 shown in the Lab Manual here */
			cyBle_discoveryInfo.discProcedure = 0x00;
            if (CYBLE_ERROR_OK == CyBle_GapcStartScan(CYBLE_SCANNING_FAST))
            {
                #if DEBUG_UART_ENABLED
                    DBG_PRINTF("Start Scan successful\r\n");
                #endif
            }
			/* End of Code 1 */
			
        break;
			
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /*Stop scanning for beacon if device is disconnected */
            
			/* Add Code 2 shown in the Lab Manual here */
			CyBle_GapcStopScan();
			/* End of Code 2 */
			
		break;
            
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
			#if DEBUG_UART_ENABLED
            	DBG_PRINTF("Scan Start Stop Event triggered \r\n");
			#endif
			
            /* If connected, then clear the scanning START_FLAG manually */
            if(CyBle_GetState()==CYBLE_STATE_CONNECTED)
            {   
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_START_FLAG;
            } 
			
            else if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED) 
            {
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            }
            break;
		
		case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:  
			
			/* Add Code 3 shown in the Lab Manual here */
            advReport = *(CYBLE_GAPC_ADV_REPORT_T *)eventParam;
            if(advReport.dataLen != 0)
            {
                /* Last three bytes of beacon broadcast data are humidity, temperature -
                and RSSI respectively. Read the temperature and humidity */
                Temp_Humid[0]=advReport.data[advReport.dataLen-2];
                Temp_Humid[1]=advReport.data[advReport.dataLen-3];
                #if DEBUG_UART_ENABLED
                    DBG_PRINTF("\r\n Beacon data received\r\n");
                #endif
                /* Send the beacon data to CyFly using notifications */
                SendDataNotification(Temp_Humid, BEACON_DATA_BYTE_COUNT);
                /* Increment the beacon data receive count */
                beaconDataCount++;
                #if DEBUG_UART_ENABLED
                    DBG_PRINTF("Temp: %d Humid: %d \r\n", Temp_Humid[0], Temp_Humid[1]);
                #endif
            }						
			/* End of Code 3 */
			
			break;
			
	    case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* This event is generated at GATT disconnection. If so, clear the
			Flight Data */
			FlightData[0] = FALSE;
			FlightData[1] = FALSE;
			FlightData[2] = FALSE;
            FlightData[3] = FALSE;
			
			break;
        
        case CYBLE_EVT_GATTS_WRITE_REQ:
			
			/* This event is received when Client sends write request */
            wrReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventParam;
            
			/* Client writes to the server */
            gattErr = CyBle_GattsWriteAttributeValue(&((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair, 0u, \
                        &((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
			
            /* If error then send error response. If no error then send write response */
			if(gattErr != CYBLE_GATT_ERR_NONE)
            {
                CYBLE_GATTS_ERR_PARAM_T err_param;
				
				#if DEBUG_UART_ENABLED
                	DBG_PRINTF("Gatt write error - %x\r\n", gattErr);
                	DBG_PRINTF("Len = %d ActualLen = %d", wrReqParam->handleValPair.value.len, wrReqParam->handleValPair.value.actualLen);
				#endif
				
                err_param.opcode = (uint8) CYBLE_GATT_WRITE_REQ;
                err_param.attrHandle = wrReqParam->handleValPair.attrHandle;
                err_param.errorCode = gattErr;

                /* Send Error Response */
                CyBle_GattsErrorRsp(wrReqParam->connHandle, &err_param);
            }
            else
            {
                CyBle_GattsWriteRsp(wrReqParam->connHandle);
            }
            
            break;
			
		case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            /* This event is generated when the connected Central device (CyFly)
			*  sends a Write request. The parameter contains the data written */
			/* Extract the Write data sent by Client */
		
			wrReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventParam;
      		if(CYBLE_FLIGHT_CONTROL_FLIGHT_SETPOINT_CHAR_HANDLE ==    
      		wrReqParam->handleValPair.attrHandle)
      		{
         		ptr = wrReqParam->handleValPair.value.val;
         		memcpy(&FlightData[0],ptr,sizeof(int32));
	   			memcpy(&FlightData[1],ptr+4,sizeof(int32));
	   			memcpy(&FlightData[2],ptr+8,sizeof(int32));
	   			memcpy(&FlightData[3],ptr+12,sizeof(int32));
				
				/* Increment the Write Request counter */
         		++softwareCounter;
				
         		if(softwareCounter > MAX_WRITE_REQ) 
         		{
           			softwareCounter= 0;
					
					#if DEBUG_UART_ENABLED
					/* Send the Flight commands over UART */
           				DBG_PRINTF("Left/Right: %ld  Forward/Backward: %ld  Yaw: %ld  Throttle: %ld\r\n",\
						FlightData[0], FlightData[1], FlightData[2],FlightData[3]);
					#endif
         		}
			}

            break;
        
        default:
       	    break;
    }   	
}

/*******************************************************************************
* Function Name: SendDataNotification
********************************************************************************
*
* Summary:
*  Performs the following tasks:
*  Sends the received temperature and humidity data to CyFly as notifications
* Parameters:
*  uint8* data:       pointer to the array containing beacon data
*  uint8 len: 		  number of bytes to be transferred as notifications
*  None.
*
* Return:
*  None.
*
*******************************************************************************/

void SendDataNotification(uint8* beaconData, uint8 len)
{
    
    CYBLE_API_RESULT_T                  bleApiResult = CYBLE_ERROR_OK;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T      BleHandle;
 	BleHandle.value.val = beaconData;
    BleHandle.value.len = len;
    BleHandle.attrHandle = CYBLE_TEMP_HUMID_TEMP_HUMID_MEASUREMENT_CHAR_HANDLE;
            
    do
    {
		/* If stack is not busy, then send the data as notifications */
		if(CyBle_GattGetBusStatus()==CYBLE_STACK_STATE_FREE)
		{
        	bleApiResult = CyBle_GattsNotification(cyBle_connHandle, &BleHandle);
		}
		
        CyBle_ProcessEvents();
    }while((CYBLE_ERROR_OK != bleApiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
        
}

/* [] END OF FILE */

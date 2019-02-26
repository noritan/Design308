/*****************************************************************************
* File Name: FRAM_SPI.c
*
* Version 1.0
*
* Description:
*  This file contains the function definitions to handle FRAM Operations
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

#include <FRAM_SPI.h>

/*******************************************************************************
* Function Name: SPI_FRAMBurstWrite
********************************************************************************
*
* Summary:
*  The function writes data to FRAM
* Parameters
*  uint32 addr             : 32 bit FRAM address for write.
*  uint8 *data_write_ptr   : Pointer to an array of data bytes to be written.
*  uint32 total_data_count : Number of data bytes to be written.
* Return:
*  uint8			  	   : returns the SPI Communication Status
*                     			0 : SPI Communication Error
*                      			1 : SPI Communication Success.
*
*******************************************************************************/

uint8 SPI_FRAMBurstWrite ( uint32 addr, uint8 *data_write_ptr, uint32 total_data_count )
{
    uint32 i;
    uint8  t_status;
    uint32 timeout;
	
	/* Number of bytes received in the SPIM Rx Buffer */
	uint8 Rx_Count=0;
	
    /* If write pointer is NULL, return error */
    if(data_write_ptr == NULL_PTR)
        return SPI_COM_ERROR;

    /* Initialize the communication status to SUCCESS */
    t_status = SPI_COM_SUCCESS;

    /* Clear transmit buffer */
    SPI_ExtMem_SpiUartClearTxBuffer();

    /* Select the F-RAM device */
    Pin_CS_FRAM_Write(0);

    /* Send WREN command */
    SPI_ExtMem_SpiUartWriteTxData(FRAM_WREN);
	
	/* Increment Rx count for every SPIM Tx byte sent */
	Rx_Count++;
	
    /* Initialize timeout */
    TimeOutInit(timeout);
	CyDelayUs(1);
	
    /* Wait for tranmission to complete */
    
    while(Rx_Count !=SPI_ExtMem_SpiUartGetRxBufferSize())
    {
        /* Wait for 1 us and then check for timeout */
        CyDelayUs(1);

        /* Check for communication timeout */
        if(IsTimeOut(timeout++))
        {
            /* Error in SPI Communication */
            t_status = SPI_COM_ERROR;
        }
    }

    /* De-select the F-RAM device */
    Pin_CS_FRAM_Write(1);

    /* Clear transmit buffer */
  	SPI_ExtMem_SpiUartClearTxBuffer();

    /* Select the F-RAM device */
    Pin_CS_FRAM_Write(0);

    /* Send the F-RAM Write Command */
    SPI_ExtMem_SpiUartWriteTxData(FRAM_SRAM_WRITE_CMD);
	Rx_Count++;
	
    /* Send 2 address bytes*/
    SPI_ExtMem_SpiUartWriteTxData(ISB_ADDR_BYTE(addr));
	Rx_Count++;
	SPI_ExtMem_SpiUartWriteTxData(LSB_ADDR_BYTE(addr));
	Rx_Count++;

    /* Initialize timeout */
    TimeOutInit(timeout);

    /* Wait for the transmission to complete */
 	while(Rx_Count!=SPI_ExtMem_SpiUartGetRxBufferSize())
    {
	}
    
    SPI_ExtMem_SpiUartClearRxBuffer();
    for(i = 0; i < total_data_count; i++ )
    {
        /* Send data byte */
        SPI_ExtMem_SpiUartWriteTxData((uint8)(data_write_ptr[i]));
    }
    
    while(total_data_count!= SPI_ExtMem_SpiUartGetRxBufferSize())
    {
	}
    
    /* De-select the F-RAM device */
    Pin_CS_FRAM_Write(1);

    /* return the communication status */
    return t_status;
}

/*******************************************************************************
* Function Name: SPI_FRAMBurstRead
********************************************************************************
*
* Summary:
*  The function reads data from FRAM
* Parameters
*  uint32 addr             : 32 bit FRAM address for read.
*  uint8 *data_read_ptr   : Pointer to an array for storing data bytes.
*  uint32 total_data_count : Number of data bytes to be read.
* Return:
*  uint8			  	   : returns the SPI Communication Status
*                     			0 : SPI Communication Error
*                      			1 : SPI Communication Success.
*
*******************************************************************************/

uint8 SPI_FRAMBurstRead ( uint32 addr, uint8 *data_read_ptr, uint32 total_data_count )
{
    uint32 i;
    uint8 t_status;
    uint32 timeout;
	
	/* Number of bytes received in the SPIM Rx Buffer */
	uint8 Rx_Count=0;

    /* If read pointer is NULL, return error */
    if(data_read_ptr == NULL_PTR)
        return SPI_COM_ERROR;

    /* Initialize the communication status to SUCCESS */
    t_status = SPI_COM_SUCCESS;

    /* Clear transmit and receive buffer */
    SPI_ExtMem_SpiUartClearTxBuffer();
	SPI_ExtMem_SpiUartClearRxBuffer();
	
    /* Select F-RAM */
    Pin_CS_FRAM_Write(0);

    /* Send F-RAM Read Command */
    SPI_ExtMem_SpiUartWriteTxData(FRAM_SRAM_READ_CMD);
	
	/* Increment Rx count for every SPIM Tx byte sent */
	Rx_Count++;
	
    /* Send 2 address bytes */
    SPI_ExtMem_SpiUartWriteTxData(ISB_ADDR_BYTE(addr));
	Rx_Count++;
    SPI_ExtMem_SpiUartWriteTxData(LSB_ADDR_BYTE(addr));
	Rx_Count++;
	
    /* Initialize timeout */
    TimeOutInit(timeout);

    /* Wait for the transmission to complete */
    while(Rx_Count !=SPI_ExtMem_SpiUartGetRxBufferSize())
    {
        /* Wait for 1 us and then check for timeout */
        CyDelayUs(1);

        /* Check for communication timeout */
        if(IsTimeOut(timeout++))
        {
            /* Error in SPI Communication */
            t_status = SPI_COM_ERROR;
        }
    }

    /* Read data bytes from F-RAM */
    for(i = 0; i < total_data_count; i++ )
    {
        /* Clear the receive buffer */
       	SPI_ExtMem_SpiUartClearRxBuffer();

        /* Send dummy byte to receive data */
        SPI_ExtMem_SpiUartWriteTxData((uint8)0x00);

        /* Initialize timeout */
        TimeOutInit(timeout);

        /* Check if the receive buffer is not empty (data received)*/
        while(1!=SPI_ExtMem_SpiUartGetRxBufferSize())
        {
            /* Wait for 1 us and then check for timeout */
            CyDelayUs(1);

            /* Check for communication timeout */
            if(IsTimeOut(timeout++))
            {
                /* Error in SPI Communication */
                t_status = SPI_COM_ERROR;
            }
        }

        /* Read the data byte and store it in read buffer */
  		 data_read_ptr[i] =  SPI_ExtMem_SpiUartReadRxData();
    }

    /* De-select F-RAM device */
    Pin_CS_FRAM_Write(1);

    /* return the communication status */
    return t_status;
}






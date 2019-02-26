/*****************************************************************************
* File Name: FRAM_SPI.h
*
* Version 1.0
*
* Description:
*  This file contains the function prototypes and constants used in
*  the FRAM_SPI.c.
*
* Note:
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

#if !defined(FRAM_SPI_H)
#define FRAM_SPI_H

#include <cytypes.h>
#include <project.h>

/********************************** Macors ******************************************/
#define SPI_COM_ERROR    					0
#define SPI_COM_SUCCESS  					1
#define MSB_ADDR_MASK 						((uint32)0xFF0000)
#define ISB_ADDR_MASK 						((uint32)0x00FF00)
#define MSB_ADDR_SHIFTBITS 					((uint8)16)
#define ISB_ADDR_SHIFTBITS 					((uint8)8)	
#define NULL_PTR         					((uint8_t*) 0u)
#define FRAM_BUFFER_SIZE            		(2u)
#define MSB_ADDR_BYTE(addr) 				((uint8)((addr & MSB_ADDR_MASK)>>MSB_ADDR_SHIFTBITS))
#define ISB_ADDR_BYTE(addr) 				((uint8)((addr & ISB_ADDR_MASK)>>ISB_ADDR_SHIFTBITS))
#define LSB_ADDR_BYTE(addr) 				((uint8)(addr))
#define SPI_COM_TIMEOUT  					(1000u)
#define FRAM_WREN            				((uint8)0x06)   // Write Enable
#define FRAM_WRDI            				((uint8)0x04)   // Write Disable
#define FRAM_SRAM_WRITE_CMD  				((uint8)0x02)   // SRAM Write
#define FRAM_SRAM_READ_CMD   				((uint8)0x03)   // SRAM Read
#define FRAM_RDSR_CMD        				((uint8)0x05)   // Read Status Register
#define FRAM_WRSR_CMD        				((uint8)0x01)   // Write Status Register
#define FRAM_WRITE_SUCCESS					1
#define FRAM_WRITE_FAIL					    0
#define FRAM_READ_SUCCESS					1
#define FRAM_READ_FAIL						0

/******************************************************************************************/

/******************************* Function Declaration *************************************/
extern uint8   SPI_FRAMBurstWrite ( uint32 , uint8* , uint32 );
extern uint8   SPI_FRAMBurstRead  ( uint32 , uint8* , uint32 );
/********************************************************************************************/

/* General Macros*/
#define TRUE							0x01
#define FALSE							0x00
#define TimeOutInit(t)  				(t = 0)
#define IsTimeOut(t) ((t > SPI_COM_TIMEOUT)? 1 : 0)

#endif

/* [] END OF FILE */







/*****************************************************************************
* File Name: bas.h
*
* Version 1.0
*
* Description:
*  This file contains the function prototypes and constants used in
*  the bas.c.
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

#include <project.h>

/***************************************
*          Constants
***************************************/

#define MEASURE_BATTERY_MAX      		3100    
#define MEASURE_BATTERY_MID        		2900  
#define MEASURE_BATTERY_75         		3000
#define MEASURE_BATTERY_MID_PERCENT 	50      
#define MEASURE_BATTERY_75_PERCENT 		75        
#define MEASURE_BATTERY_MIN         	2700 
#define OFFSET_CORRECTION           	200

/***************************************
*          General Macros
***************************************/
#define ENABLED 						1
#define DISABLED 						0

/***************************************
*       Function Prototypes
***************************************/
void MeasureBattery();

/* [] END OF FILE */

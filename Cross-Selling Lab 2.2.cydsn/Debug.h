/*****************************************************************************
* File Name: Debug.h
*
* Version 1.0
*
* Description:
*  This file contains the function prototypes and constants used in
*  the Debug.c.
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

#include <stdio.h>
#include <cytypes.h>
#include <project.h>

/********************************** Macors ******************************************/
#define YES 						1
#define NO  						0
#define DEBUG_UART_ENABLED 			YES
/******************************************************************************************/

/********************************** Conditional Macros ************************************/
#if (DEBUG_UART_ENABLED == YES)
    #define DBG_PRINT_TEXT(a)        do\
                                        {\
                                            printf((a));\
                                        } while (0)

    #define DBG_PRINT_DEC(a)         do\
                                        {\
                                           printf("%02d ", a);\
                                        } while (0)


    #define DBG_PRINT_HEX(a)         do\
                                        {\
                                           printf("%08X ", a);\
                                        } while (0)


    #define DBG_PRINT_ARRAY(a,b)     do\
                                        {\
                                            uint32 i;\
                                            \
                                            for(i = 0u; i < (b); i++)\
                                            {\
                                                printf("%02X ", *(a+i));\
                                            }\
                                        } while (0)

    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
    
    #define DBG_NEW_PAGE             UART_Debug_PutChar(0x0C)
    
    
#else
    #define DBG_PRINT_TEXT(a)
    #define DBG_PRINT_DEC(a)
    #define DBG_PRINT_HEX(a)
    #define DBG_PRINT_ARRAY(a,b)
    #define DBG_PRINTF(...)
    #define DBG_NEW_PAGE             
#endif /* (DEBUG_UART_ENABLED == YES) */

/******************************************************************************************/
/* [] END OF FILE */

/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
//        CyPins_ReadPin(Pin_PRoC_TX)?CyPins_SetPin(Pin_KitProg_RX):CyPins_ClearPin(Pin_KitProg_RX);
//        CyPins_ReadPin(Pin_KitProg_TX)?CyPins_SetPin(Pin_PRoC_RX):CyPins_ClearPin(Pin_PRoC_RX);
        Pin_KitProg_RX_Write(Pin_PRoC_TX_Read());
        Pin_PRoC_RX_Write(Pin_KitProg_TX_Read());        
    }
}

/* [] END OF FILE */

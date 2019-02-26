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
#include <project.h>

int main()
{
    CYBIT uartCRDetect = 0;
    
    CyGlobalIntEnable; // Enable global interrupts

    UART_Start();
    
    CyDelay(1000);
    
    UART_UartPutString("HELLO WORLD\r\n");

    for(;;)
    {
        uint16 ch;
        /* Place your application code here. */
        ch = UART_UartGetByte();
        if (ch >= 256) continue;
        if (uartCRDetect && (ch == '\n')) {
            uartCRDetect = 0;
            continue;
        } else if (ch == '\r') {
            ch = '\n';
            uartCRDetect = 1;
        } else {
            uartCRDetect = 0;
        }
        if (ch == '\n') {
            UART_UartPutChar('\r');
        }
        UART_UartPutChar(ch);        
    }
}

/* [] END OF FILE */

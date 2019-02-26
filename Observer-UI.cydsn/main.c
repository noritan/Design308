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
#include <stdio.h>

// I2C の送受信に使われるバッファ
struct I2cBuffer {
    uint8       temp;       // [RW] 温度データ
    uint8       humid;      // [RW] 湿度データ
    int8        rssi;       // [RW] RSSIデータ
    uint8       id;         // [RW] データのID番号
    uint8       newid;      // [RO] 見つけたいID番号
};

volatile struct I2cBuffer   i2cBuffer;  // バッファの実体

#define     RW_BOUNDARY     (4)         // RW領域のバイト数

int main(void) {
    // 割り込みを許可する
    CyGlobalIntEnable;

    // UARTを起動する
    UART_Start();
        
    // EZI2C を起動する
    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(
        sizeof i2cBuffer,
        RW_BOUNDARY,
        (uint8*)&i2cBuffer
    );

    // CapSense を起動する
    CapSense_Start();
    CapSense_ScanAllWidgets();

    // 初期メッセージを表示する
    UART_UartPutString("Beacon Observer\r\n");

    // メインループ
    for(;;) {
        // CapSense 関連の処理
        if (!CapSense_IsBusy()) {
            // CapSense の Scan がおわったら、
            // 各 Widget の状態を更新する
            CapSense_ProcessAllWidgets();
            
            // TOP ボタンで着目 ID を切り替える
            if (CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID)) {
                // ボタン0は、 ID=1 に関連付けられる
                i2cBuffer.newid = 1;
                Pin_LED_top_Write(0xFF - 0x01);     // TOPはLED0のみ点灯
                Pin_LED_bot_Write(0xff);            // BOTはすべて消灯
            } else if (CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID)) {
                // ボタン1は、 ID=2 に関連付けられる
                i2cBuffer.newid = 2;
                Pin_LED_top_Write(0xFF - 0x02);     // TOPはLED1のみ点灯
                Pin_LED_bot_Write(0xff);            // BOTはすべて消灯
            } else if (CapSense_IsWidgetActive(CapSense_BUTTON2_WDGT_ID)) {
                // ボタン2は、 ID=3 に関連付けられる
                i2cBuffer.newid = 3;
                Pin_LED_top_Write(0xFF - 0x04);     // TOPはLED2のみ点灯
                Pin_LED_bot_Write(0xff);            // BOTはすべて消灯
            } else {
                // その他の場合は現状維持
            }
            
            // 次の Scan を開始する
            CapSense_ScanAllWidgets();
        }
        
        // EZI2Cへの WRITE 動作の処理
        if (EZI2C_EzI2CGetActivity() & EZI2C_EZI2C_STATUS_WRITE1) {
            // WRITE 操作でレジスタが変更されたら、
            // RSSI の値によって LED バーの長さを変える
            if (i2cBuffer.rssi > 0) {
                Pin_LED_bot_Write(0xFF - 0x1F);     // LEDを５個点灯
            } else if (i2cBuffer.rssi > -60) {
                Pin_LED_bot_Write(0xFF - 0x0F);     // LEDを４個点灯
            } else if (i2cBuffer.rssi > -70) {
                Pin_LED_bot_Write(0xFF - 0x07);     // LEDを３個点灯
            } else if (i2cBuffer.rssi > -80) {
                Pin_LED_bot_Write(0xFF - 0x03);     // LEDを２個点灯
            } else if (i2cBuffer.rssi > -90) {
                Pin_LED_bot_Write(0xFF - 0x01);     // LEDを１個点灯
            } else {
                Pin_LED_bot_Write(0xff);            // 全消灯
            }
            // UART に受信結果を表示する
            {
                char buffer[64];    // UART出力のバッファ
                int16 t, h;         // 温度と湿度を格納する変数
                // 温度と湿度を1/10単位の整数で計算する
                t = ((17572L * i2cBuffer.temp) / 256 - 4685 + 5) / 10;
                h = ((12500L * i2cBuffer.humid) / 256 - 600 + 5) / 10;
                // Reportパケットの内容を表示する
                sprintf(buffer, "ID=%d TEMP=%d.%d HUMID=%d.%d RSSI=%d\r\n",
                    i2cBuffer.id, t/10, t%10, h/10, h%10, i2cBuffer.rssi);
                UART_UartPutString(buffer);
            }
        }        
    }
}

/* [] END OF FILE */

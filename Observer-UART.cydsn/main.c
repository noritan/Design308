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
#include <stdio.h>

// Beaconのパケット構造体
struct BeaconPacket {
    uint8   length1;            // 02
    uint8   adType1;            // 01
    uint8   adData1;            // 04
    uint8   length2;            // 1A
    uint8   adType2;            // FF
    uint8   companyId[2];       // 4C 00
    uint8   deviceType;         // 02
    uint8   length3;            // 15
    uint8   uuid[16];           // 00 05 00 01 00 00 10 00 80 00 00 80 5F 9B 01 31
    uint8   major[2];           // 00 xx
    uint8   minor[2];           // xx xx
    uint8   rssi;               // xx
};

// Advertisementパケットの情報を格納する変数
CYBLE_GAPC_ADV_REPORT_T advReport;
                

// 受信したパケットを格納する変数
struct BeaconPacket     beaconPacket;

// 探すべきUUID
const uint8 targetUuid[16] = {
    0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x01, 0x31
};

// 探すべきID(major)
uint8   id = 0;             // 着目するID

// Reportの中身を格納する変数群
volatile CYBIT  reportUpdated = 0;  // Report更新フラグ
uint8           temp;               // 温度データ
uint8           humid;              // 湿度データ
int8            rssi;               // RSSIデータ

// BLEのイベントを処理するハンドラ
void StackEventHandler(uint32 event, void *eventParam) {
    switch (event) {
        //======================================================
        //  必須イベント
        //======================================================
        case CYBLE_EVT_STACK_ON:
            // BLEスタックが構築された
    		// 高速なBLEスキャンを開始する。スキャンは止まらない。
    		CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;

        //======================================================
        //  GAPイベント
        //======================================================
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            // BLEスキャンが開始・停止された
            // 何もしない
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            // BLEスキャンがデバイスを見つけた
            {
                // Reportデータをコピーする
                advReport = *(CYBLE_GAPC_ADV_REPORT_T *)eventParam;
                
                // Reportパケットの長さが期待される長さであるか確認する
                if (advReport.dataLen == sizeof beaconPacket) {
                    // Reportパケットをバッファにコピーする
                    memcpy(&beaconPacket, advReport.data, advReport.dataLen);
                    
                    // 見つけたBeaconが着目しているBeaconか確認する
                    // UUIDフィールドとMAJORフィールドだけを検証する
                    if (
                        (memcmp(beaconPacket.uuid, targetUuid, sizeof targetUuid) == 0)
                        && (beaconPacket.major[0] == 0x00)
                        && (beaconPacket.major[1] == id)
                    ) {
                        // 取り出したい情報をコピーする
                        rssi = advReport.rssi;
                        humid = beaconPacket.minor[0];
                        temp = beaconPacket.minor[1];
                        // Report更新フラグをセットする
                        reportUpdated = 1;
                    }
                }
            }
            break;

        //======================================================
        //  汎用イベント
        //======================================================
        case CYBLE_EVT_TIMEOUT:
            // 時間切れイベント
            // 何もしない
            break;
            
        //======================================================
        //  処理されないイベント
        //======================================================
        default:
            // その他のイベントは単に無視する
            break;
    }       
}

int main(void) {
    // 割り込みを許可する
    CyGlobalIntEnable;

    // UARTを起動する
    UART_Start();
        
    // BLEを起動する
    {
        CYBLE_API_RESULT_T apiResult;
        apiResult = CyBle_Start(StackEventHandler);
        // BLEスタックの初期化が成功したか確認する
        CYASSERT(apiResult == CYBLE_ERROR_OK);
    }
    
    // 初期メッセージを表示する
    UART_UartPutString("Beacon Observer\r\n");

    for(;;) {
        // BLEスタックのイベントを処理する
        CyBle_ProcessEvents();
        
        // Reportパケットが到着したか確認する
        if (reportUpdated) {
            char buffer[64];    // UART出力のバッファ
            int16 t, h;         // 温度と湿度を格納する変数
            // 更新フラグをクリアする
            reportUpdated = 0;
            // 温度と湿度を1/10単位の整数で計算する
            t = ((17572L * temp) / 256 - 4685 + 5) / 10;
            h = ((12500L * humid) / 256 - 600 + 5) / 10;
            // Reportパケットの内容を表示する
            sprintf(buffer, "ID=%d TEMP=%d.%d HUMID=%d.%d RSSI=%d\r\n",
                id, t/10, t%10, h/10, h%10, rssi);
            UART_UartPutString(buffer);            
        }
        
        // 着目するBeaconのIDを変更する
        {
            uint32 ch = UART_UartGetChar();
            if (ch) {
                // UARTから有効な文字を検出した
                char buffer[64];
                // IDを計算して格納する
                // 0..9 が有効 その他の文字も使おうと思えば使える
                id = ch - '0';
                // 新しいIDを表示する
                sprintf(buffer, "SET ID=%d\r\n", id);
                UART_UartPutString(buffer);
            }
        }
    }
}

/* [] END OF FILE */

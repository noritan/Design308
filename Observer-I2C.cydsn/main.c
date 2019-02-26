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

// I2C 関連の宣言
#define     TEMP_INDEX      (0)     // 温度データの位置
#define     HUMID_INDEX     (1)     // 湿度データの位置
#define     RSSI_INDEX      (2)     // RSSIデータの位置
#define     ID_INDEX        (3)     // 受信したIDの位置
#define     RW_BOUNDARY     (4)
#define     NEWID_INDEX     (4)     // 指定されたIDの位置
#define     REG_SIZE        (5)
#define     I2C_ADDRESS     (0x08)

// I2C の送受信に使われるバッファ
uint8       i2cBufferOut[RW_BOUNDARY + 2];
uint8       i2cBufferIn[REG_SIZE];

// I2C の状態変数
enum I2cState {
    ST_IDLE = 0,    // 非通信状態
    ST_WRITING,     // WRITE 通信待ち
    ST_READING      // READ 通信待ち
}   i2cState = ST_IDLE;

// I2C の周期タイマフラグ
volatile CYBIT  i2cTimerFlag = 0;

// 周期的に I2C を動作させる
void i2cTimerCallback(void) {
    // Set タイマフラグ
    i2cTimerFlag = 1;
}

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

// I2C 制御ステートマシン
void i2cDispatch(void) {
    switch (i2cState) {
        case ST_IDLE:
            // 非通信状態で、
            if (i2cTimerFlag) {
                // タイマイベントが発生したら
                // タイマフラグをクリアする
                i2cTimerFlag = 0;
                // 着目するBeaconのIDを変更する
                I2C_I2CMasterReadBuf(
                    I2C_ADDRESS,
                    i2cBufferIn,
                    sizeof i2cBufferIn,
                    I2C_I2C_MODE_COMPLETE_XFER
                );
                // 状態を READ 待ちに遷移させる
                i2cState = ST_READING;
            } else if (reportUpdated) {
                // Reportパケットが到着したら
                // 更新フラグをクリアする
                reportUpdated = 0;
                // Reportパケットの内容を送信する
                i2cBufferOut[0] = 0x00;
                i2cBufferOut[1] = 0x00;
                i2cBufferOut[2 + TEMP_INDEX] = temp;
                i2cBufferOut[2 + HUMID_INDEX] = humid;
                i2cBufferOut[2 + RSSI_INDEX] = rssi;
                i2cBufferOut[2 + ID_INDEX] = id;            
                I2C_I2CMasterWriteBuf(
                    I2C_ADDRESS,
                    i2cBufferOut,
                    sizeof i2cBufferOut,
                    I2C_I2C_MODE_COMPLETE_XFER
                );
                // 状態を WRITE 待ちに遷移させる
                i2cState = ST_WRITING;
            }
            break;
        case ST_READING:
            // READ 待ち状態で、
            if (I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT) {
                // READ 通信がおわったら、
                // ID の値を更新する
                id = i2cBufferIn[NEWID_INDEX];
                // 状態を IDLE に遷移させる
                i2cState = ST_IDLE;
            }
            break;
        case ST_WRITING:
            // WRITE 待ち状態で、
            if (I2C_I2CMasterStatus() & I2C_I2C_MSTAT_WR_CMPLT) {
                // WRITE 通信がおわったら、
                // 状態を IDLE に遷移させる
                i2cState = ST_IDLE;
            }
            break;
        default:
            // 異常状態から IDLE に遷移させる
            i2cState = ST_IDLE;
            break;
    }
}

// メインルーチン
int main(void) {
    // 割り込みを許可する
    CyGlobalIntEnable;

    // I2Cを起動する
    I2C_Start();
    
    // I2C の周期タイマを起動する
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER0, i2cTimerCallback);
        
    // BLEを起動する
    {
        CYBLE_API_RESULT_T apiResult;
        apiResult = CyBle_Start(StackEventHandler);
        // BLEスタックの初期化が成功したか確認する
        CYASSERT(apiResult == CYBLE_ERROR_OK);
    }
    
    for(;;) {
        // BLEスタックのイベントを処理する
        CyBle_ProcessEvents();

        // I2C 制御ステートマシン
        i2cDispatch();
    }
}

/* [] END OF FILE */

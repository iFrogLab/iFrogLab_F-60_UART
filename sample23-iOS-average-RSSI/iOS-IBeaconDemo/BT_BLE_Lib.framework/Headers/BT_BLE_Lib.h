//
//  BT_BLE_Lib.h
//  BTBLEiOSFramework
//
//  Created by Powen Ko on 7/28/14.
//  Copyright (c) 2014 looptek ＆ iFrog Lab. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>


@protocol BT_BLE_LibDelegate;
@interface BT_BLE_Lib : NSObject< CBCentralManagerDelegate>
{
    NSObject<BT_BLE_LibDelegate> __weak *delegate;
    NSError *BTError;
     int BLEMode;
}


@property (nonatomic, weak) id<BT_BLE_LibDelegate> __weak delegate;
- (void) UARTSend:(NSString*)message;         //  UART 送到藍芽
- (int)  iBeaconStartScan;                    //  找 iBeacon
- (int)  iBeaconStopScan;                     //  停止 iBeacon
- (int)  ConnectBlueTooth;                    //  連接藍芽
- (int)  BlueToothConnectStatus;              //  藍芽連接情況
- (void) DigitalOuts:(NSString*)sender;       //  藍芽接腳數位輸出
- (void) DigitalInput;                        //  藍芽接腳數位輸入
@end

@protocol BT_BLE_LibDelegate
- (void) BlueToothStatus:(NSString*)errorMsg;  // 取的HW 藍牙的連接情況
- (void) DidReceiveData:(NSString*)message;    // 收到資料
- (void) DidReadHardwareRevisionString:(NSString*)Msg;  // 硬體相關的訊息
- (void) DigitalInputPinsChanges:(NSString*)Msg;    // 數位輸入的情況改變
- (void) DidReceiveiBeacon:(NSString*)Name  RSSI:(NSNumber *)RSSI Action:(NSString*)Action;                         // iBeacon 的情況
@end



typedef enum
{
    IDLE = 0,
    SCANNING,
    CONNECTED,
    IBEACONSCAN,
} ConnectionState;





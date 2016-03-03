//
//  ViewController.m
//  test
//
//  Created by Powen Ko on 4/20/15.
//  Copyright (c) 2015 Powen Ko. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    // 藍芽初始化
    if(m_BT_BLE_Lib==nil){
        m_BT_BLE_Lib=[[BT_BLE_Lib alloc] init];     // 初始化
        m_BT_BLE_Lib.delegate=self;                 // 接收反應的class 指定為這個
    }
    
    [self performSelector:@selector(start) withObject:nil afterDelay:1.0 ];
}
-(void)start{
    
    [m_BT_BLE_Lib iBeaconStartScan];
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
-(void) viewWillDisappear:(BOOL)animated{
    [m_BT_BLE_Lib iBeaconStopScan];
}

#pragma mark- callback

- (void) BlueToothStatus:(NSString*)errorMsg  // 取的HW 藍牙的連接情況
{
    NSLog(@"BlueToothStatus: %@",errorMsg);
}
- (void) DidReceiveData:(NSString*)message    // 收到資料
{
    
    NSLog(@"DidReceiveData: %@",message);
}
- (void) DidReadHardwareRevisionString:(NSString*)Msg  // 硬體相關的訊息
{
    NSLog(@"DidReadHardwareRevisionString: %@",Msg);
    
}
- (void) DigitalInputPinsChanges:(NSString*)Msg    // 數位輸入的情況改變
{
    
    NSLog(@"DigitalInputPinsChanges: %@",Msg);
}
- (void) DidReceiveiBeacon:(NSString*)Name  RSSI:(NSNumber *)RSSI Action:(NSString*)Action                         // iBeacon 的情況
{
    
    
    NSNumberFormatter *formatter = [NSNumberFormatter new];
    [formatter setNumberStyle:NSNumberFormatterDecimalStyle];

    NSLog(@"DidReceiveiBeacon: Name:%@  RSSI:%@  Action:%@",Name,[formatter stringFromNumber:RSSI],Action);
    
}

@end

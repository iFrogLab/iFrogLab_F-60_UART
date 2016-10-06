//
//  ViewController.m
//  iFrogLab_BT_BLE
//
//  Created by Powen Ko on 10/5/16.
//  Copyright © 2016 searchforartist. All rights reserved.
//

#import "ViewController.h"

typedef enum
{
    LOGGING,
    RX,
    TX,
} ConsoleDataType;



@interface ViewController ()
@property CBCentralManager *cm;
@property ConnectionState state;
@property UARTPeripheral *currentPeripheral;
@end

@implementation ViewController

@synthesize cm = _cm;
@synthesize currentPeripheral = _currentPeripheral;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.cm = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    [self.currentPeripheral setDelegate:self];
    [self addTextToConsole:@"Did start application" dataType:LOGGING];
 
    
    
    [self performSelector:@selector(start) withObject:nil afterDelay:1.0 ];
}
-(void)start{
    // 開始搜尋藍芽設備
    [self iBeaconStartScan];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) addTextToConsole:(NSString *) string dataType:(ConsoleDataType) dataType
{
    NSString *direction;
    switch (dataType)
    {
        case RX:
            direction = @"RX";
            break;
            
        case TX:
            direction = @"TX";
            break;
            
        case LOGGING:
            direction = @"Log";
    }
    
    NSDateFormatter *formatter;
    formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"HH:mm:ss.SSS"];
    
    self.consoleTextView2.text = [self.consoleTextView2.text stringByAppendingFormat:@"[%@] %@: %@\n",[formatter stringFromDate:[NSDate date]], direction, string];
    
    CGPoint bottomOffset = CGPointMake(0, self.consoleTextView2.contentSize.height - self.consoleTextView2.bounds.size.height);
    [self.consoleTextView2 setContentOffset:bottomOffset animated:YES];
    
   // [self.consoleTextView2 setScrollEnabled:NO];
   // NSRange bottom = NSMakeRange(self.consoleTextView.text.length-1, self.consoleTextView.text.length);
  //  [self.consoleTextView2 scrollRangeToVisible:bottom];
  //  [self.consoleTextView2 setScrollEnabled:YES];
}



#pragma mark- BLT iBeacon Find

//  找 iBeacon
- (int)  iBeaconStartScan{
    //CBCentralManagerDelegate
    //centralManagerDidUpdateState(_ central: CBCentralManager)
    
    ////
    NSLog(@"BT_BLE_Lib:iBeaconStartScan()");
    BLEMode=1;
    [self.cm stopScan];
    NSDictionary *options = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:CBCentralManagerScanOptionAllowDuplicatesKey];
    [self.cm scanForPeripheralsWithServices:nil options:options];
    NSLog(@"iBeaconStartScan ...");
    self.state=IBEACONSCAN;
    return  self.state;
}

//  停止 iBeacon
- (int)  iBeaconStopScan{
    
    NSLog(@"BT_BLE_Lib:iBeaconStopScan()");
    NSLog(@"iBeaconStopScan ...");
    BLEMode=0;
    [self.cm stopScan];
    self.state=IDLE;
    return  self.state;
}




#pragma mark- centralManager

- (void) centralManagerDidUpdateState:(CBCentralManager *)central
{
    NSString *stateString = nil;
    switch(central.state) //bluetoothManager.state)
    {
        case CBCentralManagerStateResetting: stateString = @"The connection with the system service was momentarily lost, update imminent."; break;
        case CBCentralManagerStateUnsupported: stateString = @"The platform doesn't support Bluetooth Low Energy."; break;
        case CBCentralManagerStateUnauthorized: stateString = @"The app is not authorized to use Bluetooth Low Energy."; break;
        case CBCentralManagerStatePoweredOff: stateString = @"State Powered Off"; break;
        case CBCentralManagerStatePoweredOn: stateString = @"State Powered On"; break;
        default: stateString = @"State unknown, update imminent."; break;
    }
    
    [self addTextToConsole:stateString dataType:LOGGING];
   // [_delegate BlueToothStatus :stateString]; //@"State Powered On"];
}

- (void) centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    if(BLEMode==1){
        NSString* t_name=peripheral.name;
        NSLog(@"Discovered %@ at %@",t_name , RSSI);
        
        [self addTextToConsole:[NSString stringWithFormat:@"Name=%@  RSSI=%@",peripheral.name, RSSI   ] dataType:LOGGING];
        
    
    }else{
        NSLog(@"Did discover peripheral %@", peripheral.name);
        [self.cm stopScan];
    
        self.currentPeripheral = [[UARTPeripheral alloc] initWithPeripheral:    peripheral delegate:self];
    
        [self.cm connectPeripheral:peripheral options:@{CBConnectPeripheralOptionNotifyOnDisconnectionKey: [NSNumber numberWithBool:YES]}];
    }
    

}

- (void) centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    NSLog(@"Did connect peripheral %@", peripheral.name);
    
    [self addTextToConsole:[NSString stringWithFormat:@"Did connect to %@", peripheral.name] dataType:LOGGING];
    
  
}

- (void) centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"Did disconnect peripheral %@", peripheral.name);
    

}
///////////////

@end

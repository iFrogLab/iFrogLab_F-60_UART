//
//  ViewController.h
//  iFrogLab_BT_BLE
//
//  Created by Powen Ko on 10/5/16.
//  Copyright © 2016 searchforartist. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "UARTPeripheral.h"

@interface ViewController : UIViewController < CBCentralManagerDelegate, UARTPeripheralDelegate>{
    
    
   // NSObject<BT_BLE_LibDelegate> __weak *delegate;
    
    int BLEMode;
}

@property (weak, nonatomic) IBOutlet UITextView *consoleTextView2;

@end



typedef enum
{
    IDLE = 0,
    SCANNING,
    CONNECTED,
    IBEACONSCAN,
} ConnectionState;

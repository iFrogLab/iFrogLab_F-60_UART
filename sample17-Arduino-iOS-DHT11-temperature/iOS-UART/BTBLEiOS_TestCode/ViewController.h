//
//  ViewController.h
//  BTBLEiOS_TestCode
//
//  Created by Powen Ko on 7/28/14.
//  Copyright (c) 2014 looptek. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BT_BLE_Lib.h"
@interface ViewController : UIViewController<BT_BLE_LibDelegate>{
    BT_BLE_Lib* m_BT_BLE_Lib;
}
@property (strong, nonatomic) IBOutlet UIButton *connectButton;
@property (strong, nonatomic) IBOutlet UIButton *sendButton;
@property (strong, nonatomic) IBOutlet UITextField *sendTextField;
@property (strong, nonatomic) IBOutlet UITextView *consoleTextView;

- (void) BlueToothStatus:(NSString*)errorMsg;
- (void) UARTReciver:(NSString*)message;
- (void) DidReceiveData:(NSString*)message;
- (void) DidReadHardwareRevisionString:(NSString*)Msg;

- (void) DigitalInputPinsChanges:(NSString*)Msg;

@end

//
//  ViewController.m
//  BTBLEiOS_TestCode
//
//  Created by Powen Ko on 7/28/14.
//  Copyright (c) 2014 looptek. All rights reserved.
//

#import "ViewController.h"
#import "BT_BLE_Lib.h"

/*
typedef enum
{
    IDLE = 0,
    SCANNING,
    CONNECTED,
} ConnectionState;
*/
typedef enum
{
    LOGGING,
    RX,
    TX,
} ConsoleDataType;



@interface ViewController ()
    @property ConnectionState state;
@end

@implementation ViewController
-(void) UIEnable:(BOOL)iEnable
{
    
    [self.connectButton setEnabled:iEnable];
    [self.sendButton setEnabled:iEnable];
    [self.sendTextField setEnabled:iEnable];
    
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    ////////
    
    [self UIEnable:NO];
    [self.sendTextField setDelegate:self];
    
    if(m_BT_BLE_Lib==nil){
        m_BT_BLE_Lib=[[BT_BLE_Lib alloc] init];     // 初始化
        m_BT_BLE_Lib.delegate=self;                 // 接收反應的class 指定為這個class
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark -
#pragma mark Delegate delegate methods
/*
- (void) BlueToothErrorStatus:(NSString*)errorMsg
{
    
    NSLog(@"BT Error Msg = %@",errorMsg);
    
}
*/

#pragma mark- callback
//取的硬體傳過來藍牙硬體狀態。
- (void) BlueToothStatus:(NSString*)errorMsg{
    
    if([errorMsg isEqualToString:@"Started scan ..."]){
        
        [self.connectButton setTitle:@"Started scan ..." forState:UIControlStateNormal];
    }else if([errorMsg isEqualToString:@"Connected"]){
        
        [self.connectButton setTitle:@"Disconnect" forState:UIControlStateNormal];
        
        [self.connectButton setTitle:@"Disconnect" forState:UIControlStateNormal];
        //    [self.sendButton setUserInteractionEnabled:YES];
        [self.sendTextField setUserInteractionEnabled:YES];
       // [self FunPinsEnable:YES];
    }else  if([errorMsg isEqualToString:@"Disconnected"]){
        [self.connectButton setTitle:@"Connect" forState:UIControlStateNormal];
        //   [self.sendButton setUserInteractionEnabled:NO];
        [self.sendTextField setUserInteractionEnabled:NO];
    }else  if([errorMsg isEqualToString:@"State Powered On"]){
        [self UIEnable:YES];
    }else{
      UIAlertView *alert = [[UIAlertView alloc] initWithTitle: @"BlueTooth Status"
                                                message: errorMsg
                                                delegate: nil cancelButtonTitle:@"OK"
                                                otherButtonTitles:nil];
        [alert show];
       // [alert release];
        
    }
}


// 收到硬體船過來的資料
- (void) DidReceiveData:(NSString*)message{
    [self addTextToConsole:message dataType:RX];
}

- (void) didReadHardwareRevisionString:(NSString*)Msg{
    [self addTextToConsole:Msg dataType:RX];
}



#pragma mark- button actions

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
    
    [self.consoleTextView setText:[self.consoleTextView.text stringByAppendingFormat:@"[%@] %@: %@\n",[formatter stringFromDate:[NSDate date]], direction, string]];
    
}

-(void) Fun_Display{
    
    self.state=[m_BT_BLE_Lib BlueToothConnectStatus];
    switch (self.state) {
        case IDLE:
            self.state = SCANNING;
            NSLog(@"Started scan ...");
            [self.connectButton setTitle:@"IDLE ..." forState:UIControlStateNormal];
            break;
        case SCANNING:
            self.state = IDLE;
            // NSLog(@"Stopped scan");
            [self.connectButton setTitle:@"Scanning ..." forState:UIControlStateNormal];
            break;
        case CONNECTED:
            [self.connectButton setTitle:@"Disconnect" forState:UIControlStateNormal];
            break;
    }
    
}

- (IBAction)connectButtonPressed:(id)sender {
    
    [self.sendTextField resignFirstResponder];
    [m_BT_BLE_Lib ConnectBlueTooth];
     // BlueToothConnect];
    [self Fun_Display];
    
}

- (IBAction)sendButtonPressed:(id)sender {
    [self.sendTextField resignFirstResponder];
    
    if (self.sendTextField.text.length == 0){
        return;
    }
    [self addTextToConsole:self.sendTextField.text dataType:TX];
    [m_BT_BLE_Lib UARTSend:self.sendTextField.text];
    [self.sendTextField setText:@""];
}




#pragma mark Functions reaction.
/*
-(void) FunPinsEnable:(BOOL) i_action{
  
}*/

- (void) DidReadHardwareRevisionString:(NSString*)Msg  // 硬體相關的訊息
{
    NSLog(@"%@",Msg);
}
@end

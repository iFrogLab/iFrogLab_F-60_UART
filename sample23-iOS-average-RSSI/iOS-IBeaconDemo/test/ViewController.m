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
    [self ButtonReset:nil];
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
- (void) DidReceiveData:(NSString*)message    // 收{
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
    if (Name!=nil) {
        double t1=10;
        double t_RSSI=[RSSI doubleValue];
        if(t_RSSI<0){
            if(m_lastRssi==0){
                m_lastRssi=t_RSSI;
                m_RssiMin=t_RSSI;
                m_RssiMax=t_RSSI;
            }else{
                if(m_lastRssi<t_RSSI+t1 &&  m_lastRssi>t_RSSI-t1){
                }else{
                    return;
                }
            }
            if(m_RssiMin<t_RSSI) m_RssiMin=t_RSSI;
            if(m_RssiMax>t_RSSI) m_RssiMax=t_RSSI;
            m_lastRssi=t_RSSI; //((m_lastRssi*199)+t_RSSI)/200;
            m_display++;
               
               NSNumberFormatter *formatter = [NSNumberFormatter new];
               [formatter setNumberStyle:NSNumberFormatterDecimalStyle];
               NSString* t1=[NSString stringWithFormat:@"Name:%@ \n\t RSSI:%f  Action:%@",Name,m_lastRssi,Action];
               NSLog(@"%@",t1);
               NSString* t_StringRSSI=[NSString stringWithFormat:@"%f",t_RSSI];
               NSString* t_availe1=mRSSI[t_StringRSSI];
               if(t_availe1==nil){
                   [mRSSI setObject:@"1" forKey:t_StringRSSI];
               }else{
                   int t2 = [t_availe1 intValue];
                   t2=t2+1;
                   NSString* t3=[NSString stringWithFormat:@"%d",t2];
                   [mRSSI setObject:t3 forKey:t_StringRSSI];
               }
               
               // NSString* t_availe2=mRSSI[t_StringRSSI];
               NSString* t_availe3=[NSString stringWithFormat:@"%f",
                                    [self GetBigestValueInDictionary:mRSSI]];
               
               
            
            if(m_display%20==0){
                
                m_UILabel1.text=[NSString stringWithFormat:@"%f",m_lastRssi];
            //    m_UITextView1.text=[NSString stringWithFormat:@"%@\n\t%@",t1,m_UITextView1.text];
               
               m_UILabel2.text=[NSString stringWithFormat:@"Min=%f, Max=%f ",m_RssiMin,m_RssiMax];
               
               
               m_UILabel3.text=[NSString stringWithFormat:@"%f",
                                [self calculateDistance:m_lastRssi] ];
               m_UILabel4.text=[NSString stringWithFormat:@"Ave=%@",t_availe3];
               
               if( m_display>200){
                   m_display=0;
               //    m_UITextView1.text=[NSString stringWithFormat:@"%@\n\t%@",t1,@""];
               }
            }
        }
    }
}
-(double) GetBigestValueInDictionary:(NSDictionary *)dict
{
    NSArray *keys;
    int i, count;
    id key,value;
    double key2,value2;
    
    double BigestKey=0;
    double BigestValue=0;
    
    keys = [dict allKeys];
    count = (int)[keys count];
    for (i = 0; i < count; i++)
    {
        key = [keys objectAtIndex: i];
        value = [dict objectForKey: key];
        NSLog (@"Key: %@ for value: %@", key, value);
        if(BigestValue==0){
            value2=BigestValue=[value floatValue];
            key2=BigestKey=[key floatValue];
        }else{
            value2=[value floatValue];
            if(value2>BigestValue){
                BigestValue=[value floatValue];
                BigestKey=[key floatValue];
            }
        }
       // dict[key]=dict[key]/2;
        /*
        [mRSSI setObject:[NSString stringWithFormat:@"%f",value2/1.01f]
                  forKey:key];
        if(value2<1){
            [mRSSI  removeObjectForKey:key];
        }*/
    }
    return BigestKey;
}
- (IBAction)ButtonReset:(id)sender {
    m_RssiMin=0;
    m_RssiMax=0;

    m_lastRssi=0;
    m_display=0;
    mRSSI = [NSMutableDictionary dictionary];
    
}

-(double) calculateDistance:(double) rssi {
    
                         //    0,   50cm,  100cm     150cm   200cm     250cm    300cm
    double nameArr1[] =  {-29.0f,-53.69f, -74.92f, -79.81f,-82.00f,  -84.00f,  -77.00f};
    double nameArr2[] =  {-29.0f,-53.69f, -74.92f, -79.81f,-82.00f,  -84.00f,  -86.00f};
    int index=0;
    for(index=0;(index+1)<=(sizeof(nameArr1)/sizeof(double));index++){
       if(rssi> nameArr1[index] && rssi< nameArr1[index+1] )
       {
           return index;
       }
    }
    return 0;
}


// RSSI [dBm] = -10n log10 (d) + A [dBm]
-(double) calculateAccuracy:(int) txPower   rssi:(double) rssi {
    if (rssi == 0)
    {
        return -1.0;
    }
    
    double ratio = rssi * 1.0 / txPower;
    
    if (ratio < 1.0)
    {
        return pow(ratio, 10);
    }
    else
    {
        double accuracy = (0.89976) * pow(ratio, 7.7095) + 0.111;
        return accuracy;
    }
    return 0;
}



@end

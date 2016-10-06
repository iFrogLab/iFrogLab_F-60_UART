//
//  ViewController.h
//  test
//
//  Created by Powen Ko on 4/20/15.
//  Copyright (c) 2015 Powen Ko. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BT_BLE_Lib.h"
@interface ViewController : UIViewController <BT_BLE_LibDelegate>
{
    BT_BLE_Lib* m_BT_BLE_Lib;
    __weak IBOutlet UITextView *m_UITextView1;
    __weak IBOutlet UILabel *m_UILabel1;
    __weak IBOutlet UILabel *m_UILabel2;
    __weak IBOutlet UILabel *m_UILabel3;
    __weak IBOutlet UILabel *m_UILabel4;
    double m_lastRssi;
    double m_RssiMin;
    double m_RssiMax;
    int m_display;
    NSMutableDictionary *mRSSI;
}
-(double) calculateAccuracy:(int) txPower   rssi:(double) rssi;
@end


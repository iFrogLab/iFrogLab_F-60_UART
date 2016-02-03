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
}
@end


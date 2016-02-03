package com.powenko.myimageviewer;

import java.io.UnsupportedEncodingException;
import java.text.DateFormat;
import java.util.Date;


import com.powenko.ifroglab_bt_lib.UartService;
import com.powenko.ifroglab_bt_lib.ifrog;
import com.powenko.ifroglab_bt_lib.ifrog.ifrogCallBack;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class page2  extends ActionBarActivity implements ifrogCallBack {
 
     private Button button1;
     private EditText editText1;
     private TextView textView1;
     
     private UartService mService = null;
     private BluetoothDevice mDevice = null;
     private BluetoothAdapter mBtAdapter = null;     

     private String mState;
     private String deviceAddress;
     

     private ifrog mifrog;
     @Override
     protected void onCreate(Bundle savedInstanceState) {
          super.onCreate(savedInstanceState);
          setContentView(R.layout.page2layout );          //顯示畫面資料
                                                          // Display information
         
          Bundle bundle= this.getIntent().getExtras();    // 取得第一頁傳遞過來的資料。
                                                          // Get the first page of data passed.
         
         String Names=bundle.getString("Names");
         deviceAddress=bundle.getString("Address");
         textView1=(TextView) findViewById(R.id.textView1 );   //取得TextView
         textView1.setText(Names+deviceAddress);

      	 editText1 = (EditText) findViewById(R.id.editText1);
      	
         button1=(Button) findViewById(R.id.button1  );          //取得button1
       
         
         // Handler Send button  
         button1.setOnClickListener(new View.OnClickListener() {  // 監聽按鍵動作
                                                                  // Monitor keystrokes
             @Override
             public void onClick(View v) {
             	String message = editText1.getText().toString();
             	byte[] value;
 					//send data to service
 					try {
						value = message.getBytes("UTF-8");
	 					mService.writeRXCharacteristic(value);
					} catch (UnsupportedEncodingException e) {
					} 
             }
         });
         BT_init();
     }
     public void BT_init(){

         mifrog=new ifrog();
         mifrog.setTheListener(this);
         mBtAdapter = BluetoothAdapter.getDefaultAdapter();
         if (mBtAdapter == null) {
             Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
             finish();
             return;
         }
         service_init();
     }
     
     @Override                                              //用戶按下android 機器上的back 按鍵。
                                                            // Android user presses the back button on the machine.
    
     public boolean onKeyDown(int keyCode, KeyEvent event) {    
         if (keyCode == KeyEvent.KEYCODE_BACK) {   //查用戶按下back 按鍵 ?
                                                   // Check the user presses the back button?
        	 finish();
             return true;
         }
         return super.onKeyDown(keyCode, event);     // 如果其他按鍵交給android
                                                     // If the other keys to android
     }
	@Override
	public void BTSearchFindDevice(BluetoothDevice arg0, int arg1, byte[] arg2) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void BTSearchFindDevicestatus(boolean arg0) {
		// TODO Auto-generated method stub
		
	}
	
	 private ServiceConnection mServiceConnection = new ServiceConnection() {
	        public void onServiceConnected(ComponentName className, IBinder rawBinder) {
	        		mService = ((UartService.LocalBinder) rawBinder).getService();
	        		Log.d("powenko.com", "onServiceConnected mService= " + mService);
	        		if (!mService.initialize()) {
	                    Log.e("powenko.com", "Unable to initialize Bluetooth");
	                    finish();
	                }

	        }

	        public void onServiceDisconnected(ComponentName classname) {
	       ////     mService.disconnect(mDevice);
	        		mService = null;
	        }
	  };
	  private final BroadcastReceiver UARTStatusChangeReceiver = new BroadcastReceiver() {

	        public void onReceive(Context context, Intent intent) {
	            String action = intent.getAction();

	            final Intent mIntent = intent;
	           //*********************//
	            if (action.equals(UartService.ACTION_GATT_CONNECTED)) {
	            	 runOnUiThread(new Runnable() {
	                     public void run() {
	                         textView1.append("Connected\n");
	                         mState ="Connected";
	                     }
	            	 });
	            }
	           
	          //*********************//
	            if (action.equals(UartService.ACTION_GATT_DISCONNECTED)) {
	            	 runOnUiThread(new Runnable() {
	                     public void run() {
	                         textView1.append("Disconnected\n");
	                         mState ="Disconnected";
	                         mService.close(); 
	                     }
	                 });
	            }
	            
	          
	          //*********************//
	            if (action.equals(UartService.ACTION_GATT_SERVICES_DISCOVERED)) {
	             	 mService.enableTXNotification();
	            }
	          //*********************//
	            if (action.equals(UartService.ACTION_DATA_AVAILABLE)) {
	              
	                 final byte[] txValue = intent.getByteArrayExtra(UartService.EXTRA_DATA);
	                 runOnUiThread(new Runnable() {
	                     public void run() {
	                         try {
	                         	String text = new String(txValue, "UTF-8");
		                        textView1.append("RX: "+text+"\n");
	                         } catch (Exception e) {
	                             Log.e("powenko.com", e.toString());
	                         }
	                     }
	                 });
	             }
	           //*********************//
	            if (action.equals(UartService.DEVICE_DOES_NOT_SUPPORT_UART)){
	            	runOnUiThread(new Runnable() {
	                     public void run() {
	                    	 textView1.append("Device doesn't support UART. Disconnecting");
	                     }
		             });
	            	mService.disconnect();
	            }
	            
	            
	        }
	    };

		private final Handler handler = new Handler();
	    private void service_init() {
	            Intent bindIntent = new Intent(this, UartService.class);
	            bindService(bindIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
	            LocalBroadcastManager.getInstance(this).registerReceiver(UARTStatusChangeReceiver, makeGattUpdateIntentFilter());
	            Runnable notification = new Runnable() {
			        public void run() {  
				        mDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(deviceAddress);
		                mService.connect(deviceAddress);
					}
			    };
			    handler.postDelayed(notification,1000);
			    
			  
                
	        }
	        private static IntentFilter makeGattUpdateIntentFilter() {
	            final IntentFilter intentFilter = new IntentFilter();
	            intentFilter.addAction(UartService.ACTION_GATT_CONNECTED);
	            intentFilter.addAction(UartService.ACTION_GATT_DISCONNECTED);
	            intentFilter.addAction(UartService.ACTION_GATT_SERVICES_DISCOVERED);
	            intentFilter.addAction(UartService.ACTION_DATA_AVAILABLE);
	            intentFilter.addAction(UartService.DEVICE_DOES_NOT_SUPPORT_UART);
	            return intentFilter;
	        }
     
	        
	        @Override
	        public void onDestroy() {
	        	 super.onDestroy();
	            
	            try {
	            	LocalBroadcastManager.getInstance(this).unregisterReceiver(UARTStatusChangeReceiver);
	            } catch (Exception ignore) {
	               
	            } 
	            unbindService(mServiceConnection);
	            mService.stopSelf();
	            mService= null;
	           
	        }	        
}

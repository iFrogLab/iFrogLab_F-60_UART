package com.powehnko.ifroglabbt_ap_pins;


/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *  
 *  sample code writer: Powen Ko
 *  website: www.powenko.com
 *  hardware device: www.ifroglab.com
 */



import java.io.UnsupportedEncodingException;
import java.text.DateFormat;
import java.util.Date;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.powenko.ifroglab_bt_lib.UartService;
import com.powenko.ifroglab_bt_lib.ifrog;
import com.powenko.ifroglab_bt_lib.ifrog.ifrogCallBack;

public class MainActivity extends Activity implements RadioGroup.OnCheckedChangeListener, ifrogCallBack {
    private static final int REQUEST_SELECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int UART_PROFILE_READY = 10;
    public static final String TAG = "nRFUART";
    private static final int UART_PROFILE_CONNECTED = 20;
    private static final int UART_PROFILE_DISCONNECTED = 21;
    private static final int STATE_OFF = 10;

    TextView mRemoteRssiVal;
    RadioGroup mRg;
    private int mState = UART_PROFILE_DISCONNECTED;
    private UartService mService = null;
    private BluetoothDevice mDevice = null;
    private BluetoothAdapter mBtAdapter = null;
    private ListView messageListView;
    private ArrayAdapter<String> listAdapter;
    private Button btnConnectDisconnect;
    // private Button btnSend;
  //  private EditText edtMessage;

    private Button button2on,button2off,button3on,button3off;
    private Button button4on,button4off,button5on,button5off;
    private Button button6on,button6off,button7on,button7off;
    private Button button8on,button8off,button9on,button9off;
    		
    private ifrog mifrog;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
       
        mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBtAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
          //  finish();
            return;
        }
        messageListView = (ListView) findViewById(R.id.listMessage);
        listAdapter = new ArrayAdapter<String>(this, R.layout.message_detail);
        messageListView.setAdapter(listAdapter);
        messageListView.setDivider(null);
        btnConnectDisconnect=(Button) findViewById(R.id.btn_select);
       // btnSend=(Button) findViewById(R.id.sendButton);
      //  edtMessage = (EditText) findViewById(R.id.sendText);
        service_init();
        mifrog=new ifrog();
        mifrog.setTheListener(this);
     
       
        // Handler Disconnect & Connect button
        btnConnectDisconnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mBtAdapter.isEnabled()) {
                    Log.i(TAG, "onClick - BT not enabled yet");
                    Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
                }
                else {
                	if (btnConnectDisconnect.getText().equals("Connect")){
                		
                		//Connect button pressed, open DeviceListActivity class, with popup windows that scan for devices
                		
            			Intent newIntent = new Intent(MainActivity.this, DeviceListActivity.class);
            			startActivityForResult(newIntent, REQUEST_SELECT_DEVICE);
        			} else {
        				//Disconnect button pressed
        				if (mDevice!=null)
        				{
        					mService.disconnect();
        					
        				}
        			}
                }
            }
        });
       
        //////////////è¨­å®šæŒ‰éˆ•å��æ‡‰
        button2on=(Button) findViewById(R.id.button2on);
        button2on.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	FunSendString("m2 on");
            }
        });
        button2off=(Button) findViewById(R.id.button2off);
        button2off.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	FunSendString("m2 off");
            }
        });
        button3on =FunSetButton(R.id.button3on ,"m3 on");
        button3off=FunSetButton(R.id.button3off,"m3 off");
        button4on =FunSetButton(R.id.button4on ,"m4 on");
        button4off=FunSetButton(R.id.button4off,"m4 off");
        button5on =FunSetButton(R.id.button5on ,"m5 on");
        button5off=FunSetButton(R.id.button5off,"m5 off");
        button6on =FunSetButton(R.id.button6on ,"m6 on");
        button6off=FunSetButton(R.id.button6off,"m6 off");
        button7on =FunSetButton(R.id.button7on ,"m7 on");
        button7off=FunSetButton(R.id.button7off,"m7 off");
        button8on =FunSetButton(R.id.button8on ,"m8 on");
        button8off=FunSetButton(R.id.button8off,"m8 off");
        button9on =FunSetButton(R.id.button9on ,"m9 on");
        button9off=FunSetButton(R.id.button9off,"m9 off");
        
        
    }
    
    private void FunSendString(String message){
    	//EditText editText = (EditText) findViewById(R.id.sendText);
    	// String message = editText.getText().toString();
    	byte[] value;
		try {
			//send data to service
			value = message.getBytes("UTF-8");
			mService.writeRXCharacteristic(value);
			//Update the log with time stamp
			String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
			listAdapter.add("["+currentDateTimeString+"] TX: "+ message);
       	 	messageListView.smoothScrollToPosition(listAdapter.getCount() - 1);
       	// 	edtMessage.setText("");
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	
    }
    private Button FunSetButton(int id,final String iStr ){
    	Button name=(Button) findViewById(id);
    	name.setOnClickListener(new View.OnClickListener() {
              @Override
              public void onClick(View v) {
              	FunSendString(iStr);
              }
          });
    	return name;
    }
    
    //UART service connected/disconnected
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder rawBinder) {
        	             
        		mService = ((UartService.LocalBinder) rawBinder).getService();
        		Log.d(TAG, "onServiceConnected mService= " + mService);
        		if (!mService.initialize()) {
                    Log.e(TAG, "Unable to initialize Bluetooth");
                    finish();
                }

        }

        public void onServiceDisconnected(ComponentName classname) {
       ////     mService.disconnect(mDevice);
        		mService = null;
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        
        //Handler events that received from UART service 
        public void handleMessage(Message msg) {
  
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
                         	String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                             Log.d(TAG, "UART_CONNECT_MSG");
                             btnConnectDisconnect.setText("Disconnect");
                         //    edtMessage.setEnabled(true);
                             button2on.setEnabled(true);
                             button2off.setEnabled(true);
                             button3on.setEnabled(true);
                             button3off.setEnabled(true);
                             button4on.setEnabled(true);
                             button4off.setEnabled(true);
                             button5on.setEnabled(true);
                             button5off.setEnabled(true);
                             button6on.setEnabled(true);
                             button6off.setEnabled(true);
                             button7on.setEnabled(true);
                             button7off.setEnabled(true);
                             button8on.setEnabled(true);
                             button8off.setEnabled(true);
                             button9on.setEnabled(true);
                             button9off.setEnabled(true);
                             ((TextView) findViewById(R.id.deviceName)).setText(mDevice.getName()+ " - ready");
                             listAdapter.add("["+currentDateTimeString+"] Connected to: "+ mDevice.getName());
                        	 	messageListView.smoothScrollToPosition(listAdapter.getCount() - 1);
                             mState = UART_PROFILE_CONNECTED;
                     }
            	 });
            }
           
          //*********************//
            if (action.equals(UartService.ACTION_GATT_DISCONNECTED)) {
            	 runOnUiThread(new Runnable() {
                     public void run() {
                    	 	 String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                             Log.d(TAG, "UART_DISCONNECT_MSG");
                             btnConnectDisconnect.setText("Connect");
                            // edtMessage.setEnabled(false);
                             button2on.setEnabled(false);
                             button2off.setEnabled(false);
                             button3on.setEnabled(false);
                             button3off.setEnabled(false);
                             button4on.setEnabled(false);
                             button4off.setEnabled(false);
                             button5on.setEnabled(false);
                             button5off.setEnabled(false);
                             button6on.setEnabled(false);
                             button6off.setEnabled(false);
                             button7on.setEnabled(false);
                             button7off.setEnabled(false);
                             button8on.setEnabled(false);
                             button8off.setEnabled(false);
                             button9on.setEnabled(false);
                             button9off.setEnabled(false);
                             ((TextView) findViewById(R.id.deviceName)).setText("Not Connected");
                             listAdapter.add("["+currentDateTimeString+"] Disconnected to: "+ mDevice.getName());
                             mState = UART_PROFILE_DISCONNECTED;
                             mService.close();
                            //setUiState();
                         
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
                         	String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                        	 	listAdapter.add("["+currentDateTimeString+"] RX: "+text);
                        	 	messageListView.smoothScrollToPosition(listAdapter.getCount() - 1);
                        	
                         } catch (Exception e) {
                             Log.e(TAG, e.toString());
                         }
                     }
                 });
             }
           //*********************//
            if (action.equals(UartService.DEVICE_DOES_NOT_SUPPORT_UART)){
            	showMessage("Device doesn't support UART. Disconnecting");
            	mService.disconnect();
            }
            
            
        }
    };

    private void service_init() {
    //	this.mifrog.BTService_init(mServiceConnection,UARTStatusChangeReceiver);
        Intent bindIntent = new Intent(this, UartService.class);
        bindService(bindIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
        LocalBroadcastManager.getInstance(this).registerReceiver(UARTStatusChangeReceiver, makeGattUpdateIntentFilter());
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
    public void onStart() {
        super.onStart();
    }

    @Override
    public void onDestroy() {
    	 super.onDestroy();
        Log.d(TAG, "onDestroy()");
        
        try {
        	LocalBroadcastManager.getInstance(this).unregisterReceiver(UARTStatusChangeReceiver);
        } catch (Exception ignore) {
            Log.e(TAG, ignore.toString());
        } 
        unbindService(mServiceConnection);
        mService.stopSelf();
        mService= null;
       
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "onStop");
        super.onStop();
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause");
        super.onPause();
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart");
    }

    @Override
    public void onResume() {
        super.onResume();
        
        Log.d(TAG, "onResume");
        if(mBtAdapter!=null){
        	if (!mBtAdapter.isEnabled()) {
            	Log.i(TAG, "onResume - BT not enabled yet");
            	Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            	startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        	}
        }
 
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {

        case REQUEST_SELECT_DEVICE:
        	//When the DeviceListActivity return, with the selected device address
            if (resultCode == Activity.RESULT_OK && data != null) {
                String deviceAddress = data.getStringExtra(BluetoothDevice.EXTRA_DEVICE);
                mDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(deviceAddress);
               
                Log.d(TAG, "... onActivityResultdevice.address==" + mDevice + "mserviceValue" + mService);
                ((TextView) findViewById(R.id.deviceName)).setText(mDevice.getName()+ " - connecting");
                mService.connect(deviceAddress);
                            

            }
            break;
        case REQUEST_ENABLE_BT:
            // When the request to enable Bluetooth returns
            if (resultCode == Activity.RESULT_OK) {
                Toast.makeText(this, "Bluetooth has turned on ", Toast.LENGTH_SHORT).show();

            } else {
                // User did not enable Bluetooth or an error occurred
                Log.d(TAG, "BT not enabled");
                Toast.makeText(this, "Problem in BT Turning ON ", Toast.LENGTH_SHORT).show();
                finish();
            }
            break;
        default:
            Log.e(TAG, "wrong request code");
            break;
        }
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
       
    }

    
    private void showMessage(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
  
    }
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
        	onBackPressed();
        }
        return super.onKeyDown(keyCode, event);
    }
    @Override
    public void onBackPressed() {
        if (mState == UART_PROFILE_CONNECTED) {
            Intent startMain = new Intent(Intent.ACTION_MAIN);
            startMain.addCategory(Intent.CATEGORY_HOME);
            startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(startMain);
            showMessage("nRFUART's running in background.\n             Disconnect to exit");
        }
        else {
            new AlertDialog.Builder(this)
            .setIcon(android.R.drawable.ic_dialog_alert)
            .setTitle(R.string.popup_title)
            .setMessage(R.string.popup_message)
            .setPositiveButton(R.string.popup_yes, new DialogInterface.OnClickListener()
                {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
   	                finish();
                }
            })
            .setNegativeButton(R.string.popup_no, null)
            .show();
        }
    }
	@Override
	public void BTSearchFindDevice(BluetoothDevice device, int rssi,
			byte[] scanRecord) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void BTSearchFindDevicestatus(boolean Scanning) {
		// TODO Auto-generated method stub
		
	}
}

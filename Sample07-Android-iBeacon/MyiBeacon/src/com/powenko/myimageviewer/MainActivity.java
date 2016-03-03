package com.powenko.myimageviewer;

import java.util.ArrayList;

import com.powenko.ifroglab_bt_lib.ifrog;
import com.powenko.ifroglab_bt_lib.ifrog.ifrogCallBack;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;  
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.PopupMenu;
import android.widget.PopupMenu.OnMenuItemClickListener;

import android.widget.Toast;

@SuppressLint("NewApi")
public class MainActivity extends ActionBarActivity implements ifrogCallBack {
 
	private ImageButton imageButton1;
//	private EditText editText1;
	private ListView listView1;
	

	String[] testValues= new String[]{	"Apple","Banana","Orange","Tangerine"};
	String[] testValues2= new String[]{	"Red","Yello","Orange","Yello"};

	private row adapter;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
	//	editText1=(EditText) findViewById(R.id.editText1 );          //取得imageView
		imageButton1=(ImageButton) findViewById(R.id.imageButton1 ); //取得imageView
		
		listView1=(ListView) findViewById(R.id.listView1);   //取得listView1
		//ListAdapter adapter = createAdapter(); 
		
		BTinit();
	}
	private void SetupList(){
		adapter=new row(this,testValues,testValues2);
		listView1.setAdapter(adapter);
		listView1.setOnItemClickListener(new OnItemClickListener(){      //選項按下反應
                                                                         // Options Press Reaction
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				  String item = testValues[position];      //哪一個列表
                                                           // Which list
                
			      Toast.makeText(MainActivity.this, item + " selected", Toast.LENGTH_LONG).show();       //顯示訊號
                                                                                                         // Display signal   
			}
		} );
		
	}
	private ifrog  mifrog;
	public void BTinit(){
		    mifrog=new ifrog();
	        mifrog.setTheListener(this);
	        // Use this check to determine whether BLE is supported on the device.  Then you can
	        // selectively disable BLE-related features.
	        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
	            Toast.makeText(this,"this Device doean't support Bluetooth BLE", Toast.LENGTH_SHORT).show();
	            finish();
	        }
	        final BluetoothManager bluetoothManager =
	                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
	        if (mifrog.InitCheckBT(bluetoothManager) == null) {
	            Toast.makeText(this,"this Device doean't support Bluetooth BLE", Toast.LENGTH_SHORT).show();
	            finish();
	            return;
	        }
    		mifrog.scanLeDevice(true,100000);
	}
	
		
	

		ArrayList<String> Names = new ArrayList<String>();
		ArrayList<String> Address = new ArrayList<String>();
		
		
	@Override
	public void BTSearchFindDevice(BluetoothDevice device, int rssi,byte[] scanRecord) {
		String t_address= device.getAddress();
		int index=0;
		boolean t_NewDevice=true; 
		for(int i=0;i<Address.size();i++){
			String t_Address2=Address.get(i);
			if(t_Address2.compareTo(t_address)==0){
				t_NewDevice=false;
				index=i;
				break;
			}
		}
		if(t_NewDevice==true){
			Address.add(t_address);
			Names.add(device.getName()+" RSSI="+Integer.toString(rssi));
	    	testValues = Names.toArray(new String[Names.size()]);
	    	testValues2 =Address.toArray(new String[Address.size()]);  
		}else{
			Names.set(index,device.getName()+" RSSI="+Integer.toString(rssi));
	    	testValues = Names.toArray(new String[Names.size()]);
		}
		SetupList();		
	}

	@Override
	public void BTSearchFindDevicestatus(boolean arg0) {
		if(arg0==false){
			Toast.makeText(getBaseContext(),"Stop Search", 2000).show(); 
		}else{
			Toast.makeText(getBaseContext(),"Start Search", 2000).show(); 
		}
	}
	
    @Override
    protected void onDestroy() {
        super.onDestroy();
        mifrog.BTSearchStop();
    }
    
 
}

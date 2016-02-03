package com.powenko.myimageviewer;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;

public class page2  extends ActionBarActivity {
 
     private Button button1;
     @Override
     protected void onCreate(Bundle savedInstanceState) {
          super.onCreate(savedInstanceState);
          setContentView(R.layout.page2layout );          //顯示畫面資料
                                                          // Display information
         
          Bundle bundle= this.getIntent().getExtras();    // 取得第一頁傳遞過來的資料。
                                                          // Get the first page of data passed.
         
         int t_value=bundle.getInt("intvlaue");
         String t_web=bundle.getString("Web");
         String t_data=bundle.getString("data");
         TextView textView1=(TextView) findViewById(R.id.textView1 );   //取得TextView
         textView1.setText(t_data);
         button1=(Button) findViewById(R.id.button1  );          //取得button1
         button1.setOnClickListener(new OnClickListener(){       // 監聽按鍵動作
                                                                 // Monitor keystrokes
               @Override
               public void onClick(View v) {
            	   FunBack();
               }}
          );
     }
     
     @Override                                              //用戶按下android 機器上的back 按鍵。
                                                            // Android user presses the back button on the machine.
    
     public boolean onKeyDown(int keyCode, KeyEvent event) {    
         if (keyCode == KeyEvent.KEYCODE_BACK) {   //查用戶按下back 按鍵 ?
                                                   // Check the user presses the back button?
             
         	 FunBack();
             return true;
         }
         return super.onKeyDown(keyCode, event);     // 如果其他按鍵交給android
                                                     // If the other keys to android
         
     }
     
     private void FunBack(){
    	    CheckBox Apple = (CheckBox) findViewById(R.id.checkBox1);  //取得選項元件
                                                                        // Get the option element)
            CheckBox Orange = (CheckBox) findViewById(R.id.checkBox2);
            CheckBox Watermelon = (CheckBox) findViewById(R.id.checkBox3);
            
            Intent intent=new Intent();              //準備將資料傳遞回去
                                                     // Ready to transfer data back
         
            Bundle bundle=new Bundle();
            bundle.putBoolean("Apple", Apple.isChecked());
            bundle.putBoolean("Orange", Orange.isChecked());
            bundle.putBoolean("Watermelon", Watermelon.isChecked());
            bundle.putString("MESSAGE", "hi!"); 
            intent.putExtras(bundle);
            setResult(RESULT_OK,intent);                // 傳遞資料回上一頁
                                                         // Transfer data Back
         
            finish(); 
     }
     
}

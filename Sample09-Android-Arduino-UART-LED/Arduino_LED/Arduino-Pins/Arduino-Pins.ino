// 柯博文老師 www.powenko.com
const int ledPin = 13;                                      // 設定 LED 的接腳
int incomingByte;                                          //讀取進來的資料存放位置。
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX


void setup() {
   
  Serial.begin(9600);                                   //設定傳輸速度
                                                                   // 設定LED 燈的接腳
  pinMode(ledPin, OUTPUT);
  
    // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
  
}

void loop() {
                                                                    // 看看是否有資料進來？
  if (mySerial.available() > 0) {
                                                                      // 讀取資料
    incomingByte = mySerial.read();
    Serial.println(incomingByte);
                                                                      //如果資料是'H'的話，那就把LED 燈打開
    if (incomingByte == 'H') {
      digitalWrite(ledPin, HIGH);
    } 
                                                                    //如果資料是'L'的話，那就把LED 燈關閉
    if (incomingByte == 'L') {
      digitalWrite(ledPin, LOW);
    }
  }
}




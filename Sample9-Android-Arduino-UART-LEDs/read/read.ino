// 柯博文老師 www.powenko.com

#include <SoftwareSerial.h>
int ledPin = 10;

SoftwareSerial mySerial(10, 11); // RX, TX
String readString;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); 
  Serial.println("serial on/off test 0021"); // so I can keep track

  mySerial.begin(9600);
  mySerial.println("Hello, world?");

  
  
}

void loop() {

  while (mySerial.available()) {
    delay(3);  
    char c = mySerial.read();
    readString += c; 
  }
  readString.trim();
  if (readString.length() >0) {
    if (readString == "m3 on"){
      mySerial.println("switching on");
      digitalWrite(ledPin, HIGH);
    }
    if (readString == "m3 off")
    {
      mySerial.println("switching off");
      Serial.println("switching off");
      digitalWrite(ledPin, LOW);
    }

    readString="";
  } 
}


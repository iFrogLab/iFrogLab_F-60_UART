
//
//    FILE: dht11_test.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.01
// PURPOSE: DHT library test sketch for DHT11 && Arduino
//     URL:
//
//  updated
// AUTHOR: powen Ko (www.ifroglab.com)

#include <dht.h>
#include <SoftwareSerial.h>
dht DHT;

#define DHT11_PIN 5
SoftwareSerial mySerial(10, 11); // RX, TX

void setup()
{
  Serial.begin(9600);
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.print("Hello, world?");
  
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
  
}

void loop()
{
  // READ DATA
  Serial.print("DHT11, \t");
  mySerial.write("DHT11, \n");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                mySerial.write("OK,\n"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                mySerial.write("Checksum error,\n"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                mySerial.write("Time out error,\n"); 
                break;
    case DHTLIB_ERROR_CONNECT:
               Serial.print("Connect error,\t");
               mySerial.write("Connect error,\n");
               break;
    case DHTLIB_ERROR_ACK_L:
               Serial.print("Ack Low error,\t");
               mySerial.write("Ack Low error,\n");
               break;
    case DHTLIB_ERROR_ACK_H:
               Serial.print("Ack High error,\t");
               mySerial.write("Ack High error\n");
               break;
    default: 
               Serial.print("Unknown error,\t"); 
               mySerial.write("Unknown error,\n"); 
               break;
  }
  // DISPLAY DATA
  int humidity=DHT.humidity;
  Serial.print(humidity, 1);
  String t1=String(humidity);
  mySerial.print(t1);
  Serial.print(",");
  mySerial.print(",");
  int temperature=DHT.temperature;
  Serial.println(temperature, 1);
  String t2=String(temperature)+"\n";
  mySerial.println(t2);
  delay(2000);
}
//
// END OF FILE
//




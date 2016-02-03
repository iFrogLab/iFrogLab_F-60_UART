

import java.util.Date;


import com.pi4j.io.gpio.GpioController;
import com.pi4j.io.gpio.GpioFactory;
import com.pi4j.io.gpio.GpioPinDigitalOutput;
import com.pi4j.io.gpio.PinState;
import com.pi4j.io.gpio.RaspiPin;
import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.SerialPortException;


/**
 * This example code demonstrates how to perform serial communications using the Raspberry Pi.
 * this sample is design For iFrogLab F-60 Uart module,
 * @author Robert Savage
 *         Powen Ko
 *
 */
public class SerialDigitalPin  {
    
    public static void main(String args[]) throws InterruptedException {
        // create an instance of the serial communications class
        final Serial serial = SerialFactory.createInstance();
        final GpioController  gpio=GpioFactory.getInstance();
        
        final GpioPinDigitalOutput led2 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_02);
        final GpioPinDigitalOutput led3 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_03);
        final GpioPinDigitalOutput led4 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_04);
        final GpioPinDigitalOutput led5 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_05);
        final GpioPinDigitalOutput led6 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_06);
        final GpioPinDigitalOutput led7 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_07);
        final GpioPinDigitalOutput led8 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_08);
        final GpioPinDigitalOutput led9 = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_09);
        
        
        led2.high();
        led3.high();
        led4.high();
        
        // create and register the serial data listener
        serial.addListener(new SerialDataListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                String t1=event.getData();
                System.out.print(t1);
                
                if(t1.startsWith("m2 on")){
                    led2.high();
                }else if(t1.startsWith("m2 off")){
                    led2.low();
                }else if(t1.startsWith("m3 on")){
                    led3.high();
                }else if(t1.startsWith("m3 off")){
                    led3.low();
                }else if(t1.startsWith("m4 on")){
                    led4.high();
                }else if(t1.startsWith("m4 off")){
                    led4.low();
                }else if(t1.startsWith("m5 on")){
                    led5.high();
                }else if(t1.startsWith("m5 off")){
                    led5.low();
                }else if(t1.startsWith("m6 on")){
                    led6.high();
                }else if(t1.startsWith("m6 off")){
                    led6.low();
                }else if(t1.startsWith("m7 on")){
                    led7.high();
                }else if(t1.startsWith("m7 off")){
                    led7.low();
                }else if(t1.startsWith("m8 on")){
                    led8.high();
                }else if(t1.startsWith("m8 off")){
                    led8.low();
                }else if(t1.startsWith("m9 on")){
                    led9.high();
                }else if(t1.startsWith("m9 off")){
                    led9.low();
                }
            }
        });
        
        try {
            // open the default serial port provided on the GPIO header
            serial.open(Serial.DEFAULT_COM_PORT, 9600);
            // continuous loop to keep the program running until the user terminates the program
            
            while(true) {
                try {
                    // write a formatted string to the serial transmit buffer
                    // 把現在的時間，傳遞過去。
                    // The time now, passed in the past.
                    
                    serial.write("CURRENT TIME: %s", new Date().toString());
                    // write a individual characters to the serial transmit buffer
                    // 把特定的字元 ，傳遞過去。
                    // The specific characters, pass it.
                    
                    serial.write('\r');
                    serial.write('\n');
                }
                catch(IllegalStateException ex){
                    ex.printStackTrace();
                }
                
                // wait 1 second before continuing
                Thread.sleep(5000);
            }
            
        }
        catch(SerialPortException ex) {
            System.out.println(" ==>> SERIAL SETUP FAILED : " + ex.getMessage());
            return;
        }
    }
}

// END SNIPPET: serial-snippet


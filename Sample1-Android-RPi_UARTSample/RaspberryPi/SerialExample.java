

import java.util.Date;

import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.SerialPortException;

/**
 * This example code demonstrates how to perform serial communications using the Raspberry Pi.
 *
 * @author Robert Savage
 */
public class SerialExample {
    
    public static void main(String args[]) throws InterruptedException {

        System.out.println("<--Pi4J--> Serial Communication Example ... started.");
        System.out.println(" ... connect using settings: 9600, N, 8, 1.");
        System.out.println(" ... data received on serial port should be displayed below.");
        
        // create an instance of the serial communications class
        final Serial serial = SerialFactory.createInstance();
        
        // create and register the serial data listener
        serial.addListener(new SerialDataListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                // print out the data received to the console
                
                System.out.print(event.getData());
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
                    serial.write("CURRENT TIME: %s", new Date().toString());
                    // 把特定的字元 ，傳遞過去。
                    // The specific characters, pass it.
                    
                    serial.write('\r');
                    serial.write('\n');
                    

                    
                    // 把字串傳遞過去，並且做跳行的動作（ CR+LF ）
                    // serial.writeln("Third Line");
                }
                catch(IllegalStateException ex){
                    ex.printStackTrace();
                }
                
                // wait 1 second before continuing
                Thread.sleep(1000);
            }
            
        }
        catch(SerialPortException ex) {
            System.out.println(" ==>> SERIAL SETUP FAILED : " + ex.getMessage());
            return;
        }
    }
}

// END SNIPPET: serial-snippet


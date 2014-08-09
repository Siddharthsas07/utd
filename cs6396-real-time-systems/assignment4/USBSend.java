import lejos.nxt.*;
import lejos.nxt.comm.*;
import java.io.*;
import java.net.*;

/**
 * This is a test of the UltraSonic sensor. If an object is detected at 
 * a certain distance, the sensor will transmit this data to output stream
 * 
 * @author Rutvij 
 *
 */
public class USBSend {
 UltrasonicSensor sonic = new UltrasonicSensor(SensorPort.S1);
USBConnection conn ;
		
		public static void main(String [] args) throws Exception 
	{ 
	     new USBSend();
		 }
		 
		 public USBSend(){
		 
		LCD.drawString("waiting", 0, 0);
		try{conn = USB.waitForConnection();
		}catch(Exception e){
		LCD.clear();
		LCD.drawString("error" + e, 0, 0);
		}
		LCD.drawString("connected", 0, 0);
		DataOutputStream dOut = conn.openDataOutputStream();
		int d=0;
		
		while(!Button.ESCAPE.isPressed()) {
			LCD.clear();
			LCD.drawString(sonic.getVersion(), 0, 0);
			LCD.drawString(sonic.getProductID(), 0, 1);
			LCD.drawString(sonic.getSensorType(), 0, 2);
			LCD.drawInt(d=sonic.getDistance(), 0, 3);
			try{
			Thread.sleep(200);
			
			dOut.writeInt(d);
			dOut.flush();
	        
			}
			catch(InterruptedException e){
					System.out.println("Interrupt");
					break;
				}
			catch(EOFException e){
					System.out.println("End of file");
					break;
				} catch(IOException e){
					System.out.println("IO Exception");
					break;
				}
		}
		try{
        dOut.close();
       
        conn.close();
		}catch(IOException e){
					System.out.println("IO Exception");
					
				}
		
	}
	
	

	

	
}

import java.io.DataInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import lejos.pc.comm.NXTCommLogListener;
import lejos.pc.comm.NXTConnector;
 
/**
 * This is a PC sample. It connects to the NXT, and then
 * sends an integer and waits for a reply, 100 times.
 * 
 * Compile this program with javac (not nxjc), and run it 
 * with java.
 * 
 * You need pccomm.jar on the CLASSPATH and the jfantom.dll
 * DLL or liblibnxt.so shared library on the Java library path.
 * 
 * Run the program by:
 * 
 *   java USBSend
 * 
 * Your NXT should be running a sample such as USBReceive. 
 * 
 * @author Lawrie Griffiths
 *
 */
public class USBRead {	
	
	public static void main(String[] args) {
		new USBRead();
	}
	
	public USBRead(){
		NXTConnector conn = new NXTConnector();
		
		conn.addLogListener(new NXTCommLogListener(){

			public void logEvent(String message) {
				System.out.println("USBSend Log.listener: "+message);
				
			}

			public void logEvent(Throwable throwable) {
				System.out.println("USBSend Log.listener - stack trace: ");
				 throwable.printStackTrace();
				
			}
			
		} 
		);
		
		if (!conn.connectTo("usb://")){
			System.err.println("No NXT found using USB");
			System.exit(1);
		}
		 Socket commandSocket;
		 DataInputStream inDat = conn.getDataIn();
		 OutputStream outDat;
		 while(true) {
	        	try {
	        		commandSocket = new Socket(InetAddress.getByName("192.168.94.2"), 5020);
	        		outDat = commandSocket.getOutputStream();
	        		break;
	        	} catch (UnknownHostException e) {
	        		System.err.println("Please startup the board....");
	        		try {
	        			Thread.sleep(3000);
	        		} catch (InterruptedException ioe) {}
	        	} catch (IOException e) {
	        		System.err.println("Please startup the program on the board....");
	        		try {
	        			Thread.sleep(3000);
	        		} catch (InterruptedException ioe) {}
	        	}
	        }
	        
		
		int x = 0;
		while(true)
		{ 
			
			try {
	        	 x = inDat.readInt();
	        	 outDat.write(x);
	 		 outDat.flush();

	        } catch (IOException ioe) {
	           System.err.println("IO Exception reading reply");
	        }            
	        System.out.println(" Received " + x);
	        
		}

	}
}

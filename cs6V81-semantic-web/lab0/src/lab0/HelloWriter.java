package lab0;

import java.io.*;

public class HelloWriter
{
  private String message = "Hello, World";
  private String outputLocation = "myhello.txt";
  
  public HelloWriter()
  {
    
  }
  
  public void writeToScreen()
  {
    System.out.println( message );
  }
  
  public void writeToFile()
  {
    try
    {
      BufferedWriter output = new BufferedWriter( new FileWriter( outputLocation ) );
      
      output.write( message );
      output.newLine( );
      
      output.close( );
    }
    
    catch ( IOException e )
    {
      System.out.println( "Exception: " );
      System.out.println( e.toString( ) );
    }
  }  
}

package lab0;

public class Lab0 
{
	public static void main(String[] args) 
	{
		HelloWriter writer = new HelloWriter( );
		
		Color blue = new Color( 0, 0, 255 );
		
		System.out.println( blue.get_blue( ) );
		
		Color rebecca = new Color( "Rebecca" );
		
		System.out.println( rebecca.get_name( ) );		
		
		writer.writeToScreen( );
		
		writer.writeToFile( );
	}
}

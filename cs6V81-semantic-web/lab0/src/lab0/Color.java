package lab0;

public class Color 
{
  private String name;
  private int[] rgb_value = new int[3];  
  
	public Color( int red, int green, int blue ) 
	{
	  rgb_value[0] = red;
	  rgb_value[1] = green;
	  rgb_value[2] = blue;
    
    System.out.println( "Color (" + rgb_value[0] + ", " + rgb_value[1] + ", " + rgb_value[2] + ")" );
	}
	
	public Color( String name ) 
  {
	  this.name = name;
    
    System.out.println( "Color " + name );    
  }

  public int get_blue()
  {
    return rgb_value[2];    
  }	
  
  public String get_name()
  {
    return name;
  }
}

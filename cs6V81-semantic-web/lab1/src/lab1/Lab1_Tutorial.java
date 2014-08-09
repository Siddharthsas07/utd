package lab1;

import com.hp.hpl.jena.rdf.model.*;
import com.hp.hpl.jena.vocabulary.*;

import java.io.*;

public class Lab1_Tutorial 
{
	public static void main(String[] args) 
	{		
		String nsUTD = "http://utdallas/semclass#";
		String seidaURI = nsUTD + "stevenSeida";
		String fullName = "Steven Seida";
		String fName = "Steven";
		String lName = "Seida";
		String birthday = "January 1, 1901";
		String email = "steven.seida@utdallas.edu";
		String title = "Senior Lecturer";
		
		Model model = ModelFactory.createDefaultModel();	
		
		model.setNsPrefix( "nsUTD", nsUTD );
		
		//Resource stevenSeida
			//= model.createResource( seidaURI )
			model.createResource( seidaURI ) 
				.addProperty(VCARD.FN, fullName)
				.addProperty(VCARD.EMAIL, email)
				.addProperty(VCARD.BDAY, birthday )
				.addProperty(VCARD.TITLE, title)
				.addProperty(VCARD.N,
						model.createResource()
							.addProperty(VCARD.Given, fName)
							.addProperty(VCARD.Family, lName)
						);
		
		try 
		{
			BufferedWriter outXML = new BufferedWriter( new FileWriter( "Lab1_1_gSteelman.xml" ) );
			BufferedWriter outXMLClean = new BufferedWriter( new FileWriter( "Lab1_1_gSteelman-Clean.xml" ) );
			BufferedWriter outN_TRIPLE = new BufferedWriter( new FileWriter( "Lab1_1_gSteelman.ntp" ) );
			BufferedWriter outN3 = new BufferedWriter( new FileWriter( "Lab1_1_gSteelman.n3" ) );			
			
			model.write( outXML, "RDF/XML");
			model.write( outXMLClean, "RDF/XML-ABBREV");
			model.write( outN_TRIPLE, "N-TRIPLE");
			model.write( outN3, "N3");			
			
			outXML.close();
			outXMLClean.close();
			outN_TRIPLE.close();
			outN3.close();			
		} 
		
		catch (IOException e) 
		{
			System.out.println( "Exception: " );
			System.out.println( e.toString( ) );
		}
	}
}

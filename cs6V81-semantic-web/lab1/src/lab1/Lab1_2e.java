/*
 * Lab1_2e creates multiple Jena Model instances with various ways of populating
 * the data inside the model.
 *  
 * @version
 * 1.3 9/28/2011
 * 
 * @author
 * Gary Steelman
 */

package lab1;

import com.hp.hpl.jena.rdf.model.*; 
import com.hp.hpl.jena.sdb.SDBFactory;
import com.hp.hpl.jena.sdb.Store;
import com.hp.hpl.jena.vocabulary.*;
import java.io.*;

/*
 * Lab1_2e creates multiple Jena Model instances with various ways of populating
 * the data inside the model.
 */
public class Lab1_2e {
	public static void main(String[] args) {		
		// Local variables for populating the Models
		String nsUTD    = "http://utdallas/semclass#";
		String seidaURI = nsUTD + "stevenSeida";
		String fullName = "Steven Seida";
		String fName    = "Steven";
		String lName    = "Seida";
		String birthday = "January 1, 1901";
		String email    = "steven.seida@utdallas.edu";
		String title    = "Senior Lecturer";
		
		//-------------------------------
		// PART 1
		//-------------------------------
		Model inMemoryModel = ModelFactory.createDefaultModel();			
		inMemoryModel.setNsPrefix( "nsUTD", nsUTD );
		
		// Populate the in-memory Model with the proper data
		inMemoryModel.createResource( seidaURI ) 
			.addProperty(VCARD.FN, fullName)
			.addProperty(VCARD.EMAIL, email)
			.addProperty(VCARD.BDAY, birthday )
			.addProperty(VCARD.TITLE, title)
			.addProperty(VCARD.N,
					inMemoryModel.createResource()
						.addProperty(VCARD.Given, fName)
						.addProperty(VCARD.Family, lName)
					);
		/*
		 * Commented out because the instructions request only the named SDB
		 * model's output.
		 */
		//print_results("1", inMemoryModel);
		
		//-------------------------------
		// PART 2-1
		//-------------------------------
		System.setProperty("jena.db.user", "jenasdb");
		System.setProperty("jena.db.password", "fastdb");
		
		// Connect to the SDB database using an unnamed Model
		Store store = SDBFactory.connectStore("sdb.ttl");
		Model unnamedSDBModel = SDBFactory.connectDefaultModel(store);
		
		// Clear the database of all previous information
		unnamedSDBModel.removeAll();		
		unnamedSDBModel.setNsPrefix( "nsUTD", nsUTD);
		
		unnamedSDBModel.createResource( seidaURI ) 
			.addProperty(VCARD.FN, fullName)
			.addProperty(VCARD.EMAIL, email)
			.addProperty(VCARD.BDAY, birthday )
			.addProperty(VCARD.TITLE, title)
			.addProperty(VCARD.N,
					unnamedSDBModel.createResource()
						.addProperty(VCARD.Given, fName)
						.addProperty(VCARD.Family, lName)
					);

		/*
		 * Commented out because the instructions request only the named SDB
		 * model's output.
		 */
		//print_results("2", unnamedSDBModel);
		
		unnamedSDBModel.close();	
		
		//-------------------------------
		// PART 2-2
		//-------------------------------	
		// Connect to the SDB database using a named Model
		ModelMaker modelMaker = ModelFactory.createMemModelMaker();		
		modelMaker.createModel("myrdf");
		Model namedSDBModel = SDBFactory.connectNamedModel(store, "myrdf");
		
		// Clear the database of all previous information
		namedSDBModel.removeAll();	
		namedSDBModel.setNsPrefix( "nsUTD", nsUTD);
		
		namedSDBModel.createResource( seidaURI ) 
			.addProperty(VCARD.FN, fullName)
			.addProperty(VCARD.EMAIL, email)
			.addProperty(VCARD.BDAY, birthday )
			.addProperty(VCARD.TITLE, title)
			.addProperty(VCARD.N,
					namedSDBModel.createResource()
						.addProperty(VCARD.Given, fName)
						.addProperty(VCARD.Family, lName)
					);
		
		print_results("2", namedSDBModel);
		
		namedSDBModel.close();			
	}
	
	/*
	 * Prints the contents of model to the output files for this program.
	 * 
	 * @param number -- The section of the lab being printed 
	 * 
	 * @param model -- The model to output the contents of
	 * 
	 * @see Model
	 */
	static void print_results(String number, Model model) {
		try {
			BufferedWriter outXML = new BufferedWriter( 
					new FileWriter( "Lab1_" + number + "_gSteelman.xml" ) 
					);
			model.write( outXML, "RDF/XML");
			outXML.close();
			
			
			// Commented out because the instructions do not request this file format
			/*
			BufferedWriter outXMLClean = new BufferedWriter( 
					new FileWriter( "Lab1_" + number + "_gSteelman-Clean.xml" ) 
					);
			model.write( outXMLClean, "RDF/XML-ABBREV");
			outXMLClean.close();
			*/			
			
			BufferedWriter outN_TRIPLE = new BufferedWriter( 
					new FileWriter( "Lab1_" + number + "_gSteelman.ntp" ) 
					);
			model.write( outN_TRIPLE, "N-TRIPLE");
			outN_TRIPLE.close();
			
			BufferedWriter outN3 = new BufferedWriter( 
					new FileWriter( "Lab1_" + number + "_gSteelman.n3" ) 
					);			
			model.write( outN3, "N3");			
			outN3.close();			
			
		} catch (IOException e) {			
			System.out.println( "Exception: " );
			System.out.println( e.toString( ) );
		}
	}
}

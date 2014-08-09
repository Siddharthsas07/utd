/*
 * Lab2_3 creates multiple models the relations between some books, movies,
 * authors, and directors using a custom vocabulary combined with dublin core
 * and vcard vocabularies.
 *  
 * @version
 * 1.2 10/11/2011
 * 
 * @author
 * Gary Steelman
 */

package lab2;

import com.hp.hpl.jena.rdf.model.*; 
import com.hp.hpl.jena.sdb.SDBFactory;
import com.hp.hpl.jena.sdb.Store;
import com.hp.hpl.jena.vocabulary.*;
import java.io.*;

/*
 * Lab2_3 creates multiple models the relations between some books, movies,
 * authors, and directors using a custom vocabulary combined with dublin core
 * and vcard vocabularies.
 */
public class Lab2_2 {
	public static void main(String[] args) {		
		String xsd       = "http://www.w3.org/2001/XMLSchema#";
		String rdf       = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
		String rdfs      = "http://www.w3.org/2000/01/rdf-schema#";
		String nsUTD     = "http://utdallas/semclass#";
		String nsBase    = "http://utdallas/semclass";
		String nsMovie   = nsBase + "/movie#";
		String nsPerson  = nsBase + "/person#";
		String nsBook    = nsBase + "/book#";;
		
		String kubrickURI    = nsPerson + "StanleyKubrick";
		String kubrickGiven  = "Stanley";
		String kubrickFamily = "Kubrick";
		
		String georgeURI    = nsPerson + "peterGeorge";		
		String georgeGiven  = "Peter";
		String georgeFamily = "George";	
		
		String clockworkURI   = nsMovie + "clockworkOrange";
		String clockworkTitle = "A Clockwork Orange";
		String clockworkYear  = "1971";
		String clockworkGenre = "Crime, Drama, Sci-Fi";
		
		String strangeloveURI   = nsMovie + "drStrangelove";
		String strangeloveTitle = "Dr. Strangelove or: How I Learned to Stop Worrying and Love the Bomb";
		String strangeloveYear  = "1964";
		String strangeloveGenre = "Comedy, Drama";
		
		String redAlertURI    = nsBook + "redAlert";
		String redAlertTitle  = "Red Alert";
		String redAlertYear   = "1958";			
		
		//-------------------------------
		// PART 3
		//-------------------------------
		// System.setProperty("jena.db.user", "jenasdb");
		// System.setProperty("jena.db.password", "fastdb");
		
		// Connect to the SDB database using an unnamed Model
		Store store = SDBFactory.connectStore("sdb.ttl");
		
		// Clear the database of any previous data and ensure the correct tables
		// exist for population
		store.getTableFormatter().truncate();
		store.getTableFormatter().create();
		
		// Create a model for interfacing with the database
		Model model = SDBFactory.connectDefaultModel(store);		
		
		// Namespace prefixes			
		model.setNsPrefix("xsd", xsd);
		model.setNsPrefix("rdf", rdf);
		model.setNsPrefix("rdfs", rdfs);
		model.setNsPrefix("xsd", xsd);
		model.setNsPrefix("base", nsUTD);
		model.setNsPrefix("utd", nsUTD);		
		model.setNsPrefix("movie", nsMovie);
		model.setNsPrefix("person", nsPerson);
		model.setNsPrefix("book", nsBook);		
		
		// Create the class hierarchy
		Resource movie  = model.createResource(nsMovie);
		Resource person = model.createResource(nsPerson);
		Resource book   = model.createResource(nsBook);
		 
		// Create the different class instances required
		Resource stanleyKubrick  = model.createResource(kubrickURI);
		Resource clockworkOrange = model.createResource(clockworkURI);
		Resource drStrangelove   = model.createResource(strangeloveURI);
		Resource peterGeorge     = model.createResource(georgeURI);
		Resource redAlert        = model.createResource(redAlertURI);
				
		// Create the custom properties for the classes		
		Property movieGenreProperty    = model.createProperty(nsMovie, "genre");
		Property movieDirectorProperty = model.createProperty(nsMovie, "director");				
		Property originalWorkProperty  = model.createProperty(nsMovie, "originalWork");
		
		// Create people
		stanleyKubrick.addProperty(RDF.type, person);
		stanleyKubrick.addProperty(VCARD.Given, kubrickGiven);
		stanleyKubrick.addProperty(VCARD.Family, kubrickFamily);
		
		peterGeorge.addProperty(RDF.type, person);
		peterGeorge.addProperty(VCARD.Given, georgeGiven);
		peterGeorge.addProperty(VCARD.Family, georgeFamily);
		
		// Create movies
		clockworkOrange.addProperty(RDF.type, movie);
		clockworkOrange.addProperty(DC.title, clockworkTitle);
		clockworkOrange.addProperty(DC.date, clockworkYear);
		clockworkOrange.addProperty(movieGenreProperty, clockworkGenre);
		clockworkOrange.addProperty(movieDirectorProperty, stanleyKubrick);
		clockworkOrange.addProperty(originalWorkProperty, redAlert);
		
		drStrangelove.addProperty(RDF.type, movie);
		drStrangelove.addProperty(DC.title, strangeloveTitle);
		drStrangelove.addProperty(DC.date, strangeloveYear);
		drStrangelove.addProperty(movieGenreProperty, strangeloveGenre);	
		drStrangelove.addProperty(movieDirectorProperty, stanleyKubrick);
		drStrangelove.addProperty(originalWorkProperty, redAlert);
		
		// Create books		
		redAlert.addProperty(RDF.type, book);
		redAlert.addProperty(DC.creator, peterGeorge);
		redAlert.addProperty(DC.title, redAlertTitle);
		redAlert.addProperty(DC.date, redAlertYear);		
		
		// Output results
		print_results("3", model);
		
		// Close the model to save changes
		model.close();
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
					new FileWriter("Lab2_" + number + "_gSteelman.xml") 
					);
			model.write(outXML, "RDF/XML");
			outXML.close();
			
			
			// Commented out because the instructions do not request this file format
			/*
			BufferedWriter outXMLClean = new BufferedWriter( 
					new FileWriter( "Lab2_" + number + "_gSteelman-Clean.xml" ) 
					);
			model.write( outXMLClean, "RDF/XML-ABBREV");
			outXMLClean.close();
			*/			
			
			// Commented out because the instructions do not request this file format
			/*
			BufferedWriter outN_TRIPLE = new BufferedWriter( 
					new FileWriter("Lab2_" + number + "_gSteelman.ntp") 
					);
			model.write(outN_TRIPLE, "N-TRIPLE");
			outN_TRIPLE.close();
			*/
			
			BufferedWriter outN3 = new BufferedWriter( 
					new FileWriter("Lab2_" + number + "_gSteelman.n3") 
					);			
			model.write(outN3, "N3");			
			outN3.close();			
			
		} catch (IOException e) {			
			System.out.println("Exception: ");
			System.out.println(e.toString());
		}
	}
}

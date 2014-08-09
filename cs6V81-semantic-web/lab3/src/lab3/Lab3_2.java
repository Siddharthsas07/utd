/*
 * Lab3_2 Loads an rdf file with triples in it into the default SDB model
 * backed by a MySQL database. Then queries the database and outputs the results
 * to file.
 *  
 * @version
 * 1.0 10/25/2011
 * 
 * @author
 * Gary Steelman
 */

package lab3;

import com.hp.hpl.jena.query.*;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.rdf.model.*; 
import com.hp.hpl.jena.sdb.SDBFactory;
import com.hp.hpl.jena.sdb.Store;

import java.io.*;

/*
 * @class
 * Lab3_2 Loads an rdf file with triples in it into the default SDB model
 * backed by a MySQL database. Then queries the database and outputs the results
 * to file.
 */
public class Lab3_2 {
	public static void main(String[] args) {
		try {
			String montyLoc = "75b_Lab3-monterey.rdf";
			String storeLoc = "sdb.ttl";
			String outLoc   = "Lab3_2_gSteelman.xml";
			
			Store store = SDBFactory.connectStore(storeLoc);
	        
	        // To erase the existing data from the tables 
	        store.getTableFormatter().create();
	        
	        // Create default model 
	        Model model = SDBFactory.connectDefaultModel(store);
	        
	        // Time the read of the input file
			long startTime = System.currentTimeMillis();
			
	        model.read(new FileInputStream(montyLoc), null);
	        
	        long endTime = System.currentTimeMillis();
	        
	        double readTime = Math.round((endTime - startTime)/1000.0*10.0)/10.0;
	        
			System.out.println("Load of " + montyLoc + " took " + readTime + " sec.");
			
			// Create a query 
			String queryString = "SELECT ?p ?o WHERE { <urn:monterey:#incident1> ?p ?o . }";
			Query query = QueryFactory.create(queryString);
			
			// Execute the query
			QueryExecution qexec = QueryExecutionFactory.create(query, model);
			ResultSet rs = qexec.execSelect();
			
			// Output the results
			ResultSetFormatter.outputAsXML(new FileOutputStream(outLoc), rs);			
			
			// Close the model
			model.close();
			store.close();			
		}
		
		catch (Exception e)	{
			System.out.println("Exception: ");
			System.out.println(e.toString());
		}		
	}
}

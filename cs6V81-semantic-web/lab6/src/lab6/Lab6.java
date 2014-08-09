/*
 * Lab6 queries the semantic web conference data and outputs it to the screen.
 *  
 * @version
 * 0.1 12/6/2011
 * 
 * @author
 * Gary Steelman
 */

package lab6;

import com.hp.hpl.jena.query.Query;
import com.hp.hpl.jena.query.QueryExecution;
import com.hp.hpl.jena.query.QueryExecutionFactory;
import com.hp.hpl.jena.query.QueryFactory;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.query.ResultSetFormatter;
import com.hp.hpl.jena.rdf.model.*; 
import com.hp.hpl.jena.vocabulary.*;
import java.io.*;

/*
 * Lab1_2e creates multiple Jena Model instances with various ways of populating
 * the data inside the model.
 */
public class Lab6 {
	public static void main(String[] args) throws IOException {		
		//-------------------------------
		// PART 1
		//-------------------------------
		Model model = ModelFactory.createDefaultModel();			
		
		model.read(new FileInputStream("C7b_Lab6_eswc-2008-complete_modified.rdf"), null);
		model.read(new FileInputStream("C7c_Lab6_eswc-2009-complete_modified.rdf"), null);
		
		// Create a query 
		String queryString = "PREFIX foaf: <http://xmlns.com/foaf/0.1/> PREFIX dc: <http://purl.org/dc/elements/1.1/> SELECT DISTINCT ?s ?name ?papertitle ?mbox WHERE { { ?s a foaf:Person. ?s foaf:name ?name. ?paper dc:creator ?s. ?paper dc:title ?papertitle. ?s foaf:mbox_sha1sum ?mbox. FILTER regex(str(?s), \"person2009\") }  UNION { ?s a foaf:Person. ?s foaf:name ?name. ?paper dc:creator ?s. ?paper dc:title ?papertitle. ?s foaf:mbox_sha1sum ?mbox FILTER regex(str(?s), \"person2008\") } }";
		Query query = QueryFactory.create(queryString);
		
		// Execute the query
		QueryExecution qexec = QueryExecutionFactory.create(query, model);
		ResultSet rs = qexec.execSelect();
		
		// Output the results
		ResultSetFormatter.outputAsXML(System.out, rs);			
		
		System.out.println("End of First Listing");
		
		// Close the model
		model.close();
	}
}

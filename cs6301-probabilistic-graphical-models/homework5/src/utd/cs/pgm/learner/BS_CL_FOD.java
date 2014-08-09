package utd.cs.pgm.learner;

import java.io.FileReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Scanner;

import utd.cs.pgm.core.function.Function;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.graphmodel.GraphModelType;
import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;
import utd.cs.pgm.util.WeightedEdge;
import utd.cs.pgm.util.WeightedEdgeComparator;

public class BS_CL_FOD implements IStructureLearner {
  GraphModel learnedModel = new GraphModel();
  ExampleArrayList examples = new ExampleArrayList();
  
  public BS_CL_FOD() {}
  
  public BS_CL_FOD(String path) {
    train(path);
  }
  
  @Override
  public GraphModel getLearnedModel() {
    return this.learnedModel;
  }

  @Override
  public GraphModel train(String path) {
    try {
      FileReader fr = new FileReader(path);
      Scanner sc = new Scanner(fr);
      
      int numVariables = sc.nextInt();
      int numExamples = sc.nextInt();

      // Read training data examples, one example at a time.
      for (int i = 0; i < numExamples; i++) {
        ArrayList<Integer> example = new ArrayList<Integer>();
        
        for (int j = 0; j < numVariables; j++) {
          example.add(sc.nextInt());
        }
        
        //System.out.println("example = " + example);
        
        this.examples.add(example);
      }
      
      //System.out.println(this.examples);
      
      // All finished with the examples
      sc.close();
      fr.close();
      
      // Set the structure as type BAYES
      this.learnedModel.setType(GraphModelType.BAYES);
      
      for (int i = 0; i < numVariables; i++) {
        this.learnedModel.getVariables().add(new Variable(i, 2));
      }
      
      // Calculate the mutial information scores for every undirected pair
      // of variables. 
      // Loop over every undirected edge for pairs of variables
      ArrayList<Variable> currPair = new ArrayList<Variable>();
      LinkedList<WeightedEdge> edges = new LinkedList<WeightedEdge>();
      
      for (int i = 0; i < numVariables; i++) {
        currPair.add(new Variable(i, 2)); // domain size known to be 2
        
        //for (int j = i+1; j < numVariables; j++) {
        for (int j = 0; j < numVariables; j++) {
          if (i == j) {
            continue;
          }
          currPair.add(new Variable(j, 2)); // domain size known to be 2
          
          LogDouble MI_XU = computeMutualInformationScore(currPair);

          edges.add(new WeightedEdge(i, j, MI_XU));
          
          currPair.remove(1);
        }
        currPair.remove(0);
      }
      
      // Sort the weighted edges by weight so we can use them to create a 
      // maximum spanning tree
      Collections.sort(edges, new WeightedEdgeComparator());
      
      //System.out.println("The sorted, weighted edges:");
      //System.out.println(edges);
      
      // Create a maximum spanning tree off of the weighted edges and at the 
      // same time, create functions for the learned model based on the tree!
      // Runs based on prim's algorithm
      HashSet<Integer> visited = new HashSet<Integer>();
      HashSet<Integer> fullSet = new HashSet<Integer>();
      for (int i = 0; i < numVariables; i++) {
        fullSet.add(i);
      }
      
      // Grab v1 of the most weighty edge, as it's sure to be a useful root (right...?)
      visited.add(edges.get(0).v1); 
      Function g = new Function();
      g.variables.add(new Variable(edges.get(0).v1, 2));
      g.variables.trimToSize();
      g.table.trimToSize();
      this.learnedModel.getFunctions().add(g);
      
      while (!visited.containsAll(fullSet)) {
        /*System.out.println("visited:");
        System.out.println("[" + visited.size()+ "]" + visited);
        System.out.println("edges:");
        System.out.println("[" + edges.size()+ "]"+ edges);
        */
        for (Iterator<WeightedEdge> it = edges.iterator(); it.hasNext(); ) {
          WeightedEdge e = it.next();
          boolean cv1 = visited.contains(e.v1);
          boolean cv2 = visited.contains(e.v2);
          
          // found an edge of maximal weight to add a new destination to the visited set
          if (cv1 && !cv2) {
            visited.add(e.v2);
            
            // Add function for v1 -> v2. But note that functions are stored
            // as P(a,b) actueally meaning P(B|A). You want teh hcild variable last
            // So add v1 to the function first, then v2.
            Function f = new Function();
            f.variables.add(new Variable(e.v1, 2));
            f.variables.add(new Variable(e.v2, 2));
            f.variables.trimToSize();
            f.table.trimToSize();
            this.learnedModel.getFunctions().add(f);
            
            //System.out.println("Added function over " + f.variables);
            
            it.remove();
            break;
            
          // both vertices are already part of the tree, this edge is useless
          } else if (cv1 && cv2) {
            it.remove();
          }
        }
      }
      
      System.out.println("Learned a structure!");
      System.out.println(this.learnedModel);     
    } catch (Exception e) {
      e.printStackTrace();
    }
  
    // Return the learned structure
    return learnedModel;
  }
  
  protected LogDouble computeMutualInformationScore(ArrayList<Variable> t) throws IllegalArgumentException {    
    if (t.size() != 2) {
      System.err.println(t);
      throw new IllegalArgumentException("Mutual information score must be between two variables");
    }
    
    LogDouble MI_XU = LogDouble.LS_ZERO;
    
    int numInstantiations = t.get(0).getDomainSize()*t.get(1).getDomainSize(); 
    
    // For each instantiation of the variables, compute its mutual information score
    for (int k = 0; k < numInstantiations; k++) {
      Variable.setAddress(t, k);
      
      int jointCount = this.examples.getCountOf(t) + 1;
      
      ArrayList<Variable> v1AryLst = new ArrayList<Variable>(t);
      v1AryLst.remove(1);
      int v1Count = this.examples.getCountOf(v1AryLst) + 1;
      
      ArrayList<Variable> v2AryLst = new ArrayList<Variable>(t);
      v2AryLst.remove(0);
      int v2Count = this.examples.getCountOf(v2AryLst) + 1;
      
      double blah = jointCount * Math.log10((jointCount * this.examples.size()) / (double)(v1Count*v2Count));
      
      MI_XU = MI_XU.add(LogDouble.realToLog(blah));
    }    
    
    return MI_XU;
  }
}

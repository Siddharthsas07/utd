/**
 * 
 */
package pgm;

import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Scanner;

import lib.Lib;
import lib.LogNumber;
import lib.Stopwatch;

/**
 * @author Gary
 *
 */
public class GraphModel {
  public GraphModelType type;  
  public GraphModelEliminationAlgorithm eliminationAlgorithm;  
  public GraphModelOrderHeuristic orderHeuristic;
  
  // Variables stored by ID number. variables[0] is variable number 0, etc.
  protected ArrayList<Variable> variables = new ArrayList<Variable>();
  
  // Factors stored by pointers
  protected ArrayList<FunctionTable> functions = new ArrayList<FunctionTable>();
  
  // Evidence as <variable ID, value>
  protected HashMap<Integer,Integer> evidence = new HashMap<Integer,Integer>();
  
  // Set some defaults
  public GraphModel() {
    this.type = GraphModelType.MARKOV;
    this.eliminationAlgorithm = GraphModelEliminationAlgorithm.BUCKET_ELIMINATION;
    this.orderHeuristic = GraphModelOrderHeuristic.MIN_DEGREE;
  }
  
  // Read in file info
  public void read(String networkConfigPath, String evidencePath) throws IOException {    
    try {
      // Open file at networkConfigPath, store
      FileReader networkConfigFile = new FileReader(networkConfigPath);
      Scanner networkConfigScanner = new Scanner(networkConfigFile);
      
      // Store graph type
      if (networkConfigScanner.next().compareTo("MARKOV") == 0) {
        this.type = GraphModelType.MARKOV;
      } else {
        this.type = GraphModelType.BAYES;
      }
      
      // Read variables and their scopes
      int numVariables = networkConfigScanner.nextInt();
      for (int i = 0; i < numVariables; i++) {
        variables.add(new Variable(i, networkConfigScanner.nextInt()));
      }
      
      // Read functions and their scopes
      int numFunctions = networkConfigScanner.nextInt();
      for (int i = 0; i < numFunctions; i++) {
        FunctionTable f = new FunctionTable();
        int fScopeSize = networkConfigScanner.nextInt();        
        for (int j = 0; j < fScopeSize; j++) {
          f.variableIDs.add(networkConfigScanner.nextInt());
        }
        functions.add(f);        
      }
      
      // Read function table values
      for (FunctionTable f : functions) {
        int fTableSize = networkConfigScanner.nextInt();
        for (int i = 0; i < fTableSize; i++) {
          f.table.add(new LogNumber(networkConfigScanner.nextDouble()));
        }
        f.table.trimToSize();        
      } 

      
      // All done reading network config file
      networkConfigScanner.close();
      networkConfigFile.close();
      
      // Open file at evidencePath, store
      if (evidencePath.compareTo("") != 0) {
        FileReader evidenceFile = new FileReader(evidencePath);
        Scanner evidenceScanner = new Scanner(evidenceFile);
        
        // Read evidence and store
        int numEvidence = evidenceScanner.nextInt();
        for (int i = 0; i < numEvidence; i++) {
          evidence.put(evidenceScanner.nextInt(), evidenceScanner.nextInt());
        }
        
        // All done reading evidence file
        evidenceScanner.close();
        evidenceFile.close();
      }
    } catch(Exception e) {
      e.printStackTrace();
      throw e;
    }
  }
  
  public void print(PrintStream out) {
   if (type == GraphModelType.BAYES) {
      out.println("BAYES");
    } else {
      out.println("MARKOV");
    }
    
    out.println(variables.size());
    for (Variable v : variables) {
      out.print(v.domainSize + " ");
    }
    out.println();
    
    out.println(functions.size());
    for (FunctionTable f : functions) {
      out.print(f.variableIDs.size() + " ");
      for (Integer vID : f.variableIDs) {
        out.print(vID + " ");
      }
      out.println();
    }
    out.println();
    
    for (FunctionTable f : functions) {
      out.println(f.table.size());
      for (LogNumber tEntry : f.table) {
        out.format("%.6f ", tEntry.getValue());
      }
      out.println();
    }   
    
    out.println();
    
    if (evidence.isEmpty()) {
      out.println("No evidence supplied.");
    } else {
      out.println(evidence.size());
      for (Map.Entry<Integer, Integer> ev : evidence.entrySet()) {
        out.print(" " + ev.getKey() + " " + ev.getValue());
      }
    }
    
    out.println();
  }

  public String toString() {
    StringBuilder out = new StringBuilder();
    
    if (type == GraphModelType.BAYES) {
      out.append("BAYES\n");
    } else {
      out.append("MARKOV\n");
    }
    
    out.append(variables.size()+"\n");
    for (Variable v : variables) {
      out.append(v.domainSize + " ");
    }
    out.append("\n");
    
    out.append(functions.size() + "\n");
    for (FunctionTable f : functions) {
      out.append(f.variableIDs.size() + " ");
      for (Integer vID : f.variableIDs) {
        out.append(vID + " ");
      }
      out.append("\n");
    }
    out.append("\n");
    
    for (FunctionTable f : functions) {
      out.append(f.table.size() + "\n");
      for (LogNumber tEntry : f.table) {
        //out.append(String.format("%.6f ", tEntry.getValue()));
        out.append(String.format("%.6f ", tEntry.toReal()));
      }
      out.append("\n");
    }   
    
    out.append("\n");
    
    if (evidence.isEmpty()) {
      out.append("No evidence supplied.\n");
    } else {
      out.append(evidence.size() + "\n");
      for (Map.Entry<Integer, Integer> ev : evidence.entrySet()) {
        out.append(" " + ev.getKey() + " " + ev.getValue() + "\n");
      }
    }
    
    out.append("\n");    
    
    return out.toString();
  }
  
  public void instantiateEvidence() {
    Stopwatch stopwatch = new Stopwatch();
    stopwatch.start();
    /*System.out.println("Functions before instantiation");
    for (FunctionTable f : this.functions) {
      System.out.println(f.toRealString()); }*/
    
    boolean reNormalize = this.type == GraphModelType.BAYES ? true : false;
    for (int i = 0; i < functions.size(); i++) {
      FunctionTable nVal = functions.get(i).instantiate(evidence, variables, reNormalize);
      functions.set(i, nVal);
    }
    stopwatch.stop();
    System.out.println("Instantiated evidence: " + stopwatch.time());
    /*System.out.println("Functions after instantiation");
    for (FunctionTable f : this.functions) {
      System.out.println(f.toRealString()); }*/
  }
  
  public Double computeParitionFunctionGivenEvidence() {
    Stopwatch stopwatch = new Stopwatch();
    stopwatch.start();   
    if (this.type == GraphModelType.BAYES) {
      return 1.0;
    }
    double value = 0.0;
    switch(this.eliminationAlgorithm) {
      case VARIABLE_ELIMINATION: 
      case BUCKET_ELIMINATION:
        value = doBucketElimination();
        break;
      case JUNCTION_TREE_ELIMINATION:
        value = doJunctionTreeElimination();
        break;
      default:
        System.err.println("Unknown algorithm for partition function computation.");   
    }    
    return value;
  }
  
  public Double computeProbabilityOfEvidence() {
    System.out.println("Computing probability of evidence.");
    if (this.type != GraphModelType.BAYES) {
      System.err.println("Cannot compute probability of evidence. Network must type must be BAYES.");
      return 0.0;
    }    
    Double value = 0.0;
    switch(eliminationAlgorithm) {
      case VARIABLE_ELIMINATION: 
      case BUCKET_ELIMINATION:
        value = doBucketElimination();
        break;
      case JUNCTION_TREE_ELIMINATION:
        value = doJunctionTreeElimination();
        break;
      default:
        System.err.println("Unknown algorithm for probability of evidence computation.");    
    }
    return value;
  }
  
  private Double doJunctionTreeElimination() {
    System.err.println("Junction Tree Elimination is not yet supported.");
    return 0.0;
  }

  private Double doBucketElimination() {
    instantiateEvidence();    
    ArrayList<Integer> bucketListOrder = new ArrayList<Integer>();    
    // Create the variable ordering for elimination
    switch(orderHeuristic) {
      case MIN_DEGREE:
        bucketListOrder = computeMinDegreeOrder(this.functions);
        break;
      case MIN_FILL:
        bucketListOrder = computeMinFillOrder(this.functions);
        break;
      default:
        System.err.println("Unknown heuristic for bucket elimination.");
        return 0.0;
    }
       
    // Add a bucket for -1. Trivial functions are placed in bucket -1.
    bucketListOrder.add(-1);
    
    //System.out.println("bucket list ordering: " + bucketListOrder);
    
    /*FunctionTable f9 = new FunctionTable();
    f9.table.add(new LogNumber(0.6));
    this.functions.add(f9);*/
    
    // Create and fill buckets along ordering
    LinkedList<Bucket> bucketList = createBucketListFromOrder(bucketListOrder);   
    // Stop when reached bucket -1   
    
    /*System.out.println("The bucket list:");
    StringBuilder s = new StringBuilder();
    for (Bucket b : bucketList) {
      s.append("b=" + b.bucketVariable + "\n");
      s.append("v: [" + b.variableIDs.size() + "] ");
      for (Integer v : b.variableIDs) {
        s.append(v + " ");
      } s.append("\nf: [" + b.functions.size() + "]\n");
      for (FunctionTable f : b.functions) {
        s.append(f.toRealString());
      } s.append("\n");
    } System.out.println(s.toString());*/
    
    while (bucketList.getFirst().bucketVariable != -1) { 
      System.out.println("Eliminating bucket: " + bucketList.getFirst().bucketVariable);
      
      // Multiply-sum-out the Functions in this bucket
      FunctionTable f = FunctionTable.productSumOut(
          bucketList.getFirst().functions, 
          bucketList.getFirst().bucketVariable, 
          this.variables);
      
      // Delete this bucket
      bucketList.removeFirst();
      // Put f into first available bucket
      putFunctionInFirstBucket(f, bucketList);
    }
    
    System.out.println("Eliminating bucket -1");
    
    // Compute final value
    FunctionTable r = FunctionTable.product(
        bucketList.getFirst().functions,  
        this.variables);
    
    //bucketList.removeFirst();  
    
    if (type == GraphModelType.BAYES) {
      r.normalize();
    }
    
   return r.table.get(0).getValue();
  }

  private LinkedList<Bucket> createBucketListFromOrder(ArrayList<Integer> bucketListOrder) {
    LinkedList<Bucket> bucketList = new LinkedList<Bucket>();
    // create empty buckets from order
    for (Integer vID : bucketListOrder) {
      bucketList.add(new Bucket(vID));
    }
    
    // fill buckets - put f into the bucket of its first mentioned variable in its scope
    for (FunctionTable f : this.functions) {
      putFunctionInFirstBucket(f, bucketList);
    }
    return bucketList;
  }
  
  private void putFunctionInFirstBucket(FunctionTable f, LinkedList<Bucket> bucketList) {
    if (f.isEmpty()) {
      return;
    }
    
    // f is a trivial Function, put into bucket -1 (the last bucket)
    if (f.isTrivial()) {
      //System.out.println("Put f into bucket -1");
      bucketList.getLast().functions.add(f);
      return;
    }
    
    // Add new Function to first matching bucket
    for (Bucket b : bucketList) {
      // If any of f's variables are in this bucket, add it to this bucket
      //if (Lib.containsAny(f.variableIDs, b.variableIDs) != null) {
      if (f.variableIDs.contains(b.bucketVariable)) {
        //System.out.println("Put f into bucket " + b.bucketVariable);
        b.variableIDs.addAll(f.variableIDs);
        b.functions.add(f);
        return;
      }
    }
  }

  private ArrayList<Integer> computeMinFillOrder(ArrayList<FunctionTable> functions) {
    System.err.println("Min-Fill Ordering is not yet supported.");  
    System.out.println("Width of Min-Fill order: 0" );   
    return new ArrayList<Integer>();
  }

  private HashMap<Integer,HashSet<Integer>> computeInteractionGraph(ArrayList<FunctionTable> functions) {
    HashMap<Integer,HashSet<Integer>> interactionGraph = new HashMap<Integer,HashSet<Integer>>();

    for (int i = 0; i < variables.size(); i++) {
      interactionGraph.put(i, new HashSet<Integer>());
    }
    
    for (FunctionTable f : functions) {
      for (Integer vID1 : f.variableIDs) {
        for (Integer vID2 : f.variableIDs) {
          if (vID1 != vID2) {
            interactionGraph.get(vID1).add(vID2);
          }
        }
      }
    }
    
    return interactionGraph;    
  }
  
  private ArrayList<Integer> computeMinDegreeOrder(ArrayList<FunctionTable> functions) {
    ArrayList<Integer> minDegreeOrder = new ArrayList<Integer>();
    HashMap<Integer,HashSet<Integer>> interactionGraph = computeInteractionGraph(functions);
    
    //System.out.println("Generated interaction graph:");
    //System.out.println(interactionGraph.toString());
    
    int min = 0;
    int bestVID = 0;
    int max = 0;
    
    while (!interactionGraph.isEmpty()) {
      // linear search to find min sized element
      min = Integer.MAX_VALUE;
      for (Map.Entry<Integer, HashSet<Integer>> interaction : interactionGraph.entrySet()) {
        if (interaction.getValue().size() < min) {
          bestVID = interaction.getKey();
          min = interaction.getValue().size();
        }
      }
      
      // Keep track of the maximal width along the order
      max = (min > max ? min : max);
      
      // Minimum degree node found this iteration, store
      minDegreeOrder.add(bestVID);
      
      // Add edges between all of bestVID's neighbors
      for (Integer vID1 : interactionGraph.get(bestVID)) {
        for (Integer vID2 : interactionGraph.get(bestVID)) {
          if (vID1 != vID2) {
            interactionGraph.get(vID1).add(vID2);
          }
        }
      }      
      // Remove all mentions of bestVID
      for (HashSet<Integer> edges : interactionGraph.values()) {
        edges.remove(bestVID);
      }      
      // Remove bestVID from the interaction graph
      interactionGraph.remove(bestVID);      
    }
    
    System.out.println("Width of Min-Degree order: " + max);    
    return minDegreeOrder;
  }
}



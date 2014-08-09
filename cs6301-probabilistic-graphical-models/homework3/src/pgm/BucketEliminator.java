package pgm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import lib.LogNumber;
import lib.Stopwatch;

public class BucketEliminator implements VariableEliminatorType {
  protected VariableCollectionType variables = new VariableList();
  protected EvidenceCollectionType evidence = new EvidenceHashMap();
  protected ArrayList<FunctionTable> functions = new ArrayList<FunctionTable>();
  
  BucketEliminator(
      VariableCollectionType v,
      EvidenceCollectionType e,
      ArrayList<FunctionTable> f) {
    this.variables = v;
    this.evidence = e;
    this.functions = f;
  }
  
  @Override
  public LogNumber eliminate() {
    
    instantiateEvidence();
    
    ArrayList<Integer> bucketListOrder = new ArrayList<Integer>();    
    bucketListOrder = computeMinDegreeOrder(this.functions);
        
    // Add a bucket for -1. Trivial functions are placed in bucket -1.
    bucketListOrder.add(-1);    
    
    // Create and fill buckets along ordering
    BucketList bucketList = createBucketListFromOrder(bucketListOrder);    
    
    // Eliminate buckets
    FunctionTable f;
    while (bucketList.buckets.size() > 1) {
      //System.out.println("Eliminating bucket: " + bucketList.buckets.getFirst().bucketVariable.id());
      
      // Eliminate the bucket
      f = eliminateFirstBucket(bucketList); 

      // Put f into first succeeding bucket
      putFunctionInFirstBucket(f, bucketList);
    }
    
    //System.out.println("Eliminating bucket: trivial");    
    // Compute final value
    FunctionTable r = FunctionTable.product(
        bucketList.buckets.getFirst().functions,  
        this.variables);
    
    return r.getValues().iterator().next();
  }
  
  protected HashMap<Integer,HashSet<Integer>> computeInteractionGraph(ArrayList<FunctionTable> functions) {
    HashMap<Integer,HashSet<Integer>> interactionGraph = new HashMap<Integer,HashSet<Integer>>();

    for (VariableType v : this.variables.getVariables()) {
      interactionGraph.put(v.id(), new HashSet<Integer>());
    }
    
    for (FunctionTable f : this.functions) {
      for (VariableType v1 : f.variables) {
        for (VariableType v2 : f.variables) {
          if (v1.id() != v2.id()) {
            interactionGraph.get(v1.id()).add(v2.id());
          }
        }
      }
    }
    
    return interactionGraph;    
  }
  
  protected ArrayList<Integer> computeMinDegreeOrder(ArrayList<FunctionTable> functions) {
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

  protected BucketList createBucketListFromOrder(ArrayList<Integer> bucketListOrder) {
    BucketList bucketList = new BucketList();
    
    // Create empty buckets from order. No variable with negative ID can exist
    // so add it as a special case
    for (Integer vID : bucketListOrder) {
      if (vID >= 0)
        bucketList.buckets.add(new Bucket(this.variables.get(vID)));
      else
        bucketList.buckets.add(new Bucket(new Variable(-1,0)));
    }
    
    // fill buckets - put f into the bucket of its first mentioned variable in its scope
    for (FunctionTable f : this.functions) {
      putFunctionInFirstBucket(f, bucketList);
    }
    
    return bucketList;
  }
 
  protected void putFunctionInFirstBucket(FunctionTable f, BucketList bucketList) {
    if (f.isEmpty()) {
      return;
    }
    
    // f is a trivial Function, put into bucket -1 (the last bucket)
    if (f.isTrivial()) {
      bucketList.buckets.getLast().functions.add(f);
      return;
    }
    
    // Add new Function to first bucket whose bucketVariable is in the scope of f
    for (Bucket b : bucketList.buckets) {
      if (f.variables.contains(b.bucketVariable)) {
        b.variables.addAll(f.variables);
        b.functions.add(f);
        return;
      }
    }
  } 
  
  protected FunctionTable eliminateFirstBucket(BucketList bucketList) {   
    // Multiply-sum-out the Functions in this bucket
    FunctionTable f = FunctionTable.productSumOut(
        bucketList.buckets.getFirst().functions, 
        bucketList.buckets.getFirst().bucketVariable, 
        this.variables);
    
    bucketList.buckets.removeFirst();
    
    return f;
  }  

  protected void instantiateEvidence() {
    Stopwatch stopwatch = new Stopwatch();
    stopwatch.start();
    /*System.out.println("Functions before instantiation");
    for (FunctionTable f : this.functions) {
      System.out.println(f.toRealString()); }*/
    
    for (int i = 0; i < this.functions.size(); i++) {
      FunctionTable nVal = this.functions.get(i).instantiate(
          this.evidence, 
          this.variables);
      this.functions.set(i, nVal);
    }
    stopwatch.stop();
    System.out.println("Instantiated evidence: " + stopwatch);
    /*System.out.println("Functions after instantiation");
    for (FunctionTable f : this.functions) {
      System.out.println(f.toRealString()); }*/
  }
}

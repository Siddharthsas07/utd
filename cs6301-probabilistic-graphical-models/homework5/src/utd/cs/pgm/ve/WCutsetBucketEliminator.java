package utd.cs.pgm.ve;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.core.function.Function;
import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.probability.DynamicDistribution;
import utd.cs.pgm.probability.NormalDistribution;
import utd.cs.pgm.probability.ProbabilityDistribution;
import utd.cs.pgm.util.LogDouble;

public class WCutsetBucketEliminator extends BucketEliminator {
  protected int maxClusterSize;
  protected int maxSampleIterations;
  protected VariableEliminatorType type;

  public WCutsetBucketEliminator(
      ArrayList<Function> functions, 
      ArrayList<Variable> variables,      
      int maxClusterSize, 
      int maxIterations,
      VariableEliminatorType type) {
    this.functions = functions;
    this.variables = variables;
    this.maxClusterSize = maxClusterSize;
    this.maxSampleIterations = maxIterations;
    this.type = type;
  }
  
  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();
    
    s.append(this.type + "\n");
    s.append(this.orderHeuristic + "\n");
    s.append("w = " + this.maxClusterSize + "\n");
    s.append("N = " + this.maxSampleIterations + "\n");
    
    return s.toString();
  }
  
  @Override 
  public LogDouble eliminate() {
    Stopwatch sw = new Stopwatch();
    sw.start();
    
    // Assume the stored variables are already evidence instantiated from a
    // .uai.evid file or something.
    LogDouble Z = new LogDouble(0.0);
    
    // Generate the cutset
    ArrayList<Variable> cutset = computeWCutset(); 
    
    /*System.out.println("Computed cutset:");
    for (Variable v : cutset) {
      System.out.println(v);
    }
    */
    if (cutset.isEmpty()) {
      return super.eliminate();
    }
    
    // Create a distribution over the cutset
    ProbabilityDistribution Q = null;
    if (this.type == VariableEliminatorType.VE_WCUTSET_BUCKET_ELIMINATION_SM_NORMAL) {
      Q = new NormalDistribution(cutset);
    } else if (this.type == VariableEliminatorType.VE_WCUTSET_BUCKET_ELIMINATION_SM_DYNAMIC) {
      Q = new DynamicDistribution(cutset, 100);
    } else {
      System.err.print("Unknown distribution type.");
      return LogDouble.LS_ZERO;
    }
    
    ArrayList<Variable> sampledValues;
    // Sample the distribution repeatedly, update Z
    for (int i = 0; i < this.maxSampleIterations; i++) {
      //System.out.println("Sample iteration " + i);
      sampledValues = Q.generateSample();
      
      //System.out.println("Sampled values");
      //System.out.println(Q.printSample(sampledValues));
      
      // set sampledValues as evidence
      setEvidence(sampledValues);
      
      // Create a bucket eliminator to compute numerator
      LogDouble ve = super.eliminate();
      //System.out.println("ve = " + ve.toRealString());
      
      // Get probability of sampledValues
      LogDouble qw = Q.probabilityOf(sampledValues);
      //System.out.println("qw = " + qw.toRealString());
      
      LogDouble w = ve.div(qw);
      
      if (this.type == VariableEliminatorType.VE_WCUTSET_BUCKET_ELIMINATION_SM_DYNAMIC) {
        Q.updateWeights(w);
      }
      
      Z = Z.add(w);      
    }
    
    sw.stop();
    //System.out.println(sw);
    
    return Z.div(new LogDouble(this.maxSampleIterations));
  }

  // Compute a w-cutset over the variables and functions. Assumes evidence 
  // instantiation already happened.
  protected ArrayList<Variable> computeWCutset() {
    ArrayList<Integer> order = getOrdering(); 
    if (order == null || order.isEmpty()) {
      return new ArrayList<Variable>();
    }
    
    ArrayList<ArrayList<Function>> bucketList = new ArrayList<ArrayList<Function>>(order.size());
    for (int i = 0; i < order.size(); i++) {
      bucketList.add(new ArrayList<Function>());
    }
    bucketList.trimToSize();    
    
    ArrayList<Integer> varInPos = new ArrayList<Integer>(this.variables.size());
    for (int i = 0; i < this.variables.size(); i++) {
      varInPos.add(0);
    }
    for (int i = 0; i < order.size(); i++) {
      varInPos.set(order.get(i), i);
    }
    varInPos.trimToSize();
    
    // Put functions into initial buckets
    for (Function f : this.functions) {
      int pos = this.variables.size();
      
      // Boundary case that all variables in function are evidence. 
      // Store value in return value, this function is then discarded.
      boolean allEvid = true;
      for (Variable v : f.variables) {
        if (v.isEvidence()) {
          continue;
        } 
        if (varInPos.get(v.getId()) < pos) {
          pos = varInPos.get(v.getId());
        }        
        allEvid = false;
      }
      
      if (!allEvid) {
        bucketList.get(pos).add(f);
      }
    }
    
    //System.out.println("The bucket list:");
    //System.out.println(bucketListToString(bucketList, order));
    
    // Now have a bucket list. Transform it to a cluster tree.
    ArrayList<HashSet<Integer>> clusters = new ArrayList<HashSet<Integer>>();
    
    // Create a cluster for each bucket in the bucket list.
    for (ArrayList<Function> b : bucketList) {
      HashSet<Integer> bucketVariables = new HashSet<Integer>();
      for (Function f : b) {
        for (Variable v : f.variables) {
          bucketVariables.add(v.getId());
        }
      }
      clusters.add(bucketVariables);
    }
    
    ArrayList<Variable> cutset = new ArrayList<Variable>();
    
    //System.out.println("Interaction graph:");
    //System.out.println(interactionGraphToString(clusters));

    //System.out.println("Min-degree ordering:");
    //System.out.println(orderingToString(ordering));
    
    ArrayList<Integer> counts = new ArrayList<Integer>(this.variables.size());
    
    for (int i = this.variables.size(); i > 0 ; i--) {
      counts.add(0);
    } counts.trimToSize();
    
    // compute initial counts
    for (HashSet<Integer> c : clusters) {
      for (Integer vID : c) {
        counts.set(vID, counts.get(vID) + 1);
      }
    }
    
    /*System.out.println("Counts:");
    for (int i : counts) {
      System.out.println(i);
    }*/
    
    while (!clusterSizeCheck(clusters)) {
      // find max count in counts -- this is perchance the least efficient way
      // to do this lol
      Integer mostCommonVID = counts.indexOf(Collections.max(counts));
      if (mostCommonVID == null){
        return cutset;
      } 
      
      // remove that variable ID from interaction graph by removing it from
      // all clusters
      for (HashSet<Integer> c : clusters) {
        c.remove(mostCommonVID);
      }
      
      // set count for that variable ID to 0 (it no longer appears in any clusters)
      counts.set(mostCommonVID, 0);
      
      // add variable with that ID to cutset
      cutset.add(this.variables.get(mostCommonVID));
    }
    
    return cutset;    
  }
  
  protected boolean clusterSizeCheck(ArrayList<HashSet<Integer>> clusters) {
    for (HashSet<Integer> c : clusters) {
      if (c.size() > this.maxClusterSize + 1) {
        return false;
      }
    }
    return true;
  }
    
  // Add the evidence values in sampledValues to this.variables
  protected void setEvidence(ArrayList<Variable> sampledValues) {
    for (Variable v : sampledValues) {
      this.variables.get(v.getId()).setEvidence(v.getValue());
    }    
  }
}

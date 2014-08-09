package utd.cs.pgm.ve;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.core.function.Function;
import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;

public class BucketEliminator {
  protected ArrayList<Variable> variables = new ArrayList<Variable>();
  protected ArrayList<Function> functions = new ArrayList<Function>();
  protected OrderHeuristicType orderHeuristic = OrderHeuristicType.MIN_DEGREE;
  final protected VariableEliminatorType type = VariableEliminatorType.VE_BUCKET_ELIMINATION;
  
  public BucketEliminator() {}
  
  // Assumes variables has already been instantiated with evidence
  public BucketEliminator(ArrayList<Function> functions, ArrayList<Variable> variables) {
    this.functions = functions;
    this.variables = variables;    
  }
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    
    s.append(this.type + "\n");
    s.append(this.orderHeuristic + "\n");
    
    return s.toString();
  }
  
  public void setOrderHeuristic(OrderHeuristicType orderHeuristic) {
    this.orderHeuristic = orderHeuristic;  
  }
  
  public LogDouble eliminate() {
    Stopwatch sw = new Stopwatch();
    sw.start();
    
    LogDouble probabilityOfEvidence = new LogDouble(1.0);
    
    ArrayList<Integer> order = getOrdering(); 
    if (order == null) {
      return probabilityOfEvidence;
    } else if (order.isEmpty()) {
      return probabilityOfEvidence;
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
      
      if (allEvid) {
        probabilityOfEvidence = probabilityOfEvidence.mul(f.table.get(Variable.getAddress(f.variables)));
      } else {
        // assert(pos!=(int)order.size());
        bucketList.get(pos).add(f);
      }
    }
    
    //System.out.println("The bucket list:");
    //System.out.println(bucketListToString(bucketList, order));
    
    // Process buckets along list
    for (int i = 0; i < bucketList.size(); i++) {
      if (bucketList.get(i).isEmpty()) {
        continue;
      }
      
      // Create scope of Function resulting from this bucket. Union all variables
      // from the functions in each bucket, remove variables which are evidence,
      // and finally remove the bucket variable.
      HashSet<Variable> resultantVariables = new HashSet<Variable>();
      for (Function f : bucketList.get(i)) {
        resultantVariables.addAll(f.variables);
      }
      /*for (Variable v : resultantVariables) {
        if (v.isEvidence()) {
          resultantVariables.remove(v);
        }
      }*/
      for (Iterator<Variable> itr = resultantVariables.iterator(); itr.hasNext();) {
        Variable v = itr.next();
        if (v.isEvidence()) {
          itr.remove();
        }
      }
      resultantVariables.remove(this.variables.get(order.get(i)));
      ArrayList<Variable> resultVariablesAsArrayList = new ArrayList<Variable>(resultantVariables);
      
      /*System.out.println("Resultant Variables: ");
      for (Variable v : resultVariablesAsArrayList) {
        System.out.println(v);
      } */     
      
      // Multiply functions in this bucket
      Function result = Function.productSumOut(
          resultVariablesAsArrayList, 
          bucketList.get(i),
          false);
      /*Function result = Function.productSumOut(
          resultantVariables, 
          bucketList.get(i),
          false);*/
      
      //System.out.println("Result Function:");
      //System.out.println(result.toRealString());
      
      // Handle result of multiplication. If trivial, multiply value into PE.
      // Otherwise put resultant function into first bucket.
      if (result.isTrivial()) {
        probabilityOfEvidence = probabilityOfEvidence.mul(result.table.get(0));
        continue;
      }
      
      int pos = order.size();
      for (Variable v : result.variables) {
        if (varInPos.get(v.getId()) < pos) {
          pos = varInPos.get(v.getId());
        }
      }
      
      if (pos < bucketList.size()) {
        bucketList.get(pos).add(result);
      }
      bucketList.get(i).clear();      
    }
    
    sw.stop();
    //System.out.println("Computation completed: " + sw);
    
    return probabilityOfEvidence;
  }
  
  protected String bucketListToString(
      ArrayList<ArrayList<Function>> bl,
      ArrayList<Integer> o) {
    StringBuilder s = new StringBuilder();
    for (int i = 0; i <bl.size(); i++) {
      s.append(o.get(i));
      s.append(Function.functionCollectionToRealString(bl.get(i)) + "\n");
    }
    return s.toString();
  }
  
  protected ArrayList<Integer> getOrdering() {
    ArrayList<Integer> ordering = null;
    
    ArrayList<HashSet<Integer>> interactionGraph = computeInteractionGraph();
    
    //System.out.println("Interaction graph:");
    //System.out.println(interactionGraphToString(interactionGraph));    
    
    if (this.orderHeuristic == OrderHeuristicType.MIN_DEGREE) {
      ordering = computeMinDegreeOrder(interactionGraph);
      //System.out.println("Min-degree ordering:");
      //System.out.println(orderingToString(ordering));
    } else {
      System.err.println("Error: unknown ordering heuristic.");
    }
    
    return ordering;
  }
  
  protected ArrayList<HashSet<Integer>> computeInteractionGraph() {
    ArrayList<HashSet<Integer>> interactionGraph 
        = new ArrayList<HashSet<Integer>>(this.variables.size());
    for (int i = 0; i < this.variables.size(); i++) {
      interactionGraph.add(new HashSet<Integer>());
    }
    interactionGraph.trimToSize();

    int vID1 = 0;
    int vID2 = 0;
    
    // Over each function, and the variables in that function, add edges
    // to each variable later in that variable list. Skip evidence vars.
    for(int i = 0; i < this.functions.size(); i++) {
      for (int j = 0; j < functions.get(i).variables.size(); j++) {
        vID1 = functions.get(i).variables.get(j).getId();
        if (variables.get(vID1).isEvidence()) {
          continue;
        }
        for (int k = j + 1; k < this.functions.get(i).variables.size(); k++) {
          vID2 = functions.get(i).variables.get(k).getId();
          if (variables.get(vID2).isEvidence()) {
            continue;
          }
          interactionGraph.get(vID1).add(vID2);
          interactionGraph.get(vID2).add(vID1);
        }
      }
    }
    
    return interactionGraph;
  }
  
  protected String interactionGraphToString(ArrayList<HashSet<Integer>> ig) {
    StringBuilder s = new StringBuilder();
    for (HashSet<Integer> i : ig) {
      for (Integer e : i) {
        s.append(e + " ");
      }
      s.append("\n");
    }
    return s.toString();
  }
  
  protected ArrayList<Integer> computeMinDegreeOrder(ArrayList<HashSet<Integer>> interactionGraph) {
    // Create a deep copy of interactionGraph to operate on
    ArrayList<HashSet<Integer>> ig = new ArrayList<HashSet<Integer>>(interactionGraph.size());
    for (HashSet<Integer> c : interactionGraph) {
      ig.add(new HashSet<Integer>(c));
    }    
    
    ArrayList<Boolean> processed = new ArrayList<Boolean>(this.variables.size());   
    
    int numNonEvidVars = 0;
    for (Variable v : this.variables) {
      if (v.isEvidence()) {
        processed.add(true);
      } else {
        numNonEvidVars++;
        processed.add(false);
      }
    }
    processed.trimToSize();
    
    ArrayList<Integer> minDegreeOrder = new ArrayList<Integer>(numNonEvidVars);
    ArrayList<HashSet<Integer>> clusters = new ArrayList<HashSet<Integer>>(numNonEvidVars);
    for (int i = 0; i < numNonEvidVars; i++) {
      minDegreeOrder.add(0);
      clusters.add(new HashSet<Integer>());
    }
    minDegreeOrder.trimToSize();
    clusters.trimToSize();
    
    int min = 0;
    int max = 0;
    int var = 0;
    
    for(int i = 0; i < numNonEvidVars; i++) {
      // Find variable with minimum number of neighbors 
      min = this.variables.size();
      for (int j = 0; j < ig.size(); j++) {
        if (processed.get(j)) {
          continue;
        } 
        if (ig.get(j).size() < min) {
          min = ig.get(j).size();
          minDegreeOrder.set(i, j);          
        }        
      }
      
      // Connect neighbors of buckets together
      var = minDegreeOrder.get(i);
      processed.set(var, true);
      for (Integer a : ig.get(var)) {
        for (Integer b : ig.get(var)) {
          if (a == b) {
            continue;
          }
          ig.get(a).add(b);
          ig.get(b).add(a);
        }
      }
      
      // Store updated connection into bucket list. Update max cluster size found
      clusters.set(i, ig.get(var));
      if (max < clusters.get(i).size()) {
        max = clusters.get(i).size();
      }

      // Remove var from interaction graph
      for (HashSet<Integer> a : ig) {
        a.remove(var);
      }
      
      ig.get(var).clear();
    }
    
    //System.out.println("Max cluster size = " + max);
    
    return minDegreeOrder;
  }
  
  protected String orderingToString(ArrayList<Integer> o) {
    StringBuilder s = new StringBuilder();
    for (Integer i : o) {
      s.append(i + "\n");
    }
    return s.toString();
  }
}

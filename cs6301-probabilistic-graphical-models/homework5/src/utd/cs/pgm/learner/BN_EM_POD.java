package utd.cs.pgm.learner;

import java.io.FileReader;
import java.util.ArrayList;
import java.util.Scanner;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.core.function.Function;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.graphmodel.GraphModelType;
import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;
import utd.cs.pgm.util.SparseTable;
import utd.cs.pgm.util.StaticUtilities;
import utd.cs.pgm.util.WeightedSparseTable;

public class BN_EM_POD implements IModelLearner {
  GraphModel learnedModel = new GraphModel();
  GraphModel trueModel = new GraphModel();
  //SparseTable examples = new SparseTable();
  SparseTable examples;
  //WeightedSparseTable M = new WeightedSparseTable();
  WeightedSparseTable M;// = new WeightedSparseTable();
  ArrayList<ArrayList<Integer>> functionToMMap = new ArrayList<ArrayList<Integer>>();
  int numIterationsForLearning = 0;
  int valueForUnobservedValue = -2;
  double small = 1e-100;
  
  public BN_EM_POD(int numIterationsForLearning) throws IllegalArgumentException {
    if (numIterationsForLearning <= 0) {
      throw new IllegalArgumentException("EM algorithm must run for >= 1 iterations.");
    }
    this.numIterationsForLearning = numIterationsForLearning;
  };
  
  public BN_EM_POD(String path, GraphModel structure) {
    train(path, structure);
  }
  
  @Override
  public GraphModel getLearnedModel() {
    return this.learnedModel;
  }

  @Override
  public GraphModel train(String path, GraphModel structure) {
    try {
      FileReader fr = new FileReader(path);
      Scanner sc = new Scanner(fr);
      
      int numVariables = sc.nextInt();
      int numExamples = sc.nextInt();

      this.examples = new SparseTable(numVariables);
      
      // Read training data examples, one example at a time.
      for (int i = 0; i < numExamples; i++) {
        ArrayList<Variable> example = new ArrayList<Variable>();
        
        // TODO this doesn't handle ?s
        
        /*for (int j = 0; j < numVariables; j++) {
          example.add(new Variable(j, structure.getVariables().get(j).getDomainSize(), sc.nextInt()));
        }*/
        
        for (int j = 0; j < numVariables; j++) {
          String v = sc.next();
          if (v.compareTo("?") == 0) {
            example.add(new Variable(j, structure.getVariables().get(j).getDomainSize(), -1, this.valueForUnobservedValue));
          } else {
            example.add(new Variable(j, structure.getVariables().get(j).getDomainSize(), Integer.valueOf(v)));
          }
        }
        
        //System.out.println("example = " + example);
        
        this.examples.add(example);
      }
      
      System.out.println("Examples:");
      //System.out.println(this.examples);
      
      // All finished with the examples
      sc.close();
      fr.close();
      
      // Set the learned structure as type BAYES
      this.learnedModel.setType(GraphModelType.BAYES);
      
     // Copy variables from known structure
      for (Variable v : structure.getVariables()) {
        this.learnedModel.getVariables().add(v.copy());
      }
      
      // Copy functions from known structure but initialize them to uniform or random
      for (Function f : structure.getFunctions()) {
        Function g = new Function();
        
        // Copy variables from the true structure
        for (Variable v : f.variables) {
          g.variables.add(v.copy());
        }
        
        g = g.initTableToUniform();
        
        //g = g.initTableToRandom(true);
        
        this.learnedModel.getFunctions().add(g);
      }
      
      System.out.println("Initial learned model:");
      System.out.println(this.learnedModel);
      
      // Initialize M structure and the function to M mapping
      this.M = new WeightedSparseTable(numVariables);
      
      int numFunctions = this.learnedModel.getFunctions().size();
      
      for (int i = 0; i < numFunctions; i++) {
        this.functionToMMap.add(new ArrayList<Integer>());
        Function f = this.learnedModel.getFunctions().get(i);
        int fTableSize = f.table.size();
        for (int j = 0; j < fTableSize; j++) {
          Variable.setAddress(f.variables, j);
          
          ArrayList<Variable> m_entry = new ArrayList<Variable>();        
          for (Variable v : f.variables) {
            m_entry.add(v.copy());
          }
          
          int addedIdx = this.M.add(m_entry, LogDouble.LS_ZERO);
          this.functionToMMap.get(i).add(addedIdx);
        }
      }
      
      this.functionToMMap.trimToSize();
      for (int i = 0; i < numFunctions; i++) {
        this.functionToMMap.get(i).trimToSize();
      }
      
      System.out.println("M:");
      //System.out.println(this.M);
      System.out.println("Function to M Mapping:");
      //System.out.println(this.functionToMMap);
      
      // Run EM Algorithm.
      Stopwatch sw = new Stopwatch();
      for (int k = 0; k < this.numIterationsForLearning; k++) {
        sw.start();
        EStep();
        sw.stop();
        //System.out.println("EStep: " + sw);
        //System.out.println("Updated M:");
        //System.out.println(this.M);
        
        sw.start();
        MStep();
        sw.stop();
        //System.out.println("MStep: " + sw);
        //System.out.println("Updated L:");
        //System.out.println(this.learnedModel);
      }
        
    } catch (Exception e) {
      e.printStackTrace();
    }
    
    // Return the learned model with copied variables and such.
    return learnedModel;
  }

  protected void EStep() {
    // Clear probabilities for M structure
    int ms = this.M.size();
    for (int i = 0; i < ms; i++) {
      this.M.weights.set(i, LogDouble.LS_ZERO);
    }
    
    int numFunctions = this.learnedModel.getFunctions().size();
    for (ArrayList<Variable> e : this.examples.entries) {
      // Expand this entry if it's a partially observed one
      ArrayList<ArrayList<Variable>> eExpanded = expandPartiallyObservableExample(e);
      
      // For each entry
      for (ArrayList<Variable> eE : eExpanded) {
        // Compute its probability
        LogDouble p = this.learnedModel.computeProbabilityOfFullAssignment(eE);
        // Update M where the entry matches a tuple in a function for each fucntion. Use
        // the cached mappings.
        for (int i = 0; i < numFunctions; i++) {
          Function f = this.learnedModel.getFunctions().get(i);
          int fTableSize = f.table.size();
          for (int j = 0; j < fTableSize; j++) {
            // if eExpanded matches M[mapping[i][j]]
            // then M[mapping[i][j]] += p
            int mIdx = this.functionToMMap.get(i).get(j);
            if (M.isSubsetOf(this.M.indexToEntry(mIdx), eE)) {
              M.addWeightToEntry(mIdx, p);
            }
          }          
        }
      }
    }
  }
  
  // TODO FIX THIS
  protected ArrayList<ArrayList<Variable>> expandPartiallyObservableExample(ArrayList<Variable> e) {
    ArrayList<ArrayList<Variable>> r = new ArrayList<ArrayList<Variable>>();
    int rSize = 1;
    ArrayList<Variable> missingVars = new ArrayList<Variable>();
    ArrayList<Integer> missingIndices = new ArrayList<Integer>();
    
    // Count number of entries we'll be expanding to
    for (int i = 0; i < e.size(); i++) {
      if (e.get(i).getValue() == this.valueForUnobservedValue) {
        rSize *= e.get(i).getDomainSize();
        missingVars.add(e.get(i).copy());
        missingIndices.add(i);
      }
    }
    
    // If no expansion necessary, just return e
    if (rSize == 1) {
      r.add(e);
      r.trimToSize();
      return r;
    }
    
    // For each expanded entry we have to make get the next incremental tuple for the
    // missing variables
    for (int i = 0; i < rSize; i++) {
      Variable.setAddress(missingVars, i);
      ArrayList<Variable> expandedEntry = new ArrayList<Variable>();
      
      // Copy variables from the main entry
      for (Variable v : e) {
        expandedEntry.add(v.copy());
      }
      
      // And overwrite the variables with missing values with the value from this
      // incremental tuple
      for (int j = 0; j < missingIndices.size(); j++) {
        expandedEntry.get(missingIndices.get(j)).setValue(missingVars.get(j).getValue());
      }
      
      r.add(expandedEntry);
    } 
    
    return r;
  }
  
  protected void MStep() {
    //for (Function f : this.learnedModel.getFunctions()) {
    // For each function
    int numFunctions = this.learnedModel.getFunctions().size();
    for (int i = 0; i < numFunctions; i++) {
      Function f = this.learnedModel.getFunctions().get(i);      
      int fTableSize = f.table.size();
      // For each table entry for the function
      for (int j = 0; j < fTableSize; j++) {
        Variable.setAddress(f.variables, j);
        
        // Get the MLE estimate from M for the jth entry in f's table
        LogDouble value = getMLEEstimate(f, i, j);
        
        f.table.set(j, value);
      }
      
      f = f.normalize();
      
      // now normalize f to make it a valid cpt again????
      //this.learnedModel.getFunctions().set(i, f.normalize());
      this.learnedModel.getFunctions().set(i, f);
      
      //System.out.println("Updated function " + i);
      //System.out.println(f);
    }
  }
  
  LogDouble getMLEEstimate(Function f, int functionId, int tableIdx) {
    if (f.variables.isEmpty()) {
      return LogDouble.LS_ZERO;
    }
    
    // t is a partial variable assignment. The child variable is listed last.
    // Project t onto every entry in the examples and if it matches,
    // sum the weight for that example into the value.
    //double small = 1E-20;
    LogDouble numer = new LogDouble(this.small);
    int childDomainSize = f.variables.get(f.variables.size()-1).getDomainSize();
    LogDouble denom = new LogDouble(childDomainSize * this.small);
    
    //numer = numer.add(this.M.getWeightOf(t));
    numer = numer.add(this.M.getWeight(this.functionToMMap.get(functionId).get(tableIdx)));
    
    denom = denom.add(this.M.getWeightOfDiscludeChild(f.variables));
    
    return numer.div(denom);
  }
  
  @Override
  public GraphModel getTrueModel() {
    return this.trueModel;
  }

  @Override
  public void setTrueModel(GraphModel gm) {
    this.trueModel = gm;
  }

  @Override
  public LogDouble test(String path) {
    return StaticUtilities.computeLogLikelihoodDifference(
        this.learnedModel, this.trueModel, path);
  }
  
  @Override
  public String toString() {
    StringBuffer s = new StringBuffer();
    s.append("Learned Model\n" + this.learnedModel.toString() + "\n");
    s.append("True Model\n" + this.trueModel.toString() + "\n");
    s.append("Examples\n" + this.examples.toString() + "\n");
    return s.toString();
  }
}

package utd.cs.pgm.core.graphmodel;

import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Scanner;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.core.function.Function;
import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;
import utd.cs.pgm.ve.BucketEliminator;
import utd.cs.pgm.ve.OrderHeuristicType;
import utd.cs.pgm.ve.VariableEliminatorType;
import utd.cs.pgm.ve.WCutsetBucketEliminator;

public class GraphModel {
  protected GraphModelType      type;
  protected ArrayList<Variable> variables = new ArrayList<Variable>();
  protected ArrayList<Function> functions = new ArrayList<Function>();
  
  protected boolean validState;
  
  protected BucketEliminator eliminator = new BucketEliminator();
  
  public GraphModel() {};
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    
    s.append(this.type + "\n");
    s.append(this.validState + "\n");
    s.append("e: \n");
    s.append(this.eliminator + "\n");
    s.append(Variable.variableCollectionString(this.variables));
    s.append(Function.functionCollectionToRealString(this.functions));
    
    return s.toString();
  }
  
  public void print(PrintStream out) {
    out.print(this);
  }
    
  public GraphModelType getType() {
    return this.type;
  }
  
  public void setType(GraphModelType type) {
    this.type = type;
  }
  
  public void readUAI(String path) throws IOException {
    //System.out.println("Reading " + path);
    
    FileReader fr = new FileReader(path);
    Scanner sc = new Scanner(fr);
    
    // Store graph type
    if (sc.next().compareTo("MARKOV") == 0) {
      this.type = GraphModelType.MARKOV;
    } else {
      this.type = GraphModelType.BAYES;
    }
    
    // Read variables and their scopes
    int numVariables = sc.nextInt();
    this.variables.ensureCapacity(numVariables);
    for (int i = 0; i < numVariables; i++) {
      this.variables.add(new Variable(i, sc.nextInt()));
    }
    variables.trimToSize();
    
    // Read functions and their scopes
    int numFunctions = sc.nextInt();
    this.functions.ensureCapacity(numFunctions);
    for (int i = 0; i < numFunctions; i++) {
      Function f = new Function();
      int fScopeSize = sc.nextInt(); 
      f.variables.ensureCapacity(fScopeSize);
      for (int j = 0; j < fScopeSize; j++) {
        f.variables.add(this.variables.get(sc.nextInt()));
      }
      f.variables.trimToSize();
      this.functions.add(f);        
    }
    this.functions.trimToSize();
    
    // Read function table values
    for (Function f : this.functions) {
      int fTableSize = sc.nextInt();
      f.table.ensureCapacity(fTableSize);
      for (int i = 0; i < fTableSize; i++) {
        f.table.add(new LogDouble(sc.nextDouble()));
      }
      f.table.trimToSize();
    } 
    
    sc.close();
    fr.close();
  }
  
  public void readUAIevid(String path) throws IOException {
    //System.out.println("Reading " + path);
    
    FileReader fr = new FileReader(path);
    Scanner sc = new Scanner(fr);
    
    // Read evidence variables
    int numEvidence = sc.nextInt();
    for (int i = 0; i < numEvidence; i++) {
      this.variables.get(sc.nextInt()).setEvidence(sc.nextInt());
    }
    
    sc.close();
    fr.close();
  }
  
  public void writeUAI(String path) throws IOException {
    //System.out.println("Writing " + path);
    
    // Output network type
    StringBuffer s = new StringBuffer();
    if (this.type == GraphModelType.BAYES) {
      s.append("BAYES\n");
    } else if (this.type == GraphModelType.MARKOV) {
      s.append("MARKOV\n");
    } else {
      s.append("UNKNOWN\n");
    }
    
    // output number of variables
    s.append(this.variables.size() + "\n");
    
    // output variable scopes
    for (Variable v : this.variables) {
      s.append(v.getDomainSize() + " ");
    }
    s.append("\n");
    
    // output number of functions
    s.append(this.functions.size() + "\n");
    
    // output function structures
    for (Function f : this.functions) {
      // output function scope size
      s.append(f.variables.size() + " ");
      
      // output function variables
      for (Variable v : f.variables) {
        s.append(v.getId() + " ");
      }      
      s.append("\n");
    }
    
    // output blank line
    s.append("\n");
    
    // output function tables
    for (Function f : this.functions) {
      // output function table size
      s.append(f.table.size() + "\n");
      
      //int rowSize = f.variables.get(f.variables.size()-1).getDomainSize();     
      
      // output table
      /*for (int i = 0; i < f.table.size();) {
        s.append(" ");
        for (int j = 0; j < rowSize; j++) {
          s.append(String.format("%.6f ", f.table.get(i).getRealValue()) + " ");
          ++i;
        }
        s.append("\n");
        
      }*/
      /*for (LogDouble d : f.table) {
        s.append(String.format("%.6f ", d.getRealValue()) + " ");
      }
      s.append("\n\n");*/
      
      // Normalize
      int setSize = this.variables.get(this.variables.size()-1).getDomainSize();
      int fTableSize = f.table.size();
      for (int i = 0; i < fTableSize; i+=setSize) {
        s.append(" ");
        int setEndIdx = i + setSize;
        for (int j = i; j < setEndIdx; j++) {
          s.append(f.table.get(j).toRealString() + " ");
        }
        s.append("\n");  
      }
      s.append("\n");
    }    
    
    BufferedWriter bw = new BufferedWriter(new FileWriter(path), s.length());    
    bw.write(s.toString());
    bw.close();
  }
  
  public void writeUAIevid(String path) throws IOException {
    // System.out.println("Writing " + path);
    StringBuffer s = new StringBuffer();
    
    // output number of evidence values
    // count number of evidence variables
    int numEvidVariables = 0;
    for (Variable v : this.variables) {
      if (v.isEvidence()) {
        numEvidVariables++;
      }
    }
    
    s.append(numEvidVariables + "\n");
    
    // output evidence
    for (Variable v : this.variables) {
      if (v.isEvidence()) {
        s.append(" " + v.getId() + " " + v.getValue() + "\n");
      }
    }
    
    BufferedWriter bw = new BufferedWriter(new FileWriter(path), s.length());    
    bw.write(s.toString());
    bw.close();
  }
  
  public void setup(String[] args) throws IOException {
    Stopwatch sw = new Stopwatch();
    sw.start();   
    
    this.validState = false;    
    switch (args.length) {
    case 1:
      readUAI(args[0]);
      this.eliminator = new BucketEliminator(this.functions, this.variables);
      this.eliminator.setOrderHeuristic(OrderHeuristicType.MIN_DEGREE);
      break;
    case 2:
      readUAI(args[0]);
      readUAIevid(args[1]);
      this.eliminator = new BucketEliminator(this.functions, this.variables);
      this.eliminator.setOrderHeuristic(OrderHeuristicType.MIN_DEGREE);
      break;
    case 4:
      if (Integer.parseInt(args[2]) != 0) {
        System.err.println("You supplied 4 arguments, you must choose Bucket elimination.");
        System.out.println(howToSetup());
        return;
      }
      int orderType = Integer.parseInt(args[3]);
      if (orderType != 0) {
        System.err.println("You supplied 4 arguments, you must choose min-degree ordering.");
        System.out.println(howToSetup());
        return;
      }
      readUAI(args[0]);
      readUAIevid(args[1]);      
      this.eliminator = new BucketEliminator(this.functions, this.variables); 
      if (orderType == 0) {
        this.eliminator.setOrderHeuristic(OrderHeuristicType.MIN_DEGREE);
      }   
      break;
    case 7:
      if (Integer.parseInt(args[2]) != 1) {
        System.err.println("You supplied 7 arguments, you must choose W-cutset Bucket elimination.");
        System.out.println(howToSetup());
        return;
      }
      int orderType1 = Integer.parseInt(args[3]);
      if (orderType1 != 0) {
        System.err.println("You supplied 7 arguments, you must choose min-degree ordering.");
        System.out.println(howToSetup());        
        return;
      }
      int maxClusterSize = Integer.parseInt(args[4]);
      int maxIterations = Integer.parseInt(args[5]);
      int sampleMode = Integer.parseInt(args[6]);
      if (sampleMode != 0 && sampleMode != 1) {
        System.err.println("You supplied 7 arguments, uniform or dynamic sampling.");
        System.out.println(howToSetup());
        return;
      }
      readUAI(args[0]);
      readUAIevid(args[1]);
      if (sampleMode == 0) {
        this.eliminator = new WCutsetBucketEliminator(
            this.functions,
            this.variables, 
            maxClusterSize,
            maxIterations,
            VariableEliminatorType.VE_WCUTSET_BUCKET_ELIMINATION_SM_NORMAL);
      } else if (sampleMode == 1) {
        this.eliminator = new WCutsetBucketEliminator(
            this.functions,
            this.variables, 
            maxClusterSize,
            maxIterations,
            VariableEliminatorType.VE_WCUTSET_BUCKET_ELIMINATION_SM_DYNAMIC);
      }      
      if (orderType1 == 0) {
        this.eliminator.setOrderHeuristic(OrderHeuristicType.MIN_DEGREE);
      }      
      break;
    default:
      System.err.println("You supplied an invalid number of arguments.");
      System.out.println(howToSetup());
      return;
    }
    
    sw.stop();
    //System.out.println("Setup completed: " + sw);
    
    this.validState = true;
  }
  
  public String howToSetup() {
    StringBuilder s = new StringBuilder();
    s.append("Usage:\n");
    s.append("[0] = UAI file path\n");
    s.append("[1] = UAI.evid file path\n\n");
    
    s.append("Following that you may specify algorithms and their parameters. ");
    s.append("If you don't, it defaults to Bucket elimination + min-degree ordering.\n");
    s.append("[2] = #\n");
    s.append("      0 = Bucket elimination\n");
    s.append("      1 = W-cutset Bucket elimination\n");
    s.append("[3] = #\n");
    s.append("      0 = Min-degree bucket ordering\n\n");
    
    s.append("If you specify w-cutset Bucket elimination:\n");
    s.append("[4] = #\n");
    s.append("      # = Maximum cluster size after w-cutset\n");
    s.append("[5] = #\n");
    s.append("      # = Number of samplings after w-cutset\n");
    s.append("[6] = #\n");
    s.append("      0 = Uniform distribution sampling\n");
    s.append("      1 = Dynamic-learning distribution sampling\n");    
    return s.toString();
  }
  
  public LogDouble computeZorP() {
    /*if (!this.validState) {
      System.err.println("GraphModel is not in a valid state from setup. Computation incomplete.");
      System.out.println(howToSetup());
      return new LogDouble(0.0);
    }*/
    
    //return eliminator.eliminate();
    
    this.eliminator = new BucketEliminator(this.functions, this.variables);
    return eliminator.eliminate();
  }
  
  public ArrayList<Variable> getVariables() {
    return this.variables;
  }
  
  public void setAssignment(ArrayList<Variable> assignment) {
    for (Variable e : assignment) {
      if (e.getId() >= this.variables.size()) {
        continue;
      }
      this.variables.get(e.getId()).setValue(e.getValue());
    }
  }
  
  public ArrayList<Function> getFunctions() {
    return this.functions;
  }

  public LogDouble computeProbabilityOfFullAssignment(
      ArrayList<Variable> example) {
    LogDouble p = new LogDouble(1.0);
    
    // Loop over each function and project the full assignment onto the function
    // and multiply the value into p
    for (Function f : this.functions) {
      // Store in t a subset of example which is the context of f
      ArrayList<Variable> t = new ArrayList<Variable>();
      for (Variable v : f.variables) {
        t.add(example.get(v.getId()));
      }
      
      int idx = Variable.getAddress(t);
      
      p = p.mul(f.table.get(idx));
    }
    
    return p;
  }
}

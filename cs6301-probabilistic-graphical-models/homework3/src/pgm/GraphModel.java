/**
 * 
 */
package pgm;

import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;

import java.util.Scanner;

import lib.LogNumber;
import lib.Stopwatch;

/**
 * @author Gary
 *
 */
public class GraphModel {
  protected VariableCollectionType variables = new VariableList();
  protected EvidenceCollectionType evidence = new EvidenceHashMap();
  protected ArrayList<FunctionTable> functions = new ArrayList<FunctionTable>();
  
  public VariableEliminatorType eliminator;
  
  public GraphModelType type;  
  public GraphModelEliminationAlgorithmType eliminationAlgorithm;  
  public GraphModelOrderHeuristicType orderHeuristic; 
  
  private boolean validState = false;
  
  public GraphModel() {
    this.type = GraphModelType.MARKOV;
    this.eliminationAlgorithm = GraphModelEliminationAlgorithmType.VE_BUCKET_ELIMINATION;
    this.orderHeuristic = GraphModelOrderHeuristicType.MIN_DEGREE;
    
    this.validState = false;
  }  
  
  public void setup(String[] args) throws IOException {   
    this.validState = false;
    
    switch (args.length) {
    case 1:
      this.read(args[0], "");
      this.eliminationAlgorithm = GraphModelEliminationAlgorithmType.VE_BUCKET_ELIMINATION;
      this.orderHeuristic = GraphModelOrderHeuristicType.MIN_DEGREE;
      this.eliminator = new BucketEliminator(this.variables, this.evidence, this.functions);
      this.validState = true;
      return;
    case 2:
      this.read(args[0], args[1]);
      this.eliminationAlgorithm = GraphModelEliminationAlgorithmType.VE_BUCKET_ELIMINATION;
      this.orderHeuristic = GraphModelOrderHeuristicType.MIN_DEGREE;
      this.eliminator = new BucketEliminator(this.variables, this.evidence, this.functions);
      this.validState = true;
      return;
    case 4:
      if (Integer.parseInt(args[2]) == 0) {
        this.eliminationAlgorithm = GraphModelEliminationAlgorithmType.VE_BUCKET_ELIMINATION;
      } else {
        System.out.println(this.printCmdLineHelp());
        return;
      }
      if (Integer.parseInt(args[3]) == 0) {
        this.orderHeuristic = GraphModelOrderHeuristicType.MIN_DEGREE;
      } else {
        System.out.println(this.printCmdLineHelp());
        return;
      }
      this.read(args[0], args[1]);      
      this.eliminator = new BucketEliminator(this.variables, this.evidence, this.functions);      
      this.validState = true;      
      return;
    case 7:
      if (Integer.parseInt(args[2]) == 1) {
        this.eliminationAlgorithm = GraphModelEliminationAlgorithmType.VE_WCUTSET_BUCKET_ELIMINATION;
      } else {
        System.out.println(this.printCmdLineHelp());
        return;
      }
      if (Integer.parseInt(args[3]) == 0) {
        this.orderHeuristic = GraphModelOrderHeuristicType.MIN_DEGREE;
      } else {
        System.out.println(this.printCmdLineHelp());
        return;
      }
      Integer maxClusterSize = Integer.parseInt(args[4]);
      Integer maxIterations = Integer.parseInt(args[5]);
      WCutsetBucketEliminatorSampleMode sampleMode;
      if (Integer.parseInt(args[6]) == 0) {
        sampleMode = WCutsetBucketEliminatorSampleMode.UNIFORM;
      } else if (Integer.parseInt(args[6]) == 1) {
        sampleMode = WCutsetBucketEliminatorSampleMode.DYNAMIC;
      } else {
        System.out.println(this.printCmdLineHelp());
        return;
      }
      this.read(args[0], args[1]);      
      this.eliminator = new WCutsetBucketEliminator(
          this.variables, 
          this.evidence, 
          this.functions,
          maxClusterSize,
          maxIterations,
          sampleMode);      
      this.validState = true;      
      return;
    default:
      System.out.println(this.printCmdLineHelp());
      break;
    }
  }
  
  private String printCmdLineHelp() {
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
  
  public void read(String networkConfigPath, String evidencePath) throws IOException { 
    Stopwatch stopwatch = new Stopwatch();
    stopwatch.start();
    
    try {
      System.out.println("Reading " + networkConfigPath);
      
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
        this.variables.add(new Variable(i, networkConfigScanner.nextInt()));
      }
      
      // Read functions and their scopes
      int numFunctions = networkConfigScanner.nextInt();
      for (int i = 0; i < numFunctions; i++) {
        FunctionTable f = new FunctionTable();
        int fScopeSize = networkConfigScanner.nextInt();        
        for (int j = 0; j < fScopeSize; j++) {
          f.variables.add(this.variables.get(networkConfigScanner.nextInt()));
        }
        this.functions.add(f);        
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
        System.out.println("Reading " + evidencePath);
        
        FileReader evidenceFile = new FileReader(evidencePath);
        Scanner evidenceScanner = new Scanner(evidenceFile);
        
        // Read evidence and store
        int numEvidence = evidenceScanner.nextInt();
        for (int i = 0; i < numEvidence; i++) {
          this.evidence.add(evidenceScanner.nextInt(), evidenceScanner.nextInt());
        }
        
        // All done reading evidence file
        evidenceScanner.close();
        evidenceFile.close();
      }
    } catch(Exception e) {
      e.printStackTrace();
      throw e;
    }
    
    stopwatch.stop();
    System.out.println("Read files: " + stopwatch);
  }
  
  public void print(PrintStream out) {
    if (out == null) {
      return;
    }
    
    out.println(this.type.toString());
    out.println(this.eliminationAlgorithm.toString());
    out.println(this.orderHeuristic.toString());  
    
    out.println(this.variables);    
    
    for (FunctionTable f : this.functions) {
      out.println(f.toRealString());
    }
    
    if (this.evidence.isEmpty()) {
      out.println("No evidence supplied.");
    } else {
      out.println(this.evidence);
    }
  }

  public String toString() {
    StringBuilder out = new StringBuilder();    
    out.append(this.type.toString() + "\n");
    out.append(this.eliminationAlgorithm.toString() + "\n");
    out.append(this.orderHeuristic.toString() + "\n");      
   
    out.append(this.variables + "\n");    
    
    for (FunctionTable f : this.functions) {
      out.append(f.toRealString() + "\n");
    }
    
    if (this.evidence.isEmpty()) {
      out.append("No evidence supplied.\n");
    } else {
      out.append(this.evidence + "\n");
    }
    return out.toString();
  }
   
  public LogNumber computeZorP() {
    if (!this.validState) {
      System.err.println("The graph model is not in a valid state. Use setup().");
      return new LogNumber(0.0);
    }
    
    Stopwatch stopwatch = new Stopwatch();
    stopwatch.start();
    
    if (this.type == GraphModelType.BAYES && this.evidence.isEmpty()) {
      return new LogNumber(1.0);
    }
    
    LogNumber value = new LogNumber(0);

    value = eliminator.eliminate();
    
    stopwatch.stop();
    System.out.println("Computed Z or P: " + stopwatch);
    
    return value;
  }
}

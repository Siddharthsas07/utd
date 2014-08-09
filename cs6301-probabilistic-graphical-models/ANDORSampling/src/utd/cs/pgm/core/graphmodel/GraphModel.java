package utd.cs.pgm.core.graphmodel;

import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Scanner;


import utd.cs.pgm.core.function.*;
import utd.cs.pgm.core.variable.*;
import utd.cs.pgm.util.LogDouble;

// The graphmodel has a set of variables and a set of functions over those variables.
// It has a type of BAYES or MARKOV.
public class GraphModel {
  protected GraphModelType type;
  protected ArrayList<IVariable> variables = new ArrayList<IVariable>();
  protected ArrayList<IFunction> functions = new ArrayList<IFunction>();

  // This is used only in the junction tree for tracking which functions
  // have already been added to clusters.
  protected ArrayList<Boolean> marked = new ArrayList<Boolean>();

  // This was to be used when creating the junction tree in parallel.
  public Object lock1 = new Object();

  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append(this.type + "\n");
    s.append("v: [" + this.variables.size() + "]\n");
    for (IVariable v : this.variables) {
      s.append(v + "\n");
    }
    s.append("\nf: [" + functions.size() + "]\n");
    for (IFunction f : functions) {
      s.append(f + "\n");
    }
    return s.toString();
  }

  public void print(PrintStream out) {
    out.print(this);
  }

  public void readUAI(String path) throws IOException {
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
      this.variables.add(new Variable(i, -1, sc.nextInt()));
    }
    variables.trimToSize();

    // Read functions and their scopes
    int numFunctions = sc.nextInt();
    this.functions.ensureCapacity(numFunctions);
    for (int i = 0; i < numFunctions; i++) {
      IFunction f = new Function();
      int fScopeSize = sc.nextInt();
      f.getVariables().ensureCapacity(fScopeSize);
      for (int j = 0; j < fScopeSize; j++) {
        f.getVariables().add(this.variables.get(sc.nextInt()));
      }
      f.getVariables().trimToSize();
      this.functions.add(f);
    }
    this.functions.trimToSize();

    // Read function table values
    for (IFunction f : this.functions) {
      this.marked.add(false);
      int fTableSize = sc.nextInt();
      f.getTable().ensureCapacity(fTableSize);
      for (int i = 0; i < fTableSize; i++) {
        f.getTable().add(new LogDouble(sc.nextDouble()));
      }
      f.getTable().trimToSize();
    }

    sc.close();
    fr.close();
  }

  public void readUAIevid(String path) throws IOException {
    FileReader fr = new FileReader(path);
    Scanner sc = new Scanner(fr);

    // Read evidence variables
    int numEvidence = sc.nextInt();
    for (int i = 0; i < numEvidence; i++) {
      this.variables.get(sc.nextInt()).setEvid(sc.nextInt());
    }

    sc.close();
    fr.close();
  }

  public boolean functionIsMarked(int i) {
    return this.marked.get(i);
  }

  public synchronized void markFunction(int i) {
    this.marked.set(i, true);
  }

  public void unmarkAll() {
    int size = this.marked.size();
    for(int i = 0; i < size; i++) {
      this.marked.set(i,false);
    }
  }

  // Create a moralized version of this graphmodel and return it.
  public ArrayList<HashSet<IVariable>> moralizeGraph() {
    // Create an empty structure. One HashSet for each variable's interactions.
    ArrayList<HashSet<IVariable>> structure = new ArrayList<HashSet<IVariable>>();
    for(int i = 0; i < variables.size(); i++) {
      structure.add(new HashSet<IVariable>());
    }

    // For the variables in each function, add them to each others' interactions.
    for(IFunction f : functions) {
      for(IVariable v : f.getVariables()) {
        for(IVariable v2: f.getVariables()) {
          structure.get(v.getId()).add(v2);
        }
      }
    }

    return structure;
  }

  public ArrayList<IFunction> getFunctions(){
    return this.functions;
  }

  public ArrayList<IVariable> getVariables(){
    return this.variables;
  }

  // Project the example onto each function and return the value.
  public LogDouble computeProbabilityOfFullAssignment(ArrayList<Integer> example) {
    // Probability of the example starts at 1.
    LogDouble p = LogDouble.LS_ONE;

    // For each function
    for (IFunction f : this.functions) {
      // Reduce the example to the context of f
      ArrayList<Integer> t = new ArrayList<Integer>();
      for (IVariable v : f.getVariables()) {
        t.add(example.get(v.getId()));
      }

      // Get the value from f corresponding to the example's reduced context
      int idx = f.getIndexFromAssignment(t);
      p = p.mul(f.getTable().get(idx));
    }

    return p;
  }

  public GraphModelType getType(){
    return this.type;
  }
}

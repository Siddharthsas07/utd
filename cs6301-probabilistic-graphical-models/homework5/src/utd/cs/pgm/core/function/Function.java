package utd.cs.pgm.core.function;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Random;

import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;

public class Function {
  public int id;
  public ArrayList<Variable> variables = new ArrayList<Variable>();
  public ArrayList<LogDouble> table = new ArrayList<LogDouble>();

  public Function() {
    this.id = -1;
  }
  
  public Function(
      int id, 
      ArrayList<Variable> variables,
      ArrayList<LogDouble> table) {
    this.id = id;
    this.variables = variables;
    this.table = table;
  }
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("v: [" + this.variables.size() + "] ");
    for (Variable v : this.variables) {
      s.append(v.getId() + " ");
    } s.append("\nt: [" + this.table.size() + "] ");
    for (LogDouble n : this.table) {
      s.append(String.format("%.6f ", n.getValue()));
    }
    return s.toString();
  }
  
  public String toRealString() {
    StringBuilder s = new StringBuilder();
    s.append("v: [" + this.variables.size() + "] ");
    for (Variable v : this.variables) {
      s.append(v.getId() + " ");
    } s.append("\nt: [" + this.table.size() + "] ");
    for (LogDouble n : this.table) {
      s.append(String.format("%.6f ", n.getRealValue()));
    }
    return s.toString();
  }
  
  public static String functionCollectionToString(Collection<Function> functions) {
    StringBuilder s = new StringBuilder();
    s.append("\nf: [" + functions.size() + "]\n");
    for (Function f : functions) {
      s.append(f.toString() + "\n");
    }
    return s.toString();
  }
  
  public static String functionCollectionToRealString(Collection<Function> functions) {
    StringBuilder s = new StringBuilder();
    s.append("\nf: [" + functions.size() + "]\n");
    for (Function f : functions) {
      s.append(f.toRealString() + "\n");
    }
    return s.toString();
  }
  
  public void print(PrintStream out) {
    out.println(toRealString());
  }

  public int getId() {
    return id;
  }

  public void setId(int id) {
    this.id = id;
  }
  
  public boolean isTrivial() {
    return this.variables.isEmpty() && (this.table.size() == 1);
  }
  
  public boolean isEmpty() {
    return this.variables.isEmpty() && this.table.isEmpty();
  }
  
  public static Function product(ArrayList<Function> functions) {
    return new Function();
  }
  
  public static Function sumOut(ArrayList<Function> functions) {
    return new Function();
  }
  
  public static Function productSumOut(
      ArrayList<Variable> resultScope,
      ArrayList<Function> functions,
      boolean reNormalize) {/*
  public static Function productSumOut(
      HashSet<Variable> resultScope,
      ArrayList<Function> functions,
      boolean reNormalize) {*/
    // Create scope of resulting function. Union all variables
    // from the functions and remove variables which are evidence.
    HashSet<Variable> resultantVariables = new HashSet<Variable>();
    
    for (Function f : functions) {
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
    
    ArrayList<Variable> rVarsAsAList = new ArrayList<Variable>(resultantVariables);
    rVarsAsAList.trimToSize();
    ArrayList<Variable> marg_vars = new ArrayList<Variable>(rVarsAsAList);
    marg_vars.retainAll(resultScope);
    marg_vars.trimToSize();
    // Handle case where some variables in desired resultScope are not part of 
    // possible resultantVariables
    Function r = new Function();
    r.variables.addAll(marg_vars);
    r.variables.trimToSize();
    
    /*System.out.println("PSO Resultant Variables: ");
    for (Variable v : r.variables) {
      System.out.println(v);
    } */     
    
    long numValues = Variable.productDomainSize(rVarsAsAList);
    
    long rTableSize = Variable.productDomainSize(marg_vars);
    r.table.ensureCapacity((int)rTableSize);
    for (int i = 0; i < rTableSize; i++) {
      r.table.add(new LogDouble(0.0));
    }
    r.table.trimToSize();
    
    for (int i = 0; i < numValues; i++) {
      Variable.setAddress(rVarsAsAList, i);
      LogDouble value = new LogDouble(1.0);
      for (Function f : functions) {
        int func_entry = Variable.getAddress(f.variables);
        LogDouble nVal = value.mul(f.table.get(func_entry));
        //System.out.println(nVal.toRealString() + " = " + value.toRealString() + " * " + f.table.get(func_entry).toRealString());
        value = nVal;
      }
      int idx = Variable.getAddress(marg_vars);
      
      LogDouble nVal = r.table.get(idx).add(value);
      
      //System.out.println(nVal.toRealString() + " = " + value.toRealString() + " + " + r.table.get(idx).toRealString());
      
      r.table.set(idx, nVal);
    }
    
    if (reNormalize) {
      r = r.normalize();
    }
    
    return r;
  }
  
  public Function normalize() {
    if (isTrivial() || isEmpty()) {
      return this;
    }
    
    // Create result function
    Function r = new Function();
    
    // Copy scope over
    for (Variable v : this.variables) {
      r.variables.add(v.copy());
    }
    r.variables.trimToSize();
    
    // Normalize
    int setSize = this.variables.get(this.variables.size()-1).getDomainSize();
    
    for (int i = 0; i < this.table.size(); i+=setSize) {
      LogDouble divisor = new LogDouble(0.0);
      for (int j = i; j < i+setSize; j++) {
        divisor = divisor.add(this.table.get(j));
      }
      
      for (int j = i; j < i+setSize; j++) {
        r.table.add(this.table.get(j).div(divisor));
      }      
    }
    
    r.table.trimToSize();
    
    return r;
  }
  
  public Function initTableToUniform() {
    if (isEmpty()) {
      return this;
    } else if (isTrivial()) {
      Function r = new Function();
      r.table.add(LogDouble.LS_ONE);
      r.table.trimToSize();
      return r;
    }
    
    // Create result function
    Function r = new Function();
    
    // Copy scope over
    for (Variable v : this.variables) {
      r.variables.add(v.copy());
    }
    r.variables.trimToSize();
    
    long tableSize = Variable.productDomainSize(this.variables);
    
    // Normalize
    int setSize = this.variables.get(this.variables.size()-1).getDomainSize();
    LogDouble value = new LogDouble(1.0 / setSize);
    
    for (int i = 0; i < tableSize; i++) {
        r.table.add(value);
    }    
    r.table.trimToSize();
    
    return r;
  }
  
  public Function initTableToRandom(boolean reNormalize) {
    this.table.clear();
    
    Random rng = new Random(System.nanoTime());
    
    long tableSize = Variable.productDomainSize(this.variables);
    
    // Create result function
    Function r = new Function();
    
    // Copy scope over
    for (Variable v : this.variables) {
      r.variables.add(v.copy());
    }
    
    // Put random numbers into the table
    for (int i = 0; i < tableSize; i++) {
      this.table.add(new LogDouble(rng.nextDouble()));
    }
    
    if (reNormalize) {
      return this.normalize();
    } else {
      this.table.trimToSize();
      this.variables.trimToSize();
      return this;
    }
    
  }
}

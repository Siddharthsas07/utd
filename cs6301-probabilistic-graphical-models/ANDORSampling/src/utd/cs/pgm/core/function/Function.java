package utd.cs.pgm.core.function;

import java.util.ArrayList;
import java.util.Random;

import utd.cs.pgm.core.variable.*;
import utd.cs.pgm.util.LogDouble;

public class Function implements IFunction {
  // The variables in the scope of this function.
  ArrayList<IVariable> variables = new ArrayList<IVariable>();

  // The table of values for this function. Each entry in the table corresponds
  // to the next incremental assignment of values to the variables. That is,
  // if the variables are {A,B} then table[0] = {0,0}, table[1] = {0,1},
  // table[2] = {1,0}, and table[3] = {1,1}. The table should always be
  // of length equal to the product of the domain sizes of the variables.
  ArrayList<LogDouble> table = new ArrayList<LogDouble>();

  // A string which prints out the values in the table in log space.
  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("v: [" + this.variables.size() + "] ");
    for (IVariable v : this.variables) {
      s.append(v.getId() + " ");
    } s.append("\nt: [" + this.table.size() + "] ");
    for (LogDouble n : this.table) {
      s.append(String.format("%.6f ", n.getValue()));
    }
    return s.toString();
  }

  // A string which prints out the values in the table in real space.
  public String toRealString() {
    StringBuilder s = new StringBuilder();
    s.append("v: [" + this.variables.size() + "] ");
    for (IVariable v : this.variables) {
      s.append(v.getId() + " ");
    } s.append("\nt: [" + this.table.size() + "] ");
    for (LogDouble n : this.table) {
      s.append(String.format("%.6f ", n.getRealValue()));
    }
    return s.toString();
  }

  // Following 3 functions are unused because we accidentally named them
  // something else and went with the different names.
  @Override
  public LogDouble rowToValue(ArrayList<Integer> row) {
    return null;
  }

  @Override
  public int rowToIndex(ArrayList<Integer> row) {
    return 0;
  }

  @Override
  public ArrayList<Integer> indexToRow(int index) {
    return null;
  }

  @Override
  public ArrayList<IVariable> getVariables() {
    return this.variables;
  }

  public void setVariables(ArrayList<IVariable> vars) {
    this.variables = vars;
  }

  public ArrayList<LogDouble> getTable() {
    return this.table;
  }

  public void setTable(ArrayList<LogDouble> table) {
    this.table = table;
  }

  // Consider the evidence set for each variable in this function and compute
  // an index for the evidence into this function's table. This is the same
  // as converting a number whose digits have arbitrary bases into a number
  // in base 10.
  @Override
  public int getIndexFromEvidence() {
    int domain = 1;
    int sum = 0;
    for(int i = this.variables.size()-1; i >= 0; i--)
    {
      sum += this.variables.get(i).getEvid() * domain;
      domain *= this.variables.get(i).getDomainSize();
    }

    return sum;
  }

  // Consider the evidence set for each variable in this function as if it
  // were set by the value in assignment and compute
  // an index for the evidence into this function's table. This is the same
  // as converting a number whose digits have arbitrary bases into a number
  // in base 10.
  @Override
  public int getIndexFromAssignment(ArrayList<Integer> assignment) {
    int domain = 1;
    int sum = 0;
    for(int i = this.variables.size()-1; i >= 0; i--)
    {
      sum += assignment.get(i) * domain;
      domain *= this.variables.get(i).getDomainSize();
    }

    return sum;
  }

  //this sets the evidence (always returns null) for a given
  //index. (maybe change this to setEvidenceFromIndex?)
  @Override
  public ArrayList<IVariable> getEvidenceFromIndex(int idx) {
    // TODO Auto-generated method stub
    int vars = this.variables.size();

    int domain = 1;
    for(int i = 0; i < vars; i++)
      domain *= this.variables.get(i).getDomainSize();

    for(int i = 0; i < vars; i++) {
      domain /= this.variables.get(i).getDomainSize();
      this.variables.get(i).setEvid(idx/domain);
      idx -= this.variables.get(i).getEvid() * domain;
    }

    return null;
  }

  // Sets the table to the uniform value given the variables have already
  // been set then returns the new table.
  @Override
  public Function tableToUniform() {
    // Create result function
    Function r = new Function();

    // Copy scope over
    for (IVariable v : this.variables) {
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

  // Sets the this table of values to random values in [0,1] and then normalizes
  // if reNormalize is true. Then returns the new function.
  @Override
  public Function tableToRandom(boolean reNormalize) {
    this.table.clear();

    Random rng = new Random(System.nanoTime());

    long tableSize = Variable.productDomainSize(this.variables);

    // Create result function
    Function r = new Function();

    // Copy scope over
    for (IVariable v : this.variables) {
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

  // Normalize this function's table, assuming variables have already
  // been set. Then returns the normalized function.
  @Override
  public Function normalize() {
    // Create result function
    Function r = new Function();

    // Copy scope over
    for (IVariable v : this.variables) {
      r.variables.add(v.copy());
    }
    r.variables.trimToSize();

    // Normalize
    int setSize = this.variables.get(this.variables.size()-1).getDomainSize();

    for (int i = 0; i < this.table.size(); i+=setSize) {
      LogDouble divisor = LogDouble.LS_ZERO;
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

  // Deep copies this function and returns the deep copy.
  public Function clone() {
    // Create result function
    Function r = new Function();

    // Copy scope over
    for (IVariable v : this.variables) {
      r.variables.add(v.copy());
    }
    r.variables.trimToSize();

    // Copy table over
    for (LogDouble d : this.table) {
      r.table.add(d.copy());
    }
    r.table.trimToSize();

    return r;
  }
}

package utd.cs.pgm.probability;

import java.util.ArrayList;
import java.util.Random;

import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;

public class DynamicDistribution implements ProbabilityDistribution {
  ArrayList<Variable> variables = new ArrayList<Variable>();
  ArrayList<ArrayList<LogDouble>> P = new ArrayList<ArrayList<LogDouble>>();
  ArrayList<ArrayList<LogDouble>> W = new ArrayList<ArrayList<LogDouble>>();
  Random rng = new Random(System.currentTimeMillis());  
  int sampleCounter = 0;
  int updateAfterSampleCount = 0;
  
  public DynamicDistribution(ArrayList<Variable> variables, int updateAfterSamples) {      
    // Create P and W tables
    for (Variable v : variables) {
      this.variables.add(v.copy());
      
      // Fill with uniform value for each variable's distribution
      ArrayList<LogDouble> pd = new ArrayList<LogDouble>(v.getDomainSize());
      ArrayList<LogDouble> wd = new ArrayList<LogDouble>(v.getDomainSize());
      for (int i = 0; i < v.getDomainSize(); i++) {
        double uniformValue = 1.0 / v.getDomainSize();
        pd.add(new LogDouble(uniformValue));
        //wd.add(new LogDouble(0.0));
        wd.add(new LogDouble(uniformValue));
      }
      pd.trimToSize();
      wd.trimToSize();
      this.P.add(pd);
      this.W.add(wd);
    }
    
    this.P.trimToSize();
    this.W.trimToSize();
    this.variables.trimToSize();
    
    //System.out.println("Initial Distributions:");
    //System.out.println(this);
    
    this.updateAfterSampleCount = updateAfterSamples;
  }
  
  @Override
  public ArrayList<Variable> generateSample() {
    // Loop over all variables.
    // Roll a rand() in [0,1] for each variable in the distribution.
    // Then setEvidence on the appropriate value from its distribution.
    // Pick rand() % domainSize for each variable and return it.
    LogDouble rngVal = null;
    int evidVal = 0;
    for (int i = 0; i < P.size(); i++) {
      rngVal = new LogDouble(rng.nextDouble());
      evidVal = getAssignmentFor(rngVal, P.get(i)); 
      this.variables.get(i).setEvidence(evidVal);
    }
    
    return this.variables;
  }
  
  // Loop upward from 0 until we find an assignment for v
  protected int getAssignmentFor(LogDouble v, ArrayList<LogDouble> t) {
    if (t.isEmpty()) {
      return -1;
    } else if (t.size() == 1) {
      return 0;
    }
    
    LogDouble temp = new LogDouble(0.0);
    
    for (int i = 0; i < t.size(); i++) {
      temp = temp.add(t.get(i));
      if (v.compareTo(temp) == -1) {
        return i;
      }
    }
    
    return 0;    
  }

  @Override
  public LogDouble probabilityOf(ArrayList<Variable> assignment) {
    if (this.P.size() != assignment.size()) {
      System.err.println("Mismatched assignment query.");
      return new LogDouble(0.0);
    }
    
    LogDouble v = new LogDouble(1.0);
    
    for (int i = 0; i < this.P.size(); i++) {
      v = v.mul(this.P.get(i).get(assignment.get(i).getValue()));
    }
    
    return v;
  }
  
  @Override
  public String printSample(ArrayList<Variable> sample) {
    StringBuilder s = new StringBuilder();
    
    s.append("sample = \n");
    for (Variable v : sample) {
      s.append(v + "\n");
    }
    
    return s.toString();
  }
  
  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();
    
    s.append("V = \n");
    for (Variable v : this.variables) {
      s.append(v + "\n");
    }
    s.append("P = \n");
    for (ArrayList<LogDouble> t : P) {
      for (LogDouble d : t) {
        s.append(d.toRealString() + " ");
      }
      s.append("\n");
    }
    s.append("W = \n");
    for (ArrayList<LogDouble> t : W) {
      for (LogDouble d : t) {
        s.append(d.toRealString() + " ");
      }
      s.append("\n");
    }
    
    return s.toString();
  }

  @Override
  public void updateWeights(LogDouble w) {
    //System.out.println("Updating W with weight = " + w.toRealString());
    
    // Update weights by adding w to each index of W where it's consistent with
    // the last sample returned. 
    for (int i = 0; i < this.W.size(); i++) {
      LogDouble v = this.W.get(i).get(this.variables.get(i).getValue());
      //System.out.println("v = " + v.toRealString());
      LogDouble nVal = v.add(w);
      //System.out.println("nval = " + nVal.toRealString());
      
      //LogDouble qq = new LogDouble(0.0000000001);
      //LogDouble vv = qq.add(new LogDouble(1.0));
      //System.out.println("vv = " + vv.toRealString());
      
      this.W.get(i).set(this.variables.get(i).getValue(), nVal);
    }
    
    /*System.out.println("Updated W table:");
    for (ArrayList<LogDouble> t : W) {
      for (LogDouble d : t) {
        System.out.print(d.toRealString() + " ");
      }
      System.out.println();
    }*/
    
    ++sampleCounter;
    sampleCounter = sampleCounter % this.updateAfterSampleCount;
    // 100 samples have been taken since last P update. Normalize tuples in W.
    // Copy values to P.
    
    if (sampleCounter == 0) {
      // Normalize tuples in W one at a time.
      for (int i = 0; i < this.W.size(); i++) {
        LogDouble divisor = new LogDouble(0.0);
        for (int j = 0; j < this.W.get(i).size(); j++) {
          divisor = divisor.add(this.W.get(i).get(j));
        }
        
        for (int j = 0; j < this.P.get(i).size(); j++) {
          P.get(i).set(j, W.get(i).get(j).div(divisor));
        }
      }
      
      //System.out.println("LEARNED.");
      //System.out.println(this);
    }
    
    /*if (sampleCounter == 0) {
      // Normalize tuples in W one at a time.
      for (ArrayList<LogDouble> t : this.W) {
        LogDouble divisor = new LogDouble(0.0);
        for (LogDouble d : t) {
          divisor = divisor.add(d);
        }
        
        for (int i = 0; i < t.size(); i++) {
          t.set(i, t.get(i).div(divisor));
        }        
      }      
      
      // Copy values to P.
      for (int i = 0; i < P.size(); i++) {
        for (int j = 0; j < P.get(i).size(); j++) {
          P.get(i).set(j, W.get(i).get(j));
        }
      }
      
      //System.out.println("LEARNED.");
      //System.out.println(this);
    }*/
  }
}

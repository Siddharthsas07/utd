package utd.cs.pgm.probability;

import java.util.ArrayList;
import java.util.Random;

import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;

public class NormalDistribution implements ProbabilityDistribution {
  ArrayList<Variable> variables = new ArrayList<Variable>();
  long variablesDomainSize = 0;
  LogDouble probabilityOfAssignment;
  Random rng = new Random(System.currentTimeMillis());  
  
  public NormalDistribution(ArrayList<Variable> variables) {
    for (Variable v : variables) {
      this.variables.add(v.copy());
    } this.variables.trimToSize();
    
    /*System.out.println("Distribution variables:");
    for (Variable v : this.variables) {
      System.out.println(v);
    }*/
    
    this.probabilityOfAssignment = new LogDouble(1.0);
    
    for (Variable v : this.variables) {
      this.probabilityOfAssignment = this.probabilityOfAssignment.div(new LogDouble(v.getDomainSize()));
    }
    
    //this.variablesDomainSize = Variable.productDomainSize(this.variables);
    
    //System.out.println("variablesDomainSize = " + this.variablesDomainSize);
    
    //this.probabilityOfAssignment = new LogDouble(1.0 / this.variablesDomainSize);
    //System.out.println("Probability of an assignment = " + this.probabilityOfAssignment.toRealString());
  }
  
  @Override
  public ArrayList<Variable> generateSample() {
    // Pick rand() % domainSize for each variable and return it.
    for (Variable v : this.variables) {
      v.setEvidence(rng.nextInt(v.getDomainSize()));
    }
    
    return this.variables;
  }

  @Override
  public LogDouble probabilityOf(ArrayList<Variable> assignment) {
    return this.probabilityOfAssignment;
  }
  
  @Override
  public String printSample(ArrayList<Variable> sample) {
    StringBuilder s = new StringBuilder();
    
    s.append("P = \n");
    for (Variable v : sample) {
      s.append(v + "\n");
    }
    
    return s.toString();
  }
  
  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();
    
    s.append("P = \n");
    for (Variable v : this.variables) {
      s.append(v + "\n");
    }
    
    return s.toString();
  }

  @Override
  public void updateWeights(LogDouble w) {
   // System.err.println("Updating weights of a normal distribution does nothing.");    
  }
}

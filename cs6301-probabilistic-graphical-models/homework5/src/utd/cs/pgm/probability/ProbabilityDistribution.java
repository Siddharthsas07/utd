package utd.cs.pgm.probability;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;

public interface ProbabilityDistribution {
  public ArrayList<Variable> generateSample();
  
  public LogDouble probabilityOf(ArrayList<Variable> assignment);
  
  public String toString();
  
  public String printSample(ArrayList<Variable> sample);

  public void updateWeights(LogDouble w);
}

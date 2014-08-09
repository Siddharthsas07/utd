package utd.cs.pgm.probability;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.util.LogDouble;

public interface IProbabilityDistribution {
  public ArrayList<IVariable> generateSample();
  public LogDouble probabilityOf(ArrayList<IVariable> assignment);
  public String toString();
  public String printSample(ArrayList<IVariable> sample);
  public void updateWeights(LogDouble w);
}

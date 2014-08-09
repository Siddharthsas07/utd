package utd.cs.pgm.core.function;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.util.LogDouble;

public interface IFunction {
  public ArrayList<IVariable> getVariables();
  public void setVariables(ArrayList<IVariable> vars);

  public ArrayList<LogDouble> getTable();
  public void setTable(ArrayList<LogDouble> table);

  public LogDouble rowToValue(ArrayList<Integer> row);
  public int rowToIndex(ArrayList<Integer> row);

  public ArrayList<Integer> indexToRow(int index);

  public String toString();

  public int getIndexFromEvidence();
  public int getIndexFromAssignment(ArrayList<Integer> assignment);
  public ArrayList<IVariable> getEvidenceFromIndex(int idx);

  public IFunction tableToRandom(boolean reNormalize);
  public IFunction tableToUniform();
  public IFunction normalize();
}

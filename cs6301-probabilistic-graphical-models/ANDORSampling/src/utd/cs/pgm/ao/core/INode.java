package utd.cs.pgm.ao.core;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.util.LogDouble;

public interface INode {
  public INode getParent();
  public void setParent(INode n);

  public ArrayList<INode> getChildren();
  public void setChildren(ArrayList<INode> c);
  public void addChild(INode n);

  public IVariable getVariable();
  public void setVariable(IVariable v);

  public String toString();
  public LogDouble computeValue();
}

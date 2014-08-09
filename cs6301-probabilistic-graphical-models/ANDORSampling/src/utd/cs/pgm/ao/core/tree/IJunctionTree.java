package utd.cs.pgm.ao.core.tree;

import java.util.ArrayList;
import java.util.Stack;

import utd.cs.pgm.ao.core.INode;
import utd.cs.pgm.ao.core.JTNode;
import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.probability.DynamicDistributionDos;

public interface IJunctionTree {
  public void buildTree(INode pt_n, JTNode jt_n, Stack<IVariable> context, DynamicDistributionDos Q);
  public JTNode getRoot();
  public void setRoot(JTNode jt);
  public ArrayList<JTNode> getLeaves();
  public String toString();
}

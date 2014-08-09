package utd.cs.pgm.ao.core;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.util.LogDouble;

public class Node implements INode {
  IVariable var;
  ArrayList<INode> children;
  INode parent;

  // Constructor sets the parent and variable for a node.
  public Node(INode p, IVariable v)  {
    this.setParent(p);
    children = new ArrayList<INode>();
    this.setVariable(v);
  }

  @Override
  public INode getParent() {
    return this.parent;
  }

  @Override
  public ArrayList<INode> getChildren() {
    return this.children;
  }

  @Override
  public IVariable getVariable() {
    return this.var;
  }

  @Override
  public void setVariable(IVariable v) {
    this.var = v;
  }

  @Override
  public void setParent(INode n) {
    this.parent = n;
  }

  @Override
  public void setChildren(ArrayList<INode> c) {
    this.children = c;
  }

  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();

    s.append("v = " + this.var + "\n");
    s.append("p = " + this.parent + "\n");
    s.append("c = \n");
    for (INode n : this.children) {
      s.append(n + "\n");
    }

    return s.toString();
  }

  @Override
  public LogDouble computeValue() {
    return LogDouble.LS_ZERO;
  }

  @Override
  public void addChild(INode n) {
    this.children.add(n);
  }
}

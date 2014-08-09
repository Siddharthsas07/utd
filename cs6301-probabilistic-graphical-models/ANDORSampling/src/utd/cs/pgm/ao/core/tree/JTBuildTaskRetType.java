package utd.cs.pgm.ao.core.tree;

import java.util.ArrayList;

import utd.cs.pgm.ao.core.JTNode;

// This class was intended for use when building the junction graph in parallel.
// However since that task wasn't completed this isn't used.
public class JTBuildTaskRetType {
  public ArrayList<JTBuildTask> tasks;
  public ArrayList<JTNode> leaves;

  public JTBuildTaskRetType(ArrayList<JTBuildTask> tasks, ArrayList<JTNode> leaves) {
    this.tasks = tasks;
    this.leaves = leaves;
  }
}

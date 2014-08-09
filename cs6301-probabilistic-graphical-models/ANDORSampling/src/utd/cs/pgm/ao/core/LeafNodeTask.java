package utd.cs.pgm.ao.core;

import java.util.concurrent.Callable;

import utd.cs.pgm.util.LogDouble;

// A class simply for parallelizing the junction graph computation.
public class LeafNodeTask implements Callable<LogDouble> {
  // The JTNode to expand
  protected JTNode leaf;

  public LeafNodeTask(JTNode n){
    this.leaf = n;
  }

  // Computes the value for this node and returns it to the future
  // created in the JunctionTree
  @Override
  public LogDouble call() throws Exception {
    return leaf.computeNodeValue();
  }
}

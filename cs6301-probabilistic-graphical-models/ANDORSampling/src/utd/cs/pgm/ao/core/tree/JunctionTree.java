package utd.cs.pgm.ao.core.tree;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import java.util.concurrent.CompletionService;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import utd.cs.pgm.ao.core.INode;
import utd.cs.pgm.ao.core.JTNode;
import utd.cs.pgm.ao.core.LeafNodeTask;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.probability.DynamicDistributionDos;
import utd.cs.pgm.util.LogDouble;

// The junction tree/graph for computing the partition function approximation.
public class JunctionTree implements IJunctionTree {
  // The root node has its parent as null.
  protected JTNode root = new JTNode(null);

  // A list of the leaves in the junction graph.
  protected ArrayList<JTNode> leaves = new ArrayList<JTNode>();

  // A pointer to the graphmodel which this junction graph is built from.
  protected GraphModel gm;

  // The samples generated from the proposal distribution Q!
  protected ArrayList<ArrayList<Integer>> samples;

  public JunctionTree(GraphModel g, ArrayList<ArrayList<Integer>> s){
    this.gm = g;
    this.samples = s;
  }

  // This function builds the junction graph by recursively, depth first,
  // expanding each node. Fill in the node's context, add functions to it,
  // ad entries from Q to it, if it's a leaf, add it to the leaves set,
  // then expand the children nodes.
  @Override
  public void buildTree(INode pt_n, JTNode jt_n, Stack<IVariable> context, DynamicDistributionDos Q) {
    context.push(pt_n.getVariable());
    jt_n.setContext(context);
    jt_n.addFunctions(gm);
    jt_n.fillOutSparseTable(samples, Q);
    if(pt_n.getChildren().isEmpty()) {
      leaves.add(jt_n);
    }

    // Now recursively (depth first) expand the child nodes.
    int size = pt_n.getChildren().size();
    for(int i = 0; i < size; i++){
      JTNode temp = new JTNode(jt_n);
      jt_n.addChild(temp);
      buildTree(pt_n.getChildren().get(i), temp, context, Q);
    }

    // Necessary to clear this cluster's added context when the calls go back
    // up the graph one level.
    context.pop();
  }

  // This function was supposed to build the junction grpah in parallel, thus
  // speeding up the building process. However, due to messing with Q and with F
  // in parallel, there are some nuances we didn't have time to fix. We'll
  // come back to it later.
  public void buildTreeDos(INode pt_n, JTNode jt_n, Stack<IVariable> context, DynamicDistributionDos Q) throws InterruptedException, ExecutionException {
    ExecutorService es = Executors.newFixedThreadPool(1);
    CompletionService<JTBuildTaskRetType> pool = new ExecutorCompletionService<JTBuildTaskRetType>(es);

    //start at the root
    pool.submit(new JTBuildTask(jt_n, pt_n, context, Q, gm, samples));

    //every time a node returns, add its children as new tasks and any leaves are added to leaves.
    int total = 1;
    for(int i = 0; i < total; i++){
      JTBuildTaskRetType ret = pool.take().get();
      for(JTBuildTask jtb : ret.tasks){
        pool.submit(jtb);
        ++total;
      }
      for(JTNode jtl : ret.leaves)
        this.leaves.add(jtl);
    }

    //shutdown the pool
    es.shutdown();
  }

  @Override
  public JTNode getRoot() {
    return root;
  }

  @Override
  public void setRoot(JTNode jt) {
    root = jt;
  }

  @Override
  public ArrayList<JTNode> getLeaves() {
    return leaves;
  }

  // String for this tree is build recursively. Yurp.
  public String toStringHelper(int indent, JTNode n) {
    StringBuilder sb = new StringBuilder();
    sb.append(n.getContext().get(n.getContext().size()-1).getId());

    if(!n.getChildren().isEmpty())
    sb.append("->");

    for(JTNode t : n.getChildren()){
      sb.append(toStringHelper(indent+1, t));
    }
    sb.append("\n");
    for(int i = 0; i < indent; i++)
      sb.append("  ");

    return sb.toString();
  }

  @Override
  public String toString() {
    return toStringHelper(0,root);
  }

  // Parallelized computation of the partition frunction approximation (z) by
  // starting at the leaf nodes and computing up.
  public LogDouble computeZ() throws InterruptedException, ExecutionException {
    // Use a thread pool of size 16.
    ExecutorService pool = Executors.newFixedThreadPool(16);

    // Stores a list of values computed at each node. Each value comes from
    // JTNode.computeNodeValue. Thus, if the value is nonzero, it is the
    // partition function approximation.
    List<Future<LogDouble>> futures = new ArrayList<Future<LogDouble>>(leaves.size());

    // Add a task to the work queue for each leaf node.
    for(int i = 0; i < leaves.size(); i++) {
       futures.add(pool.submit(new LeafNodeTask(leaves.get(i))));
    }

    // Call .get() on each future, thus making it compute.
    LogDouble z = LogDouble.LS_ONE;
    for(Future<LogDouble> future : futures) {
      LogDouble clusterValue = future.get();

      // If the value of the cluster after its computations is nonzero, consider
      // it the partition function approximation and store it.
      if(clusterValue != LogDouble.LS_ZERO) {
        z = clusterValue;
      }
    }

    pool.shutdown();

    return z;
  }
}

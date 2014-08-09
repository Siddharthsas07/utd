package utd.cs.pgm.ao.core.tree;

import java.util.ArrayList;
import java.util.Stack;
import java.util.concurrent.Callable;

import utd.cs.pgm.ao.core.INode;
import utd.cs.pgm.ao.core.JTNode;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.probability.DynamicDistributionDos;

// This class was intended for use when creating the junction graph in parallel.
// However it turned out to be more complicated than anticipated and this
// isn't finished.
public class JTBuildTask implements Callable<JTBuildTaskRetType> {
  protected JTNode jtNode;
  protected INode ptNode;
  protected Stack<IVariable> context;
  protected DynamicDistributionDos Q;
  protected GraphModel gm;
  protected ArrayList<ArrayList<Integer>> samples;

  public JTBuildTask(JTNode n, INode p, Stack<IVariable> context, DynamicDistributionDos Q, GraphModel gm, ArrayList<ArrayList<Integer>> samples) {
    this.jtNode = n;
    this.ptNode = p;
    this.context = context;
    this.Q = Q;
    this.gm = gm;
    this.samples = samples;
  }

  @Override
  public JTBuildTaskRetType call() throws Exception {
    return buildTree();

  }

  protected JTBuildTaskRetType buildTree(){
    //array lists for return type
    ArrayList<JTBuildTask> jtArray = new ArrayList<JTBuildTask>();
    ArrayList<JTNode> leaves = new ArrayList<JTNode>();

    //add the current pseudo tree nodes variable to context
    context.push(ptNode.getVariable());

    //set the context on the jt node
    jtNode.setContext(context);

    //add the appropriate functions (addFunctions is synchronized)
    jtNode.addFunctions(gm);

    //fill out the sparse table
    jtNode.fillOutSparseTable(samples, Q);

    //if there are no children then add to leaves (the return type)
    if(ptNode.getChildren().isEmpty())
      leaves.add(jtNode);

    int size = ptNode.getChildren().size();

    //for each child create a new task to be submitted
    for(int i = 0; i < size; i++){
      JTNode temp = new JTNode(jtNode);
      jtNode.addChild(temp);
      Stack<IVariable> copyStack = new Stack<IVariable>();
      copyStack.addAll(context);
      jtArray.add(new JTBuildTask(temp, ptNode.getChildren().get(i), copyStack, Q, gm, samples));
    }

    JTBuildTaskRetType r = new JTBuildTaskRetType(jtArray, leaves);

    return r;
  }
}

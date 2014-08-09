package utd.cs.pgm.ao.core.tree;

import java.util.ArrayList;
import java.util.HashSet;

import utd.cs.pgm.ao.core.INode;
import utd.cs.pgm.ao.core.Node;
import utd.cs.pgm.core.variable.IVariable;

// This is a pseudotree, intended for generation from the initial graphmodel.
// This makes no attempt to find a minimal depth pseudotree.
public class PseudoTree implements IPseudoTree {
  INode root;

  // Takes an interaction graph in the form of HashSets for each variable,
  // indexed from 0 upward.
  public PseudoTree(ArrayList<HashSet<IVariable>> structure) {
    // DFS using the structure
    root = new Node(null, null);
    HashSet<IVariable> inTree = new HashSet<IVariable>();
    buildTree(structure,root,inTree);
  }

  // Recursively, depth-first, build the pseudotree based on the structure.
  protected void buildTree(ArrayList<HashSet<IVariable>> structure,
      INode currNode,
      HashSet<IVariable> inTree) {
    // If the current node is the root of the pseudotree
    if(currNode.getParent() == null){
      // Root it at the very first variable in the structure.
      for(IVariable v : structure.get(0)) {
        currNode.setVariable(v);
        inTree.add(currNode.getVariable());
        break;
      }
    }

    // For each variable in this variable's interactions
    for(IVariable v : structure.get(currNode.getVariable().getId())) {
      // If the variable being interacted with is already in the tree
      // meaning some other edge was created to this variable first,
      // skip it. Otherwise add it to the tree.
      if(!inTree.contains(v)){
        inTree.add(v);
        INode newNode = new Node(currNode, v);
        currNode.addChild(newNode);
        buildTree(structure, newNode, inTree);
      }
    }
  }

  @Override
  public INode getRoot() {
    return this.root;
  }

  @Override
  public void setRoot(INode n) {
    this.root = n;
  }

  // String for the tree is constructed depth first recursively.
  public String toStringHelper(int indent, INode n) {
    StringBuilder sb = new StringBuilder();

    sb.append(n.getVariable().getId());
    if(!n.getChildren().isEmpty()) {
      sb.append("->");
    }
    for(INode t : n.getChildren()) {
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

  // Construct a DFS ordering from this tree
  public void buildOrdering(INode curNode, ArrayList<IVariable> ordering) {
    ordering.add(curNode.getVariable());
    for(INode n : curNode.getChildren()){
      buildOrdering(n, ordering);
    }
  }

  // Construct a DFS ordering from this tree
  @Override
  public ArrayList<IVariable> getOrdering() {
    ArrayList<IVariable> ordering = new ArrayList<IVariable>();
    buildOrdering(this.getRoot(), ordering);
    return ordering;
  }
}

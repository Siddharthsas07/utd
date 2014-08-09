package utd.cs.pgm.ao.core;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;

import utd.cs.pgm.core.function.IFunction;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.probability.DynamicDistributionDos;
import utd.cs.pgm.util.LogDouble;
import utd.cs.pgm.util.SparseTable;

// This class is a cluster in the Junction Graph. We called it a node, but
// it's actually a cluster of variables.
public class JTNode {
  protected JTNode parent;
  protected ArrayList<JTNode> children = new ArrayList<JTNode>();

  // The context of this cluster is a set of variables including itself and
  // its ancestors from the pseudotree.
  protected ArrayList<IVariable> context = new ArrayList<IVariable>();

  // Instantiated in fillOutSparseTable.
  protected SparseTable st;

  // A list of functions matching the context of this cluster. Pointers
  // point directly to the ones in the initial GraphModel.
  protected ArrayList<IFunction> functions = new ArrayList<IFunction>();

  // A list of incoming messages. These get multiplied together and sent
  // up the junction graph towards the root.
  protected ArrayList<SparseTable> messages = new ArrayList<SparseTable>();

  // A subset of the proposal distribution relevant to the context of this
  // cluster. Contains rows from Q.
  protected ArrayList<ArrayList<LogDouble>> qSubset = new ArrayList<ArrayList<LogDouble>>();

  public JTNode(JTNode p) {
    this.parent = p;
  }

  public void setContext(Collection<IVariable> c) {
    this.context.addAll(c);
  }

  public void addChild(JTNode child) {
    this.children.add(child);
  }

  public ArrayList<JTNode> getChildren() {
    return children;
  }

  // This function multiplies the functions initially placed in this cluster
  // with samples and stores the results in st. It grabs weights from Q.
  public void fillOutSparseTable(ArrayList<ArrayList<Integer>> samples,
      DynamicDistributionDos Q) {
    this.st = new SparseTable(context);
    if (samples.isEmpty()) {
      return;
    }

    // Reduces each sample in samples to the context of this cluster
    // and stores them in st.
    for(ArrayList<Integer> s : samples) {
      ArrayList<Integer> currTuple = new ArrayList<Integer>(this.context.size());

      for (IVariable v : this.context) {
        currTuple.add(s.get(v.getId()));
      }

      this.st.insert(currTuple);
    }

    // Set the weights of each entry in the sparse table to the count of that
    // entry. This is done so division later washes out to 1.0 if needed.
    int sts = this.st.size();
    if(this.functions.isEmpty()){
      for(int i = 0; i < sts; i++)
        this.st.setWeight(i, new LogDouble(this.st.getCount(i)));
      return;
    }

    // Compute the context of all the functions in this cluster multiplied
    // together. This functionContext is a subset of the cluster context
    int fs = this.functions.size();
    int cs = this.context.size();
    HashSet<IVariable> functionContext = new HashSet<IVariable>();
    for(IFunction f : this.functions){
      functionContext.addAll(f.getVariables());
    }

    // Compute the weight for each local sample.
    for (int i = 0; i < sts; i++) {
      // Track the weight for this local sample, starting at 1.0
      LogDouble currWeight = LogDouble.LS_ONE;

      // Project the local sample onto each function in the cluster and
      // multiply the value from each function into currWeight.
      for (int j = 0; j < fs; j++) {
        // Reduces the context of an assignment to the function context so
        // we can call Function.getIndexFromAssignment.
        ArrayList<Integer> assignment = new ArrayList<Integer>();
        int fSize = this.functions.get(j).getVariables().size();
        for(int k = 0; k < fSize; k++) {
          for(int m = 0; m < cs; m++) {
            if(this.functions.get(j).getVariables().get(k).getId()==context.get(m).getId()) {
              assignment.add(st.getKey(i).get(m));
            }
          }
        }

        // Use assignment to query the current function for a value and multiply
        // into currWeight.
        int idx = this.functions.get(j).getIndexFromAssignment(assignment);
        currWeight = currWeight.mul(this.functions.get(j).getTable().get(idx));
      }

      // Multiply currWeight by the count for the local sample.
      LogDouble currCount = new LogDouble(this.st.getCount(i));
      currWeight = currWeight.mul(currCount);

      // Divide by Q's value for the local sample.
      LogDouble prob = Q.probabilityOfSubset(st.getKey(i), this.context, functionContext);
      if(prob.compareTo(LogDouble.LS_ZERO) == 0) {
        prob = LogDouble.LS_ONE;
      }
      currWeight = currWeight.div(prob);

      // Store currWeight
      st.setWeight(i, currWeight);
    }

    // Now mark all of the Q entries used for this cluster used, so that
    // no other cluster uses them and we accidentally double count.
    for(IVariable v : functionContext){
      Q.setMarked(v.getId());
    }
  }

  // Multiply all incoming messages (which are sparse tables) with this cluster's
  // sparse table to begin computing the outgoing message to the next cluster.
  // Note that every incoming message's context is a subset of this cluster's
  // context. NOTE: This operates directly on this cluster's sparse table!
  public SparseTable multiplyMessages(){
    if(this.messages.isEmpty()) {
      return this.st;
    }

    // This isn't necessary anymore, we could just call this.st later on.
    SparseTable temp = this.st;

    // Maps temp's columns into each message's columns so we can easily see
    // which entries between sparse tables match by looking at matching columns
    // and then values for rows in those columns.
    ArrayList<ArrayList<Integer>> mappings = new ArrayList<ArrayList<Integer>>();
    int sts = this.st.size();
    int contextSize = this.st.getVariables().size();

    // Look at each message
    for (SparseTable msg : this.messages) {

      // Look at the context of this node's sparse table
      int msgContextSize = msg.getVariables().size();
      ArrayList<Integer> mapping = new ArrayList<Integer>();
      for (int i = 0; i < contextSize; i++) {
        for (int j = 0; j < msgContextSize; j++) {
          // If the variable IDs match, add an index for this cluster's st context
          // to the mapping. This is because the incoming messages' contexts
          // are a subset of this cluster's context.
          if (this.st.getVariables().get(i).getId() == msg.getVariables().get(j).getId()) {
            mapping.add(i);
          }
        }
      }
      mappings.add(mapping);
    }

    // With mappings in hand to quickly compute it, multiply incoming messages
    // with this cluster's sparse table
    int numMessages = this.messages.size();

    // For each entry in this cluster's sparse table
    for (int i = 0; i < sts; i++) {
      // For each incoming message
      for (int j = 0; j < numMessages; j++) {
        // Reduce the entry in this sparse table to the context of the incoming message
        // so we can ask the message's sparse table for a weight easily
        ArrayList<Integer> prunedTuple = new ArrayList<Integer>();
        int mappingSize = mappings.get(j).size();
        for (int k = 0; k < mappingSize; k++) {
          prunedTuple.add(temp.getKey(i).get(mappings.get(j).get(k)));
        }

        // Actually query the message table for a weight
        LogDouble w = this.messages.get(j).getWeight(prunedTuple);

        // Multiply the message's weight into this sparse table
        temp.setWeight(i, temp.getWeight(i).mul(w));
      }
    }

    // Return the result of multiplying all incoming messages into this cluster's
    // sparse table.
    return temp;
  }

  // Compute the value of this cluster. If this cluster is the root cluster
  // (because its parent is null) this returns the parition function
  // approximation. Otherwise this returns 0.
  public LogDouble computeNodeValue() {
    LogDouble value = LogDouble.LS_ZERO;

    //handle multiplication of messages
    SparseTable temp = multiplyMessages();

    // If this cluster is the root cluster, average out all the variables
    // in the sparse table. This is the value for the partition function
    // approximation.
    if(this.parent==null) {
      int size = temp.size();
      // Track the sum of the counts so we can average out later.
      long sum = 0;
      for(int i = 0; i < size; i++) {
        value = value.add(temp.getWeight(i));
        sum += temp.getCount(i);
      }
      value = value.div(new LogDouble(sum));
      return value;
    }

    // if this cluster isn't the root cluster, compute the message to the parent
    // cluster and add it to that parent's messages.
    SparseTable msg = computeMessageToParent(temp);
    value = this.parent.addMessage(msg);

    return value;
  }

  // Add an incoming message to this cluster's list of messages.
  public synchronized LogDouble addMessage(SparseTable st){
    LogDouble res = LogDouble.LS_ZERO;
    messages.add(st);

    // If this cluster has received all of its expected incoming messages
    // one from each child cluster, compute the value of the cluster
    // and return it.
    if(messages.size()==this.children.size()) {
      res = computeNodeValue();
    }

    return res;
  }

  public ArrayList<IVariable> getContext(){
    return this.context;
  }

  // Whereas multiplyMessages is a product, this is an average out.
  // Average out the variables not in the parent cluster's context from this
  // cluster's sparse table, thus giving us the message to the parent.
  public SparseTable computeMessageToParent(SparseTable temp) {
    // Compute the message context for the parent (intersection of variable ids)
    ArrayList<IVariable> msgContext = new ArrayList<IVariable>(this.context);
    msgContext.retainAll(this.parent.context);

    // Create a new sparse table for the resulting message over the computed context.
    SparseTable r = new SparseTable(msgContext);

    // Compute which columns must be kept from this cluster's sparse table
    // to have msgContext be the context of the outgoing message.
    ArrayList<Integer> indicesForContext = new ArrayList<Integer>(msgContext.size());
    int cs = this.context.size();
    for (int i = 0; i < cs; i++) {
      // This only works because the pointers to variables in the contexts are
      // the same pointers: to the variables in the initial graphmodel.
      if (msgContext.contains(this.context.get(i))) {
        indicesForContext.add(i);
      }
    }

    // For each entry in this cluster's sparse table, reduce that entry
    // to the context of the outgoing message.
    int sts = temp.size();
    int ifc = indicesForContext.size();
    // For each entry in this cluster's sparse table.
    for (int i = 0; i < sts; i++) {
      // Create a new entry for the outgoing message and add variables
      // specific to the outgoing message context.
      ArrayList<Integer> rEntry = new ArrayList<Integer>(msgContext.size());
      for (int j = 0; j < ifc; j++) {
        rEntry.add(temp.getKey(i).get(indicesForContext.get(j)));
      }

      // Add the new entry to the outgoing message.
      r.insertSumWeights(rEntry, temp.getCount(i), temp.getWeight(i));
    }

    // Average the weight for each entry in the outgoing message by dividing
    // by the count for that entry.
    int rsize = r.size();
    for(int i = 0; i < rsize; i++) {
      r.setWeight(i, r.getWeight(i).div(new LogDouble(r.getCount(i))));
    }

    // Return the averaged-out message.
    return r;
  }

  // Place functions from gm into this cluster if the context matches and
  // that function hasn't already been used in another cluster.
  public void addFunctions(GraphModel gm) {
    // This lock was initially intended for use whenever we were trying to
    // parallelize creation of the tree. It's not really necessary with a
    // serial implementation.
    synchronized(gm.lock1) {
      // For each function in gm
      int fsize = gm.getFunctions().size();
      for(int i = 0; i < fsize; i++){
         // If the function context matches this cluster and it hasn't been marked.
        if(!gm.functionIsMarked(i)
            && context.containsAll(gm.getFunctions().get(i).getVariables())) {
          // Add the function to this cluster and mark it used so no other
          // cluster uses this function (avoid double counting!)
          this.functions.add(gm.getFunctions().get(i));
          gm.markFunction(i);
        }
      }
    }
  }

  // Place entries from Q into this cluster if the context matches and
  // that qEntry hasn't been placed in another cluster already.
  public void addQ(DynamicDistributionDos q) {
    // This lock was initially intended for use whenever we were trying to
    // parallelize creation of the tree. It's not really necessary with a
    // serial implementation.
    synchronized(q.lock1) {
      // Get the context of all functions as if they were multiplied together
      HashSet<IVariable> functionContext = new HashSet<IVariable>();
      for(IFunction f : this.functions) {
        functionContext.addAll(f.getVariables());
      }

      // For each variable in the functionContext, if it isn't already used
      // in another cluster, use it in this cluster.
      for(IVariable v : functionContext) {
        if (!q.getMarked(v.getId())) {
          this.qSubset.add(q.getDistribution().get(v.getId()));
          q.setMarked(v.getId());
        }
      }
    }
  }
}

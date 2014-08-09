package utd.cs.pgm.probability;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;

import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.util.LogDouble;

// This class holds a probability distribution over some variables. It assumes
// the variables are independent. Not just conditionally independent.
public class DynamicDistributionDos {
  // This is the table of probabilities for each variable. It has length equal
  // to the number of variables sent in from the constructor. Each entry in it
  // has length equal to the domain size of the corresponding variable in the
  // constructor.
  protected ArrayList<ArrayList<LogDouble>> q = new ArrayList<ArrayList<LogDouble>>();

  // q's size so we don't have to call q.size() repeatedly everywhere.
  protected int qSize = 0;

  // This is used solely for allocating pieces of q to clusters in the junction
  // graph.
  protected ArrayList<Boolean> marked = new ArrayList<Boolean>();

  // A set of weights. Each weight corresponds to a single sample generated.
  // These are used to update later.
  protected ArrayList<LogDouble> w = new ArrayList<LogDouble>();

  // Used for randomly generating a value for a variable when asked for a sample.
  protected Random rng = new Random(System.nanoTime());

  // This was to be used whenever the junction graph was being created in
  // parallel.
  public Object lock1 = new Object();

  // The variables this was created for.
  protected ArrayList<IVariable> vars = new ArrayList<IVariable>();

  // Sets up the initial q.
  public DynamicDistributionDos(ArrayList<IVariable> initialSetup){
    this.vars = initialSetup;

    // For each variable in the initial set
    for (IVariable v : initialSetup) {

      // Create a qEntry for it as the uniform if it is not evidence. If it
      // has already been set as evidence, set all non-evidence probabilities
      // to 0 and the evidence probability to 1. This keeps us from
      // sampling nonevidence values for variables.
      int vDomSize = v.getDomainSize();
      ArrayList<LogDouble> qEntry = new ArrayList<LogDouble>(vDomSize);

      if (v.isEvid()) {
        int evidVal = v.getEvid();
        for (int i = 0; i < vDomSize; i++) {
          if (i == evidVal) {
            qEntry.add(LogDouble.LS_ONE);
          } else {
            qEntry.add(LogDouble.LS_ZERO);
          }
        }
      } else {
        for (int i = 0; i < vDomSize; i++) {
          double uniformValue = 1.0 / vDomSize;
          qEntry.add(new LogDouble(uniformValue));
        }
      }

      qEntry.trimToSize();
      this.q.add(qEntry);
      this.marked.add(false);
    }

    this.qSize = this.q.size();
  }

  public ArrayList<ArrayList<Integer>> generateSamples(int numSamples, GraphModel gm) {
    // Loop over all variables.
    // Roll a rand() in [0,1] for each variable in the distribution.
    // Then setEvidence on the appropriate value from its distribution.
    // Pick rand() % domainSize for each variable and return it.
    this.w.clear();
    LogDouble rngVal = null;
    int evidVal = 0;
    ArrayList<ArrayList<Integer>> samples = new ArrayList<ArrayList<Integer>>();

    for (int i = 0; i < numSamples; i++) {
      ArrayList<Integer> sample = new ArrayList<Integer>();

      for (int j = 0; j < qSize; j++) {
          rngVal = new LogDouble(rng.nextDouble());
          evidVal = getAssignmentFor(rngVal, this.q.get(j));
          sample.add(evidVal);
        }

      samples.add(sample);
      LogDouble ve = gm.computeProbabilityOfFullAssignment(sample);

      this.w.add(ve.div(probabilityOfSample(sample)));
    }

    return samples;
  }

  // Given t is a set of probabilities like {0.5, 0.25, 0.25} for a variable
  // with domain size 3 and a random probability v sampled in [0,1] this
  // returns the index into the set of probabilities corresponding to v.
  // Loops upward from 0 and adds each entry in the probability set up. Once
  // the sum is greater than v, return the index which caused the sum to be
  // greater than v. So if v = 0.76, this checks sum = 0.5, then sum = 0.5+0.25,
  // then sum = 0.5+0.25+0.25. Finally the sum > v, so it returns 2.
  protected int getAssignmentFor(LogDouble v, ArrayList<LogDouble> t) {
    if (t.isEmpty()) {
      return -1;
    } else if (t.size() == 1) {
      return 0;
    }

    LogDouble sum = LogDouble.LS_ZERO;
    for (int i = 0; i < t.size(); i++) {
      sum = sum.add(t.get(i));
      if (v.compareTo(sum) == -1) {
        return i;
      }
    }

    return 0;
  }

  // Assumes sample is a full assignment to all variables in q. Projects sample
  // onto each entry in q and multiples the value together.
  public LogDouble probabilityOfSample(ArrayList<Integer> sample) {
    LogDouble p = LogDouble.LS_ONE;

    // sample and qSize are the same.
    for (int i = 0; i < this.qSize; i++) {
      p = p.mul(this.q.get(i).get(sample.get(i)));
    }

    return p;
  }

  // Does not assume sample is a full assignment to all variables in q.
  public LogDouble probabilityOfSubset(ArrayList<Integer> sample, ArrayList<IVariable> context, HashSet<IVariable> fContext) {
    LogDouble p = LogDouble.LS_ONE;
    int cSize = context.size();

    //multiply sample weights that appear in the function context (fContext)
    for(IVariable v : fContext) {
      for(int i = 0; i < cSize; i++) {
        if(context.get(i).getId()==v.getId() && !this.marked.get(v.getId())) {
          p = p.mul(this.q.get(v.getId()).get(sample.get(i)));
        }
      }
    }

    return p;
  }

  // Updates the q values based on the weights stored when the samples were
  // generated.
  public void update(ArrayList<ArrayList<Integer>> samples) {
    LogDouble learningRate = new LogDouble(10);

    //Step 1: sum weights for division later (normalization)
    LogDouble total = LogDouble.LS_ZERO, numer;
    for(LogDouble ld : w){
      total = total.add(ld);
    }

    //Step 2: add weights to numer where dirac-delta function = 1
    int ss = samples.size();
    for (int i = 0; i < this.qSize; i++) {
      for(int j = 0; j < this.q.get(i).size(); j++) {
        numer = learningRate;
        total = total.add(learningRate);
        for(int k = 0; k < ss; k++) {
          numer = numer.add(w.get(k).mul((samples.get(k).get(i) == j ? LogDouble.LS_ONE : LogDouble.LS_ZERO)));
        }
        this.q.get(i).set(j, numer.div(total));
      }
    }

    // Step 3: normalize by dividing by the total weights
    for (int j = 0; j < this.qSize; j++) {
      // If the variable at this index is evidence, reset the q values to
      // 0 for nonevidence values, and 1 for evidence values.
      if(this.vars.get(j).isEvid())  {
        int evidVal = vars.get(j).getEvid();
        for (int i = 0; i < vars.get(j).getDomainSize(); i++) {
          if (i == evidVal) {
            q.get(j).set(i, LogDouble.LS_ONE);
          } else {
            q.get(j).set(i, LogDouble.LS_ZERO);
          }
        }
        continue;
      }

      // If the variable wasn't evidence, update it normally.
      ArrayList<LogDouble> qEntry = this.q.get(j);
      int qEntrySize = qEntry.size();
      LogDouble sum = LogDouble.LS_ZERO;

      // Sum the weights for this variable's q entry
      for (int k = 0; k < qEntrySize; k++) {
        sum = sum.add(qEntry.get(k));
      }

      // if the sum was 0 for some reason, set this to the uniform.
      if(sum.compareTo(LogDouble.LS_ZERO)==0){
        for(int k = 0; k < qEntrySize; k++){
          qEntry.set(k, new LogDouble(1.0/(double)qEntrySize));
        }

      // Otherwise set it to the normalized value.
      } else {
        for (int k = 0; k < qEntrySize; k++) {
          qEntry.set(k, qEntry.get(k).div(sum));
        }
      }
    }
  }

  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();

    s.append("Q = \n");
    for (ArrayList<LogDouble> t : q) {
      for (LogDouble d : t) {
        s.append(d.toRealString() + " ");
      }
      s.append("\n");
    }
    s.append("W = \n");
    for (LogDouble t : w) {
      s.append(t.toRealString() + " ");
      s.append("\n");
    }

    return s.toString();
  }

  // This doesn't ened to be synchronized for the serial implementation.
  // This marks an entry in q as used so it is only placed into one cluster
  // in the junction graph.
  public synchronized void setMarked(int i){
    this.marked.set(i, true);
  }

  public boolean getMarked(int i){
    return this.marked.get(i);
  }

  public void unmarkAll(){
    int size = this.marked.size();
    for(int i = 0; i < size; i++){
      this.marked.set(i,false);
    }
  }

  public ArrayList<ArrayList<LogDouble>> getDistribution(){
    return this.q;
  }
}

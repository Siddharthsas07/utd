package utd.cs.pgm.util;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.IVariable;

// This class is a sparse table. It stores {key, count, value} triplets.
// Keys are ArrayList<Integer>, counts are ints, and values are LogDoubles.
public class SparseTable implements Cloneable {
  // The variables this table is over. Essentially headers.
  protected ArrayList<IVariable> vars = new ArrayList<IVariable>();

  // The entries in the table.
  protected ArrayList<ArrayList<Integer>> keys = new ArrayList<ArrayList<Integer>>();

  // This counts the number of times a key has been inserted into the table.
  // This table keeps only one instance of a key in memory, so the count
  // represents how many times the key has been inserted.
  protected ArrayList<Integer> counts = new ArrayList<Integer>();

  // This contains the weight/value of each key entry.
  protected ArrayList<LogDouble> weights = new ArrayList<LogDouble>();

  public SparseTable(ArrayList<IVariable> vars){
    this.vars = vars;
  }

  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("size = [" + this.keys.size() + "]\n");
    for (IVariable v : this.vars) {
      s.append(v.getId() + " ");
    }
    s.append("\n");
    int sz = this.keys.size();
    for (int i = 0; i < sz; i++) {
      s.append("[" + i + "] ");
      s.append(this.weights.get(i).toRealString() + " ");
      s.append(this.counts.get(i) + " ");
      int esz = this.keys.get(i).size();
      for (int j = 0; j < esz; j++) {
        s.append(this.keys.get(i).get(j) + " ");
      }
      s.append("\n");
    }
    return s.toString();
  }

  public ArrayList<IVariable> getVariables() {
    return this.vars;
  }

  // Gets the weight corresponding to a key in the form of a set of assignments
  // to the variables this table's keys.
  public LogDouble getWeight(ArrayList<Integer> key){
    int size = keys.size();
    for(int i = 0; i < size; i++){
      if(keys.get(i).equals(key))
        return weights.get(i);
    }
    return LogDouble.LS_ZERO;
  }

  // Gets the index corresponding to a key in the form of a set of assignments
  // to the variables in this table's keys.
  public int getIndex(ArrayList<Integer> key){
    int size = keys.size();
    for(int i = 0; i < size; i++){
      if(keys.get(i).equals(key))
        return i;
    }
    return -1;
  }

  // gets the count corresponding to to a key in the form of a set of assignments
  // to the variables in this table's keys.
  public int getCount(ArrayList<Integer> key){
    int size = keys.size();
    for(int i = 0; i < size; i++){
      if(keys.get(i).equals(key))
        return counts.get(i);
    }
    return -1;
  }

  // deep copy of the table.
  @Override
  public SparseTable clone() {
    // shallow copy variables
    SparseTable temp = new SparseTable(this.vars);

      // deep copy keys
    for (ArrayList<Integer> key : this.keys) {
      ArrayList<Integer> newEntry = new ArrayList<Integer>(key.size());
      for (Integer i : key) {
        newEntry.add(i.intValue());
      }
      newEntry.trimToSize();
      temp.keys.add(newEntry);
    }
    temp.keys.trimToSize();

    // deep copy counts
    for (Integer i : this.counts) {
      temp.counts.add(i.intValue());
    }
    temp.counts.trimToSize();

    // deep copy weights
    for (LogDouble w : this.weights) {
      temp.weights.add(w.copy());
    }
    temp.weights.trimToSize();

    return temp;
  }

  // Returns the key at the given index.
  public ArrayList<Integer> getKey(int index) {
    return keys.get(index);
  }

  // Returns the weight at the given index.
  public LogDouble getWeight(int index) {
    return weights.get(index);
  }

  // Returns the count at the given index.
  public int getCount(int index) {
    return counts.get(index);
  }

  // Adds a new entry to the table with the specified count and weight.
  //DOES NOT check if the entry is already in the table!
  public void insert(ArrayList<Integer> entry, int count, LogDouble weight) {
    entry.trimToSize();
    keys.add(entry);
    counts.add(count);
    weights.add(weight);
  }

  // Inserts a key into the table. If it is not already in the table, sets
  // its count to 1 and its weight to 0. If it is already in the table,
  // simply increments the count for the key.
  public void insert(ArrayList<Integer> entry) {
    int size = keys.size();
    for(int i = 0; i < size; i++) {
      if(keys.get(i).equals(entry)) {
        incrementCount(i);
        return;
      }
    }

    insert(entry, 1, LogDouble.LS_ZERO);
  }

  // Inserts a key into the table. if it is not already present in the table,
  // adds {entry, count, weight} as a new entry. If it is already present,
  // simply increments the count of the entry and adds the specified weight
  // to that entry's weight.
  public void insertSumWeights(ArrayList<Integer> entry, Integer count, LogDouble weight) {
    int size = keys.size();
    for(int i = 0; i < size; i++) {
      if(keys.get(i).equals(entry)) {
        counts.set(i, counts.get(i) + count);
        weights.set(i, weights.get(i).add(weight));
        return;
      }
    }

    insert(entry, count, weight);
  }

  // Increments the count for key i.
  public void incrementCount(int i) {
    counts.set(i,counts.get(i)+1);
  }

  // Sets the weight for the key to w. If the key does not exist in the table,
  // the table is unaltered.
  public void setWeight(ArrayList<Integer> key, LogDouble w){
    int size = keys.size();
    for(int i = 0; i < size; i++) {
      if(keys.get(i).equals(key)) {
        weights.set(i, w);
        break;
      }
    }
  }

  public void setWeight(int index, LogDouble w){
    weights.set(index, w);
  }

  public int size() {
    return this.keys.size();
  }
}

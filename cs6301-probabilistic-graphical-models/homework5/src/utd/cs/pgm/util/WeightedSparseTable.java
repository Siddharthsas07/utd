package utd.cs.pgm.util;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.Variable;

public class WeightedSparseTable extends SparseTable {
  public WeightedSparseTable(int maxEntryLength) {
    super(maxEntryLength);
  }

  public ArrayList<LogDouble> weights = new ArrayList<LogDouble>();
  
  public int add(ArrayList<Variable> entry, LogDouble w) {
    this.entries.add(entry);
    this.weights.add(w);
    return this.entries.size()-1;
  }
   
  public LogDouble getWeightOf(ArrayList<Variable> t) {
    if (t.isEmpty()) {
      return LogDouble.LS_ZERO;
    }
    
    LogDouble w = LogDouble.LS_ZERO;
    
    int es = this.entries.size();    
    for (int i = 0; i < es; i++) {
      if (isSubsetOf(t, this.entries.get(i))) {
        w = w.add(this.weights.get(i));
      }
    }
    
    return w;
  }
  
  public LogDouble getWeightOfDiscludeChild(ArrayList<Variable> t) {
    if (t.isEmpty()) {
      return LogDouble.LS_ZERO;
    }
    
    ArrayList<Variable> t2 = new ArrayList<Variable>(t);
    t2.remove(t2.size()-1);
    
    return getWeightOf(t2);
  }
  
  public LogDouble getWeight(int i) {
    return this.weights.get(i);
  }
  
  public void setWeight(int i, LogDouble w) {
    this.weights.set(i, w);
  }
  
  public void addWeightToEntry(ArrayList<Variable> t, LogDouble w) {
    int idx = entryToIndex(t);
    
    if (idx < 0) {
      return;
    }
    
    this.weights.set(idx, this.weights.get(idx).add(w));
  }
  
  public void addWeightToEntry(int i, LogDouble w) {
    if (i < 0) {
      return;
    }
    
    this.weights.set(i, this.weights.get(i).add(w));
  }
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("size = [" + entries.size() + "]\n");
    s.append("max entry length = " + this.maxEntryLength + "\n");
    int sz = entries.size();
    for (int i = 0; i < sz; i++) {
      s.append("[" + i + "] ");
      s.append(weights.get(i).toRealString() + " : ");
      int esz = entries.get(i).size();
      for (int j = 0; j < esz; j++) {
        s.append(entries.get(i).get(j) + " ");
      }
      s.append("\n");
    }
    return s.toString();
  }
}

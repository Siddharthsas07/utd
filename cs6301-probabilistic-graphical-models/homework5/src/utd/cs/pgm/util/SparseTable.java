package utd.cs.pgm.util;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.Variable;

public class SparseTable {
  public ArrayList<ArrayList<Variable>> entries = new ArrayList<ArrayList<Variable>>();
  
  int maxEntryLength = 0;
  public SparseTable(int maxEntryLength) {
    this.maxEntryLength = maxEntryLength;
  }
  
  public int add(ArrayList<Variable> entry) {
    this.entries.add(entry);
    int lastIdx = this.entries.size()-1;
    this.entries.get(lastIdx).trimToSize();
    return lastIdx;
  }
  
  public int getCountOf(ArrayList<Variable> t) {
    if (t.isEmpty()) {
      return 0;
    }
    
    int count = 0;
    
    int es = this.entries.size();
    
    for (int i = 0; i < es; i++) {
      if (isSubsetOf(t, this.entries.get(i))) {
        ++count;
      }
    }
    
    return count;
  }
  
  public int getCountOfDiscludeChild(ArrayList<Variable> t) {
    if (t.isEmpty()) {
      return 0;
    }
    
    ArrayList<Variable> t2 = new ArrayList<Variable>(t);
    t2.remove(t2.size()-1);
    
    return getCountOf(t2);
  }
  
  // ?????????????????????????????????????????????????????????
  // any way to make this faster
  public boolean isSubsetOf(ArrayList<Variable> t, ArrayList<Variable> e) {
    // See if values all the variables in t match their corresponding ones in e.
    try {
      // If e is a full assignment, can use the varible IDs in t to index into it
      // to check for speed.
      if (e.size() == this.maxEntryLength) {
        for (Variable v : t) {
          if (e.get(v.getId()).getValue() != v.getValue()) {
            return false;
          }
        }    
        return true;
      } else if (t.size() > e.size()) {
        return false;
      // If e is not a full assignment, must linearly search e for each variable
      // in t and see if the values match for corresponding IDs. If a varaible in
      // t doesn't exist in e, automatically return false. Otherwise check its
      // value, if it doesn't match, return false. if it does match, continue
      // to next variable in t. Must linearly search because e is not sorted!
      } else {  
        boolean OK = false;
        for (Variable vt : t) {
          OK = false;
          for (Variable ve : e) {
            // IDs match
            if (vt.getId() == ve.getId()) {
              // Values match, OK!
              if (vt.getValue() == ve.getValue()) {
                OK = true;
                break;
              // Values don't match, Bad!
              } else {
                return false;
              }
            }
          }
          
          // made it through the entire e set looking for vt and didn't find
          // a matching ID. Bad!
          if (!OK) {
            return false;
          }
        }
        // Made it through entire loop without dying elsewhere, means all of
        // vts are in e, and their IDs and values match.
        return true; 
      }
      
    } catch (Exception excep) {
      excep.printStackTrace();
      System.out.println("Error Matching:");
      System.out.println("t = " + t);
      System.out.println("e = " + e);
      return false;
    }    
  }
  
  public int size() {
    return this.entries.size();
  }
  
  public int entryToIndex(ArrayList<Variable> t) {
    int r = -1;
    
    int entriesSize = this.entries.size();
    for (int i = 0; i < entriesSize; i++) {
      if (this.entries.get(i).size() != t.size()) {
        continue;
      }
      
      int es = this.entries.get(i).size();
      for (int j = 0; j < es; j++) {
        if (!this.entries.get(i).get(j).equals(t.get(j))) {
          continue;
        }
      }
      
      r = i;
      break;
    }
    
    return r;
  }
  
  public ArrayList<Variable> indexToEntry(int i) {
    if (i < 0 || i >= this.entries.size()) {
      return null;
    }
    
    return this.entries.get(i);
  }
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("size = [" + entries.size() + "]\n");
    s.append("max entry length = " + this.maxEntryLength + "\n");
    int sz = entries.size();
    for (int i = 0; i < sz; i++) {
      s.append("[" + i + "] ");
      int esz = entries.get(i).size();
      for (int j = 0; j < esz; j++) {
        s.append(entries.get(i).get(j) + " ");
      }
      s.append("\n");
    }
    return s.toString();
  }
}

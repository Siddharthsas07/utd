package utd.cs.pgm.learner;

import java.util.ArrayList;

import utd.cs.pgm.core.variable.Variable;

public class ExampleArrayList extends ArrayList<ArrayList<Integer>> {  
  @Override 
  public String toString() {
    StringBuilder s = new StringBuilder();
    for (ArrayList<Integer> e : this) {
      for (Integer i : e) {
        s.append(i + " ");
      }
      s.append("\n");
    }
    return s.toString();
  }
  
  public int getCountOf(ArrayList<Variable> t) {
    if (t.isEmpty()) {
      return 0;
    }
    
    int count = 0;
    
    for (ArrayList<Integer> e : this) {
      if (matches(t, e)) {
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
  
  protected boolean matches(ArrayList<Variable> t, ArrayList<Integer> s) {
    for (Variable v : t) {
      // Check tuple s (stored as a series of 10101011100011 etc) at index of
      // v's ID to see if the value stored at v's index matches v's value.
      if (s.get(v.getId()) != v.getValue()) {
        return false;
      }
    }    
    return true;
  }  
}

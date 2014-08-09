package pgm;

import java.util.ArrayList;

public class Tuple {
  public ArrayList<Integer> vars = new ArrayList<Integer>(); // v
  public ArrayList<Integer> assn = new ArrayList<Integer>();   // a
  public ArrayList<Integer> doms = new ArrayList<Integer>(); // b
  
  public Tuple() {}
  
  public Tuple(int n) {
    vars = new ArrayList<Integer>(n);
    assn = new ArrayList<Integer>(n);
    doms = new ArrayList<Integer>(n);
  }
  
  public boolean isEmpty() {
    return assn.isEmpty() && doms.isEmpty();
  }
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("v: ["  + this.vars.size() + "] ");
    for (Integer v : this.vars) {
      s.append(v + " ");
    } s.append("\na: [" + this.assn.size() + "] ");
    for (Integer a : this.assn) {
      s.append(a + " ");
    } s.append("\nd: [" + this.doms.size() + "] ");
    for (Integer d : this.doms) {
      s.append(d + " ");
    } s.append("\n");
    return s.toString();  
  }
}

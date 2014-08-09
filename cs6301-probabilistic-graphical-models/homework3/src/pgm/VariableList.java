package pgm;

import java.util.ArrayList;
import java.util.Collection;

public class VariableList implements VariableCollectionType {
  public ArrayList<VariableType> vars = new ArrayList<VariableType>();
  
  VariableList() {}
  
  VariableList(int n) {
    vars.ensureCapacity(n);
  }
  
  @Override
  public VariableType get(int vID) {
    return vars.get(vID);
  }

  @Override
  public boolean set(int vID, VariableType v) {
    vars.set(vID, v);
    return true;
  }

  @Override
  public boolean add(VariableType v) {
    vars.add(v);
    return true;
  }

  @Override
  public boolean remove(VariableType v) {
    vars.remove(v);
    return true;
  }

  @Override
  public boolean remove(int idx) {
    vars.remove(idx);
    return true;
  }
  
  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("v: [" + size() + "] ");    
    for (VariableType v : vars) {
      s.append(v + " ");
    }  
    return s.toString();
  }

  @Override
  public int size() {
    return vars.size();
  }
  
  @Override
  public boolean isEmpty() {
    return size() == 0;
  }

  @Override
  public Collection<VariableType> getVariables() {
    return this.vars;
  }

  @Override
  public boolean contains(int vID) {
    for (VariableType v : this.vars) {
      if (v.id() == vID) {
        return true;
      }
    }
    return false;
  }

  @Override
  public boolean contains(VariableType v) {
    return vars.contains(v);
  }
}

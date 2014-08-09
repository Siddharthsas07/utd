package pgm;

import java.util.Collection;

public interface VariableCollectionType {
  public VariableType get(int vID);
  public Collection<VariableType> getVariables();
  public boolean  set(int vID, VariableType v);
  public boolean  add(VariableType v);
  public boolean  remove(VariableType v);
  public boolean  remove(int vID);
  public String   toString();
  public int      size();  
  public boolean  isEmpty();
  public boolean contains(int vID);
  public boolean contains(VariableType v);
}

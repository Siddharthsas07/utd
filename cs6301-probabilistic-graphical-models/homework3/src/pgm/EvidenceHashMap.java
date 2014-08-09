package pgm;

import java.util.HashMap;
import java.util.Map;

public class EvidenceHashMap implements EvidenceCollectionType {
  HashMap<Integer,Integer> evid = new HashMap<Integer,Integer>();  
  
  @Override 
  public String toString() {
    StringBuilder s = new StringBuilder();
    s.append("e: [" + size() + "] ");    
    for (Map.Entry<Integer,Integer> e : evid.entrySet()) {
      s.append(e.getKey() + ":" + e.getValue() + " ");
    }
    return s.toString();    
  }
  
  @Override
  public Integer get(VariableType v) {
    return get(v.id());
  }

  @Override
  public Integer get(Integer vID) {
    return evid.get(vID);
  }

  @Override
  public boolean set(VariableType v, Integer e) {
    return set(v.id(), e);
  }

  @Override
  public boolean set(Integer vID, Integer e) {
    evid.remove(vID);
    evid.put(vID, e);
    return true;
  }

  @Override
  public boolean add(VariableType v, Integer e) {
    return add(v.id(), e);
  }

  @Override
  public boolean add(Integer vID, Integer e) {
    if (evid.containsKey(vID)) {
      return false;
    }
    evid.put(vID, e);
    return true;
  }

  @Override
  public boolean remove(VariableType v) {
    return remove(v.id());
  }

  @Override
  public boolean remove(Integer vID) {
    Integer r = evid.remove(vID);
    return r == null ? false : true;
  }

  @Override
  public int size() {
    return evid.size();
  }
  
  @Override
  public boolean isEmpty() {
    return size() == 0;
  }

  @Override
  public boolean contains(VariableType v) {
    return contains(v.id());
  }

  @Override
  public boolean contains(Integer vID) {
    return evid.containsKey(vID);
  }
}

package pgm;

// Intended for use alongside the VariableCollectionType. Thus things are stored
// as variable IDs and values from domains only!
public interface EvidenceCollectionType {
  public String toString();
  
  public boolean contains(VariableType v);
  public boolean contains(Integer vID);
  
  public Integer get(VariableType v);
  public Integer get(Integer vID);
  
  public boolean set(VariableType v, Integer e);
  public boolean set(Integer vID, Integer e);
  
  public boolean add(VariableType v, Integer e);
  public boolean add(Integer vID, Integer e);
  
  public boolean remove(VariableType v);
  public boolean remove(Integer vID);
  
  public int size();
  public boolean isEmpty();
  
}

package pgm;

public class Variable implements VariableType {
  public int id;
  public int domainSize;
  //int value;
  //boolean isEvidence;
  
  public Variable() {
    this.id = -1;
    this.domainSize = -1;
  }
  
  /*public Variable(int domainSize) {
    this.id = -1;
    this.domainSize = domainSize;
  }*/
  
  public Variable(int id, int domainSize) {
    this.id = id;
    this.domainSize = domainSize;
  }

  @Override
  public int domainSize() {
    return this.domainSize;
  }

  @Override
  public int id() {
    return this.id;
  }
  
  @Override
  public String toString() {
    return id() + ":" + domainSize(); 
  }
}

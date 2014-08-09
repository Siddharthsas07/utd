package pgm;

public class VariableWithAssignment extends Variable {
  public int assn;
  
  public VariableWithAssignment() {
    this.id = -1;
    this.domainSize = -1;
    this.assn = -1;
  }
  
  public VariableWithAssignment(int id, int ds, int a) {
    this.id = id;
    this.domainSize = ds;
    this.assn = a;
  }
  
  @Override 
  public String toString() {
    return id() + ":" + assignment() + ","+ domainSize();
  }
  
  public int assignment() {
    return this.assn;
  }  
}

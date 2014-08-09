package pgm;

public class Variable {
  // Unique ID of this variable
  int id;
  
  // Number of different values this variable can take.
  int domainSize;
  
  // Value from [0..domainSize-1] this variable takes if it is evidence.
  //long value;
  
  // Whether this variable is part of the evidence or not.
  //boolean isEvidence;
  
  public Variable() {
    this.id = -1;
    this.domainSize = -1;
    //value = -1;
    //isEvidence = false;
  }
  
  public Variable(int domainSize) {
    this.domainSize = domainSize;
  }
  
  public Variable(int id, int domainSize) {
    this.id = id;
    this.domainSize = domainSize;
  }  
}

package pgm;

import java.util.ArrayList;
import java.util.HashSet;

public class Bucket {
  int bucketVariable;
  public HashSet<Integer> variableIDs = new HashSet<Integer>();
  public ArrayList<FunctionTable> functions = new ArrayList<FunctionTable>();
  
  public Bucket(int bucketVariable) {
    this.bucketVariable = bucketVariable;
    this.variableIDs.add(bucketVariable);
  }
  
  public Bucket() {
    this.bucketVariable = 0;
  }  
}

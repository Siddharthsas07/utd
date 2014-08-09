package pgm;

import java.util.ArrayList;
import java.util.HashSet;

public class Bucket {
  VariableType bucketVariable;
  public HashSet<VariableType> variables = new HashSet<VariableType>();
  public ArrayList<FunctionTable> functions = new ArrayList<FunctionTable>();
  
  public Bucket(VariableType bucketVariable) {
    this.bucketVariable = bucketVariable;
    this.variables.add(bucketVariable);
  }
}

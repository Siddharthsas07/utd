package pgm;

import java.util.LinkedList;

public class BucketList {
  public LinkedList<Bucket> buckets = new LinkedList<Bucket>();
  
  public String toString() {
    StringBuilder s = new StringBuilder();
    for (Bucket b : this.buckets) {
      s.append("b: " + b.bucketVariable.id() + "\n");
      s.append("v: [" + b.variables.size() + "] ");
      for (VariableType v : b.variables) {
        s.append(v + " ");
      } s.append("\nf: [" + b.functions.size() + "]\n");
      for (FunctionTable f : b.functions) {
        s.append(f.toRealString() + "\n");
      }
    }
    return s.toString();
  }
}

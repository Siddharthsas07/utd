package utd.cs.pgm.util;

public class WeightedEdge {
  public int v1;
  public int v2;
  public LogDouble w;
  
  public WeightedEdge(int v1, int v2, LogDouble w) {
    this.v1 = v1;
    this.v2 = v2;
    this.w = w;
  }
  
  public String toString() {
    return "(" + this.v1 + ", " + this.v2 + ", " + this.w.toRealString() + ")";
  }
}

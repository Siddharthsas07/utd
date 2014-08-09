package utd.cs.pgm.util;

import java.util.Comparator;

public class WeightedEdgeComparator implements Comparator<WeightedEdge>{
  @Override
  public int compare(WeightedEdge lhs, WeightedEdge rhs) {
    return lhs.w.compareToDescending(rhs.w);
  }
}

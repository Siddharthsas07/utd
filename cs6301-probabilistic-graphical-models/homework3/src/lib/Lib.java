package lib;

import java.util.Collection;

public class Lib {
  
  /*
   * returns first element of c1 which is anywhere in c2.
   * ie containsAny({0,1,2},{5,10,2}) => 2.
   * returns null if no element of c1 is in c2.
   */
  public static <T> T containsAny(Collection<T> c1, Collection<T> c2) {
    for (T elm : c1) {
      if (c2.contains(elm)) {
        return elm;
      }
    }    
    return null;
  }
}

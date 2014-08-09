package utd.cs.pgm.core;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.util.LogDouble;

public class Main {
  public static void main(String[] args) {
    try {
      //System.out.println("Running directory: " + System.getProperty("user.dir"));
      GraphModel gm = new GraphModel();
      gm.setup(args);
      
      //System.out.println(gm);
      
      Stopwatch sw = new Stopwatch();
      sw.start();
      
      LogDouble zp = gm.computeZorP();
      
      sw.stop();
      //System.out.println("log_10(Z or P) = " + zp);
      //System.out.println("       Z or P  = " + zp.getRealValue());
      System.out.println(zp);
      System.out.println(sw);
    } catch (Exception e) {
      e.printStackTrace();
    } 
  }
}

package lib;

/**
 * @author Gary
 *
 */
public class Stopwatch {
  private long startTime;
  private long stopTime;

  public static final double NANOS_PER_SEC = 1000000000.0;

  public void start() {  
    startTime = System.nanoTime();
  }

  public void stop() {  
    stopTime = System.nanoTime();  
  }

  // Returns recorded time in seconds
  public double time() {  
    return (stopTime - startTime) / NANOS_PER_SEC;
  }

  public String toString() {   
    return time() + " s.";
  }

  // Returns recorded time in nanoseconds
  public long timeInNanoseconds() {  
    return (stopTime - startTime);  
  }
  
  public String toNanoString() {
    return timeInNanoseconds() + " ns.";
  }
}


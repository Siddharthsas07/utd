package utd.cs.pgm.util;

public class LogDouble implements Comparable<LogDouble>{
  private double value;   // The value of the number, stored in log value.
  private boolean isZero; // True if this number is 0 in real space.
  
  public static final double MIN_VAL = 10e-250; // Smallest number that is nonzero.
  public static final double LS_MIN_VAL = -(Double.MAX_VALUE/2.0); // Smallest number that is nonzero from logspace
  
  public static final LogDouble LS_ZERO = new LogDouble(0.0);
  public static final LogDouble LS_ONE = new LogDouble(1.0);
  
  public LogDouble() {
    value = 0.0;
    isZero = false;
  }
  
  // Create a LogDouble from Real Space if fromLS is false
  public LogDouble(double v) {
    if (v < MIN_VAL) {
      this.isZero = true;
    } else {
      this.value = Math.log10(v);
      this.isZero = false;
    }
  }
  
  // Create a LogDouble from a number already in log10 space if fromLS is true
  public LogDouble(double v, boolean fromLS) {
    if (v < LS_MIN_VAL) {
      this.isZero = true;
    } else {
      this.value = v;
      this.isZero = false;
    }
  }
  
  public LogDouble copy() {
    return new LogDouble(this.value, true);
  }
  
  public String toString() {
    return ((Double)getValue()).toString();
  }
  
  public String toRealString() {
    if (this.isZero) {
      return "0.0";
    } else {
      return ((Double)this.getRealValue()).toString();
    }
  }
  
  // Use this when you would multiply the two numbers in real space
  public LogDouble mul(LogDouble rhs) {    
    if (this.isZero || rhs.isZero) {
      return LogDouble.LS_ZERO;
    } else {
      return new LogDouble(this.value + rhs.value, true);    
    }
  }
  
  // Use this when you would divide two numbers in real space
  public LogDouble div(LogDouble rhs) {
    if (this.isZero) {
      return LogDouble.LS_ZERO;
    } else if (rhs.isZero) {
      System.err.println("LogDouble: div() divided by 0.");
      return LogDouble.LS_ZERO;
    } else {
      return new LogDouble(this.value - rhs.value, true);
    }
  }
  
  // Use this when you would add the two numbers in real space
  public LogDouble add(LogDouble rhs) {
    if (this.isZero && rhs.isZero) {
      return LogDouble.LS_ZERO;
    } else if (this.isZero) {
      return rhs.copy();
    } else if (rhs.isZero) {
      return this.copy();
    } else {
      double a = Math.max(this.value, rhs.value);
      double b = Math.min(this.value, rhs.value);
      //System.out.println(Math.pow(10, b-a));
      double c = a + Math.log10(1.0 + Math.pow(10, b-a));
      //System.out.println(c);
      return new LogDouble(c, true);
    }
  }
  
  // Use this when you would subtract two numbers in real space
  public LogDouble sub(LogDouble rhs) throws IllegalArgumentException {
    if (this.isZero && rhs.isZero) {
      return LogDouble.LS_ZERO;
    } else if (this.isZero && !rhs.isZero) {
      return new LogDouble(0.0);
      //throw new IllegalArgumentException("LogDouble: 0.sub(positive value) is invalid operation.");
    } else if (!this.isZero && rhs.isZero) {
      return this.copy();
    } else if (this.value < rhs.value) {
      return new LogDouble(0.0);
    } else {
      //double a = Math.max(this.value, rhs.value);
      //double b = Math.min(this.value, rhs.value);
      double a = this.value;
      double b = rhs.value;
      //System.out.println(Math.pow(10, b-a));
      double c = a + Math.log10(1.0 - Math.pow(10, b-a));
      //System.out.println(c);
      return new LogDouble(c, true);
    }
    /*if (this.isZero && rhs.isZero) {
      return LogDouble.LS_ZERO;
    } else if (this.isZero) {
      throw new IllegalArgumentException("LogDouble: 0.sub(positive value) is invalid operation.");
    } else if (rhs.isZero) {
      return this.copy();
    } else {
      double a = Math.max(this.value, rhs.value);
      double b = Math.min(this.value, rhs.value);
      //System.out.println(Math.pow(10, b-a));
      double c = a + Math.log10(1.0 + Math.pow(10, (-b)-a));
      //System.out.println(c);
      return new LogDouble(c, true);
    }*/
  }
  
  // Use this when you want the absolute difference between two numbers in
  // real space
  public LogDouble absDif(LogDouble rhs) {
    if (this.isZero) {
      return rhs.sub(this);
    } else if (rhs.isZero) {
      return this.sub(rhs);
    } else {
      return (this.value > rhs.value) ? this.sub(rhs) : rhs.sub(this);
    }
  }
  
  // Returns a LogDouble with the stored value as v transformed to log_base
  public static LogDouble realToLog(double v) {
    return new LogDouble(v);
  }
  
  // Returns a double with v transformed from log_base to real
  public static double logToReal(LogDouble n) {
    if (n.isZero) {
      return 0.0;
    } else {
      return Math.pow(10.0, n.value);
    }
  } 
  
  //Returns this.value in log_base form 
  public double getValue() {
    return this.value;
  }
  
  // Returns this.value v transformed from log_base to real
  public double getRealValue() {
    if (this.isZero) {
      return 0.0;
    } else {
      return Math.pow(10.0, this.value);
    }
  }
  
  // Stores in this.value v transformed from real to log_base 
  public void fromRealValue(double v) {
    if (v < LogDouble.MIN_VAL) {
      this.isZero = true;
    } else {
      this.value = Math.log10(v);
      this.isZero = false;
    }
  }
  
  // Returns this.isZero
  public boolean isZero() {
    return this.isZero;
  }

  @Override
  public int compareTo(LogDouble rhs) {
    if (this.isZero && rhs.isZero) {
      return 0;
    } else if (this.isZero && !rhs.isZero) {
      return -1;
    } else if (!this.isZero && rhs.isZero) {
      return 1;
    } else if (this.value < rhs.value) {
      return -1;
    } else if (this.value > rhs.value) {
      return 1;
    } else {
      return 0;
    }
  }
  
  public int compareToAscending(LogDouble rhs) {
    return this.compareTo(rhs);
  }
  
  public int compareToDescending(LogDouble rhs) {
    return this.compareTo(rhs) * -1;
  }
  
  public boolean equals(LogDouble rhs) {
    if (this.isZero) {
      return this.isZero == rhs.isZero;
    } else {
      return this.value == rhs.value;
    }
  }
}

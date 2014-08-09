package lib;


public class LogNumber {
  // The value of the number, stored in log value.
  public double value;
  
  // If true, the number stored was 0. Thus the value of this.value is 0 in
  // real space. This is to avoid math errors.
  public boolean isZero;
  
  // For checking whether two values are sufficiently close.
  public static final double MIN_VAL = 10e-20;
  public static final double LS_MIN_VAL = -(Double.MAX_VALUE/2.0);
  
  public LogNumber() {
    value = 0.0;
    isZero = false;
  }
  
  // Create a LogNumber from Real Space if fromLS is false
  public LogNumber(double v) {
    if (v < MIN_VAL) {
      this.isZero = true;
    } else {
      this.value = Math.log10(v);
      this.isZero = false;
    }
  }
  
  // Create a LogNumber from a number alrady in log10 space if fromLS is true
  public LogNumber(double v, boolean fromLS) {
    if (v < LS_MIN_VAL) {
      this.isZero = true;
    } else {
      this.value = v;
      this.isZero = false;
    }
  }
  
  public String toString() {
    //return ("(" + this.getValue() + ", " + this.isZero() + ")");
    if (this.isZero()) {
      return "0.0";
    } else {
      return ((Double)this.getValue()).toString();
    }
  }
  
  // Use this when you would multiply the two numbers in real space
  public LogNumber mul(LogNumber rhs) {    
    if (this.isZero() || rhs.isZero()) {
      return new LogNumber(0.0);
    } else {
      double v = this.value + rhs.value;
      return new LogNumber(v, true);    
    }
  }
  
  // Use this when you would divide two numbers in real space
  public LogNumber div(LogNumber rhs) {
    if (this.isZero()) {
      return new LogNumber(0.0);
    } else if (rhs.isZero()) {
      System.err.println("LogNumber: div() divided by 0.");
      return new LogNumber(0.0);
    } else {
      double v = this.value - rhs.value;
      return new LogNumber(v, true);
    }
  }
  
  // Use this when you would add the two numbers in real space
  public LogNumber add(LogNumber rhs) {
    if (this.isZero() && rhs.isZero()) {
      return new LogNumber(0.0);
    } else if (this.isZero()) {
      return rhs;
    } else if (rhs.isZero()) {
      return this;
    } else {
      double a = Math.max(this.value, rhs.value);
      double b = Math.min(this.value, rhs.value);
      return new LogNumber(a + Math.log10(1.0 + Math.pow(10, b-a)), true);
    }
  }
  
  // Not supported yet
  public LogNumber sub(LogNumber rhs) {
    return new LogNumber(0.0);
  }
  
  // Returns a LogNumber with the stored value as v transformed to log_base
  public static LogNumber realToLog(double v) {
    return new LogNumber(v);
  }
  
  // Returns a double with v transformed from log_base to real
  public static double logToReal(LogNumber n) {
    if (n.isZero()) {
      return 0.0;
    } else {
      return Math.pow(10.0, n.value);
    }
  } 
  
  // Returns this.value v transformed from log_base to real
  public double toReal() {
    if (this.isZero()) {
      return 0.0;
    } else {
      return Math.pow(10.0, this.value);
    }
  }
  
  // Stores in this.value v transformed from real to log_base 
  public void fromReal(double v) {
    if (v < LogNumber.MIN_VAL) {
      this.isZero = true;
    } else {
      this.value = Math.log10(v);
      this.isZero = false;
    }
  }
  
  // Doesn't work yet
  /*public LogNumber toLN() {
    return this.mul(new LogNumber(TO_LN_MULTIPLIER, false));    
  }*/
  
  // Returns this.value in log_base form 
  public double getValue() {
    return this.value;
  }
  
  // Returns this.isZero
  public boolean isZero() {
    return this.isZero;
  }
}

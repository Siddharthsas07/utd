package utd.cs.pgm.core.variable;

import java.util.ArrayList;
import java.util.Collection;

public class Variable {
  protected int id;
  protected int domainSize;
  protected int value; // [0, d-1] if evidence, -1 elsewise
  protected int addrValue;
  
  public Variable() {
    this.id = -1;
    this.domainSize = -1;
    this.value = -1;
    this.addrValue = -1;
  }
  
  public Variable(int id, int domainSize) {
    this.id = id;
    this.domainSize = domainSize;
    this.value = -1;
    this.addrValue = -1;
  }
  
  public Variable(int id, int domainSize, int value) {
    this.id = id;
    this.domainSize = domainSize;
    this.value = value;
    this.addrValue = -1;
  }
  
  public Variable(int id, int domainSize, int value, int addrValue) {
    this.id = id;
    this.domainSize = domainSize;
    this.value = value;
    this.addrValue = addrValue;
  }
  
  public Variable copy() {
    return new Variable(this.id, this.domainSize, this.value, this.addrValue);
  }
  
  public int compareTo(Variable rhs) {
    if (this.id < rhs.id) {
      return -1;
    } else if (this.id > rhs.id) {
      return 1;
    } else {
      if (this.domainSize < rhs.domainSize) {
        return -1;
      } else if (this.domainSize > rhs.domainSize) {
        return 1;
      } else {
        if (this.value < rhs.value) { 
          return -1;
        } else if (this.value > rhs.value) {
          return 1;
        } else {
          return 0;
        }
      }
    }
  }
  
  public boolean equals(Variable rhs) {
    return ((this.id == rhs.id) && (this.getValue() == rhs.getValue()));
  }
  
  public String toString() {
    return id + ":" + getValue() + "c" + domainSize;
  }
  
  public static String variableCollectionString(Collection<Variable> vars) {
    StringBuilder s = new StringBuilder();
    s.append("v: [" + vars.size() + "]\n");    
    for (Variable v : vars) {
      s.append(v + "\n");
    }  
    return s.toString();
  }
  
  public int getId() {
    return id;
  }
  
  public void setId(int id) {
    this.id = id;
  }
  
  public int getDomainSize() {
    return domainSize;
  }
  
  public void setDomainSize(int domainSize) {
    this.domainSize = domainSize;
  }
  
  public boolean isEvidence() {
    return this.value != -1;
  }
  
  public int getValue() {
    if (this.value > -1) {
      return this.value;
    }
    return addrValue;
  }
  
  public void setValue(int value) {
    if (value == -1) {
      System.err.println("Can't set value of variable to -1");
      return;
    }
    this.addrValue = value;
  }

  public void setEvidence(int value) {
    this.value = value;
  }
  
  public static long productDomainSize(Collection<Variable> variables) {
    int domainSize = 1;
    for (Variable v : variables) {
      domainSize *= v.getDomainSize();
    }
    return domainSize;
  }
  
  public static int getAddress(ArrayList<Variable> variables) {
    int address = 0;
    int multiplier = 1;
    for (int i = variables.size() - 1; i > -1; i--) {
      address += (multiplier * variables.get(i).getValue());
      multiplier *= variables.get(i).getDomainSize();
    }    
    return address;
  }
  
  public static void setAddress(ArrayList<Variable> variables, int address) {
    for (int i = variables.size() - 1; i > -1; i--) {
      if (variables.get(i).isEvidence()) {
        continue;
      }
      variables.get(i).setValue(address % variables.get(i).getDomainSize());
      address /= variables.get(i).getDomainSize();
    }
  }
}

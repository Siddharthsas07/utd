package utd.cs.pgm.core.variable;

public interface IVariable {
  public int getId();
  public void setId(int id);

  public int getEvid();
  public void setEvid(int evid);

  public int getDomainSize();
  public void setDomainSize(int ds);

  public IVariable copy();

  public String toString();
  public int compareTo(IVariable v2);
  public boolean isEvid();
}

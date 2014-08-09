package utd.cs.pgm.learner;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Scanner;

import utd.cs.pgm.core.function.Function;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.graphmodel.GraphModelType;
import utd.cs.pgm.core.variable.Variable;
import utd.cs.pgm.util.LogDouble;
import utd.cs.pgm.util.StaticUtilities;

public class BN_MLE_FOD implements IModelLearner {
  GraphModel learnedModel = new GraphModel();
  GraphModel trueModel = new GraphModel();
  ExampleArrayList examples = new ExampleArrayList();    
  
  public BN_MLE_FOD() {};
  
  public BN_MLE_FOD(String path, GraphModel structure) {
    train(path, structure);
  }
  
  @Override
  public GraphModel getLearnedModel() {
    return this.learnedModel;
  }

  @Override
  public GraphModel train(String path, GraphModel structure) {
    try {
      FileReader fr = new FileReader(path);
      Scanner sc = new Scanner(fr);
      
      int numVariables = sc.nextInt();
      int numExamples = sc.nextInt();

      // Read training data examples, one example at a time.
      for (int i = 0; i < numExamples; i++) {
        ArrayList<Integer> example = new ArrayList<Integer>();
        
        for (int j = 0; j < numVariables; j++) {
          example.add(sc.nextInt());
        }
        
        //System.out.println("example = " + example);
        
        this.examples.add(example);
      }
      
      //System.out.println(this.examples);
      
      // All finished with the examples
      sc.close();
      fr.close();
      
      // Set the structure as type BAYES
      this.learnedModel.setType(GraphModelType.BAYES);
      
      // Now start filling in the learned model.
      // First copy the variables from the true model
      for (Variable v : structure.getVariables()) {
        this.learnedModel.getVariables().add(v.copy());
      }
      
      // Now copy the structure
      for (Function f : structure.getFunctions()) {
        Function g = new Function();
        
        // Copy variables from the true structure
        for (Variable v : f.variables) {
          g.variables.add(v.copy());
        }
        
        long gTableSize = Variable.productDomainSize(g.variables);
        
        // Fill in g's table with the MLE estimates from counts by iterating
        // over each tuple in g. G is a CPT so it represents a posterior.
        // Compute posterior for g. IE P(B|A) is actually stored as F(AB)
        // so compute that as P(A,B) / P(A) from the counts table
        for (int i = 0; i < gTableSize; i++) {
          Variable.setAddress(g.variables, i);         

          // Operates on the stored samples
          LogDouble value = getMLEEstimate(g.variables);
          
          // Store the ratio now
          g.table.add(value);
        }
        
        //g.table.trimToSize();
        
        // Since this is a CPT it must be normalized.
        g = g.normalize();
        
        learnedModel.getFunctions().add(g);
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
    
    // Return the learned model with copied variables and such.
    return learnedModel;
  }

  protected LogDouble getMLEEstimate(ArrayList<Variable> t) {
    if (t.isEmpty()) {
      return LogDouble.LS_ZERO;
    }
    
    // t is a full variable assignment. Say P(B|A) is the tuple then it's 
    // stored as F(AB). Compute as P(A,B) / P(A) from the counts table. 
    
    // MLE estimate for t is always at least the uniform value for its tuple.
    // Same as laplace correction. 
    int numer = 1;
    int denom = t.get(t.size()-1).getDomainSize();
    
    // Get number of examples matching g's ith tuple.
    numer += this.examples.getCountOf(t);

    // Get number of examples amtching g's parents' ith tuple.
    denom += this.examples.getCountOfDiscludeChild(t);
    
    return new LogDouble((double)numer / (double)denom);
  }
  
  @Override
  public GraphModel getTrueModel() {
    return this.trueModel;
  }

  @Override
  public void setTrueModel(GraphModel gm) {
    this.trueModel = gm;
  }

  @Override
  public LogDouble test(String path) {
    return StaticUtilities.computeLogLikelihoodDifference(
        this.learnedModel, this.trueModel, path);
  }
  
  @Override
  public String toString() {
    StringBuffer s = new StringBuffer();
    s.append("Learned Model\n" + this.learnedModel.toString() + "\n");
    s.append("True Model\n" + this.trueModel.toString() + "\n");
    s.append("Examples\n" + this.examples.toString() + "\n");
    return s.toString();
  }
}

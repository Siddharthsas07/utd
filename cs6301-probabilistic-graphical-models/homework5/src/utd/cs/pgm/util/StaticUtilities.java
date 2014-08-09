package utd.cs.pgm.util;

import java.io.FileReader;
import java.util.ArrayList;
import java.util.Scanner;

import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.core.variable.Variable;

public class StaticUtilities {
  public static LogDouble computeLogLikelihoodDifference(
      GraphModel gm1,
      GraphModel gm2,
      String testFilePath) {
    LogDouble totalDifference = new LogDouble(0.0);
    try {
      FileReader fr = new FileReader(testFilePath);
      Scanner sc = new Scanner(fr);
      
      int numVariables = sc.nextInt();
      int numExamples = sc.nextInt();

      // Create ArrayList to hold the current example (this is reused later)
      ArrayList<Variable> example = new ArrayList<Variable>(numVariables);
      for (int i = 0; i < numVariables; i++) {
        example.add(new Variable(i, 2));
      }
      
      LogDouble ll1;
      LogDouble ll2;      
      
      // Read training data examples, one example at a time.
      for (int i = 0; i < numExamples; i++) {
        // Read the example
        for (int j = 0; j < numVariables; j++) {
          //example.get(j).setEvidence(sc.nextInt());
          example.get(j).setEvidence(sc.nextInt());
        }
        
        //System.out.println(Variable.variableCollectionString(example));
        
        // Calculate probability of this example from each mode. Then calculate
        // the difference in those and accumulate it into totalDifference.
        /*this.learnedModel.setAssignment(example);
        this.trueModel.setAssignment(example);*/       
        
        ll1 = gm1.computeProbabilityOfFullAssignment(example);
        ll2 = gm2.computeProbabilityOfFullAssignment(example);
        
        double logDiff = Math.abs(ll1.getValue() - ll2.getValue()); 
        
        /*System.out.println("ll1 = " + ll1.toRealString());
        System.out.println("ll2 = " + ll2.toRealString());
        System.out.println("l.absDif(t) = " + ll1.absDif(ll2).toRealString());
        System.out.println("t.absDif(l) = " + ll2.absDif(ll1).toRealString());
        
        System.out.println("ll1 = " + ll1.toString());
        System.out.println("ll2 = " + ll2.toString());
        System.out.println("l.absDif(t) = " + ll1.absDif(ll2).toString());
        System.out.println("t.absDif(l) = " + ll2.absDif(ll1).toString());*/
        
        totalDifference = totalDifference.add(LogDouble.realToLog(logDiff));
        //System.out.println("totalDifference =" + totalDifference.toRealString());
      }
      
      // All finished with the examples
      sc.close();
      fr.close();
    } catch (Exception e) {
      e.printStackTrace();
    }
    
    return totalDifference;    
  }
}

package utd.cs.pgm.core;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.learner.BN_EM_POD;
import utd.cs.pgm.learner.BN_MLE_FOD;
import utd.cs.pgm.learner.BS_CL_FOD;
import utd.cs.pgm.learner.IModelLearner;
import utd.cs.pgm.learner.IStructureLearner;
import utd.cs.pgm.util.LogDouble;

public class Main {
  public static void main(String[] args) {
    
    if (args.length < 5) {
      System.err.println("Incorrect command line parameters.");
      System.err.println(howToRun());
      return;
    }
    
    try {
      /*
      LogDouble x = new LogDouble(1.0);
      LogDouble y = new LogDouble(0.5);
      LogDouble z = new LogDouble(0.0);
      LogDouble w = new LogDouble(500.0);
      
      System.out.println(x.toRealString() + " - " + y.toRealString() + " = " + x.sub(y).toRealString());
      System.out.println(x.toRealString() + " - " + z.toRealString() + " = " + x.sub(z).toRealString());
      System.out.println(x.toRealString() + " - " + w.toRealString() + " = " + x.sub(w).toRealString());
      
      System.out.println(y.toRealString() + " - " + x.toRealString() + " = " + y.sub(x).toRealString());
      System.out.println(y.toRealString() + " - " + z.toRealString() + " = " + y.sub(z).toRealString());
      System.out.println(y.toRealString() + " - " + w.toRealString() + " = " + y.sub(w).toRealString());
      
      System.out.println(z.toRealString() + " - " + x.toRealString() + " = " + z.sub(x).toRealString());
      System.out.println(z.toRealString() + " - " + y.toRealString() + " = " + z.sub(y).toRealString());
      System.out.println(z.toRealString() + " - " + w.toRealString() + " = " + z.sub(w).toRealString());
      
      System.out.println(w.toRealString() + " - " + x.toRealString() + " = " + w.sub(x).toRealString());
      System.out.println(w.toRealString() + " - " + y.toRealString() + " = " + w.sub(y).toRealString());
      System.out.println(w.toRealString() + " - " + z.toRealString() + " = " + w.sub(z).toRealString());
      
      System.out.println("|" + x.toRealString() + " - " + y.toRealString() + "| = " + x.absDif(y).toRealString());
      System.out.println("|" + x.toRealString() + " - " + z.toRealString() + "| = " + x.absDif(z).toRealString());
      System.out.println("|" + x.toRealString() + " - " + w.toRealString() + "| = " + x.absDif(w).toRealString());
      System.out.println("|" + z.toRealString() + " - " + w.toRealString() + "| = " + z.absDif(w).toRealString());
      System.out.println("|" + z.toRealString() + " - " + y.toRealString() + "| = " + z.absDif(y).toRealString());
      */
      /*for (String s : args) {
        System.out.println(s);
      }*/
      
      // Read true model from file
      GraphModel trueModel = new GraphModel();
      String trueModelPath = args[0];
      trueModel.readUAI(trueModelPath);
      //System.out.println("True model:");
      //System.out.println(trueModel);
      
      // Create and train learner model
      IModelLearner learner = new BN_MLE_FOD();
      String trainDataPath = args[1];
      
      if (args[4].compareTo("0") == 0) { 
        learner = new BN_MLE_FOD();
        learner.train(trainDataPath, trueModel);
      } else if (args[4].compareTo("1") == 0) {
        learner = new BN_EM_POD(Integer.valueOf(args[5]));
        learner.train(trainDataPath, trueModel);
      } else if (args[4].compareTo("2") == 0) {
        IStructureLearner sl = new BS_CL_FOD();
        GraphModel learnedStructure = sl.train(trainDataPath);
        learner = new BN_MLE_FOD();
        learner.train(trainDataPath, learnedStructure);
      } else { 
        System.err.println("Incorrect learning mode.");
        System.err.println(howToRun());
        return;
      }
      
      //System.out.println("Learned a model.");
      //System.out.println(learner.getLearnedModel());
      
      // Write learned model to UAI file
      String outputUAIpath = args[3];
      learner.getLearnedModel().writeUAI(outputUAIpath);
      
      // Set the true model to compare against and test
      learner.setTrueModel(trueModel);      
      String testDataPath = args[2];      
      LogDouble lld = learner.test(testDataPath);
      
      System.out.println("Log likelihood difference = " + lld.toRealString());
    } catch (Exception e) {
      e.printStackTrace();
    } 
  }
  
  public static String howToRun() {
    StringBuilder s = new StringBuilder();
    s.append("Usage:\n");
    s.append("[0] = Input UAI file path\n");
    s.append("[1] = Train file path\n");
    s.append("[2] = Test file path\n");
    s.append("[3] = Output UAI file path\n");
    s.append("[4] = #\n");
    s.append("      0 = MLE FOD Learning");
    s.append("      1 = EM POD Learning");
    s.append("      2 = CL FOD Learning");
    s.append("[5] = #");
    s.append("      k = Number of iterations to run EM. This is only used for EM.");    
    return s.toString();
  }
}

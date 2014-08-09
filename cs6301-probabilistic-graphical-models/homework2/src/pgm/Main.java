package pgm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;

import lib.Lib;
import lib.LogNumber;
import lib.Stopwatch;

public class Main {

	/**
	 * @param args
	 */
  public static void main(String[] args) {
	  String networkConfigPath;
	  String evidencePath;
	  
	  switch (args.length) {
	    case 0:
	      System.err.println("You must supply at least one file path to the network configuration.");
	      return;
	    case 1:
	      networkConfigPath = args[0];
	      evidencePath = "";
	      break;
	    case 2:
	    default:
	      networkConfigPath = args[0];
	      evidencePath = args[1];
	      break;	      
	  }  
	  
    Stopwatch stopwatch = new Stopwatch();

		try {
		  System.out.println(System.getProperty("user.dir"));
		  
		  stopwatch.start();
		  
      GraphModel gm = new GraphModel();
      gm.orderHeuristic = GraphModelOrderHeuristic.MIN_DEGREE;
      gm.eliminationAlgorithm = GraphModelEliminationAlgorithm.BUCKET_ELIMINATION;
      gm.type = GraphModelType.MARKOV;
      stopwatch.start();
      gm.read(networkConfigPath, evidencePath);
      stopwatch.stop();
      System.out.println("File read: " + stopwatch.time());
      
      //System.out.println(gm);
      // Run bucket elimination
      if (gm.type == GraphModelType.BAYES) {
        System.out.println("Probability of evidence log10: " + gm.computeProbabilityOfEvidence());
      } else {
        System.out.println("Partition function log10: " + gm.computeParitionFunctionGivenEvidence());
      }/*
      
      //gm.print(System.out);
		  
		  ArrayList<Integer> libC1 = new ArrayList<Integer>();
      libC1.add(0);
      libC1.add(1);
      libC1.add(2);
      ArrayList<Integer> libC2 = new ArrayList<Integer>();
      libC2.add(10);
      libC2.add(50);
      libC2.add(2);
      LinkedHashSet<Integer> libC3 = new LinkedHashSet<Integer>();
      libC3.add(10);
      libC3.add(50);
      libC3.add(2);
      System.out.println(Lib.containsAny(libC1, libC2));
      System.out.println(Lib.containsAny(libC1, libC3));
      System.out.println(Lib.containsAny(libC2, libC3));
      System.out.println(Lib.containsAny(libC3, libC1));
		  
		  ArrayList<Variable> vars = new ArrayList<Variable>();
      vars.add(new Variable(6));
      vars.add(new Variable(8));
      vars.add(new Variable(5));
      vars.add(new Variable(4));
      vars.add(new Variable(2));
      vars.add(new Variable(2));
      vars.add(new Variable(3));
      
      FunctionTable f2 = new FunctionTable();
      f2.variableIDs.add(3);
      f2.table.add(new LogNumber(.25));
      f2.table.add(new LogNumber(.25));
      f2.table.add(new LogNumber(.25));
      f2.table.add(new LogNumber(.25));
      System.out.println(f2.toRealString());
      ArrayList<FunctionTable> a2 = new ArrayList<FunctionTable>();
      a2.add(f2);      
      
      FunctionTable f3 = new FunctionTable();
      f3.variableIDs.add(2);
      f3.table.add(new LogNumber(.2));
      f3.table.add(new LogNumber(.2));
      f3.table.add(new LogNumber(.2));
      f3.table.add(new LogNumber(.2));
      f3.table.add(new LogNumber(.2));
      System.out.println(f3.toRealString());
      a2.add(f3);
      
      // (3)[1x4] x (2)[1x5]
      FunctionTable f4 = FunctionTable.product(a2, vars);
      System.out.println(f4.toRealString());
      
      FunctionTable f5 = new FunctionTable();
      f5.variableIDs.add(4);
      f5.table.add(new LogNumber(.5));
      f5.table.add(new LogNumber(.5));
      System.out.println(f5.toRealString());
      ArrayList<FunctionTable> a3 = new ArrayList<FunctionTable>();
      a3.add(f5);      
      
      FunctionTable f6 = new FunctionTable();
      f6.variableIDs.add(5);
      f6.variableIDs.add(4);
      f6.table.add(new LogNumber(.9));
      f6.table.add(new LogNumber(.1));
      f6.table.add(new LogNumber(0));
      f6.table.add(new LogNumber(1));
      System.out.println(f6.toRealString());
      a3.add(f6);
      
      // (5,4)[2x2] x (4)[1x2] 
      FunctionTable f7 = FunctionTable.product(a3, vars);
      System.out.println(f7.toRealString());
      
      // ()[1x1] x (5,4)[2x2]
      FunctionTable f8 = new FunctionTable();
      f8.table.add(new LogNumber(.5));
      System.out.println(f8.toRealString());
      ArrayList<FunctionTable> a4 = new ArrayList<FunctionTable>();
      a4.add(f8);
      a4.add(f6);
      FunctionTable f9 = FunctionTable.product(a4, vars);
      System.out.println(f9.toRealString());
      
      // (4)[1x2] x ()[1x1]
      ArrayList<FunctionTable> a5 = new ArrayList<FunctionTable>();
      a5.add(f5);
      a5.add(f8);
      FunctionTable f10 = FunctionTable.product(a5, vars);
      System.out.println(f10.toRealString());
      
      // ()[1x1] x ()[1x1]
      FunctionTable f11 = new FunctionTable();
      f11.table.add(new LogNumber(.7));
      ArrayList<FunctionTable> a6 = new ArrayList<FunctionTable>();
      a6.add(f11);
      a6.add(f8);
      FunctionTable f12 = FunctionTable.product(a6, vars);
      System.out.println(f12.toRealString());
      
      // (5,4)[2x2] x (4)[1x2] x (6,4)[3x2]
      FunctionTable f13 = new FunctionTable();
      f13.variableIDs.add(6);
      f13.variableIDs.add(4);
      f13.table.add(new LogNumber(0.1));
      f13.table.add(new LogNumber(0.25));
      f13.table.add(new LogNumber(0.25));
      f13.table.add(new LogNumber(0.1));
      f13.table.add(new LogNumber(0.1));
      f13.table.add(new LogNumber(0.2));
      ArrayList<FunctionTable> a7 = new ArrayList<FunctionTable>();
      a7.add(f6);
      a7.add(f5);
      a7.add(f13);
      FunctionTable f14 = FunctionTable.product(a7, vars);
      System.out.println(f14.toRealString());
      
      // (5,4)[2x2] x (4)[1x2] x (6,4)[3x2] x ()[1x1]
      a7.add(f8);
      FunctionTable f15 = FunctionTable.product(a7, vars);
      System.out.println(f15.toRealString());     
      
      // (5,4)[2x2].sumOut(4)
      HashSet<Integer> s1 = new HashSet<Integer>();
      s1.add(4);
      FunctionTable f16 = FunctionTable.sumOut(f6, s1, vars);
      System.out.println(f16.toRealString());     
      
      // (5,4)[2x2].sumOut(4)
      FunctionTable f17 = FunctionTable.sumOut(f6, new HashSet<Integer>(), vars);
      System.out.println(f17.toRealString());
      
      // (4)[1x2].sumOut(4)
      FunctionTable f18 = FunctionTable.sumOut(f5, s1, vars);
      System.out.println(f18.toRealString());
      
      // ()[1x1].sumOut(5)
      FunctionTable f19 = FunctionTable.sumOut(f11, s1, vars);
      System.out.println(f19.toRealString());
      
      // (5,4)[2x2] x (4)[1x2] x (6,4)[3x2] x ()[1x1] .sumOut(4)
      FunctionTable f20 = FunctionTable.productSumOut(a7, s1, vars);
      System.out.println(f20.toRealString());

      // (5,4)[2x2].instantiate(5=0)
      HashMap<Integer,Integer> e1 = new HashMap<Integer,Integer>();
      e1.put(5, 0);
      FunctionTable f21 = f6.instantiate(e1, vars);
      System.out.println(f21.toRealString());
      
      // (5,4)[2x2].instantiate(5=0, 4=1)
      e1.put(4, 1);
      FunctionTable f22 = f6.instantiate(e1, vars);
      System.out.println(f22.toRealString());
      
      // (4)[1x2].instantiate(5=0, 4=1)
      FunctionTable f23 = f5.instantiate(e1, vars);
      System.out.println(f23.toRealString());
      
      // ()[1x1].instantiate(5=0, 4=1)
      FunctionTable f24 = f11.instantiate(e1, vars);
      System.out.println(f24.toRealString());
           
      // Uniform normalize
      f3.normalize();
      System.out.println(f3.toRealString());
      
      // nonuniform normalize
      f6.normalize();
      System.out.println(f6.toRealString());
      
      // nonuniform normalize
      f11.normalize();
      System.out.println(f11.toRealString());
      
		  System.out.println("LS_MIN_VAL: " + LogNumber.LS_MIN_VAL);
		  LogNumber n1 = new LogNumber(10.0, false);
		  System.out.println( "1: " + n1);
		  LogNumber n2 = new LogNumber(0.0, false);
		  System.out.println( "2: " + n2);
		  LogNumber n3 = new LogNumber(0.5, false);
		  System.out.println( "3: " + n3);
		  LogNumber n4 = new LogNumber(0.00001, false);
		  System.out.println( "4: " + n4);
		  
		  // Test multiply
		  LogNumber n6 = n1.mul(n2);
		  System.out.println( "1*2: " + n6);
		  LogNumber n7 = n1.mul(n3);
      System.out.println( "1*3: " + n7);
      System.out.println( "1*3->R: " + n7.toReal());
      LogNumber n8 = n3.mul(n4); // 0.177579776681694
      System.out.println( "3*4: " + n8);
      System.out.println( "3*4->R: " + n8.toReal());
      LogNumber n9 = n2.mul(n4);
      System.out.println( "2*4: " + n9);
      
      // Test divide
      LogNumber n10 = n1.div(n2); // divide by 0
      System.out.println("1/2: " + n10);
      LogNumber n11 = n1.div(n3);      
      System.out.println("1/3: " + n11);
      System.out.println("1/3->R " + n11.toReal());
      LogNumber n12 = n2.div(n3);
      System.out.println("2/3: " + n12); // 0
      LogNumber n13 = n3.div(n4);
      System.out.println("3/4: " + n13);
      System.out.println("3/4->R " + n13.toReal());
      
      // Test add
      LogNumber n14 = n1.add(n2); // add 0
      System.out.println("1+2: " + n14);
      System.out.println("1+2->R " + n14.toReal());
      LogNumber n15 = n2.add(n3); // 0 add
      System.out.println("2+3: " + n15);
      System.out.println("2+3->R " + n15.toReal());
      LogNumber n16 = n1.add(n3); // 10+.5
      System.out.println("1+3: " + n16);
      System.out.println("1+3->R " + n16.toReal());
      LogNumber n17 = n3.add(n4); // .5+.00001
      System.out.println("3+4: " + n17);
      System.out.println("3+4->R " + n17.toReal());
      
      // Test subtract
      // not implemented yet (;_;)	  */
		  
      stopwatch.stop();
      System.out.println(stopwatch.toString());
      
    } catch (Exception e) {
      e.printStackTrace();
    }		
	}

}

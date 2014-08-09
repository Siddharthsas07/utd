package utd.cs.pgm.core;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Stack;
import java.util.concurrent.ExecutionException;

import utd.cs.lib.Stopwatch;
import utd.cs.pgm.ao.core.tree.JunctionTree;
import utd.cs.pgm.ao.core.tree.PseudoTree;
import utd.cs.pgm.core.graphmodel.*;
import utd.cs.pgm.core.variable.IVariable;
import utd.cs.pgm.probability.DynamicDistributionDos;
import utd.cs.pgm.util.LogDouble;

public class Main {
  public static void main(String[] args) throws InterruptedException, ExecutionException {
    if (args.length != 3) {
      printHelp();
    }

    String uaiPath = args[0];
    String evidPath = args[1];
    int numSamples = Integer.valueOf(args[2]);

    // create graphical model from the uai and uaievid files.
    GraphModel gm = new GraphModel();
    try {
      gm.readUAI(uaiPath);
      gm.readUAIevid(evidPath);
    } catch (IOException e) {
      e.printStackTrace();
    }

    // If gm is BAYES moralize it and make it MARKOV.
    ArrayList<HashSet<IVariable>> markovStruct = gm.moralizeGraph();

    // Create a pseudotree off of the structure of gm.
    PseudoTree t = new PseudoTree(markovStruct);

    // The partition function approximation.
    LogDouble z = LogDouble.LS_ZERO;

    // A running total of all the partition function approximations.
    LogDouble sum = LogDouble.LS_ZERO;

    // Create a proposal distribution, Q, for sampling.
    DynamicDistributionDos Q = new DynamicDistributionDos(gm.getVariables());

    Stopwatch sw = new Stopwatch();
    sw.start();

    // Compute numIterations approximations, off of numSamplesPerIteration samples,
    // then average those and return the result.
    int numSamplesPerIteration = 100;
    int numIterations = numSamples / numSamplesPerIteration;
    for (int i = 0; i < numIterations; i++) {
      Q.unmarkAll();

      ArrayList<ArrayList<Integer>> samples = Q.generateSamples(numSamplesPerIteration, gm);

      JunctionTree j = new JunctionTree(gm, samples);

      j.buildTree(t.getRoot(), j.getRoot(), new Stack<IVariable>(), Q);

      z = j.computeZ();

      sum = sum.add(z);

      Q.update(samples);

      gm.unmarkAll();
    }

    sw.stop();

    //System.out.println("z = " + sum.div(new LogDouble(numSamples/100)));
    //System.out.println("t = " + sw);
    System.out.println(sum.div(new LogDouble(numSamples/100)) + "," + sw);
  }

  protected static void printHelp() {
    System.out.println("[0] = uai file \n[1] = uai evid file \n[2] = num samples.");
  }
}

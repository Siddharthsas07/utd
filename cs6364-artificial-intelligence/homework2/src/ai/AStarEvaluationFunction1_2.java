package ai;

import aima.core.search.framework.EvaluationFunction;
import aima.core.search.framework.HeuristicFunction;
import aima.core.search.framework.Node;
import aima.core.search.framework.PathCostFunction;

public class AStarEvaluationFunction1_2 implements EvaluationFunction
{
	private PathCostFunction gf = new PathCostFunction();
	private HeuristicFunction hf = null;
	private double weight = 0;

	public AStarEvaluationFunction1_2(HeuristicFunction hf, double w) 
	{
		this.hf = hf;
		weight = w;
	}

	public double f(Node n) 
	{
		// f(n) = g(n) + h(n)
		return (1-weight)*gf.g(n) + weight*hf.h(n.getState());
	}
}

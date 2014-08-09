package ai;

import aima.core.search.framework.EvaluationFunction;
import aima.core.search.framework.HeuristicFunction;
import aima.core.search.framework.Node;
import aima.core.search.framework.PathCostFunction;

public class AStarEvaluationFunction1_3 implements EvaluationFunction
{
	private PathCostFunction gf = new PathCostFunction();
	private HeuristicFunction hf = null;

	public AStarEvaluationFunction1_3(HeuristicFunction hf) 
	{
		this.hf = hf;
	}

	public double f(Node n) 
	{
		// f(n) = g(n) + h(n)
		return gf.g(n) + hf.h(n.getState());
	}
}

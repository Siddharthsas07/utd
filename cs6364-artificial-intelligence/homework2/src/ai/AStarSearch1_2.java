package ai;

import aima.core.search.framework.HeuristicFunction;
import aima.core.search.framework.QueueSearch;
import aima.core.search.informed.AStarEvaluationFunction;
import aima.core.search.informed.BestFirstSearch;

public class AStarSearch1_2 extends BestFirstSearch 
{
	public AStarSearch1_2(QueueSearch search, HeuristicFunction hf, double w) 
	{
		super(search, new AStarEvaluationFunction1_2(hf, w));		
	}
}

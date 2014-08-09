package ai;

import aima.core.search.framework.HeuristicFunction;
import aima.core.search.framework.QueueSearch;
import aima.core.search.informed.AStarEvaluationFunction;
import aima.core.search.informed.BestFirstSearch;

public class AStarSearch1_3 extends BestFirstSearch 
{
	public AStarSearch1_3(QueueSearch search, HeuristicFunction hf) 
	{
		super(search, new AStarEvaluationFunction1_3(hf));		
	}
}

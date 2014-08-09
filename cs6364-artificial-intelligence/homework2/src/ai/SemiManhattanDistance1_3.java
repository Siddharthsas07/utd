package ai;

import aima.core.environment.map.AdaptableHeuristicFunction;
import aima.core.environment.map.Map;
import aima.core.util.datastructure.Point2D;

public class SemiManhattanDistance1_3 extends AdaptableHeuristicFunction 
{
	public SemiManhattanDistance1_3(Object goal, Map map) 
	{
		this.goal = goal;
		this.map = map;
	}

	public double h(Object state) 
	{
		double result = 0.0;
		Point2D pt1 = map.getPosition((String) state);
		Point2D pt2 = map.getPosition((String) goal);
		
		if (pt1 != null && pt2 != null) 
		{
			result = Math.sqrt( 
						Math.abs(
							pt1.getY() - pt2.getY()
						) 
						+ 
						Math.abs(
							pt1.getX() - pt2.getX()
						) 
					);
		}
		
		return result;
	}
}

/*
 * @file homework2.java
 * @auth Gary Steelman
 * @desc CS6364 Homework 2 Question 1 - A* Search
 */


package ai;

import aima.core.environment.map.Map;
import aima.core.environment.map.MapFunctionFactory;
import aima.core.environment.map.MapStepCostFunction;
import aima.core.environment.map.SimplifiedRoadMapOfPartOfRomania;
import aima.core.environment.map.StraightLineDistanceHeuristicFunction;
import aima.core.search.framework.DefaultGoalTest;
import aima.core.search.framework.GraphSearch;
import aima.core.search.framework.Problem;
import aima.core.search.framework.Search;
import aima.core.search.framework.SearchAgent;
import aima.core.search.informed.AStarSearch;

public class homework2 
{
	public static void main(String[] args) 
	{
		try
		{
			// Set up the map and the problem for homework 2's map of Romania
			Map romaniaMap = new SimplifiedRoadMapOfPartOfRomania();
			
			Problem problem = new Problem(
					SimplifiedRoadMapOfPartOfRomania.TIMISOARA,
					MapFunctionFactory.getActionsFunction(romaniaMap),
					MapFunctionFactory.getResultFunction(), 
					new DefaultGoalTest(SimplifiedRoadMapOfPartOfRomania.BUCHAREST),
					new MapStepCostFunction(romaniaMap)
					);

			// Problem 1-1
			Search search1_1 = new AStarSearch(
					new GraphSearch(),
					new StraightLineDistanceHeuristicFunction(
							SimplifiedRoadMapOfPartOfRomania.BUCHAREST, romaniaMap
							)
					);
			
			SearchAgent agent1_1 = new SearchAgent(problem, search1_1);
			
			print_results("1.1", "Default A*", agent1_1);
			
			// Problem 1-2 (w=.75)
			Search search1_2_75 = new AStarSearch1_2(
				new GraphSearch(),
				new StraightLineDistanceHeuristicFunction(
						SimplifiedRoadMapOfPartOfRomania.BUCHAREST, romaniaMap
						),
				.75
			);
			
			SearchAgent agent1_2_75 = new SearchAgent(problem, search1_2_75);
			
			print_results("1.2", "Weighted A* (w=0.75)", agent1_2_75);
			
			// Problem 1-2 (w=.25)
			Search search1_2_25 = new AStarSearch1_2(
				new GraphSearch(),
				new StraightLineDistanceHeuristicFunction(
						SimplifiedRoadMapOfPartOfRomania.BUCHAREST, romaniaMap
						),
				.25
			);
			
			SearchAgent agent1_2_25 = new SearchAgent(problem, search1_2_25);
			
			print_results("1.2", "Weighted A* (w=0.25)", agent1_2_25);
			
			// Problem 1-3
			Search search1_3 = new AStarSearch1_3(
				new GraphSearch(),
				new SemiManhattanDistance1_3(
						SimplifiedRoadMapOfPartOfRomania.BUCHAREST, romaniaMap
						)
			);
			
			SearchAgent agent1_3 = new SearchAgent(problem, search1_3);
			
			print_results("1.3", "Default A* + Semi-Manhattan Distance", agent1_3);
		}
		
		catch ( Exception e )
		{
			System.out.println( "Exception: " );
			System.out.println( e.toString( ) );
		}
	}
	
	static void print_results( String number, String title, SearchAgent agent )
	{
		System.out.println(number);
		System.out.println(title + ":");
		System.out.println("Solution Path:");
		for ( int i = 0; i < agent.getActions().size(); i++ )
			System.out.println("\t" + agent.getActions().get(i).toString());
		System.out.println("Nodes Expanded: " + agent.getInstrumentation().getProperty("nodesExpanded"));
		System.out.println("Path cost: " + agent.getInstrumentation().getProperty("pathCost"));
		System.out.println();
	}

}

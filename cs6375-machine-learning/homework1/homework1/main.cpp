#include <iostream>
#include <string>
#include <vector>

// #include "hr_timer.h"
#include "dataset.h"
#include "decision_tree.h"
#include "d_tree_heuristic.h"
#include "h_infogain.h"
#include "h_1step_lookahead.h"
#include "h_variance_impurity.h"

using namespace std;

void main( int argc, char* argv[] )
{
  // Capture command line parameters
  if ( argc != 7 )
  {
    cout << "Incorrect command line parameters. See instructions for proper usage." << endl;
    return;
  }

  int L = atoi( argv[1] );
  int K = atoi( argv[2] );
  string trainingSetLoc = argv[3];
  string validationSetLoc = argv[4];
  string testSetLoc = argv[5];
  string toPrint = argv[6];

  // Load data sets from files
  DataSet<bool> trainingData;
  DataSet<bool> validationData;
  DataSet<bool> testData;

  if ( !trainingData.load( trainingSetLoc ) 
    || !validationData.load( validationSetLoc )
    || !testData.load( testSetLoc ) )
  {
    cout << "Could not load data sets." << endl;
    return;
  }

  trainingData.POS = true;
  trainingData.NEG = false;
  validationData.POS = true;
  validationData.NEG = false;
  testData.POS = true;
  testData.NEG = false;

  // Set of heuristics to use
  vector<DTreeHeuristic<bool>* > heuristics;
  heuristics.push_back( new h_InfoGain<bool>() );
  heuristics.push_back( new h_1StepLookAhead<bool>() );
  heuristics.push_back( new h_VarianceImpurity<bool>() );

  // Vector of decisionTree, one for each heuristic.
  vector<DecisionTree<bool> > trees( heuristics.size() );  

  // CStopWatch timer;  

  // Generate trees.
  for ( unsigned int i = 0; i < trees.size(); i++ )
  {
    // timer.startTimer();
    trees[i].setHeuristic( heuristics[i] );
    trees[i].ID3( trainingData, trainingData.m_n.size()-1 );
    // timer.stopTimer();

    // cout << "Tree created using the " << heuristics[i]->m_label 
    //   << " heuristic took " << timer.getElapsedTime() << " seconds." << endl;
  }

  // Output accuracy of trees.
  for ( unsigned int i = 0; i < trees.size(); i++ )
  {
    float accuracy = trees[i].test( testData, testData.m_n.size()-1 );
    cout << "Tree created using " << heuristics[i]->m_label 
      << ": Pre-Prune Accuracy on Test Data = " << (accuracy*100.0f) << "%" << endl;
  }

  // Prune some trees.
  for ( unsigned int i = 0; i < trees.size(); i ++ )
  {
    // timer.startTimer();
    trees[i].prune( trainingData, validationData, validationData.m_n.size()-1, L, K );
    // timer.stopTimer();
    
    // cout << "Pruning tree created using " << heuristics[i]->m_label 
    //   << " heuristic took " << timer.getElapsedTime() << " seconds." << endl;
  }
  
  // Output accuracy of pruned trees.
  for ( unsigned int i = 0; i < trees.size(); i++ )
  {
    float accuracy = trees[i].test( testData, testData.m_n.size()-1 );
    cout << "Tree created using " << heuristics[i]->m_label 
      << ": Post-Prune Accuracy on Test Data = " << (accuracy*100.0f) << "%" << endl;
  }

  // TODO Output trees
  if ( toPrint == "yes" || toPrint == "YES" )
  {
    for ( unsigned int i = 0; i < trees.size(); i++ )
    {
      trees[i].print();
    }
  }  

  return;
}


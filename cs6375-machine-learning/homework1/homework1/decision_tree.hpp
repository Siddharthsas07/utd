#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "dataset.h"
#include "dnode.h"
#include "d_tree_heuristic.h"

using namespace std;

template <class T>
DecisionTree<T>::DecisionTree() : m_size(0)
{
  m_root = NULL; 
  m_heuristic = NULL; 
}

template <class T>
DecisionTree<T>::~DecisionTree()
{
  f_deleteTree( m_root );
  delete m_root;
}

template <class T>
void DecisionTree<T>::f_deleteTree( DNode<T>* root )
{
  if ( root == NULL || isLeaf( root ) )
  {
    return;
  }

  // Depth-first search to bottom-left of the tree
  for ( unsigned int i = 0; i < root->m_branches.size(); i++ )
  {
    f_deleteTree( root->m_branches[i].first );
  }

  // Delete this node's children
  for ( unsigned int i = 0; i < root->m_branches.size(); i++ )
  {
    delete root->m_branches[i].first;
  }

  root->m_branches.clear();
}

template <class T>
DecisionTree<T>& DecisionTree<T>::operator=( const DecisionTree<T>& rhs )
{
  if ( this != &rhs )
  {
    copy( &rhs );
  }

  return *this;
}

template <class T>
void DecisionTree<T>::copy( const DecisionTree<T>* rhs )
{
  // Clear the contents of this tree.
  clear();

  if ( rhs->m_root != NULL )
  {
    m_root = new DNode<T>( NULL );
    f_copyTree( m_root, rhs->m_root );

    m_size = rhs->m_size;
    m_num_leaves = rhs->m_num_leaves;

    // Not a deep copy since heuristic is not a member of tree
    m_heuristic = rhs->m_heuristic;
  }  
}

// assumes lhs has no chilluns and has had its parent set
template <class T>
void DecisionTree<T>::f_copyTree( DNode<T>* lhs, DNode<T>* rhs )
{  
  if ( isLeaf( rhs ) )
  {
    convertToLeaf( lhs, rhs->m_branches[0].second );
  }

  else
  {
    lhs->m_branchAttr = rhs->m_branchAttr;
    lhs->m_label = rhs->m_label;

    for ( unsigned int i = 0; i < rhs->m_branches.size(); i++ )
    {            
      DNode<T>* newChild = addBranch( lhs, rhs->m_branches[i].second );
      f_copyTree( newChild, rhs->m_branches[i].first );
    }
  }  
}

template <class T>
void DecisionTree<T>::ID3( const DataSet<T>& data, unsigned int target )
{
  // Check any obvious conditions that will cause ID3 algorithm to fail.
  if ( data.m_d.empty() )
  {
    cout << "DataSet is empty." << endl;
    return;
  }

  else if ( data.m_n.empty() )
  {
    cout << "DataSet headers are empty." << endl;
    return;
  }

  else if ( target > data.m_n.size()-1 )
  {
    cout << "Target class attribute is not within range of data set." << endl;
    cout << "Attribute column = " << target << " and range is [0, " 
      << data.m_n.size()-1 << ")." << endl;
    return;
  }

  else if ( data.m_d[0].size() != data.m_n.size() )
  {
    cout << "Data width does not match number of headers." << endl;
    cout << "Data width (columns) = " << data.m_d.size() << endl;
    cout << "Header width (columns) = " << data.m_n.size() << endl;
    return;
  }

  else if ( m_heuristic == NULL )
  {
    cout << "No heuristic has been set." << endl;
    return;
  }

  // Generate a list of available attributes (by index) in data for ID3 to
  // branch on. Disclude the target (class) attribute (column).
  vector<unsigned int> attrAvail;
  for ( unsigned int i = 0; i < data.m_n.size(); i++ )
  {
    if ( i != target )
    {
      attrAvail.push_back( i ); 
    }
  }

  // Clear any previously generated tree.
  if ( m_root != NULL )
  {
    clear();
    delete m_root;
    m_root = NULL;
  }

  // Create a new root node whose parent is NULL.
  m_root = new DNode<T>( NULL );
  m_size++;

  // Execute real ID3.
  f_ID3( data, target, attrAvail, m_root );
}

template <class T>
void DecisionTree<T>::f_ID3( const DataSet<T>& data, 
                            unsigned int target, 
                            const vector<unsigned int>& attrAvail,
                            DNode<T>* root )
{
  /* Base cases */
  if ( data.pure( target, data.POS ) )
  {
    convertToLeaf( root, data.POS );
    return;
  }

  if ( data.pure( target, data.NEG ) )
  {
    convertToLeaf( root, data.NEG );
    return;
  }

  if ( attrAvail.empty() )
  {
    convertToLeaf( root, data.getMostCommonValue( target ) );
    return;
  }

  /* Data for this node requires child generation. */
  root->m_branchAttr = m_heuristic->evaluate( data, target, attrAvail );
  root->m_label = data.m_n[root->m_branchAttr];

  // Get possible attribute values for chosen branch attribute.
  vector<T> uniqueAttrVals;
  data.getUniqueValues( root->m_branchAttr, uniqueAttrVals );

  // Create child as branch for each possible value.
  for ( unsigned int i = 0; i < uniqueAttrVals.size(); i++ )
  {
    // Add a branch from root to a new, emptpy child, branching on 
    // the possible value i for the chosen attribute.
    DNode<T>* newChild = addBranch( root, uniqueAttrVals[i] );    

    // Populate new node as either leaf or child.
    DataSet<T> dataMatch;
    data.getMatchingVectors( dataMatch, root->m_branchAttr, uniqueAttrVals[i] );
    dataMatch.copyHeaders( data );

    if ( dataMatch.m_d.empty() )
    {
      // No matching feature vectors, no more solid data to use. 
      // Guess using majority vote.
      convertToLeaf( newChild, data.getMostCommonValue( target ) );  
      return;
    }

    else
    {
      // Populate child. Remove chosen attribute from available attributes.
      vector<unsigned int> updatedAttrAvail( attrAvail );
      vector<unsigned int>::iterator it;
      for ( it = updatedAttrAvail.begin(); it != updatedAttrAvail.end(); ++it )
      {
        if ( *it == root->m_branchAttr )
        {
          updatedAttrAvail.erase( it );
        }
        break;
      } 

      f_ID3( dataMatch, target, updatedAttrAvail, newChild );
    }
  }
}

template <class T>
void DecisionTree<T>::prune( const DataSet<T>& trainingData, 
                            const DataSet<T>& validationData,
                            unsigned int target,
                            unsigned int numTrees,
                            unsigned int K )
{
  if ( m_root == NULL || isLeaf( m_root ) )
  {
    return;
  }

  else
  {
    DecisionTree<T> bestTree;
    bestTree = *this;

    // Build different candidate trees.
    for ( unsigned int i = 1; i < numTrees; i++ )
    {
      DecisionTree<T> candidateTree;
      candidateTree = *this;

      srand( time(NULL));
      
      unsigned int M = rand() % K + 1;
      if ( M > candidateTree.m_size ) 
      {
        M = candidateTree.m_size;
      }

      // Prune candidate tree M times. 
      for ( unsigned int j = 1; j < M; j++ ) 
      {
        chooseAndPrune( candidateTree, candidateTree.m_root, trainingData, target, 0 );
      }
      
      // See if pruned tree is more accurate.
      float candidateAccuracy = candidateTree.test( validationData, target );
      float bestAccuracy = bestTree.test( validationData, target );

      if ( candidateAccuracy > bestAccuracy )
      {
        bestTree = candidateTree;
      }
    }

    // This tree is the best tree found via pruning.
    *this = bestTree;
  }
}

template <class T>
void DecisionTree<T>::chooseAndPrune( DecisionTree<T>& tree,
                                     DNode<T>* root,
                                     const DataSet<T>& data, 
                                     unsigned int target,
                                     unsigned int depth )
{
  if ( root == NULL )
  {
    return;
  }

  srand( time(NULL) );
  unsigned int childChoice = rand() % (root->m_branches.size());
  DNode<T> * child = root->m_branches[childChoice].first;

  // 2nd to bottom level, auto-prune.
  if ( isLeaf( child ) )
  {
    f_prune( tree, root );

    T value = data.getMostCommonValue( target );
    convertToLeaf( root, value );
  }

  else
  {
    
    unsigned int cutoff = depth*depth;
    unsigned int roll = rand() % 100;
    
    // If below the cutoff, prune subtree from tree
    if ( roll <= cutoff )
    {
      f_prune( tree, root );
      T value = data.getMostCommonValue( target );
      convertToLeaf( root, value );
    }

    else
    {
      DataSet<T> subset;
      subset.copyHeaders( data );
      subset.POS = data.POS;
      subset.NEG = data.NEG;
      data.getMatchingVectors( subset, target, root->m_branches[childChoice].second );
      chooseAndPrune( tree, child, subset, target, depth+1 );
    }
  }
}

// prune tree rooted at root and replace with data's value
template <class T>
void DecisionTree<T>::f_prune( DecisionTree<T>& tree, DNode<T>* root )
{
  if ( root == NULL || isLeaf( root ) )
  {
    return;
  }

  // Depth-first search to bottom-left of the tree
  for ( unsigned int i = 0; i < root->m_branches.size(); i++ )
  {
    f_prune( tree, root->m_branches[i].first );
  }

  // Delete this node's children
  for ( unsigned int i = 0; i < root->m_branches.size(); i++ )
  {
    delete root->m_branches[i].first;
    tree.m_size--;
  }

  root->m_branches.clear();
}


template <class T>
float DecisionTree<T>::test( const DataSet<T>& data, unsigned int target )
{
  if ( m_root == NULL )
  {
    return 0.0f;
  }

  unsigned int correctClassifications = 0;

  for ( unsigned int i = 0; i < data.m_d.size(); i++ )
  {
    if ( classify( data.m_d[i], m_root ) == data.m_d[i][target] )
    {
      correctClassifications++;
    }
  }

  return static_cast<float>(correctClassifications) / static_cast<float>(data.m_d.size());
}

template <class T>
T DecisionTree<T>::classify( const vector<T>& v, DNode<T>* root )
{
  // Child node. Return classification.
  if ( isLeaf( root ) )
  {
    return root->m_branches[0].second;
  }

  else
  {
    for ( unsigned int i = 0; i < root->m_branches.size(); i++ )
    {
      // If v's value at this node's branching attribute is that of
      // the i'th branch, descend to that node.
      if ( v[root->m_branchAttr] == root->m_branches[i].second )
      {
        return classify( v, root->m_branches[i].first );
      }
    }
  }
}

template <class T>
DNode<T> * DecisionTree<T>::root()
{
  return m_root;
}

template <class T>
void DecisionTree<T>::setHeuristic( DTreeHeuristic<T>* h )
{
  m_heuristic = h;
}

template <class T>
void DecisionTree<T>::clear()
{
  f_deleteTree( m_root );
  delete m_root;

  m_root = NULL;  
  m_heuristic = NULL; 
  m_size = 0;
}

template <class T>
bool DecisionTree<T>::empty()
{
  return (m_size == 0);
}

template <class T>
unsigned int DecisionTree<T>::size()
{
  return m_size;
}

template <class T>
void DecisionTree<T>::print()
{
  if ( m_root == NULL )
  {
    cout << "Empty tree" << endl;
    return;
  }

  cout << "\nTree generated using " << m_heuristic->m_label << ":" << endl;

  f_print( m_root, 0 );  
}

template <class T>
void DecisionTree<T>::f_print( DNode<T>* root, unsigned int depth )
{ 
  // Base case: Leaf node. Print the only attribute value, which is the class value.
  if ( isLeaf( root ) )
  {
    cout << root->m_branches[0].second;
  }

  // Non-leaf node. Print spaces, label (index) XE, value used to branch to next child.
  else
  {
    for ( unsigned int i = 0; i < root->m_branches.size(); i++ )
    {
      cout << endl;
      cout << repeat( "| ", depth ) << root->m_label << " = " 
        << root->m_branches[i].second << " : ";
      f_print( root->m_branches[i].first, depth+1 );
    }      
  }
}

string repeat( string s, unsigned int n )
{
  string r("");
  for ( unsigned int i = 0; i < n; i++ )
  {
    r += s;
  }
  return r;
}

template <class T>
DNode<T>* DecisionTree<T>::addBranch( DNode<T>* parent )
{
  m_size++;

  // Create a new branch containing a new node with this node as parent.
  pair<DNode<T>*, T> newBranch( new DNode<T>( parent, 0 ), (T)0 );
  parent->m_branches.push_back( newBranch );

  return newBranch.first;
}

template <class T>
DNode<T>* DecisionTree<T>::addBranch( DNode<T>* parent, T attrBranch )
{
  m_size++;

  // Create a new branch containing a new node with this node as parent.
  pair<DNode<T>*, T> newBranch( new DNode<T>( parent, 0 ), attrBranch );
  parent->m_branches.push_back( newBranch );

  return newBranch.first;
}

template <class T>
DNode<T>* DecisionTree<T>::addBranch( DNode<T>* parent, DNode<T>* child )
{
  m_size++;

  // Create a new branch containing the child node.
  pair<DNode<T>*, T> newBranch( child, 0 );
  parent->m_branches.push_back( newBranch );

  return child;
}

template <class T>
void DecisionTree<T>::convertToLeaf( DNode<T>* node, T value )
{
  m_num_leaves++;

  node->m_branchAttr = -1;
  node->m_label = "leaf";

  pair<DNode<T>*, T> newBranch( NULL, value );
  node->m_branches.push_back( newBranch );
}

template <class T>
bool DecisionTree<T>::isLeaf( DNode<T>* node )
{
  if ( node == NULL )
  {
    return false;
  }

  else if ( node->m_branchAttr == -1 )
  {
    return true;
  }

  return false;
}
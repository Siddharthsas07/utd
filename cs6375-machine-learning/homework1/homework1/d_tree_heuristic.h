#ifndef H_TREE_HEURISTIC
#define H_TREE_HEURISTIC 

#include <vector>
#include "dataset.h"

using namespace std;

/**
 * \brief Interface for a decision tree heuristic.
 */
template <class T>
class DTreeHeuristic
{
  public:

    /**
     * \brief Evaluates data with respect to target and returns the 
     *  expected information gain.
     * \pre target is in [0, data.m_d.size()).
     * \param data -- DataSet to evaluate.
     * \param target -- The target attribute in data.
     * \param attrAvail -- A vector of attributes available to consider for
     *  branching on.
     */
    virtual unsigned int evaluate( 
      const DataSet<T>& data, 
      unsigned int target,
      const vector<unsigned int>& attrAvail ) = 0;

    /**
     * \brief The name of the heuristic.
     */
    string m_label;
};

#endif

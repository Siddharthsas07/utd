#ifndef H_VARIANCE_IMPURITY
#define H_VARIANCE_IMPURITY

#include <vector>
#include "dataset.h"

using namespace std;

/**
 * \brief The Variance Impurity heuristic for a decision tree.
 */
template <class T>
class h_VarianceImpurity : public DTreeHeuristic<T>
{
  public:

    /**
     * \brief The default constructor.
     */
    h_VarianceImpurity()
    { 
      m_label = "Variance Impurity";
    }

    /**
     * \brief Evaluates data with respect to target and returns the 
     *  expected information gain.
     * \pre target is in [0, data.m_d.size()).
     * \param data -- DataSet to evaluate.
     * \param target -- The target attribute in data.
     * \param attrAvail -- A vector of attributes available to consider for
     *  branching on.
     * \return The attribute to branch on which will result in maximum
     *  information gain. If an attribute cannot be chosen, -1 will be
     *  returned.
     */
    unsigned int evaluate( const DataSet<T>& data, unsigned int target, const vector<unsigned int>& attrAvail )
    {
      if ( attrAvail.empty() )
      {
        return static_cast<unsigned int>(-1);
      }
      
      // For each of the available attributes, calculate the gain.
      // Maintain maximum gain and which attribute resulted in it.
      float maxGain = 0;
      unsigned int bestAttrIdx = 0;

      for ( unsigned int i = 0; i < attrAvail.size(); i++ )
      { 
        float infoGain = gain( data, target, attrAvail[i] );

        if ( infoGain > maxGain )
        {
          maxGain = infoGain;
          bestAttrIdx = i;
        }
      }  

      return attrAvail[bestAttrIdx];
    }

    /**
     * \brief Calculates the reduction in entropy by choosing attribute attr.
     * \pre attr is in [0, data.m_d.size()).
     * \pre target is in [0, data.m_d.size()).
     * \param data -- The DataSet to look in.
     * \param target -- The class attribute in data.
     * \param attr -- The attribute to calculate reduction for.
     * \return The reduciton in entropy expected by choosing attribute attr.
     */
    float gain( const DataSet<T>& data, unsigned int target, unsigned int attr )
    {
      return data.varianceImpurity( target ) - varianceReduction( data, target, attr );
    }

    /**
     * \brief Calculates the reduction in variance by choosing attribute attr.
     * \pre attr is in [0, data.m_d.size()).
     * \pre target is in [0, data.m_d.size()).
     * \param data -- The DataSet to look in.
     * \param target -- The class attribute in data.
     * \param attr -- The attribute to calculate reduction for.
     * \return The reduciton in entropy expected by choosing attribute attr.
     */
    float varianceReduction( const DataSet<T>& data, unsigned int target, unsigned int attr )
    {
      float varianceReduction = 0;

      vector<T> attrUniqueValues;
      data.getUniqueValues( attr, attrUniqueValues );

      for ( unsigned int i = 0; i < attrUniqueValues.size(); i ++ )
      {
        DataSet<T> partitionedData;
        data.getMatchingVectors( partitionedData, attr, attrUniqueValues[i] );

        varianceReduction += 
          (   static_cast<float>(partitionedData.m_d.size()) 
            / static_cast<float>(data.m_d.size()) 
            * partitionedData.varianceImpurity( target ) 
          );
      }

      return varianceReduction;
    }
};

#endif

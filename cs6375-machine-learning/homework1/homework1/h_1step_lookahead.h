#ifndef H_1STEP_LOOKAHEAD
#define H_1STEP_LOOKAHEAD

#include <vector>
#include "dataset.h"

using namespace std;

/**
 * \brief The 1-step look-ahead heuristic for a decision tree.
 */
template <class T>
class h_1StepLookAhead : public DTreeHeuristic<T>
{
  public:

    /**
     * \brief The default constructor.
     */
    h_1StepLookAhead()
    { 
      m_label = "1-Step Look-Ahead";
    }

    /**
     * \brief Evaluates data with respect to target and returns the 
     *  the attribute to branch on to create the shallowest tree.
     * \pre target is in [0, data.m_d.size()).
     * \param data -- DataSet to evaluate.
     * \param target -- The target attribute in data.
     * \param attrAvail -- A vector of attributes available to consider for
     *  branching on.
     * \return The attribute to branch on which will result in maximum
     *  minimal classification errors for the next tree level.
     */
    unsigned int evaluate( const DataSet<T>& data, unsigned int target, const vector<unsigned int>& attrAvail )
    {
      if ( attrAvail.empty() )
      {
        return static_cast<unsigned int>(-1);
      }
      
      // For each of the available attributes, calculate the gain.
      // Maintain current maximum gain and which attribute resulted in it.
      unsigned int minErrors = -1;
      unsigned int bestAttrIdx = 0;

      for ( unsigned int i = 0; i < attrAvail.size(); i++ )
      { 
        unsigned int errors = 0;

        DataSet<T> setPOS;
        setPOS.copyHeaders( data );
        DataSet<T> setNEG;
        setNEG.copyHeaders( data );

        data.getMatchingVectors( setPOS, attrAvail[i], data.POS );
        data.getMatchingVectors( setNEG, attrAvail[i], data.NEG );

        T targetMajValNEG = setNEG.getMostCommonValue( target );
        T targetMajValPOS = setPOS.getMostCommonValue( target );

        for ( unsigned int j = 0; j < setNEG.m_d.size(); j++ )
        {
          if ( setNEG.m_d[j][target] != targetMajValNEG )
          {
            errors++;
          }
        }

        for ( unsigned int j = 0; j < setPOS.m_d.size(); j++ )
        {
          if ( setPOS.m_d[j][target] != targetMajValPOS )
          {
            errors++;
          }
        }

        if ( errors < minErrors )
        {
          minErrors = errors;
          bestAttrIdx = i;
        }
      }

      return attrAvail[bestAttrIdx];
    }
};

#endif

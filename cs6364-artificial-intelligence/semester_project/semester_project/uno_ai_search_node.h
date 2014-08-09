/*! \file uno_ai_search_node.h
    \brief Contains a weighted player view of the game. The weight represents
      how good the AI thinks the state is. 
*/

#ifndef UNO_AI_SEARCH_NODE_H
#define UNO_AI_SEARCH_NODE_H

#include "uno_pstate.h"
#include "uno_action.h"
#include <vector>

using namespace std;

/** 
 * \brief Contains a history of actions to reach the contained Uno_PState and
 *   a state evaluation score for the contained Uno_PState. 
 */
class Uno_AI_Search_Node
{
  public:
    /**
     * \brief Default constructor.
     *
     * Constructs an Uno_AI_Think_State with no score.
     */
    Uno_AI_Search_Node() : m_score(0)
    {}

    /**
     * \brief Returns true if LHS has a lower score than RHS.
     * \param rhs The right hand side of the comparison.
     * \retval true If the calling object has a lower score.
     * \retval false Otherwise (including if the scores are equal).
     */
    inline bool operator<( const Uno_AI_Search_Node& rhs ) const
    {
      return ( m_score < rhs.m_score );
    }

    /**
     * \brief The limited, player-view of the game state the AI is considering.
     */
    Uno_PState m_state;

    /**
     * \brief The list of actions taken to reach this state. 
     */
    vector<Uno_Action> m_actions;

    /**
     * \brief The AI's state evaluation score for this state.
     */
    float m_score;
};

#endif
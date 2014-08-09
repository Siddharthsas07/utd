/*! \file uno_gstate.h
    \brief Contains the full omniscient server view of the game state. 
    \sa uno_gstate.cpp
*/

#ifndef UNO_GSTATE_H
#define UNO_GSTATE_H

#include "uno_deck.h"
#include "uno_state.h"
#include "uno_player.h"
#include <vector>

using namespace std;

/** 
 * \brief Contains the full omniscient server view of the game state. 
 *
 * This class contains the information for:
 * \li List of Uno_Player (s).
 * \li Player at play.
 * \li Turn count.
 * \li Unplayed deck of cards.
 * \li Played deck of cards. 
 * \li Time allowance per turn. 
 */
class Uno_GState : public Uno_State
{
  public:
    /**
     * \brief The list of players currently in the game. 
     */
    vector<Uno_Player*> m_players;

    /**
     * \brief The list of hands for players currently in the game. 
     */
    vector<vector<card> > m_hands;

    /**
     * \brief The unplayed deck of cards for this game.
     *
     * This is the face-down stack of cards players draw from when they cannot
     * play a card from their hand.
     */
    deck m_unplayed;
};

#endif
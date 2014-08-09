/*! \file uno_pstate.h
    \brief Contains the player, limited view  of the game state.
    \sa uno_gstate.cpp
*/

#ifndef UNO_PSTATE_H
#define UNO_PSTATE_H

#include "uno_state.h"
#include "uno_card.h"
#include <vector>
#include <list>

using namespace std;

/** 
 * \brief Type definition for a hand of Uno cards. 
 */
typedef vector<card> hand;

#define UNO_INIT_HAND_SIZE 7    /*!< Initial hand size for an Uno hand. */

/** 
 * \brief Contains the player, limited view  of the game state.
 *
 * This class contains the information for:
 * \li Count of cards in players' hands.
 * \li List of cards in this player's hand.
 * \li Player at play.
 * \li Turn count.
 * \li Count of cards left in unplayed deck.
 * \li Played deck of cards. 
 * \li Time allowance per turn. 
 */
class Uno_PState : public Uno_State
{
  public:
    /**
     * \brief Default constructor.
     *
     * Constructs an Uno_PState with empty and zero values.
     */
    Uno_PState() : m_unplayed_count(0)
    {}

    /**
     * \brief Counts of cards in all players' hands. 
     */
    vector<unsigned int> m_hand_counts;

    /** 
     * \brief The player's hand of cards.
     *
     * The player's hand of cards is initially 7 cards drawn from a deck.
     */
    hand m_hand;

    /**
     * \brief The count of the cards in the unplayed deck.
     *
     * This is the count of the face-down stack of cards players draw 
     * from when they cannot play a card from their hand.
     */
    unsigned int m_unplayed_count;
};

#endif
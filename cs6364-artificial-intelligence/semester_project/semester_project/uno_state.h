/*! \file uno_state.h
    \brief Contains the basic elements of a state common to the server and players. 
    \sa uno_state.cpp
*/

#ifndef UNO_STATE_H
#define UNO_STATE_H

#include "uno_deck.h"
#include <string>

using namespace std;

#define UNO_NUM_PLAYERS 2    /*!< The number of players for an Uno game. */
#define UNO_MAX_PLAYERS 10   /*!< Maximum number of players for an Uno game. */
#define UNO_MIN_PLAYERS 2    /*!< Minimum number of players for an Uno game. */

/** 
 * \brief Contains the basic elements of a state common to the server and players. 
 *
 * This class contains the information for:
 * \li Player at play.
 * \li Turn count.
 * \li Played deck of cards. 
 * \li Time allowance per turn. 
 */
class Uno_State
{
  public:
    /**
     * \brief Default constructor.
     *
     * Constructs an Uno_State with empty and zero values.
     */
    Uno_State() : m_at_play(0), m_turn_count(0), m_time_per_turn(0), m_msg(""), m_forward(true), m_ai_difficulty_levels(0)
    {}

    /**
     * \brief The ID of the player whose turn it is. 
     */
    unsigned int m_at_play;

    /**
     * \brief The number of turns elapsed since the game began. 
     */
    unsigned int m_turn_count;

    /**
     * \brief The played deck of cards for this game. 
     *
     * This is the face-up stack of cards players put cards into from their hands.
     */
    deck m_played;

    /**
     * \brief The number of second allotted to a player to choose their move
     * for a turn.
     */
    double m_time_per_turn;

    /**
     * \brief A multipurpose message used by the server and the player.
     */
    string m_msg;

    /**
     * \brief The order players take turns in; true is ascending, false is descending
     */
    bool m_forward;

    /**
     * \brief The number of difficulty levels for AI players
     */
    unsigned int m_ai_difficulty_levels;
};

#endif
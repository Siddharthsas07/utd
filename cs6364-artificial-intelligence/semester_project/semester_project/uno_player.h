/*! \file uno_player.h
    \brief Contains the functionality for having a default player in the Uno game.
    \sa uno_player.cpp
*/

#ifndef UNO_PLAYER_H
#define UNO_PLAYER_H

#include "uno_deck.h"
#include "uno_card.h"
#include "uno_pstate.h"
#include "uno_action.h"
#include <string>
#include <list>

using namespace std;

/** 
 * \brief Base Uno_Player class.
 *
 * This class does nothing, use an Uno_AI_Player or an Uno_Human_Player instead.
 * \sa Uno_AI_Player
 * \sa Uno_Human_Player
 */
class Uno_Player
{
  public:
    /** 
     * \brief Default constructor.
     *
     * Constructs an Uno_Player with empty and zero values. 
     */
    Uno_Player() : m_name(""), m_score(0), m_id(0)
    {}

    /** 
     * \brief Full value specification constructor. 
     * 
     * Constructs an Uno_Player with the given values.
     * \param n The name of the Uno_Player.
     * \param s The score for the Uno_Player.
     */
    Uno_Player( const string& n, unsigned int s );   

    /**
     * \brief Calls the player to take his turn. 
     *
     * This function is invoked by the Uno_Runner whenever it is this player's
     * turn in the game. 
     * \param s The state of the game as visible from this player's perspective.
     * \retval Uno_Action The Uno_Action the player chooses this turn. 
     */
    virtual Uno_Action take_turn( const Uno_PState& s ) = 0;    

    /**
     * \brief The player's name.
     *
     * A string representing the player's name. This can be just about anything
     * you want it to be. 
     */
    string m_name;

    /**
     * \brief The player's score. 
     *
     * Each player attains a number of points at the end of each game. The 
     * point total for a player is tracked using this. The first player to 
     * reach 500 points in a regulation game is considered the winner. 
     */
    unsigned int m_score;

    /**
     * \brief The player's unique ID number, as it changes per game
     */
    unsigned int m_id;
};

#endif
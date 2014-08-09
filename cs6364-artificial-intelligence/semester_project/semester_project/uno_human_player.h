/*! \file uno_human_player.h
    \brief Contains the functionality for having a human player in the Uno game.
    \sa uno_human_player.cpp
*/

#ifndef UNO_HUMAN_PLAYER_H
#define UNO_HUMAN_PLAYER_H

#include "uno_player.h"
#include "uno_pstate.h"
#include "uno_action.h"

using namespace std;

/** 
 * \brief A human player for the game. 
 *
 * This class is used to represent a human player for an Uno game. 
 */
class Uno_Human_Player : public Uno_Player
{
  public:
    /** 
     * \brief Default constructor. 
     * 
     * Constructs an Uno_Human_Player with empty and zero values.
     */
    Uno_Human_Player(){}

    /** 
     * \brief Full value specification constructor. 
     *
     * Constructs an Uno_Human_Player with the given values.
     * \param n The name of the Uno_Human_Player.
     * \param s The score for the Uno_Human_Player.
     */
    Uno_Human_Player( const string& n, unsigned int s );
    
    /**
     * \brief Calls the player to take his turn. 
     *
     * This function is invoked by the Uno_Runner whenever it is this player's
     * turn in the game. 
     * \param s The state of the game as visible from this player's perspective.
     * \retval Uno_Action The Uno_Action the player chooses this turn. 
     */
    Uno_Action take_turn( const Uno_PState& s );
};

#endif

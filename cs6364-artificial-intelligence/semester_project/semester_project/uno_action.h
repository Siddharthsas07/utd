/*! \file uno_action.h
    \brief Contains the construct for an Uno_Action.
*/

#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <iostream>

using namespace std;

/*!< The possible actions a player may take in the game */
enum _UNO_ACTIONS {
  UNO_ACTION_DRAW = 0,
  UNO_ACTION_PLAY,
  UNO_ACTION_PASS,
  UNO_ACTION_QUIT,
  UNO_ACTION_HELP,
  UNO_ACTION_NONE,
  UNO_ACTION_RESERVED,
  UNO_ACTION_COUNTOF
};

/**
 * \brief Contains  for an Uno_Action.
 * Currently supported values are:
 * \li 0 = Draw.
 * \li 1 = Play a card.
 * \li 2 = Surrender (quit).
 * \li 3 = Pass.
 */
class Uno_Action
{
  public:
    /**
     * \brief Default constructor. Does nothing. 
     */
    Uno_Action() : m_act(UNO_ACTION_NONE), m_idx(0), m_color(0)
    {}

    /** 
     * \brief Full value constructor. Assigns parameters to member data.
     */
    Uno_Action( unsigned int act, unsigned int idx, unsigned int color ) : m_act(act), m_idx(idx), m_color(color)
    {}

    /**
     * \brief Prints out the action in readable text.
     */
    void print()
    {
      cout << m_act;
      if ( m_act == UNO_ACTION_PLAY )
      {
        cout << " @ index " << m_idx;
        if ( m_color != 0 )
        { 
          cout << " w/ color " << m_color;
        }
      }
    }

    /**
     * \brief Number representing an action in Uno that a player can take 
     * during his turn.
     * 
     * Currently supported values are:
     * \li 0 = Draw.
     * \li 1 = Play a card.
     * \li 2 = Surrender (quit).
     * \li 3 = Pass.
     */
    unsigned int m_act;

    /**
     * \brief The index of the card a player tries to play.
     *
     * The index of the card in the player's hand he wants to play if the
     * action is 1 (play a card).
     */
    unsigned int m_idx;

    /**
     * \brief The color the player wishes to switch to if he played a wild.
     *
     * This is the color the player wishes to switch to if he played a wild 
     * or a wild draw four card. 
     */
    char m_color;
};

#endif
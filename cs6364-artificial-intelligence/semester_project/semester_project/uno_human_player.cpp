/*! \file uno_human_player.cpp
    \brief Contains the functionality for having a human player in the Uno game.
    \sa uno_human_player.h
*/

#include "uno_human_player.h"
#include "uno_action.h"

#include <iostream>

using namespace std;

Uno_Human_Player::Uno_Human_Player( const string& n, unsigned int s )
{
  m_name = n;
  m_score = s;
}

Uno_Action Uno_Human_Player::take_turn( const Uno_PState& s )
{
  Uno_Action action;

  unsigned int p_action = 0;
  cout << "Your action? ";
  cin >> p_action;

  action.m_act = ((p_action < (unsigned int)UNO_ACTION_NONE) ? p_action : (unsigned int)UNO_ACTION_NONE);

  // If the user chooses to play a card from their hand
  if ( action.m_act == UNO_ACTION_PLAY )
  {
    cout << "Play which card? ";
    cin >> action.m_idx;

    if ( action.m_idx <= (s.m_hand.size()-1) )
    {
      card c = s.m_hand[action.m_idx];
      if ( CARDTYPE( c ) == UNO_WILD || CARDTYPE( c ) == UNO_WILD_DRAW_FOUR )
      {
        cout << "Change to which color? (ex: r/b/g/y) ";
        cin >> action.m_color;
      }
    }
  }

  return action;
}

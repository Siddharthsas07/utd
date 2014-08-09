/*! \file driver.cpp
    \author Gary Steelman
    \date 12/5/2011
    \brief The driver for the AI to play the card game Uno.
  
    This is a that models a player for playing the card game Uno. The AI doesn't
    think that intelligently right now, but I hope to improve it over time.
 */

#include <iostream>
using namespace std;

#include "uno_card.h"
#include "uno_deck.h"
#include "uno_player.h"
#include "uno_ai_player.h"
#include "uno_human_player.h"
#include "uno_runner.h"
#include "uno_gstate.h"
#include "uno_pstate.h"

/**
 * The entry point for the program.
 */
int main( int argc, char * argv[] ) 
{  
  // Create a runner.
  Uno_Runner runner;

  runner.set_difficulty_levels( 3 );

  // Add two players.
  Uno_Player *p1 = new Uno_Human_Player( "Gary", 0 );  
  Uno_Player *p2 = new Uno_AI_Player( "Dr. Moldovan", 0, 3 );
  runner.add_player( p1 );
  runner.add_player( p2 );

  runner.setup();

  cout << "===The initial game state===" << endl;
  runner.print_state();

  runner.run();

  // cin.get();

  return 0;
}
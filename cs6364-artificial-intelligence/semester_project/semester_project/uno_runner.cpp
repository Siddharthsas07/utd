/*! \file uno_runner.cpp
    \brief Contains the functions for running and playign an Uno game.
    \sa uno_runner.h
*/

#include "uno_deck.h"
#include "uno_card.h"
#include "uno_player.h"
#include "uno_ai_player.h"
#include "uno_human_player.h"
#include "uno_gstate.h"
#include "uno_pstate.h"
#include "uno_action.h"
#include "uno_runner.h"

#include <ctime>
#include <locale>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

Uno_Runner::Uno_Runner() 
{}

bool Uno_Runner::setup()
{
  // Map the names and weights of all cards
  map_names();
  map_weights();

  // Add players to the game until we have enough players
  for ( unsigned int i = 0; num_players() < UNO_MIN_PLAYERS; i++ )
  {
    stringstream ss;
    ss << i;

    Uno_Player *p = new Uno_AI_Player( "Player" + ss.str(), 0, 0 );
    add_player( p );
  }

  // Set the unplayed deck to default unless another was already specified
  if ( m_gstate.m_unplayed.empty() )
  {
    deck D;
    create_deck( D );
    shuffle_deck( D );
    set_unplayed_deck( D );
  }

  // Set the played deck to use unless another was already specified
  if ( m_gstate.m_played.empty() )
  {
    deck G;
    set_played_deck( G );
  }

  m_gstate.m_hands.resize( num_players() );

  // Each player must draw UNO_INIT_HAND_SIZE cards
  if ( m_gstate.m_unplayed.size() < (num_players() * (unsigned int)UNO_INIT_HAND_SIZE) + 1 )
  {
    cout << "Insufficient card quantity in unplayed deck to support this " 
      << "many players." << endl;
    return false;
  }

  // Add cards to the players' hands and assign their IDs
  else
  {
    for ( unsigned int i = 0; i < num_players(); i++ )
    {
      m_gstate.m_players[i]->m_id = i;

      for ( unsigned int j = 0; j < (unsigned int)UNO_INIT_HAND_SIZE; j++ )
      {
        if ( !draw_card( i, m_gstate.m_unplayed ) )
        {
          cout << "Insufficient card quantity in unplayed deck to draw "
            << "more cards for opening hand." << endl;
          return false;
        }
      }
    }
  }

  // Put the first card down to play off of
  discard_card( m_gstate.m_unplayed, m_gstate.m_played );

  m_gstate.m_at_play = 0;
  m_gstate.m_turn_count = 0;
  m_gstate.m_time_per_turn = UNO_TIME_PER_TURN;
  m_gstate.m_msg = "";
  m_gstate.m_forward = true;
  m_gstate.m_ai_difficulty_levels = 3;

  return true;
}

bool Uno_Runner::shut_down()
{
  // Free the allocated memory for the players
  for( vector<Uno_Player*>::iterator it = m_gstate.m_players.begin(); it != m_gstate.m_players.end(); ++it )
  {
    delete *it;
  }

  return true;
}

bool Uno_Runner::add_player( Uno_Player * p )
{
  if ( num_players() + 1 > UNO_MAX_PLAYERS )
  {
    return false;
  }

  m_gstate.m_players.push_back( p );

  return true;
}

bool Uno_Runner::remove_player( unsigned char i, unsigned char c )
{
  if ( num_players() == 0 )
  {
    return false;
  }

  // Tend to the player's cards
  //0 = Place cards back onto bottom of the unplayed deck.
  if ( c == 0 )
  {
    m_gstate.m_unplayed.insert( m_gstate.m_unplayed.begin(),
      m_gstate.m_hands[i].begin(),
      m_gstate.m_hands[i].end()
      );
  }

  // 1 = Place cards back onto bottom of the played deck.
  else if ( c == 1 )
  {
    m_gstate.m_played.insert( m_gstate.m_played.begin(),
      m_gstate.m_hands[i].begin(),
      m_gstate.m_hands[i].end()
      );
  }

  // 2 = Remove cards from the game state completely. Aka do nothing.

  // Remove player from the game.
  vector<Uno_Player*>::iterator it = m_gstate.m_players.begin();
  delete *(it + i);
  m_gstate.m_players.erase( it + i );
  return true;
}

unsigned int Uno_Runner::num_players()
{
  return m_gstate.m_players.size();
}

void Uno_Runner::set_unplayed_deck( const deck &d )
{
  m_gstate.m_unplayed = d;
}

void Uno_Runner::set_played_deck( const deck &d )
{
  m_gstate.m_played = d;
}

void Uno_Runner::print_played( unsigned char f )
{
  if ( f == 0 )
  {
    print_deck( m_gstate.m_played );
  }

  else if ( f == 1 )
  {
    print_deck_values( m_gstate.m_played );
  }
}

void Uno_Runner::print_unplayed( unsigned char f )
{
  if ( f == 0 )
  {
    print_deck( m_gstate.m_unplayed );
  }

  else if ( f == 1 )
  {
    print_deck_values( m_gstate.m_unplayed );
  }
}

void Uno_Runner::print_state()
{
  cout << "Turn: " << m_gstate.m_turn_count << endl;
  cout << "At play: " << m_gstate.m_at_play << endl;
  cout << "Time left: " << m_gstate.m_time_per_turn << endl;
  
  // Print out the player statuses
  for ( unsigned int i = 0; i < num_players(); i++ )
  {
    cout << "<" << i << ", " 
      << m_gstate.m_players[i]->m_name << ", "
      << m_gstate.m_players[i]->m_score << ", {";

    for ( unsigned int j = 0; j < m_gstate.m_hands[i].size(); j++ )
    {
      cout << card_name( m_gstate.m_hands[i][j] ) << " ";
    }
    cout << "}>" << endl;
  }

  cout << "  Played: ";
  print_played( 0 );
  cout << "[" << m_gstate.m_played.size() << "] " << endl;

  cout << "Unplayed: ";
  print_unplayed( 0 );
  cout << "[" << m_gstate.m_unplayed.size() << "] " << endl;
}

void Uno_Runner::print_turn()
{
  cout << "================================================================================" << endl;
  cout << "Turn #: " << m_pstate.m_turn_count << "  |  ";
  cout << "At play: " << m_gstate.m_players[m_gstate.m_at_play]->m_name << "  |  ";
  cout << "Time left: " << m_pstate.m_time_per_turn << " seconds" << "  |  ";
  cout << "Game direction: " << (m_pstate.m_forward ? "ASC" : "DESC") << endl;
  
  // Print out the player statuses
  for ( unsigned int i = 0; i < num_players(); i++ )
  {
    cout << "<" << i << ", " 
      << m_gstate.m_players[i]->m_name << ", "
      << m_gstate.m_players[i]->m_score << ", {["
      << m_pstate.m_hand_counts[i] << "]}>" << endl;
  }
  
  cout << "Your hand: {";
  for ( unsigned int i = 0; i < m_pstate.m_hand.size(); i++ )
  {
    cout << card_name( m_pstate.m_hand[i] ) << " ";
  }
  cout << "}" << endl;

  cout << "  Played: ";
  print_played( 0 );
  cout << "[" << m_pstate.m_played.size() << "] " << endl;

  cout << "Unplayed: ";
  cout << "[" << m_pstate.m_unplayed_count << "] " << endl;
}

void Uno_Runner::print_options()
{
  cout << "You can take the following actions this turn: " << endl;
  cout << "  " << (unsigned int)UNO_ACTION_DRAW << " = Draw a card from the deck." << endl;
  cout << "  " << (unsigned int)UNO_ACTION_PLAY << " = Play a card from your hand." << endl;
  cout << "  " << (unsigned int)UNO_ACTION_PASS << " = Pass your turn (do nothing." << endl;
  cout << "  " << (unsigned int)UNO_ACTION_QUIT << " = Surrender (quit) out of the game." << endl;
  cout << "  " << (unsigned int)UNO_ACTION_HELP << " = Instructions and help." << endl;
}

void Uno_Runner::print_help()
{
  cout << "Uno is a card game that consists of multiple players taking turns "
    << "playing cards from their hands trying to be the first to play all of "
    << "their cards. Once a game has started, a player will be called to take "
    << "his turn. During a turn, the current game state will be shown to the "
    << "player and then the player will have 10 seconds to choose an action. "
    << "The actions available to the player will also be shown to him. To choose "
    << "an action, the player simply needs to enter the number corresponding to "
    << "that action.\n\n";
  cout << "When choosing to play a card, the player must enter the index of "
    << "the card in his hand. To play the left-most card in your hand, enter 0. "
    << "\n The rules for playing a card are: " << endl;
  cout << "1) You can play a card of the same color as on the top of the played deck." << endl;
  cout << "2) You can play a card of the same value as on the top of the played deck." << endl;
  cout << "3) You can play a wild card no matter what is on the top of the played deck." << endl;
  cout << "4) If you cannot play a card, you must draw card(s) until you can play or "
    << "there are no cards left to draw, at which point your turn will be passed." << endl;
  cout << "\nThe first player to empty his hand and play all of his cards wins!" << endl;
}

void Uno_Runner::run()
{
  srand( time(NULL) );

  cout << "Beginning playing a game of Uno! Good luck players!" << endl;
  cout << "  (Secretly I'm rooting for " 
    << m_gstate.m_players[rand()%num_players()]->m_name 
    << " to win!)" << endl;

  // Tracks whether a player is repeating his turn due to an invalid action
  // in the previous turn
  bool retry_turn = false; 

  // The number of players to advance for the next turn (changes with skips, 
  // draw twos, or wild draw fours)
  int players_to_advance = 0;
  int next_to_draw = 0;

  // The number of cards the player next turn will be forced to draw (changes
  // with draw twos or wild draw fours)
  unsigned int cards_to_draw = 0;

  while ( true )
  {
    // If player is not repeating action increment turn counter
    if ( !retry_turn )
    {
      m_gstate.m_turn_count++;
    }

    // Construct the state and also print out the current turn to the user
    construct_player_state( retry_turn );
    print_turn();
    print_options();

    // Call player to take his turn
    m_action = m_gstate.m_players[m_gstate.m_at_play]->take_turn( m_pstate );

    if ( !check_action( m_gstate, m_action ) )
    {
      retry_turn = true;
      continue;
    }

    retry_turn = false;
    players_to_advance = 1;
    cards_to_draw = 0;

    if ( m_action.m_act == UNO_ACTION_DRAW )
    {
      // cout << "Drawing a card..." << endl;

      // If no cards exist in the unplayed deck, shuffle the unplayed into it
      // and then try to draw. Afterward, if the played deck is still empty
      // attempt to put a card on top of it so we can play off of it
      if ( m_gstate.m_unplayed.empty() )
      {
        swap_decks( m_gstate.m_unplayed, m_gstate.m_played );
        shuffle_deck( m_gstate.m_unplayed );        
      }

      draw_card( m_gstate.m_at_play, m_gstate.m_unplayed );
      players_to_advance = 0;

      if ( m_gstate.m_played.empty() )
      {
        discard_card( m_gstate.m_unplayed, m_gstate.m_played );      
      }
    }

    else if ( m_action.m_act == UNO_ACTION_PLAY )
    {
      // cout << "playing a card" << endl;
      // Play the card
      card c = play_card( m_action.m_idx );

      // Handle special effects for special cards
      if ( CARDTYPE( c ) == UNO_REVERSE )
      {
        m_gstate.m_forward = !m_gstate.m_forward;
        if ( num_players() == 2)
        {
          players_to_advance = 0;
        }
      }

      if ( CARDTYPE( c ) == UNO_SKIP )
      {
        players_to_advance = 2;
      }

      if ( CARDTYPE( c ) == UNO_DRAW_TWO )
      {
        cards_to_draw = 2;
        players_to_advance = 2;
      }

      if ( CARDTYPE( c ) == UNO_WILD_DRAW_FOUR )
      {
        cards_to_draw = 4;
        players_to_advance = 2;
      }

      if ( CARDTYPE( c ) == UNO_WILD_DRAW_FOUR || CARDTYPE( c ) == UNO_WILD )
      {
        char new_color = 0;
        m_action.m_color = tolower( m_action.m_color );
        if ( m_action.m_color == 'r' )
        {
          new_color = (unsigned int)UNO_RED;
        }

        else if ( m_action.m_color == 'b' )
        {
          new_color = (unsigned int)UNO_BLUE;
        }

        else if ( m_action.m_color == 'g' )
        {
          new_color = (unsigned int)UNO_GREEN;
        }

        else if ( m_action.m_color == 'y' )
        {
          new_color = (unsigned int)UNO_YELLOW;
        }

        c = CARD( CARDTYPE( c ), new_color );
      } 

      // If the previous card on the top of the deck is a wild, clean its color
      if ( !m_gstate.m_played.empty() )
      {
        card d = m_gstate.m_played.back();
        if ( CARDTYPE( d ) == UNO_WILD || CARDTYPE( d ) == UNO_WILD_DRAW_FOUR )
        {
          m_gstate.m_played[m_gstate.m_played.size() - 1] = CARD( UNO_NO_COLOR, CARDTYPE( d ) );
        }
      }

      // Push new card onto top of deck. Wild cards have color after indicating 
      // what the color was changed to.
      m_gstate.m_played.push_back( c );
    }

    else if ( m_action.m_act == UNO_ACTION_QUIT )
    {
      // Place removed player's cards on the bottom of the played deck
      remove_player( m_gstate.m_at_play, 1 );
    }

    else if ( m_action.m_act == UNO_ACTION_PASS )
    {
      // Nothing...
    }

    else if ( m_action.m_act == UNO_ACTION_HELP )
    {
      print_help();
    }

    else if ( m_action.m_act == UNO_ACTION_NONE )
    {
      cout << "Elected no action, something broke, ending game." << endl;
      break;
    }

    if ( game_over() )
    {
      break;      
    }

    int curr_player = m_gstate.m_at_play;

    // Player played a WD4 or D2 card, make someone draw!
    if ( cards_to_draw != 0 )
    {
      int player_to_draw = curr_player;

      // Calculate the player who has to draw cards!
      if ( m_gstate.m_forward )
      {
        player_to_draw++;
        player_to_draw = player_to_draw % num_players();
      }

      else
      {
        player_to_draw--;
        if ( player_to_draw < 0 )
        {
          player_to_draw += num_players();
        }
      }

      for ( unsigned int i = 0; i < cards_to_draw; i++ )
      {
        if ( m_gstate.m_unplayed.empty() )
        {
          swap_decks( m_gstate.m_unplayed, m_gstate.m_played );
          shuffle_deck( m_gstate.m_unplayed );          
        }

        draw_card( player_to_draw, m_gstate.m_unplayed );

        if ( m_gstate.m_played.empty() )
        {
          discard_card( m_gstate.m_unplayed, m_gstate.m_played );      
        }
      }
    }    

    // Calculate the next player
    // Normal, forward play, wrap around to lower # players if necessary
    if ( m_gstate.m_forward )
    {
      curr_player += players_to_advance;
      curr_player = curr_player % num_players();
    }
    
    // Reversed, backward play, wrap around to higher # players if necessary
    else
    {
      curr_player -= players_to_advance;
      if ( curr_player < 0 )
      {
        curr_player += num_players();
      }
    }   

    m_gstate.m_at_play = curr_player;

    // Record this turn's state
    m_history.push_back( m_gstate );
  }

  cout << "The game is over!" << endl;
  //cout << "The winner is " << find_winner() << endl;

  shut_down();
}

void Uno_Runner::construct_player_state( bool repeat )
{
  if ( !repeat )
  {
    m_pstate.m_at_play = m_gstate.m_at_play;
    m_pstate.m_forward = m_gstate.m_forward;    
    m_pstate.m_hand = m_gstate.m_hands[m_gstate.m_at_play];    
    m_pstate.m_hand_counts.resize( m_gstate.m_hands.size() );
    for ( unsigned int i = 0; i < num_players(); i++ )
    {
      m_pstate.m_hand_counts[i] = m_gstate.m_hands[i].size();  
    }    
    m_pstate.m_msg = m_gstate.m_msg;
    m_pstate.m_played = m_gstate.m_played;
    m_pstate.m_time_per_turn = m_gstate.m_time_per_turn;
    m_pstate.m_turn_count = m_gstate.m_turn_count;
    m_pstate.m_unplayed_count = m_gstate.m_unplayed.size();
    m_pstate.m_ai_difficulty_levels = m_gstate.m_ai_difficulty_levels;
  }
}

bool Uno_Runner::check_validity()
{
  // Invoke move checker on m_state.
  return true;  
}

bool Uno_Runner::check_action( const Uno_GState& s, const Uno_Action& a )
{
  bool action_ok = false;

  if ( a.m_act == UNO_ACTION_DRAW )
  {
    action_ok = can_draw( s );
    if ( !action_ok )
    {
      cout << "There are no cards for you to draw. Play a card or pass your turn." << endl;
    }
  }

  else if ( a.m_act == UNO_ACTION_PLAY )
  {
    action_ok = can_play_selected( s, a );
    if ( !action_ok )
    {
      cout << "You cannot play that card." << endl;      
    }
  }

  else if ( a.m_act == UNO_ACTION_PASS )
  {
    action_ok = !can_do_something( s );
    if ( !action_ok )
    {
      cout << "You can still play a card or draw." << endl;
    }
  }

  else if ( a.m_act == UNO_ACTION_QUIT || a.m_act == UNO_ACTION_HELP )
  {
    action_ok = true;
  }

  else if ( a.m_act == UNO_ACTION_NONE )
  {
    action_ok = false;
  }

  return action_ok;
}

bool Uno_Runner::can_play_selected( const Uno_GState& s, const Uno_Action& a )
{
  // Trying to play a card that's not in their hand
  if ( a.m_idx > (s.m_hands[s.m_at_play].size()-1) )
  {
    return false;
  }

  if ( s.m_played.empty() )
  {
    return true;
  }

  card p_card = s.m_hands[s.m_at_play][a.m_idx];
  card d_card = s.m_played.back();

  // If the color or type of the card to play matches
  if ( ( CARDCOLOR( p_card ) == CARDCOLOR( d_card ) ) 
    || ( CARDTYPE( p_card ) == CARDTYPE( d_card ) ) 
    )
  {
    return true;
  }

  char c = tolower(a.m_color);
  // If the type of card to be played is a wild, ensure color choice is ok
  if ( (CARDTYPE( p_card ) == UNO_WILD || CARDTYPE( p_card ) == UNO_WILD_DRAW_FOUR) 
    && (c == 'r' || c == 'b' || c == 'g' || c == 'y') )
  {
    return true;
  }

  return false;
}

bool Uno_Runner::can_do_something( const Uno_GState& s )
{
  if ( can_draw( s ) || !s.m_hands[s.m_at_play].empty() )
  {
    return true;
  }

  return false;
}

bool Uno_Runner::can_draw( const Uno_GState& s )
{
  return ( !s.m_played.empty() || !s.m_unplayed.empty() );
}

bool Uno_Runner::game_over()
{
  if ( num_players() <= 1 )
  {
    return true;
  }

  for ( unsigned int i = 0; i < num_players(); i++ )
  {
    if ( m_gstate.m_hands[i].empty() )
    {
      return true;
    }
  }

  return false;
}

bool Uno_Runner::draw_card( unsigned int i, deck &d )
{
  if ( !d.empty() )
  {
    m_gstate.m_hands[i].push_back( take_card( d ) );
    return true;
  }
  
  return false;
}

card Uno_Runner::play_card( unsigned char i )
{
  card c = m_gstate.m_hands[m_gstate.m_at_play][i];
  m_gstate.m_hands[m_gstate.m_at_play].erase( m_gstate.m_hands[m_gstate.m_at_play].begin() + i );

  return c;
}

bool Uno_Runner::discard_card( deck& d, deck& g )
{
  if ( !d.empty() )
  {
    g.push_back( take_card( d ) );

    return true;
  }

  return false;
}

/*
card Uno_Player::play_card_by_name( card c )
{
  list<card>::iterator it;

  for ( it = m_hand.begin(); it != m_hand.end(); ++it )
  {
    if ( *it == c )
    {
      m_hand.erase(it);

      // Just return c to avoid another dereference
      return c;
    }
  }

  return CARD(UNO_NO_COLOR, UNO_RESERVED);
}*/
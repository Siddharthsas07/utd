/*! \file uno_ai_player.cpp
    \brief Contains the Uno AI Player class and functionality.
    \sa uno_ai_player.h
*/

#include "uno_ai_player.h"
#include <algorithm>
#include <iostream>
#include <ctime>

using namespace std;

#define UNO_TIME_PER_TURN 10 /*!< Maximum number of seconds per turn the AI is allowed to think */

void wait( int seconds )
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

Uno_AI_Player::Uno_AI_Player( const string& n, unsigned int s, unsigned int l )
{
  m_name = n;
  m_score = s;
  m_level = l;
}

Uno_Action Uno_AI_Player::take_turn( const Uno_PState& s )
{
  // Start the timer. This assumes that the AI will always return a valid action
  // and never be called to repeat a turn. 
  m_time_left = (double)UNO_TIME_PER_TURN - (double)(s.m_ai_difficulty_levels - m_level) * ((double)UNO_TIME_PER_TURN / (double)s.m_ai_difficulty_levels);

  // cout << "Taking " << m_time_left << " seconds to think this turn." << endl;

  // Convert the PState into a search node
  Uno_AI_Search_Node sn;
  sn.m_state = s;

  // Initialize the frontier
  m_frontier.clear();
  m_initial_state = sn;
  make_heap( m_frontier.begin(), m_frontier.end() );

  // Expand the frontier, look into the future, find a good state
  think();

  // Fetch the best action
  choose_action();

  // Return the action that results in the best far-future state
  return m_best_action;
}

void Uno_AI_Player::think()
{  
  // Create a local, sorted copy of the default deck for set union-ing later
  create_deck( m_sorted_default_deck );
  sort( m_sorted_default_deck.begin(), m_sorted_default_deck.end() );

  // vector of succcessors for a given state
  vector<Uno_AI_Search_Node> successors;

  for ( unsigned int num_plys = 0; num_plys < 1 && m_time_left > 0; num_plys++ )
  {
    m_timer.startTimer();    

    successor( m_initial_state, successors );    

    // Insert the successors into the frontier
    for ( unsigned int i = 0; i < successors.size(); i++ )
    {
      // cout << "Storing successor node with action "; 
      // successors[i].m_actions.front().print(); 
      // cout << " and score " << successors[i].m_score << endl;
      
      push_frontier_node( successors[i] );

      // cout << "Top of the heap: " << m_frontier.front().m_score << endl;
    }

    // Decrease time left by the amount used to search this ply
    m_timer.stopTimer();    
    m_time_left -= m_timer.getElapsedTime();
    // cout << "Time left for this turn " << m_time_left << endl;
  }
}

void Uno_AI_Player::choose_action()
{
  if ( !m_frontier.empty() && !m_frontier.front().m_actions.empty() )
  {
    m_best_action = m_frontier.front().m_actions.front();
  }

  else
  {
    m_best_action.m_act = UNO_ACTION_PASS;
  }  
}

void Uno_AI_Player::successor( const Uno_AI_Search_Node& s, vector<Uno_AI_Search_Node>& suc )
{  
  // the node to add to the frontier!
  Uno_AI_Search_Node to_add;

  // If the AI is at play this turn
  if ( s.m_state.m_at_play == m_id )
  {
    // Copy the played deck for us to use for set difference
    deck played_deck_copy( s.m_state.m_played );
    sort( played_deck_copy.begin(), played_deck_copy.end() );

    deck drawable( m_sorted_default_deck.size(), 0 );
    deck::iterator drawable_it;

    drawable_it = set_symmetric_difference( 
      m_sorted_default_deck.begin(), 
      m_sorted_default_deck.end(), 
      played_deck_copy.begin(), 
      played_deck_copy.end(), 
      drawable.begin() 
      );

    // cout << "Size of unplayed deck: " << int(drawable_it - drawable.begin()) << endl;
    // print_deck( drawable );
    // cout << endl;

    suc.clear();  

    // Determine if we can make any actions with this node, if not, pass.
    if ( drawable.empty() && !can_play_any( s ) )
    {
      to_add = s;
      populate_successor_node_pass( to_add );
      to_add.m_score = (1.0 / 1.0) * state_eval( to_add );
      suc.push_back( to_add );
      return;
    }

    // TODO improvements for not generating the same state twice?!
    for ( deck::iterator it = drawable.begin(); (it != drawable.end()) && (*it != 0); ++it )
    {
      to_add = s;
      populate_successor_node_draw( to_add, *it );
      to_add.m_score = (1.0 / drawable.size()) * state_eval( to_add );
      suc.push_back( to_add );
      drawable.pop_back();
    }
    
    // Add successor node as if drawing any single card from drawable cards
    unsigned int playable_count = can_play_count( s );

    // Add successor node for playing any single card from the hand
    for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
    {
      if ( can_play_selected( s, i ) )
      {
        to_add = s;
        card c = to_add.m_state.m_hand[i];

        // Special case of having to add 4 different colors of cards to the fronier
        if ( CARDTYPE( c ) == UNO_WILD || CARDTYPE( c ) == UNO_WILD_DRAW_FOUR )
        {
          to_add = s;
          populate_successor_node_play( to_add, i, 'r' );
          to_add.m_score = (1.0 / playable_count) * state_eval( to_add );
          suc.push_back( to_add );

          to_add = s;
          populate_successor_node_play( to_add, i, 'g' );
          to_add.m_score = (1.0 / playable_count) * state_eval( to_add );
          suc.push_back( to_add );

          to_add = s;
          populate_successor_node_play( to_add, i, 'b' );
          to_add.m_score = (1.0 / playable_count) * state_eval( to_add );
          suc.push_back( to_add );

          to_add = s;
          populate_successor_node_play( to_add, i, 'y' );
          to_add.m_score = (1.0 / playable_count) * state_eval( to_add );
          suc.push_back( to_add );
        }

        // Just a normal card, add only one successor node
        else
        {
          to_add = s;
          populate_successor_node_play( to_add, i, 0 );
          to_add.m_score = (1.0 / playable_count) * state_eval( to_add );
          suc.push_back( to_add );
        }        
      }
    }
  }

  // Another player is at play this turn 
  else
  {
    
  }
}

void Uno_AI_Player::populate_successor_node_pass( Uno_AI_Search_Node& s )
{
  // cout << "Populating a pass node" << endl;
  advance_players( s, 1 );
  s.m_actions.push_back( Uno_Action( UNO_ACTION_PASS, 0, 0 ) );
}

void Uno_AI_Player::populate_successor_node_draw( Uno_AI_Search_Node& s, card d )
{
  // cout << "Populating a draw node with card " << card_name( d ) << endl;
  // Add card to hand from unplayed, set action, and evaluate new state
  s.m_state.m_hand.push_back( d );
  s.m_state.m_hand_counts[m_id]++;

  if ( s.m_state.m_unplayed_count == 0 )
  {
    s.m_state.m_unplayed_count = s.m_state.m_played.size();
    s.m_state.m_unplayed_count--;
    s.m_state.m_played.clear();
  }

  else
  {
    s.m_state.m_unplayed_count--;
  }

  s.m_actions.push_back( Uno_Action( UNO_ACTION_DRAW, 0, 0 ) );
}

void Uno_AI_Player::populate_successor_node_play( Uno_AI_Search_Node& s, unsigned int idx, char color )
{  
  unsigned int players_to_advance = 1;
  unsigned int cards_to_draw = 0;
  unsigned int num_players = s.m_state.m_hand_counts.size();
  card c = s.m_state.m_hand[idx];
  s.m_actions.push_back( Uno_Action( UNO_ACTION_PLAY, idx, color ) );

  // cout << "Populating a play node with card " << card_name( c ) << endl;

 // Handle special effects for special cards
  if ( CARDTYPE( c ) == UNO_REVERSE )
  {
    s.m_state.m_forward = !s.m_state.m_forward;
    if ( num_players == 2)
    {
      players_to_advance = 0;
    }
  }

  else if ( CARDTYPE( c ) == UNO_SKIP )
  {
    players_to_advance = 2;
  }

  else if ( CARDTYPE( c ) == UNO_DRAW_TWO )
  {
    cards_to_draw = 2;
    players_to_advance = 2;
  }

  else if ( CARDTYPE( c ) == UNO_WILD_DRAW_FOUR )
  {
    cards_to_draw = 4;
    players_to_advance = 2;
  }

  if ( CARDTYPE( c ) == UNO_WILD_DRAW_FOUR || CARDTYPE( c ) == UNO_WILD )
  {
    char new_color = 0;
    if ( color == 'r' )
    {
      new_color = (unsigned int)UNO_RED;
    }

    else if ( color == 'b' )
    {
      new_color = (unsigned int)UNO_BLUE;
    }

    else if ( color == 'g' )
    {
      new_color = (unsigned int)UNO_GREEN;
    }

    else if ( color == 'y' )
    {
      new_color = (unsigned int)UNO_YELLOW;
    }

    c = CARD( CARDTYPE( c ), new_color );
  }   

  // If the previous card on the top of the deck is a wild, clean its color
  if ( !s.m_state.m_played.empty() )
  {
    card d = s.m_state.m_played.back();
    if ( CARDTYPE( d ) == UNO_WILD || CARDTYPE( d ) == UNO_WILD_DRAW_FOUR )
    {
      s.m_state.m_played[s.m_state.m_played.size() - 1] = CARD( UNO_NO_COLOR, CARDTYPE( d ) );
    }
  }

  s.m_state.m_played.push_back( c );
  advance_players( s, players_to_advance );
  force_draw_cards( s, cards_to_draw );
}    

void Uno_AI_Player::advance_players( Uno_AI_Search_Node &s, unsigned int p )
{
  // cout << "Advancing " << p << " players" << endl;
  unsigned int curr_player = s.m_state.m_at_play;
  unsigned int num_players = s.m_state.m_hand_counts.size();

  // Calculate the next player
  // Normal, forward play, wrap around to lower # players if necessary
  if ( s.m_state.m_forward )
  {
    curr_player += p;
    curr_player = curr_player % num_players;
  }
  
  // Reversed, backward play, wrap around to higher # players if necessary
  else
  {
    curr_player -= p;
    if ( curr_player < 0 )
    {
      curr_player += num_players;
    }
  }   

  s.m_state.m_at_play = curr_player;
}

void Uno_AI_Player::force_draw_cards( Uno_AI_Search_Node &s, unsigned int c )
{
  // cout << "Forcing draw of " << c << " cards" << endl;
  unsigned int curr_player = s.m_state.m_at_play;
  unsigned int num_players = s.m_state.m_hand_counts.size();

  // Player played a WD4 or D2 card, make someone draw!
  if ( c != 0 )
  {
    int player_to_draw = curr_player;

    // Calculate the player who has to draw cards!
    if ( s.m_state.m_forward )
    {
      player_to_draw++;
      player_to_draw = player_to_draw % num_players;
    }

    else
    {
      player_to_draw--;
      if ( player_to_draw < 0 )
      {
        player_to_draw += num_players;
      }
    }

    for ( unsigned int i = 0; i < c; i++ )
    {
      s.m_state.m_hand_counts[player_to_draw]++;
      if ( s.m_state.m_unplayed_count == 0 )
      {
        s.m_state.m_unplayed_count = s.m_state.m_played.size();
        s.m_state.m_unplayed_count--;
        s.m_state.m_played.clear();
      }

      else
      {
        s.m_state.m_unplayed_count--;
      }
    }
  }    
}

float Uno_AI_Player::state_eval( const Uno_AI_Search_Node& s )
{
  float max_score = 1000.0;
  float min_score = 0.0;

  unsigned int num_cards = s.m_state.m_hand.size();

  // Positive goal, we won
  if ( goal_test( s ) )
  {
    return max_score;
  }

  // Negative goal, someone else has won 
  for ( unsigned int i = 0; i < s.m_state.m_hand_counts.size(); i++ )
  {
    if ( s.m_state.m_hand_counts[i] == 0 && i != m_id )
    {
      return min_score;
    }
  }

  // Target queue contains prioritized list of targets to attack in game. 
  // Pairs are <hand count, player ID>
  vector<std::pair<unsigned int, unsigned int> > target_queue;
  construct_target_queue( s, target_queue );
  // cout << "Targeting queue: " << endl;
  // for ( unsigned int i = 0; i < target_queue.size(); i++ )
  // {
    // cout << "<" << target_queue[i].first << ", " << target_queue[i].second << ">" << endl;
  // }

  // Pairs are <color, count>
  vector<std::pair<unsigned int, unsigned int> > hand_colors;
  count_hand_colors( s, hand_colors );
  // cout << "Hand colors: " << endl;
  // for ( unsigned int i = 0; i < hand_colors.size(); i++ )
  // {
    // cout << "<" << hand_colors[i].first << ", " << hand_colors[i].second << ">" << endl;
  // }

  // Base score is half of max, adjust this score using various metrics
  float score = max_score / 2.0;

  // If can play a card rather than draw, +150, else -35
  score += ( can_play_any( s ) ? 225.0 : -35.0 );

  // Subtract points for the weight of each card in the hand
  for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
  {
    score -= (float)card_weight( s.m_state.m_hand[i] );
  }

  // If state has < 3 cards for the player, it's a relatively good state
  if ( s.m_state.m_hand.size() <= 3 )
  {
    score += 200.0;
  }
  
  // If the player's hand contains < 25% of the matching color for played
  // deck, deduct points. Except in the case a wild exists in the hand.
  if ( !have_card( s, UNO_WILD ) && !have_card( s, UNO_WILD_DRAW_FOUR ) )
  {
    for ( unsigned int i = 0; i < hand_colors.size(); i++ )
    {
      if ( !s.m_state.m_played.empty()
        && (hand_colors[i].first == CARDCOLOR(s.m_state.m_played.back())) 
        && (hand_colors[i].second <= .25 * s.m_state.m_hand.size()) )
      {
        score -= 100.0;
      }
    }
  }

  for ( unsigned int i = 0; i < target_queue.size(); i++ )
  {
    if ( i != m_id )
    {
      // Account for number of cards in other players' hands.
      // -30 score for each card less than 5 in opponents' hands.
      // +20 score for each card more than 5 in opponents' hands.
      score += ( 
        (target_queue[i].first >= 5) ? 
        ((target_queue[i].first - 5) * 20.0) :
        ((5 - target_queue[i].first) * 30.0)
        );

      // If this player in the target queue is within range of special effects
      if ( player_distance( s.m_state.m_forward, 
                          s.m_state.m_hand_counts.size(), 
                          m_id, 
                          target_queue[i].second ) == 1 )
      {
        if ( have_card( s, UNO_WILD_DRAW_FOUR ) && can_play_cardtype( s, UNO_WILD_DRAW_FOUR ) )
        {
          //+300 points for owning least player, -25 points for each player up
          score += (300.0 - (25.0 * i));
        }

        else if ( have_card( s, UNO_DRAW_TWO ) && can_play_cardtype( s, UNO_DRAW_TWO ) )
        {
          score += (250.0 - (25.0 * i));
        }
        
        else if ( have_card( s, UNO_SKIP ) && can_play_cardtype( s, UNO_SKIP ) )
        {
          score += (200.0 - (25.0 * i));
        }

        else if ( have_card( s, UNO_REVERSE ) && can_play_cardtype( s, UNO_REVERSE ) )
        {
          score += (150.0 - (25.0 * i));
        }
      }
    }
  }

  if ( score > 1000.0 )
  {
    score = 999.5;
  }

  else if ( score < 0 )
  {
    score = 0.5;
  }

  return score;
}

void Uno_AI_Player::push_frontier_node( const Uno_AI_Search_Node& s )
{
  m_frontier.push_back( s );
  push_heap( m_frontier.begin(), m_frontier.end() );
}

Uno_AI_Search_Node Uno_AI_Player::pop_frontier_node()
{
  if ( !m_frontier.empty() )
  {
    Uno_AI_Search_Node s = m_frontier.front();
    pop_heap( m_frontier.begin(), m_frontier.end() );
    m_frontier.pop_back();
    return s;
  } 
}

bool Uno_AI_Player::can_play_selected( const Uno_AI_Search_Node& s, unsigned int i )
{
  if ( s.m_state.m_played.empty() )
  {
    return true;
  }

  card p_card = s.m_state.m_hand[i];
  card d_card = s.m_state.m_played.back();

  // If the color or type of the card to play matches
  if ( (CARDCOLOR( p_card ) == CARDCOLOR( d_card )) 
    || (CARDTYPE( p_card ) == CARDTYPE( d_card )) 
    || (CARDTYPE( p_card ) == UNO_WILD)
    || (CARDTYPE( p_card ) == UNO_WILD_DRAW_FOUR)
    )
  {
    return true;
  }

  return false;
}

bool Uno_AI_Player::can_play_any( const Uno_AI_Search_Node& s ) 
{
  for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
  {
    if ( can_play_selected( s, i ) )
    {
      return true;
    }
  }

  return false;
}

unsigned int Uno_AI_Player::can_play_count( const Uno_AI_Search_Node& s )
{
  unsigned int count = 0;

  for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
  {
    if ( can_play_selected( s, i ) )
    {
      count++;
    }
  }

  return count;
}

bool Uno_AI_Player::goal_test( const Uno_AI_Search_Node &s )
{
  if ( s.m_state.m_hand.size() == 0 && s.m_state.m_at_play == m_id )
  {
    return true;
  }

  return false;
}

bool target_comp( const std::pair<unsigned int, unsigned int>& lhs,
                  const std::pair<unsigned int, unsigned int>& rhs )
{
  return (lhs.first < rhs.first);
}

bool color_comp( const std::pair<unsigned int, unsigned int>& lhs,
                 const std::pair<unsigned int, unsigned int>& rhs )
{
  return (lhs.second < rhs.second);
}


void Uno_AI_Player::construct_target_queue( const Uno_AI_Search_Node& s, vector<std::pair<unsigned int, unsigned int> >& q )
{
  unsigned int num_players = s.m_state.m_hand_counts.size();
  q.clear();
  std::pair<unsigned int, unsigned int> to_add;

  for ( unsigned int i = 0; i < num_players; i++ )
  {
    // Don't place ourselves in the targeting queue
    if ( i == m_id )
    {
      continue;
    }
    to_add.first  = s.m_state.m_hand_counts[i];
    to_add.second = i;
    q.push_back( to_add );
  }
  
  sort( q.begin(), q.end(), target_comp );
}

void Uno_AI_Player::count_hand_colors( const Uno_AI_Search_Node& s, vector<std::pair<unsigned int, unsigned int> >& q )
{
  // Create a map of the <color, number of appearances>
  std::map<unsigned int, unsigned int> color_counts;
  for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
  {
    if (CARDCOLOR( s.m_state.m_hand[i] ) != UNO_NO_COLOR )
    {
      color_counts[CARDCOLOR( s.m_state.m_hand[i] )]++;
    }
  }

  // Transform map into pairs for sorting
  q.clear();
  for ( std::map<unsigned int, unsigned int>::iterator it = color_counts.begin(); it != color_counts.end(); it++ )
  {
    q.push_back( (*it) );
  }
  sort( q.begin(), q.end(), color_comp );
}

unsigned int Uno_AI_Player::player_distance( bool dir, unsigned int num_players, unsigned int from, unsigned int to )
{
  if ( dir == true )
  {
    if ( to < from )
    {
      return (num_players - (from - to));
    }

    else
    {
      return (to - from);
    }
  }

  else
  {
    if ( from < to )
    {
      return (num_players - (to - from));
    }

    else
    {
      return (to - from);
    }
  }

  return -1;
}

bool Uno_AI_Player::have_card( const Uno_AI_Search_Node& s, card c )
{
  for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
  {
    if ( CARDTYPE( s.m_state.m_hand[i] ) == c )
    {
      return true;
    }
  }

  return false;
}

bool Uno_AI_Player::can_play_cardtype( const Uno_AI_Search_Node& s, card c )
{
  for ( unsigned int i = 0; i < s.m_state.m_hand.size(); i++ )
  {
    if ( (CARDTYPE( s.m_state.m_hand[i] ) == c) 
      || ( !s.m_state.m_played.empty()
        && (CARDCOLOR( s.m_state.m_hand[i] ) == CARDCOLOR( s.m_state.m_played.back() ))
        ) 
      )
    {
      return true;
    }
  }

  return false;
}
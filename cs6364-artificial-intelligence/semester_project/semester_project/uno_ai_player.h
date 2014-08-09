/*! \file uno_ai_player.h
    \brief Contains the functionality for having a computer player in the Uno game.
    \sa uno_ai_player.cpp
*/

#ifndef UNO_AI_PLAYER_H
#define UNO_AI_PLAYER_H

#include "uno_player.h"
#include "uno_pstate.h"
#include "uno_ai_search_node.h"
#include "uno_action.h"
#include "hr_timer.h"

using namespace std;

/** 
 * \brief An AI player for the game. 
 *
 * This class is used to represent a computer player for an Uno game. 
 */
class Uno_AI_Player : public Uno_Player
{
  public:
    /** 
     * \brief Default constructor. 
     * 
     * Constructs an Uno_AI_Player with empty and zero values. 
     */
    Uno_AI_Player() : m_level(0), m_nodes_to_keep(50000)
    {}

    /** 
     * \brief Full value specification constructor. 
     * 
     * Constructs an Uno_AI_Player with the given values.
     * \param n The name of the Uno_AI_Player.
     * \param s The score for the Uno_AI_Player.
     * \param l The difficulty level for the Uno_AI_Player.
     */
    Uno_AI_Player( const string& n, unsigned int s, unsigned int l );

    /**
     * \brief Calls the player to take his turn. 
     *
     * This function is invoked by the Uno_Runner whenever it is this player's
     * turn in the game. 
     * \param s The state of the game as visible from this player's perspective.
     * \retval Uno_Action The Uno_Action the player chooses this turn. 
     */
    Uno_Action take_turn( const Uno_PState& s );

    /**
     * \brief The player's AI difficulty level.
     * 
     * The level of the AI represents its difficulty level. A higher value
     * means that an AI player will make more intelligent plays and be harder
     * to defeat. 
     */
    unsigned char m_level;

  private:
    /**
     * \brief The Uno_AI_Player thinks about which Action to take. 
     *
     * This function is the brains of the AI. The AI will think() each turn
     * until it finds a suitable move or until time runs out. 
     * \sa UNO_TIME_PER_TURN
     */
    void think();

    /** 
     * \brief Returns the best move found by think().
     * \note It is not necessary, but the AI player should think() before
     * choosing a move.
     */
    void choose_action();

    /**
     * \brief Generates legal/plausible successor states from s.
     * \param s The state to generate successor states for.
     * \param suc The vector in which to store the successor states of s.
     * \note The results of this action will be stored in m_frontier. 
     */
    void successor( const Uno_AI_Search_Node& s, vector<Uno_AI_Search_Node>& suc );

    /**
     * \brief Transforms s to its successor with an action UNO_ACTION_PASS.
     *
     * Transforms s into a successor state of s with the action taken as 
     * UNO_ACTION_PASS.
     * \param s The state to populate.
     */
    void populate_successor_node_pass( Uno_AI_Search_Node& s );

    /**
     * \brief Transforms s to its successor with an action UNO_ACTION_DRAW.
     *
     * Transforms s into a successor state of s with the action taken as 
     * UNO_ACTION_DRAW.
     * \param s The state to populate.
     * \param d The specific card for the player at play in s to draw. 
     */
    void populate_successor_node_draw( Uno_AI_Search_Node& s, card d );

    /**
     * \brief Transforms s to its successor with an action UNO_ACTION_PLAY.
     *
     * Transforms s into a successor state of s with the action taken as 
     * UNO_ACTION_PLAY.
     * \param s The state to populate.
     * \param idx The index of the card in the player at play's hand to play.
     * \param color If the index of the card to play correlates to a wild or
     * wild draw four, specify this as well.
     */
    void populate_successor_node_play( Uno_AI_Search_Node& s, unsigned int idx, char color );
      
    /**
     * \brief Advances the player at play in s p positions.
     * \param s The state to advance the player position in.
     * \param p The number of positions to advance the player at play in s.
     */     
    void advance_players( Uno_AI_Search_Node& s, unsigned int p );

    /**
     * \brief Forces the next player in s to draw c cards.
     * \param s The state to force the next player to draw cards in. 
     * \param c The number of cards to force to draw.
     */
    void force_draw_cards( Uno_AI_Search_Node & s, unsigned int c );

    /**
     * \brief Pushes s into the frontier and maintains the heap.
     * 
     * Adds s to the frontier. Also re-heapifys the frontier as to maintain 
     * the max heap properties.
     * \param s The search node to add to the frontier
     */
    void push_frontier_node( const Uno_AI_Search_Node& s );

    /**
     * \brief Pops the top of the frontier heap and returns it.
     * 
     * Removes the top of the heap from the frontier. Also re-heapifys the
     * frontier as to maintain the max heap properties.
     * \return Uno_AI_Search_Node The top of the frontier.
     */
    Uno_AI_Search_Node pop_frontier_node();

    /**
     * \brief Returns the AI's score for the given node.
     * \param s The search node whose m_state to evaluate.
     * \return float The AI's score for the state in s.
     */
    float state_eval( const Uno_AI_Search_Node& s );

    /**
     * \brief Returns whether or not the player at play can play card i in his hand.
     * \retval true if the player at play in s can play card i from his hand.
     * \retval false Otherwise. 
     */
    bool can_play_selected( const Uno_AI_Search_Node& s, unsigned int i );
    
    /**
     * \brief Returns whether or not the player at play can play any card from his hand.
     * \retval true if the player can play any single card from his hand.
     * \retval false otherwise. 
     */
    bool can_play_any( const Uno_AI_Search_Node& s );

    /**
     * \brief Returns if the AI player can make any plays this turn.
     * \pre m_frontier has at least one search node on it.
     * \retval true If the AI player can play a card or draw this turn.
     * \retval false Otherwise. 
     */
    //bool can_do_anything();

    /**
     * \brief Returns if the AI player can make any play a card from his hand.
     * \pre m_frontier has at least one search node on it.
     * \retval true If the AI player can play a card this turn.
     * \retval false Otherwise. 
     */
    //bool can_play_anything();

    /**
     * \brief Returns if the AI player can draw a card this turn. 
     * \pre m_frontier has at least one search node on it.
     * \retval true If the AI player can draw a card this turn. 
     * \retval false Otherwise. 
     */
    //bool can_draw_anything();

    /**
     * \brief Determines if a given state is a goal state.
     * \param s The state to determine if it is a goal state.
     * \retval true If the state is a goal state for this player.
     * \retval false If the state is not a goal state for this player. 
     */
    bool goal_test( const Uno_AI_Search_Node& s );

    /**
     * \brief Constructs a targeting queue to target players who are closer to winning.
     * 
     * Places an ordered list of player ID numbers into q. The
     * list is sorted in ascending order of the number of cards in players hands. A player 
     * listed earlier in the queue is more of a threat to the AI losing.
     * \param s The current search node. 
     * \param q The targeting queue to be constructed. 
     */
    void construct_target_queue( const Uno_AI_Search_Node& s, vector<std::pair<unsigned int, unsigned int> >& q );
    
    /**
     * \brief Constructs an <color, count> mapping of the counts of the colors in 
     * the current player's hand.
     * \param s The current search node. 
     * \param q The queue in which to place the mapping of <color, count>.
     */
    void count_hand_colors( const Uno_AI_Search_Node& s, vector<std::pair<unsigned int, unsigned int> >& q );

    /**
     * \brief Calculates the number of positions between one player to another.
     * \param dir The direction of the game. true=forward, false=backward.
     * \param num_players The number of players in the game.
     * \param from The first player.
     * \param to The player to calculate the distance to. 
     * \return usigned int The distance from p layer from to player to. 
     */     
    unsigned int player_distance( bool dir, unsigned int num_players, unsigned int from, unsigned int to );

    /**
     * \brief Returns whether or not the player at play in s has card c in hand.
     * \param s The current state.
     * \param c The card to search the player's hand for.
     * \retval true If the player at play in s has c in hand.
     * \retval false Otherwise. 
     */
    bool have_card( const Uno_AI_Search_Node& s, card c );
    
    /** 
     * \brief Returns whether or not the player can play a card of type c.
     *
     * A card of type c must be in the player's hand and the top of the unplayed
     * deck must match the type c. 
     * \param s The current state.
     * \param c The the card type to search for.
     * \retval true If the player can play a card of type c.
     * \retval false Otherwise. 
     */
    bool can_play_cardtype( const Uno_AI_Search_Node& s, card c );
    
    /**
     * Returns the number of cards in the player at play's hand that he can play.
     *
     * To be able to play a card must match the color or type of the top of the
     * played deck.
     * \param s The current state.
     * \return unsigned int the number of cards the player can play this turn. 
     */
    unsigned int can_play_count( const Uno_AI_Search_Node& s );
    
    /**
     * \brief The set of states the AI considers considers per turn.
     *
     * The frontier contains, at any given time, the different states the AI
     * can see currently. The nodes are arraned in a max_heap fashion using
     * the C++ STL make_heap, pop_heap, and push_heap functions. Thus, after
     * pushing or popping a node from the frontier, the node at the front() 
     * of the vector will contain the best-known state. The best action 
     */
    vector<Uno_AI_Search_Node> m_frontier;

    /**
     * \brief The initial state passed to the AI at the beginning of his turn
     */
    Uno_AI_Search_Node m_initial_state;

    /**
     * \brief The best action found so far by the AI. 
     *
     * This will be returned as the chosen action at the end of each turn.
     */
    Uno_Action m_best_action;

    /**
     * \brief Used to time how long the AI takes to perform searching. 
     */
    CStopWatch m_timer;

    /**
     * \brief The amount of time left (in seconds) for the AI to think this turn.
     */
    double m_time_left;

    /** 
     * \brief The number of nodes to keep in the search space for the frontier.
     */
    unsigned int m_nodes_to_keep;

    /**
     * \brief A copy of the gobal default deck that is sorted in ascending order. 
     */
    deck m_sorted_default_deck;
};

#endif

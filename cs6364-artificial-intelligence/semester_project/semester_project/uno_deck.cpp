/*! \file uno_deck.cpp
    \brief Contains function definitions for an Uno deck.
    \sa uno_deck.h
*/

#include "uno_deck.h"
#include "uno_card.h"
#include <algorithm>
#include <iostream>

using namespace std;

void create_deck( deck& d )
{
  d.clear();

  for ( int i = 0; i < UNO_DECKS_PER_GAME; i++ )
  {
    d.insert( d.end(), DEFAULT_DECK, DEFAULT_DECK + UNO_DECK_SIZE );
  }
}

void shuffle_deck( deck& d )
{
  random_shuffle( d.begin(), d.end() );
}

void swap_decks( deck& d, deck& g )
{
  d.swap( g );
}

card take_card( deck& d )
{
  if ( !d.empty() )
  {
    card c = d.back();

    d.pop_back();

    return c;
  }

  else
  {
    return CARD( UNO_NO_COLOR, UNO_RESERVED );
  }
}

void print_deck( deck& d )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
  {
    cout << card_name( d[i] ) << " ";
  }
}

void print_deck_values( deck& d )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
  {
    cout << d[i] << " ";
  }
}

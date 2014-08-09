/*! \file uno_card.cpp
    \brief Contains function definitions for an Uno deck.
    \sa uno_card.h
*/

#include "uno_card.h"

using namespace std;

/** 
 * \brief A map of card types to card names.
 * 
 * The map is arranged such that card names can be accessed using the CARD
 * macro function. 
 * \note The map must be populated with map_names() before this map
 * can be effectively used.
 * \sa void map_names()
 * \sa string card_name( card c )
 */
card_name_dictionary CARD_NAME;

/** 
 * \brief A map of card types to card weights.
 * 
 * The map is arranged such that card weights can be accessed using the CARD
 * macro function. 
 * \note The map must be populated with map_weights() before this map
 * can be effectively used.
 * \sa void map_weights()
 * \sa unsigned char card_weight( card c )
 */
card_weight_dictionary CARD_WEIGHT;

void map_names()
{
  CARD_NAME[CARD(UNO_RED, UNO_ZERO)] = "R0";
  CARD_NAME[CARD(UNO_RED, UNO_ONE)] = "R1";
  CARD_NAME[CARD(UNO_RED, UNO_TWO)] = "R2";
  CARD_NAME[CARD(UNO_RED, UNO_THREE)] = "R3";
  CARD_NAME[CARD(UNO_RED, UNO_FOUR)] = "R4";
  CARD_NAME[CARD(UNO_RED, UNO_FIVE)] = "R5";
  CARD_NAME[CARD(UNO_RED, UNO_SIX)] = "R6";
  CARD_NAME[CARD(UNO_RED, UNO_SEVEN)] = "R7";
  CARD_NAME[CARD(UNO_RED, UNO_EIGHT)] = "R8";
  CARD_NAME[CARD(UNO_RED, UNO_NINE)] = "R9";
  CARD_NAME[CARD(UNO_RED, UNO_SKIP)] = "RS";
  CARD_NAME[CARD(UNO_RED, UNO_REVERSE)] = "RR";
  CARD_NAME[CARD(UNO_RED, UNO_DRAW_TWO)] = "RD2";

  CARD_NAME[CARD(UNO_GREEN, UNO_ZERO)] = "G0";
  CARD_NAME[CARD(UNO_GREEN, UNO_ONE)] = "G1";
  CARD_NAME[CARD(UNO_GREEN, UNO_TWO)] = "G2";
  CARD_NAME[CARD(UNO_GREEN, UNO_THREE)] = "G3";
  CARD_NAME[CARD(UNO_GREEN, UNO_FOUR)] = "G4";
  CARD_NAME[CARD(UNO_GREEN, UNO_FIVE)] = "G5";
  CARD_NAME[CARD(UNO_GREEN, UNO_SIX)] = "G6";
  CARD_NAME[CARD(UNO_GREEN, UNO_SEVEN)] = "G7";
  CARD_NAME[CARD(UNO_GREEN, UNO_EIGHT)] = "G8";
  CARD_NAME[CARD(UNO_GREEN, UNO_NINE)] = "G9";
  CARD_NAME[CARD(UNO_GREEN, UNO_SKIP)] = "GS";
  CARD_NAME[CARD(UNO_GREEN, UNO_REVERSE)] = "GR";
  CARD_NAME[CARD(UNO_GREEN, UNO_DRAW_TWO)] = "GD2";

  CARD_NAME[CARD(UNO_BLUE, UNO_ZERO)] = "B0";
  CARD_NAME[CARD(UNO_BLUE, UNO_ONE)] = "B1";
  CARD_NAME[CARD(UNO_BLUE, UNO_TWO)] = "B2";
  CARD_NAME[CARD(UNO_BLUE, UNO_THREE)] = "B3";
  CARD_NAME[CARD(UNO_BLUE, UNO_FOUR)] = "B4";
  CARD_NAME[CARD(UNO_BLUE, UNO_FIVE)] = "B5";
  CARD_NAME[CARD(UNO_BLUE, UNO_SIX)] = "B6";
  CARD_NAME[CARD(UNO_BLUE, UNO_SEVEN)] = "B7";
  CARD_NAME[CARD(UNO_BLUE, UNO_EIGHT)] = "B8";
  CARD_NAME[CARD(UNO_BLUE, UNO_NINE)] = "B9";
  CARD_NAME[CARD(UNO_BLUE, UNO_SKIP)] = "BS";
  CARD_NAME[CARD(UNO_BLUE, UNO_REVERSE)] = "BR";
  CARD_NAME[CARD(UNO_BLUE, UNO_DRAW_TWO)] = "BD2";

  CARD_NAME[CARD(UNO_YELLOW, UNO_ZERO)] = "Y0";
  CARD_NAME[CARD(UNO_YELLOW, UNO_ONE)] = "Y1";
  CARD_NAME[CARD(UNO_YELLOW, UNO_TWO)] = "Y2";
  CARD_NAME[CARD(UNO_YELLOW, UNO_THREE)] = "Y3";
  CARD_NAME[CARD(UNO_YELLOW, UNO_FOUR)] = "Y4";
  CARD_NAME[CARD(UNO_YELLOW, UNO_FIVE)] = "Y5";
  CARD_NAME[CARD(UNO_YELLOW, UNO_SIX)] = "Y6";
  CARD_NAME[CARD(UNO_YELLOW, UNO_SEVEN)] = "Y7";
  CARD_NAME[CARD(UNO_YELLOW, UNO_EIGHT)] = "Y8";
  CARD_NAME[CARD(UNO_YELLOW, UNO_NINE)] = "Y9";
  CARD_NAME[CARD(UNO_YELLOW, UNO_SKIP)] = "YS";
  CARD_NAME[CARD(UNO_YELLOW, UNO_REVERSE)] = "YR";
  CARD_NAME[CARD(UNO_YELLOW, UNO_DRAW_TWO)] = "YD2";

  CARD_NAME[CARD(UNO_NO_COLOR, UNO_WILD)] = "W";
  CARD_NAME[CARD(UNO_RED, UNO_WILD)] = "WR";
  CARD_NAME[CARD(UNO_GREEN, UNO_WILD)] = "WG";
  CARD_NAME[CARD(UNO_BLUE, UNO_WILD)] = "WB";
  CARD_NAME[CARD(UNO_YELLOW, UNO_WILD)] = "WY";
  CARD_NAME[CARD(UNO_NO_COLOR, UNO_WILD_DRAW_FOUR)] = "WD4";
  CARD_NAME[CARD(UNO_RED, UNO_WILD_DRAW_FOUR)] = "WD4R";
  CARD_NAME[CARD(UNO_GREEN, UNO_WILD_DRAW_FOUR)] = "WD4G";
  CARD_NAME[CARD(UNO_BLUE, UNO_WILD_DRAW_FOUR)] = "WD4B";
  CARD_NAME[CARD(UNO_YELLOW, UNO_WILD_DRAW_FOUR)] = "WD4Y";

  return;
}

string card_name( card c )
{
  return CARD_NAME[c];
}

void map_weights()
{
  CARD_WEIGHT[UNO_ZERO] = 0;
  CARD_WEIGHT[UNO_ONE] = 1;
  CARD_WEIGHT[UNO_TWO] = 2;
  CARD_WEIGHT[UNO_THREE] = 3;
  CARD_WEIGHT[UNO_FOUR] = 4;
  CARD_WEIGHT[UNO_FIVE] = 5;
  CARD_WEIGHT[UNO_SIX] = 6;
  CARD_WEIGHT[UNO_SEVEN] = 7;
  CARD_WEIGHT[UNO_EIGHT] = 8;
  CARD_WEIGHT[UNO_NINE] = 9;
  CARD_WEIGHT[UNO_SKIP] = 20;
  CARD_WEIGHT[UNO_REVERSE] = 20;
  CARD_WEIGHT[UNO_DRAW_TWO] = 20;
  CARD_WEIGHT[UNO_WILD] = 50;
  CARD_WEIGHT[UNO_WILD_DRAW_FOUR] = 50;

  return;
}

unsigned char card_weight( card c )
{
  // The card weight depends only on card type, not color. Filter out the color
  // before querying for the weight.

  return CARD_WEIGHT[CARDTYPE(c)];
}
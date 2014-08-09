/*! \file uno_card.h
    \brief Contains macros and function prototypes for an Uno card.
    \sa uno_card.cpp

    \example ../examples/uno_card_examples.cpp
*/

#ifndef UNO_CARD_H
#define UNO_CARD_H

#include <map>
#include <string>

using namespace std;

/** 
 * \brief Type definition for a single uno card represented by a char.
 *
 * The Uno card is equivalent to a char type. A card's construction utilizes 
 * the bits of a char rather than its decimal value. 
 * The construction looks like this:
 *   
 * 76543210
 *
 * RGBYVVVV
 *
 * The R bit is 1 if the card is red. 
 * The G bit is 1 if the card is green.
 * The B bit is 1 if the card is blue. 
 * The Y bit is 1 if the card is yellow.
 * You may use more than one color, but it is not regulation.
 * The right-most 4 bits are reserved for the card's face value. So if the card
 * is a "red 8" card, its construction is 10001000. A "yellow draw two" 
 * is 00011100. Wild and Wild Draw Four cards have no color. 
 * \sa UNO_COLOR
 * \sa UNO_TYPE
 */
typedef unsigned char card;

/**
 * \brief A dictionary for the pretty names of cards based on their types.
 * \sa card_name()
 */
typedef map<card, string> card_name_dictionary;

/**
 * \brief A dictionary of the weighted values for each card. Face values for
 * cards are worth their own numbers, while special cards are worth 20 points.
 * \sa card_weight()
 */
typedef map<card, unsigned char> card_weight_dictionary;

/**
 * Each Uno card (excluding the Wild cards) has one of four colors:
 * Red, Green, Blue, or Yellow. These colors are represented by these
 * definitions. 
 */
#define UNO_NO_COLOR ( 0 )       /*!< No color Uno card value. */
#define UNO_RED      ( 1 << 7 )  /*!< Red Uno card value. */
#define UNO_GREEN    ( 1 << 6 )  /*!< Green Uno card value. */
#define UNO_BLUE     ( 1 << 5 )  /*!< Blue Uno card value. */
#define UNO_YELLOW   ( 1 << 4 )  /*!< Yellow Uno card value. */
#define UNO_NUM_COLORS 4         /*!< Number of colors of Uno cards. */

/** 
 * \brief An array of UNO_COLOR available for use in the game 
 */
const unsigned char UNO_COLOR[] = {
  UNO_RED,
  UNO_GREEN,
  UNO_BLUE,
  UNO_YELLOW
};

/** 
 * Each Uno card has a face value, called a card type. These card values are
 * represented with these definitions.
 */
#define UNO_ZERO            0    /*!< Uno card 0 value. */
#define UNO_ONE             1    /*!< Uno card 1 value. */
#define UNO_TWO             2    /*!< Uno card 2 value. */
#define UNO_THREE           3    /*!< Uno card 3 value. */
#define UNO_FOUR            4    /*!< Uno card 4 value. */
#define UNO_FIVE            5    /*!< Uno card 5 value. */
#define UNO_SIX             6    /*!< Uno card 6 value. */
#define UNO_SEVEN           7    /*!< Uno card 7 value. */
#define UNO_EIGHT           8    /*!< Uno card 8 value. */
#define UNO_NINE            9    /*!< Uno card 9 value. */
#define UNO_SKIP            10   /*!< Uno card skip value. */
#define UNO_REVERSE         11   /*!< Uno card reverse value. */
#define UNO_DRAW_TWO        12   /*!< Uno card draw two value. */
#define UNO_WILD            13   /*!< Uno card wild value. */
#define UNO_WILD_DRAW_FOUR  14   /*!< Uno card wild draw four value. */
#define UNO_RESERVED        15   /*!< Uno card reserved value. */
#define UNO_NUM_TYPES       15   /*!< Number of different types of Uno cards. */

/** 
 * \brief An array of UNO_TYPE available for use in the game 
 */
const unsigned char UNO_TYPE[] = {
  UNO_ZERO,
  UNO_ONE,
  UNO_TWO,
  UNO_THREE,
  UNO_FOUR,
  UNO_FIVE,
  UNO_SIX,
  UNO_SEVEN,
  UNO_EIGHT,
  UNO_NINE,
  UNO_SKIP,
  UNO_REVERSE,
  UNO_DRAW_TWO,
  UNO_WILD,
  UNO_WILD_DRAW_FOUR
};

/** 
 * \brief Converts an UNO_COLOR and UNO_TYPE to a usable card.
 *
 * Takes an Uno color and an uno card type and converts it to a usable card.  
 * \param a An UNO_COLOR. 
 * \param b An UNO_TYPE.
 * \sa typedef char card;
 * \sa UNO_COLOR
 * \sa UNO_TYPE
 * 
 * For example:
 *
 * CARD( UNO_RED, UNO_THREE ); designates a Red 3 card.
 *
 * CARD( UNO_COLOR[1], UNO_TYPE[UNO_DRAW_TWO] ); designates a green draw two.
 *
 * \note The parameter order may be switched, ie, an UNO_TYPE then an UNO_COLOR
 * but to work properly you must have one both.
 */
#define CARD(a, b) ((a)|(b))

/**
 * \brief Gets the type of a card.
 *
 * This value is the type of the card, like UNO_ONE or UNO_DRAW_TWO.
 * \param a An Uno card created with CARD() 
 * \sa UNO_TYPE
 */
#define CARDTYPE(a) ((a)&(0xF))

/**
 * \brief Gets the color of a card.
 *
 * This value is the color of the card, like UNO_BLUE or UNO_RED.
 * \param b An Uno card created with CARD()
 * \sa UNO_COLOR  
 */
#define CARDCOLOR(b) ((b)&(0xF0))

/** 
 * \brief Instantiates a user-readable dictionary of names for a card. 
 * \note You only need to do this one time.
 */
void map_names();

/** 
 * \brief Gets the name of an Uno card.
 * \param c The card to get the name of.
 * \retval string The name of the card.
 *
 * The card names will come back something like "R5" for a red five, or "YD2" 
 * for a yellow draw two.
 * \note A reverse card will have a type of "R". So "YR" means a yellow reverse.
 * This is not to be confused with "R" for the color red. 
 */
string card_name( card c );

/** 
 * \brief Instantiates a user-readable dictionary of weights for cards.
 * \note You only need to do this one time.
 * For a regulation game, the weights are the points a player gets at the end
 * of the game for his opponents having a given card, c, in their hand. 
 */
void map_weights();

/** 
 * \brief Gets the weight of an Uno card.
 * \param c The card to get the weight of.
 * \retval unsigned char The weight of the card.
 * For a regulation game, the weights are the points a player gets at the end
 * of the game for his opponents having a given card, c, in their hand. 
 * Point values mapped according to regulation, are:
 * \li Number cards - Face value points (0-9 is 0-9 points, respectively)
 * \li Draw Two – 20 Points 
 * \li Reverse – 20 Points 
 * \li Skip – 20 Points 
 * \li Wild – 50 Points 
 * \li Wild Draw – 50 Points 
 */
unsigned char card_weight( card c );

#endif
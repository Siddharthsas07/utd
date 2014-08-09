#include "uno_card.h"
#include <iostream>

int main( int argc, char * argv[] ) 
{
  map_names();
  map_weights();
  
  // Numeric value for a green zero.
  cout << CARD( UNO_GREEN, UNO_ZERO ) << endl;
  
  // The name of a green zero.
  cout << CARD_NAME[CARD( UNO_GREEN, UNO_ZERO )] << endl;
  
  // The weight of a green zero.
  cout << CARD_WEIGHT[CARD( UNO_GREEN, UNO_ZERO )] << endl;
  
  if ( CARDCOLOR( CARD( UNO_GREEN, UNO_ZERO ) ) == UNO_GREEN )
  {
    cout << "The card is green." << endl;
  }
  
  if ( CARDTYPE( CARD( UNO_GREEN, UNO_ZERO ) ) == UNO_ZERO )
  {
    cout << "The card is a zero." << endl;
  }
  
}
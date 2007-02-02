
#include "RichRegistry.h"

using namespace Rich::MC::Digi;

// Static data members
RichRegistry::RegisterMap RichRegistry::theRegister;

void RichRegistry::cleanUp()
{
  for ( RegisterMap::iterator i = theRegister.begin();
        i != theRegister.end(); ++i ) 
  {
    RichBase * const & ptr = *i;
    if ( ptr ) { delete ptr; }
  }
}

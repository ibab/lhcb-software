
#include "RichRegistry.h"


// Static data members
RichRegistry::RegisterMap RichRegistry::theRegister;

void RichRegistry::cleanUp()
{
  for ( RegisterMap::iterator i = theRegister.begin();
        i != theRegister.end(); ++i ) {
    RichBase * const & ptr = *i;
    if ( ptr ) { delete ptr; }
  }
}















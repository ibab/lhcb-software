
#include "RichRegistry.h"
#include "RichBase.h"

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

const RichBase * RichRegistry::GetNewBase( std::vector<RichSmartID> & pixels )
{
  RichBase* theBase = new RichBase();
  theBase->upDate(pixels);
  return theBase;
}

const RichBase * RichRegistry::GetBase( )
{
  RegisterMap::iterator f = theRegister.begin();
  return (*f);
}

void RichRegistry::RegisterMe(RichBase*b)
{
  theRegister.insert(b);
}













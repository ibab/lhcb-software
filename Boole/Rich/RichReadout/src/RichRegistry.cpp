
#include "RichReadout/RichRegistry.h"
#include "RichReadout/RichBase.h"

const RichBase * RichRegistry::GetNewBase( std::vector< unsigned int > pixels )
{

  RichBase* theBase = SetupBase(pixels);
  pixels.clear();

  return theBase;

}

const RichBase * RichRegistry::GetBase( )
{

  RegisterMap::iterator f = theRegister.begin();

  return (*f);

}

RichBase* RichRegistry::SetupBase( std::vector<unsigned int> pixels)
{

  RichBase* theBase;
  theBase->upDate(pixels);

  return theBase;
}

RichRegistry::RegisterMap RichRegistry::theRegister;

void RichRegistry::RegisterMe(RichBase*b)
{
  theRegister.insert(b);
}













#ifndef RichRegistry_h
#define RichRegistry_h

#include <set>
#include <string>
#include <vector>

#include "RichBase.h"

class RichSmartID;

class RichRegistry {

public:

  RichRegistry() {}

  ~RichRegistry() { cleanUp(); }

  static const RichBase* GetNewBase( const std::vector<RichSmartID> & pixels );

  static const RichBase* GetBase();

  static void RegisterMe(RichBase*);

private: // methods

  void cleanUp();

private: // data

  typedef std::set<RichBase*,std::less<RichBase*> > RegisterMap;
  static RegisterMap theRegister;

};

inline const RichBase * RichRegistry::GetNewBase( const std::vector<RichSmartID> & pixels )
{
  RichBase * theBase = new RichBase();
  theBase->upDate(pixels);
  return theBase;
}

inline const RichBase * RichRegistry::GetBase( )
{
  return *(theRegister.begin());
}

inline void RichRegistry::RegisterMe(RichBase*b)
{
  theRegister.insert(b);
}

#endif

#ifndef RichRegistry_h
#define RichRegistry_h

#include <set>
#include <string>
#include <vector>

class RichBase;
class RichSmartID;

class RichRegistry {

public:

  RichRegistry() {}

  ~RichRegistry() { cleanUp(); }

  static const RichBase* GetNewBase( std::vector<RichSmartID> & pixels );

  static const RichBase* GetBase();

  static void RegisterMe(RichBase*);

private: // methods

  void cleanUp();

private: // data

  typedef std::set<RichBase*,std::less<RichBase*> > RegisterMap;
  static RegisterMap theRegister;

};

#endif

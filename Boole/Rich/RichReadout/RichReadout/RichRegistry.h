#ifndef RichRegistry_h
#define RichRegistry_h

#include <set>
#include <string>
#include <vector>

class RichBase;

class RichRegistry {

public:

  static const RichBase* GetNewBase( std::vector< unsigned int > );

  static const RichBase* GetBase();

  static RichBase* SetupBase( std::vector<unsigned int> );

  static void RegisterMe(RichBase*);


private:

  typedef set<RichBase*,less<RichBase*> > RegisterMap;
  static RegisterMap theRegister;

};

#endif






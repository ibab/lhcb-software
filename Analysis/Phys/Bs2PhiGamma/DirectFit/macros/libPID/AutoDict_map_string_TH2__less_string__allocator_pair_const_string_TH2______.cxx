#include "map"
#include "string"
class TH2;
#ifdef __CINT__ 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class map<string,TH2*,less<string>,allocator<pair<const string,TH2*> > >+;
#pragma link C++ class map<string,TH2*,less<string>,allocator<pair<const string,TH2*> > >::*;
#pragma link C++ operators map<string,TH2*,less<string>,allocator<pair<const string,TH2*> > >::iterator;
#pragma link C++ operators map<string,TH2*,less<string>,allocator<pair<const string,TH2*> > >::const_iterator;
#pragma link C++ operators map<string,TH2*,less<string>,allocator<pair<const string,TH2*> > >::reverse_iterator;
#endif

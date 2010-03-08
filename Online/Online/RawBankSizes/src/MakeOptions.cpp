#include "Event/RawEvent.h"
#include "cstdio"
int main(int /* argc */, char** /* argv */)
{
  std::string bname;
  printf("======== Running MakeOptions...\n");
  FILE *f = fopen("/tmp/banks.in","w");
  for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
  {
    bname = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
    fprintf(f,"%s \n",bname.c_str());
  }
  
  return 0;
}

#include "Event/RawEvent.h"
#include "cstdio"
int main(int /* argc */, char** /* argv */)
{
  std::string bname;
  std::string fname;
  printf("======== Running MakeOptions...\n");
#ifdef WIN32
  fname = getenv("TMP");
  fname += "\\banks.in";
#else
  fname = "/tmp";
  fname += "/banks.in";
#endif
  FILE *f = fopen(fname.c_str(),"w");
  for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
  {
    bname = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
    fprintf(f,"%s \n",bname.c_str());
  }
  fclose(f);
  
  return 0;
}

#include "Event/RawEvent.h"
#include "stdio.h"
main(int argc, char *argv[])
{
  int i;
  std::string bname;
  printf("======== Running MakeOptions...\n");
  FILE *f;
  f = fopen("/tmp/banks.in","w");
  for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
  {
    bname = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
    fprintf(f,"%s \n",bname.c_str());
  }
  
  return 0;
}

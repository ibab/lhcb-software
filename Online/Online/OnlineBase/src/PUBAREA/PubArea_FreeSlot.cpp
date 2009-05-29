//
// PUBAREA_FreeSlot.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/PubArea.h"

extern "C" int pubarea_freeslot(int argc,char** argv)  {
  void *Slot;
  const char* name = "Unknown";
  if ( !((argc==3)&&(sscanf(argv[2],"%p",&Slot)==1)) ) Slot=0;
  if (argc==2) { name = argv[1]; argc--; }

  printf("Calling: pubarea_freeslot %p\n",Slot);
  printf("         Slot Address   %p\n",Slot);

  PubArea PA(name);
  int status = PA.LinkPubArea(~0);
  if (!(status&1))   {
    printf("PubArea_FreeSlot: Failed to link pubarea\n");
    return status;
  }
  status = PA.FreeSlot(Slot);
  if  (!(status&1))   {
    printf("PubArea_FreeSlot: Failed to deallocate slot\n");
  }
  return 0;
}

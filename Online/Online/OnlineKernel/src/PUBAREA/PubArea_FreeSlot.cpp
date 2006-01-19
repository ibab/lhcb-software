//
// PUBAREA_FreeSlot.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubarea_freeslot(int argc,char** argv)  {
  void *Slot;
  if ( !((argc==2)&&(sscanf(argv[1],"%d",&Slot)==1)) ) Slot=0;
  printf("Calling: pubarea_freeslot %d\n\0",(int*)Slot);
  printf("         Slot Address   %d\n\0",(int*)Slot);

  PubArea PA;
  int status = PA.LinkPubArea();
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

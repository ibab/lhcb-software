//
// PUBAREA_AllocateSlot.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include <cstring>
#include "CPP/PubArea.h"

extern "C" int pubarea_allocateslot(int argc,char** argv)  {
  int Mask, Size,Type;
  const char* name = "None";
  if ((argc==5)&&(sscanf(argv[4],"%d",&Mask)==1)) {argc--;} else {Mask=64;}
  if ((argc==4)&&(sscanf(argv[3],"%d",&Size)==1)) {argc--;} else {Size=32;}
  if ((argc==3)&&(sscanf(argv[2],"%d",&Type)==1)) {argc--;} else {Type=1;}
  if (argc==2) {name=argv[1]; argc--;}
  printf("Using: pubarea_allocateslot %s %d %d\n",name, Type,Size);
  printf("       Slot Type   %2d\n",Type);
  printf("       Slot Size %4d bytes\n",Size);
  printf("       Mask  %d\n",Mask);

  PubArea PA(name);
  int status = PA.LinkPubArea(~0);
  if (!(status&1))   {
    printf("PubArea_AllocateSlot: Failed to link pubarea\n");
    return(status);
  }
  void* SlotPtr = 0;

  status = PA.AllocateSlot(Type,Size,SlotPtr);
  if  (!(status&1))   {
    printf("PubArea_AllocateSlot: Failed to allocate slot\n");
  }
  ::memset(SlotPtr,Mask,Size);
  ::printf("PubArea_AllocateSlot: Allocated slot with address %p\n",SlotPtr);

  return(0);
}

//
// PUBAREA_AllocateSlot.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubarea_allocateslot(int argc,char** argv)  {
  int Mask, Size,Type;
  if ((argc==4)&&(sscanf(argv[3],"%d",&Mask)==1)) {argc--;} else {Mask=64;}
  if ((argc==3)&&(sscanf(argv[2],"%d",&Size)==1)) {argc--;} else {Size=32;}
  if ((argc==2)&&(sscanf(argv[1],"%d",&Type)==1)) {argc--;} else {Type=1;}

  printf("Using: pubarea_allocateslot %d %d\n\0",Type,Size);
  printf("       Slot Type   %2d\n\0",Type);
  printf("       Slot Size %4d bytes\n\0",Size);
  printf("       Mask  %d\n\0",Mask);

  PubArea PA;
  int status = PA.LinkPubArea();
  if (!(status&1))   {
    printf("PubArea_AllocateSlot: Failed to link pubarea\n");
    return(status);
  }
  void* SlotPtr = 0;

  status = PA.AllocateSlot(Type,Size,SlotPtr);
  if  (!(status&1))   {
    printf("PubArea_AllocateSlot: Failed to allocate slot\n");
  }
  memset(SlotPtr,Mask,Size);
  printf("PubArea_AllocateSlot: Allocated slot with address %d\n\0",
    (int*)SlotPtr);

  return(0);
}

//
// PUBAREA_GetSlotofType.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/PubArea.h"

extern "C" int pubarea_getslotoftype(int argc,char** argv)  {
  const char* name = "Unknown";
  if (argc<3)   {
      printf("Usage: pubarea_getslotoftype <name> <SlotType>\n");
      printf("       <SlotType> - Type of slot (integer 0-99)\n");
      return(0);
   }

   int Type;
   if ( (argc==3) && (sscanf(argv[2],"%d",&Type)==1) ) {argc--;} else {Type=1;}
   if (argc==2) { name = argv[1]; argc--; }

   PubArea PA(name);
   int status = PA.LinkPubArea(~0);
   if (!(status&1))   {
     printf("PubArea_AllocateSlot: Failed to link pubarea\n");
     return(status);
   }
   void* SlotPtr = 0;
   status = PA_SUCCESS;
   int context = -1;
   while (status==PA_SUCCESS)   {  
     status = PA.GetSlotofType(Type,&context,SlotPtr);
     if (status!=PA_SUCCESS) break;
     //      strcpy((char*)SlotPtr,"Hello");
     printf("PubArea_GetSlotofType: Found slot %p\n",SlotPtr);
   }
   return(0);
}

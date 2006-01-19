//
// PUBAREA_GetSlotofType.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubarea_getslotoftype(int argc,char** argv)  {
   if (argc==1)   {
      printf("Usage: pubarea_getslotoftype <SlotType>\n\0");
      printf("       <SlotType> - Type of slot (integer 0-99)\n\0");
      return(0);
   }

   int Type;
   if ( (argc==2) && (sscanf(argv[1],"%d",&Type)==1) ) {
     argc--;
   } else {
     Type=1;
   }

   PubArea PA;
   int status = PA.LinkPubArea();
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
     //      strcpy((char*)SlotPtr,"Hello\0");
     printf("PubArea_GetSlotofType: Found slot %d\n\0",(int*)SlotPtr);
   }
   return(0);
}

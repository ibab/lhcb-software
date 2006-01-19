//
// PUBAREA_LOCK.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubarea_lock(int argc,char** argv)  {
   PubArea PA;
   int status = PA.LinkPubArea();
   if (!(status&1))   {
     printf("PubArea_Lock: Failed to link pubarea\n");
     return(status);
   }
   status = PA.Lock();
   if (!(status&1))   {
     printf("PubArea_Lock: Failed to lock pubarea\n");
     return(status);
   }

   char dummy[20];
   printf("<<< Press any key to unlock the publishing area >>>\n\0");
   gets(dummy);
   return(0);
}

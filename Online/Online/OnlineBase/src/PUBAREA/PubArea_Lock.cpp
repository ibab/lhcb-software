//
// PUBAREA_LOCK.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/PubArea.h"

extern "C" int pubarea_lock(int argc,char** argv)  {
   PubArea PA(argc>1 ? argv[1]: "Unknown");
   int status = PA.LinkPubArea(~0);
   if (!(status&1))   {
     printf("PubArea_Lock: Failed to link pubarea\n");
     return(status);
   }
   status = PA.Lock();
   if (!(status&1))   {
     printf("PubArea_Lock: Failed to lock pubarea\n");
     return(status);
   }
   printf("<<< Press any key to unlock the publishing area >>>\n");
   ::getc(stdin);
   return(0);
}

//
// PUBAREA_UNLOCK.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubarea_unlock(int argc,char** argv)  {
   PubArea PA;
   int status = PA.LinkPubArea();
   if (!(status&1))   {
     printf("PubArea_UnLock: Failed to link pubarea\n");
     return(status);
   }
   status = PA.UnLock();
   if (!(status&1))   {
     printf("PubArea_UnLock: Failed to unlock pubarea\n");
     return(status);
   }
   return(0);
}

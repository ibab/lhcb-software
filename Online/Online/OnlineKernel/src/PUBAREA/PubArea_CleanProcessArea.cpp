//
// PUBAREA_DISPLAY.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubarea_cleanprocarea(int argc,char** argv)  {
   PubArea PA;
   int status = PA.LinkPubArea();
   if (!(status&1))   {
     printf("PubArea_Display: Failed to link pubarea\n");
     return(status);
   }
   return PA.CleanProcessArea();
}

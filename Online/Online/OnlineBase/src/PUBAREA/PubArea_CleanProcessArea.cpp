//
// PUBAREA_DISPLAY.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/PubArea.h"

extern "C" int pubarea_cleanprocarea(int argc,char** argv)  {
  char    *cptr;
  const char *name = 0;
  while (--argc > 0)     {           /* process options */
    if (*(cptr = *++argv) == '-')    {
      switch (*(cptr+1) | 0x20)  {
      case 'n':
        name = cptr+3;
        printf("Pubarea Name is %s\n",name);
        break;
      default:
        printf("Usage: pubarea_clean [,-opt.]\n");
        printf("Options:\n");
        printf("    -n=<name>       Publishing area name (if not default)\n");
        return(0);
      }
    }
  }
  PubArea PA(name);
  int status = PA.LinkPubArea(~0);
  if (!(status&1))   {
    printf("PubArea_Display: Failed to link pubarea\n");
    return(status);
  }
  return PA.CleanProcessArea();
}

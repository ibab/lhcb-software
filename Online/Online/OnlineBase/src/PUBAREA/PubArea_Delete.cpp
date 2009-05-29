//
// PUBAREA_DELETE.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/PubArea.h"

extern "C" int pubarea_delete(int argc, char* argv[] )  {
  char *cptr, *name = 0;
  while (--argc > 0)   {             /* process options */
    if (*(cptr = *++argv) == '-')    {
      switch (*(cptr+1) | 0x20)   {
      case 'n':
        name = cptr+3;
        printf("Pubarea Name is %s\n",name);
        break;
      default:
        printf("Usage: pubarea_delete [,-opt.]\n");
        printf("Options:\n");
        printf("    -n=<name>       Publishing area name (if not default)\n");
        return(0);
      }
    }
  }
  PubArea PA(name);
  int status = PA.DeletePubArea();
  ::printf("%s to delete pubarea\n", status&1 ? "Succeeded" : "Failed");
  return(0);
}

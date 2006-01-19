//
// PUBAREA_CREATE.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/PubArea.h"

extern "C" int pubarea_create(int argc,char** argv)  {
  int Size = -1;
  int status = 0;
  char    *cptr;
  const char *name = 0;

  while (--argc > 0)  {              /* process options */
    if (*(cptr = *++argv) == '-')    {
      switch (*(cptr+1) | 0x20)   {
      case 'n':
        name = cptr+3;
        printf("Pubarea Name is %s\n",name);
        break;
      case 's':
        status = sscanf(cptr+2,"=%d",&Size);
        if( status != 1 )   {
          printf("Error reading publishing area size\n");
          return(0);
        }
        break;
      default:
        printf("Usage: pubarea_create [,-opt.]\n");
        printf("Function: Publishing area creation\n");
        printf("Options:\n");
        printf("    -n=<name>       Publishing area name (if not default)\n");
        printf("    -s=<size> [%d]  Publishing area size (bytes)\n",Size);
        return(0);
      }
    }
  }

  PubArea PA(name);
  status = PA.CreatePubArea(Size);
  printf("%s to create pubarea\n", status&1 ? "Succeeded" : "Failed");
  return(0);
}

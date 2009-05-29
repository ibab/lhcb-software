//
// PUBAREA_DISPLAY.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include <cstring>
#include "CPP/PubArea.h"

extern "C" int pubarea_display(int argc,char** argv)  {
  char    *cptr, *name = 0;
  char Node[16] = "\0";
  while (--argc > 0) {               /* process options */
    if (*(cptr = *++argv) == '-')	 {
      switch (*(cptr+1) | 0x20)   {
        case 'm':
          name = cptr+3;
          printf("Pubarea Name is %s\n",name);
          break;
        case 'n':
          sscanf(cptr+3,"%s",Node);
          printf("Pubarea Node is %s\n",Node);
          break;
        default:
          printf("Usage: pubarea_display [,-opt.]\n");
          printf("Function: Publishing area creation\n");
          printf("Options:\n");
          printf("    -m=<name>       Publishing area name (if not default)\n");
          printf("    -n=<node>       Publishing area node (if not default)\n");
          return(0);
      }
    }
  }

  PubArea PA(name);
  if (strcmp(Node,"\0")==0)   {
    printf("PubArea_Display: Getting the display of this node\n");
  }
  else   {
    printf("PubArea_Display: Getting the display of node %s\n",Node);
  }
  //............................................ Linking to the publishing area
  int status = PA.LinkPubArea(Node);
  if (status!=PA_SUCCESS)  {
    printf("PubArea_Display: Failed to link pubarea\n");
    return(0);
  }
  status = PA.PrintHeader();
  if (status!=PA_SUCCESS) return(0);

  PA.PrintIndex();
  if (status!=PA_SUCCESS) return(0);
  return(0);
}

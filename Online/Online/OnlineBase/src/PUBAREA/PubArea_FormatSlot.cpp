//
// PUBAREA_DISPLAY.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include <cstring>
#include "CPP/PubArea.h"

extern "C" int pubareas_formatslot(int argc,char** argv)  {
  int Type = 0;
  char Format[16];
  const char* name = "Unknown";
  if ( !((argc--==4)&&(sscanf(argv[3],"%s",Format)==1)) )  {
    strncpy(Format,"10I",sizeof(Format));
    Format[sizeof(Format)-1]=0;
  }
  if ( !((argc==3) && (sscanf(argv[2],"%d",&Type)==1)) ) Type = 1;
  if (argc==2) { name = argv[1]; argc--; }

  printf("PubArea_FormatSlot: Set format to slot type %d as <%s>\n",Type,Format);

  PubArea PA(name);
  //............................................ Linking to the publishing area
  int status = PA.LinkPubArea(~0);
  if (status!=PA_SUCCESS)  {
    printf("PubArea_Display: Failed to link pubarea\n");
    return 0;
  }
  status = PA.SetSlotTypeFormat(Type, Format);
  if (status!=PA_SUCCESS)  {
    printf("PubArea_FormatSlot: Failed to set the format\n");
  }
  return 0;
}

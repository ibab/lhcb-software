//
// PUBAREA_DISPLAY.CC ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "CPP/pubarea.h"

extern "C" int pubareas_formatslot(int argc,char** argv)  {
  int Type = 0;
  char Format[16] = "\0";
  if ( !((argc--==3)&&(sscanf(argv[2],"%s",Format)==1)) ) 
    strcpy(Format,"10I\0");
  if ( !((argc==2) && (sscanf(argv[1],"%d",&Type)==1)) ) Type = 1;

  printf("PubArea_FormatSlot: Set format to slot type %d as <%s>\n\0",Type,Format);

  PubArea PA;
  //............................................ Linking to the publishing area
  int status = PA.LinkPubArea();
  if (status!=PA_SUCCESS)  {
    printf("PubArea_Display: Failed to link pubarea\n");
    return 0;
  }
  status = PA.SetSlotTypeFormat(Type, Format);
  if (status!=PA_SUCCESS)  {
    printf("PubArea_FormatSlot: Failed to set the format\n\0");
  }
  return 0;
}

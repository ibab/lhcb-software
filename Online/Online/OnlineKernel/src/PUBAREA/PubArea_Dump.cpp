//
// PubArea_Dump.cxx ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include "AMS/amsdef.h"
#include "CPP/pubarea.h"

extern "C" int rtl_pubarea_dump(int argc,char** argv)   {
  char* me = "PubArea_Dump:\0";
  char node[32];
  if ((argc==4)&&(sscanf(argv[3],"%s",node)==1)) {argc--;} else {strcpy(node,"\0");}
  int slot;
  if ((argc==3)&&(sscanf(argv[2],"%d",&slot)==1)) {argc--;} else {slot=-1;}
  int type;
  if ((argc==2)&&(sscanf(argv[1],"%d",&type)==1)) {argc--;} else {type=-1;}

  printf("%s pubarea_dump %d %d %s\n\0",me,type,slot,node);

  //.... Only selection by type is possible for no slot number selection
  if (slot==-1)     {
    if (type==-1)
      printf("%s Dumping all slot types (-1)\n\0",me);
    else
      printf("%s Dumping all slots of type %d\n\0",me,type);

    printf("%s Dumping all slots (-1)\n\0",me);
  }
  else   {
    if (type!=-1) printf("%s Ignoring slot type\n\0");
    printf("%s Dumping slot number %d\n\0",me,slot);
  }

  if (strcmp(node,"\0")==0)   {
    printf("%s Dumping from this node\n\0",me);
  }
  else   {
    printf("%s Dumping from node %s\n\0",me,node);
    //....... Initialize AMS for network access to VMS nodes
    amsc_init(0);
  }

  PubArea PA;
  int status = PA.LinkPubArea(node);
  if (!(status&1))   {
    printf("PubArea_Dump: Failed to link pubarea\n");
    return(status);
  }
  void* SlotPtr = 0;

  status = PA_SUCCESS;

  if (slot!=-1)  {
    status = PA.DumpSlot(slot);
    if (status!=PA_SUCCESS)
      printf("%s Failed to dump the slot number %d\n\0",me,slot);
    return(0);
  }

  int context = -1;
  while (status==PA_SUCCESS)   {
    status = PA.GetSlotofType(type,&context,SlotPtr);
    if (status==PA_NOMORE) break;
    if (status!=PA_SUCCESS)   {
      printf("PubArea_Dump: Failing to retrieve slots\n\0");
      return(0);
    }
    status = PA.DumpSlot(SlotPtr);
    if (status!=PA_SUCCESS)    { 
      printf("PubArea_Dump: Failing to dump slot %d\n\0",SlotPtr);
    }
  }
  return(0);
}

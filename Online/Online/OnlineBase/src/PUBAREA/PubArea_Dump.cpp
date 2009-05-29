//
// PubArea_Dump.cxx ! Andreu Pacheco ! 6-8-96
//
#include <cstdio>
#include <cstring>
#include "CPP/PubArea.h"

extern "C" int rtl_pubarea_dump(int argc,char** argv)   {
  const char* me = "PubArea_Dump:";
  const char* name = "Unknown";
  char node[32];
  if ((argc==5)&&(sscanf(argv[4],"%s",node)==1)) {argc--;} else {memset(node,0,sizeof(node));}
  int slot;
  if ((argc==4)&&(sscanf(argv[3],"%d",&slot)==1)) {argc--;} else {slot=-1;}
  int type;
  if ((argc==3)&&(sscanf(argv[2],"%d",&type)==1)) {argc--;} else {type=-1;}
  if (argc==2) { name = argv[1]; argc--; }

  printf("%s pubarea_dump %d %d %s\n",me,type,slot,node);

  //.... Only selection by type is possible for no slot number selection
  if (slot==-1)     {
    if (type==-1)
      printf("%s Dumping all slot types (-1)\n",me);
    else
      printf("%s Dumping all slots of type %d\n",me,type);

    printf("%s Dumping all slots (-1)\n",me);
  }
  else   {
    if (type!=-1) printf("%s Ignoring slot type %d\n",me,slot);
    printf("%s Dumping slot number %d\n",me,slot);
  }
  printf("Dump publishing area: %s", name);
  if (strcmp(node,"")==0)   {
    printf(" from this node\n");
  }
  else   {
    printf(" from node %s\n",node);
  }

  PubArea PA(name);
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
      printf("%s Failed to dump the slot number %d\n",me,slot);
    return(0);
  }

  int context = -1;
  while (status==PA_SUCCESS)   {
    status = PA.GetSlotofType(type,&context,SlotPtr);
    if (status==PA_NOMORE) break;
    if (status!=PA_SUCCESS)   {
      printf("PubArea_Dump: Failing to retrieve slots\n");
      return(0);
    }
    status = PA.DumpSlot(SlotPtr);
    if (status!=PA_SUCCESS)    { 
      printf("PubArea_Dump: Failing to dump slot %p\n",SlotPtr);
    }
  }
  return(0);
}

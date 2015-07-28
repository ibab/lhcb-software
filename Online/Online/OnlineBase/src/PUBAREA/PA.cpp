/*
  A_PUBAREA$SRC:PUBAREA.CC
  Created           :  5-AUG-1996 by A.Pacheco
*/
#include <cstdio> 
#include <cctype>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>
#include <cstring>
//#include <process.h>
#include "RTL/rtl.h"
#include "AMS/amsdef.h"
#include "CPP/FormatCnv.h" //...................(A_CPP) Needed for format conversion

#include "PA.h" //............. The pubarea class and structure definitions

static char LocalProcess[64] = "\0";
#include "RTL/rtl.h"

PubArea::PubArea(const char* name)   {
  m_ptr = 0;    //... Pointer to the global section/data module
  m_header = 0; //... Pointer to the header of the pubarea header
  m_index  = 0; //... Pointer to the index table
  m_name   = name;
  for(size_t i=0; i<m_name.length(); ++i)  {
    m_name[i] = char(::toupper(m_name[i]));
  }
  std::string lck_name = m_name;
  lck_name += "_lock";
  int status = lib_rtl_create_lock(lck_name.c_str(), &m_lock);
  if ( !lib_rtl_is_success(status) )  {
    throw std::runtime_error("PubArea::PubArea: Failed to create lock:"+lck_name);
  }
  ::lib_rtl_get_node_name(m_node, 32);
  ::snprintf(LocalProcess,sizeof(LocalProcess),"PID.%06d",lib_rtl_pid());
  m_locked = 0;
  m_remote = 0;
  //................................................. Initializing data structures
  m_hdrFmt = 0;
  m_idxFmt  = 0;
  for (int i=0;i<PA_MAX_INDEX;i++) m_idxBuffer[i] = 0;
}

PubArea::~PubArea() {
  //.............................................. Delete data structure objects
  if (m_hdrFmt!=0) delete m_hdrFmt;
  if (m_idxFmt!=0) delete m_idxFmt;
  for (int i=0;i<PA_MAX_INDEX;i++)
    if (m_idxBuffer[i]!=0) delete m_idxBuffer[i];
  if (m_locked) UnLock(); //................... Unlock the Public Area
  if (m_remote) ::operator delete(m_remote);
}

int PubArea::ExitHandler(void* param)  {
  lib_rtl_gbl_t hdl = (lib_rtl_gbl_t)param;
  void* m_ptr = hdl->address;
  PubAreaHeader *head = (PubAreaHeader*)(m_ptr);
  PubAreaIndex  *ind  = (PubAreaIndex*)((char*)(m_ptr) + sizeof(PubAreaHeader));
  if ( head )  {
    for ( int i = 0; i < head->NumIndex; i++, ind++)  {
      if ( strcmp(LocalProcess, ind->SlotProcess) == 0 )
        ind->InUse = 0;
    }
  }
  return 1;
}

//******************************************************************************
//*** PubArea::AllocateSlot ****************************************************
//******************************************************************************
// Returns the pointer to slot of a given type and size.
// Input:  Type of the slot 0-300
//         Size of the slot in bytes
// Outpt:  Slot pointer (void*)
// Note:   The size of the slot will be octaword aligned (16 bytes)
//         To avoid problems with relocating structures in the returned buffer
//	   
int PubArea::AllocateSlot(int typ, int SSize, void* &slot)
{
  static const char* me="PubArea::AllocateSlot:";
  int ReqSize = SSize; 
  //................................... The data size has to be octaword aligned
  if ( (SSize%16) != 0 ) SSize += ( 16 - SSize%16 );

  int status = Lock();
  if (status!=PA_SUCCESS)  {
    PubAreaPrint(2,0,"%s Failed to get the lock",me);
    return status;
  }
  //................................................ Get number of index records
  int heap = 0; //................................Limit of the buffer index area
  PubAreaIndex  *ind = m_index;
  PubAreaHeader *head  = m_header;

  for ( int i = 0; i < head->NumIndex; i++, ind++)  {
    if ( ind->SlotType == typ && ind->InUse == 0 )  {
      ind->InUse = 1;
      goto Alloc;
    }
  }
  // Check if all available slots are used.
  if (head->NumIndex == PA_MAX_INDEX)  {  
    PubAreaPrint(3,0,"%s Need increase PA_MAX_INDEX = %d in PubArea.h",me,PA_MAX_INDEX);
    status = UnLock();
    if (status!=PA_SUCCESS)  {
      PubAreaPrint(2,0,"%s Failed to free the lock",me);
      return status;
    }
    slot = 0;
    return PA_FAILURE;
  }
  //... Check space  **********************
  heap = sizeof(PubAreaHeader) + head->NumIndex * sizeof(PubAreaIndex);    
  if (heap >= (head->PubAreaBufferOffset-SSize-4))   {
    PubAreaPrint(3,0,"%s Memory full. Needs increase of PA_SIZE_xxx_DEFAULT in pubarea.h",me);
    slot =0;
    return PA_FAILURE;
  }

  ind->InUse = 1;
  head->NumIndex++;
  ind->SlotOffset = (head->PubAreaBufferOffset -= SSize);
  ind->SlotType   = typ;
  ind->SlotSize   = ReqSize;
  PubAreaPrint(0,0,"%s New slot created with index %d!",me,head->NumIndex);
 Alloc:
  strncpy(ind->SlotProcess,LocalProcess,sizeof(ind->SlotProcess));
  ind->SlotProcess[sizeof(ind->SlotProcess)-1]=0;
  slot = (void*)((char*)(m_ptr) + ind->SlotOffset);
  return UnLock();
}

int PubArea::FreeSlot(void* SPtr) {
  static const char *me = "PubArea::FreeSlot:";
  if (SPtr==0) return PA_SUCCESS; //.... Null slot is dummy

  PubAreaHeader *head = m_header;
  if (!head->NumIndex)  {
    PubAreaPrint(1,0,"%s No entries in index!",me);
    return PA_FAILURE;
  }
  int Offset = (char*)(SPtr) - (char*)(m_ptr);
  PubAreaIndex *ind = m_index;
  for (int i=0; i<head->NumIndex; i++, ind++)  {
    if (Offset == ind->SlotOffset)    {
      ind->InUse = 0;
      return PA_SUCCESS;
    }
  } 
  PubAreaPrint(3,0,"%s Slot %d not found. Offset %d",me,SPtr,Offset);
  return PA_FAILURE;
}

int PubArea::PrepareSlot(void* /* SlotPtr */)   {
  return PA_SUCCESS;
}

int PubArea::LinkPubArea(char* Node)    {
  int ret = 0; int status = PA_SUCCESS;
#ifdef _WIN32
  if (Node) ret = ::stricmp(m_node,Node); // ret==0 nodes are equal
#else
  if (Node) ret = ::strcasecmp(m_node,Node); // ret==0 nodes are equal
#endif
  //.......................................................... Local node access
  if ((Node==0)||(ret==0)||(strcmp("\0",Node)==0))  {
    PubAreaPrint(0,0,"PubArea::LinkPubArea(node): Linking local pubarea");
    status = LinkPubArea(-1);
    return status;
  }

  //................................................... Allocate big work buffer
  if (m_remote==0) m_remote = new char[PA_SIZE_VMS_DEFAULT+64];
  //....................................................... Node is a VMS node ?
  if ((strncmp(Node,"AXA",3)==0)||(strncmp(Node,"axa",3)==0))  {
    status = GetPAfromVMS(Node);
    if (status==PA_SUCCESS) {
      m_ptr = m_remote;
      m_header = (PubAreaHeader *)(m_ptr);
      m_index  = (PubAreaIndex *)((char*)(m_ptr) + sizeof(PubAreaHeader));
      CheckStamp();
      return PA_SUCCESS;
    }
    else  {
      m_ptr    = 0;
      m_header = 0;
      m_index  = 0;
      PubAreaPrint(3,0,"PubArea::LinkPubArea: Failed to get pubarea from %s",Node);
      return PA_FAILURE;
    }
  }
  //.................................................... else they are OS9 nodes
  status = GetPAfromOS9(Node);
  if (status==PA_SUCCESS)  {
    m_ptr = m_remote;
    m_header = (PubAreaHeader *)(m_ptr);
    m_index  = (PubAreaIndex *)((char*)(m_ptr) + sizeof(PubAreaHeader));
    ConvertPubAreafromOS9();
    CheckStamp();
    //................................... Rewrite the node name just in case...
    strncpy(m_header->NodeName,Node,sizeof(m_header->NodeName));
    m_header->NodeName[sizeof(m_header->NodeName)-1] = 0;
    return PA_SUCCESS;
  }
  m_ptr    = 0;
  m_header = 0;
  m_index  = 0;
  PubAreaPrint(3,0,"PubArea::LinkPubArea: Failed to get pubarea from %s",Node);
  return PA_FAILURE;
}

int PubArea::LinkPubArea(int Size /* in bytes */)    {
  // If already linked. Do not retry for local node
  if ((m_ptr)&&(m_remote)) return PA_FAILURE;
  if (m_ptr) return PA_SUCCESS;
  if ( Size <= 0 )  {
#ifdef OSK
    Size = PA_SIZE_OSK_DEFAULT;
#else
    Size = PA_SIZE_VMS_DEFAULT;
#endif
  }
  lib_rtl_gbl_t hdl;
  int status = ::lib_rtl_map_section(m_name.c_str(),Size,&hdl);
  if ( !lib_rtl_is_success(status) )  {
    PubAreaPrint(3,status,"PubArea::LinkPubArea: Cannot map global section");
    m_ptr = 0;
    m_header = 0;
    return status;
  }
  m_ptr = hdl->address;
  m_header = (PubAreaHeader *)(m_ptr);
  m_index  = (PubAreaIndex *)((char*)(m_ptr) + sizeof(PubAreaHeader));
  ::lib_rtl_declare_exit(PubArea::ExitHandler,hdl);
  PubAreaPrint(0,0,"PubArea::LinkPubArea: m_ptr:       %d",(int*)m_ptr);
  PubAreaPrint(0,0,"PubArea::LinkPubArea: m_header: %d",(int*)m_header);
  PubAreaPrint(0,0,"PubArea::LinkPubArea: m_index:  %d",(int*)m_index);

  status = CheckStamp();
  if (!(status&1))   { 
    PubAreaPrint(3,0,"PubArea::LinkPubArea: Failed to check the stamp"); 
    return PA_FAILURE;
  }
  return PA_SUCCESS;
}

int PubArea::CleanPubArea()   {
  if (!m_header)  {
    PubAreaPrint(3,0,"PubArea::CleanPubArea: No PubArea exists!");
    return PA_FAILURE;
  }
  int Size=m_header->AreaSize;
  if (!Size)  {
    PubAreaPrint(3,0,"PubArea::CleanPubArea: PubArea size zero found on header!");
    return PA_FAILURE;
  }
  InitPubArea(Size);
  return PA_SUCCESS;
}

int PubArea::InitPubArea(int Size)  {
  if (!m_ptr)  {
    PubAreaPrint(3,0,"PubArea::InitPubArea: No PubArea pointer");
    return PA_FAILURE;
  }
  //...................................................... Zero the memory area 
  memset(m_ptr,0,Size);
  //............................................. Set the pointer to the header
  m_header = (PubAreaHeader *)(m_ptr);
  m_index  = (PubAreaIndex *)((char*)(m_ptr) + sizeof(PubAreaHeader));

  PubAreaPrint(0,0,"PubArea::InitPubArea: m_ptr:       %d",(int*)m_ptr);
  PubAreaPrint(0,0,"PubArea::InitPubArea: m_header: %d",(int*)m_header);
  PubAreaPrint(0,0,"PubArea::InitPubArea: m_index:  %d",(int*)m_index);
  PubAreaPrint(0,0,"PubArea::InitPubArea: Size Header: %d bytes",sizeof(PubAreaHeader));
  PubAreaPrint(0,0,"PubArea::InitPubArea: Size Index : %d bytes",sizeof(PubAreaIndex));

  int status = Lock();
  if (status!=PA_SUCCESS)  {
    PubAreaPrint(3,0,"PubArea::InitPubArea: Failed to lock the pubarea");
    return status;
  }

  PubAreaHeader *h = m_header;
  strncpy(h->PubAreaStamp,PA_STAMP,sizeof(h->PubAreaStamp));
  h->PubAreaStamp[sizeof(h->PubAreaStamp)-1]=0;
  h->MagicWord = 1234;
#ifdef _OSK
  strcpy(h->OpSys,"OSK\0");
#else
  strcpy(h->OpSys,"VMS\0");
#endif
  h->OpSys[sizeof(h->OpSys)-1]=0;
  ::strncpy(h->NodeName,m_node,sizeof(h->NodeName));
  h->NodeName[sizeof(h->NodeName)-1]=0;
  ::strncpy(h->Name,m_name.c_str(),sizeof(h->Name));
  h->Name[sizeof(h->Name)-1]=0;
  h->PubAreaBufferOffset = Size;
  h->AreaSize = Size; 
  h->NumIndex = 1;
  PubAreaIndex *i = m_index;
  i->SlotType = 0;
  i->SlotSize = 0;
  i->SlotOffset = 0;
  ::strncpy(i->SlotProcess,LocalProcess,sizeof(i->SlotProcess));
  i->SlotProcess[sizeof(i->SlotProcess)-1] = 0;
  i->InUse = 1;
  status = UnLock();
  if (status!=PA_SUCCESS)   {
    PubAreaPrint(3,0,"PubArea::InitPubArea: Failed to unlock the pubarea");
    return status;
  }
  return PA_SUCCESS;
}

int PubArea::CreatePubArea(int Size /* in bytes */)   {
  PubAreaPrint(0,0,"PubArea::CreatePubArea(Size=%d bytes): called",Size);

  if ( Size <= 0 )  {
#ifdef OSK
    Size = PA_SIZE_OSK_DEFAULT;
#else
    Size = PA_SIZE_VMS_DEFAULT;
#endif
  }
  lib_rtl_gbl_t hdl;
  int status = lib_rtl_create_section(m_name.c_str(),Size,&hdl);
  if ( !lib_rtl_is_success(status) )  {
    m_ptr = 0;
    m_header = 0;
    PubAreaPrint(3,status,"PubArea::CreatePubArea: Failed to create global section");
    return status;
  }
  m_ptr = hdl->address;
  // Header formatting 
  InitPubArea(Size);
  //PrintHeader();
  return PA_SUCCESS;
}

int PubArea::RefreshPubArea() {
  char ReqNode[64];
  if (m_header==0)   {
    PubAreaPrint(3,0,"PubArea::RefreshPubArea: PubArea does not exist");
    return  PA_FAILURE;
  }
  //................................. Get the node name from the PubArea itself 
  strncpy(ReqNode, m_header->NodeName, sizeof(ReqNode));
  ReqNode[sizeof(ReqNode)-1]=0;
  //...... Return if local node is requested (refresh is only needed for remote)
  if ( strcmp(ReqNode,m_node) == 0 ) return PA_SUCCESS;
  //...................................... Request a new copy of the public area
  int status = LinkPubArea(ReqNode);
  if (status!=PA_SUCCESS)  { 
    PubAreaPrint(3,0,"PubArea::RefreshPubArea: Could not refresh the pubarea");
    return PA_FAILURE;
  }
  return PA_SUCCESS;
}

int PubArea::ConversionNeeded()   {
  if (!m_header)  {
    PubAreaPrint(3,0,"PubArea::ConversionNeeded: No PubArea exists!");
  }
  int ret = strcmp(m_header->OpSys,"VMS");
#ifdef _OSK
  if (ret!=0) return(0);
#else
  if (ret==0) return(0);
#endif
  return(1);
}

int PubArea::CleanProcessArea() {
  PubAreaHeader *head = m_header;
  if (!m_index)   {
    PubAreaPrint(2,0,"PubArea::CleanProcessArea: No index exists!");
    return PA_FAILURE;
  }
  if (head->NumIndex==0) return PA_SUCCESS;
  PubAreaIndex  *ind = m_index;
  for (int ret=0, i=0; i<head->NumIndex; i++, ++ind )  {
    ret = strcmp(LocalProcess,ind->SlotProcess);
    if (ret==0)     {
      ind->InUse = 0;
      PubAreaPrint(0,0,"PubArea::CleanProcessArea: Cleaned index slot %d",i);
    }
  }
  return PA_SUCCESS;
}

void PubArea::PubAreaPrint(int Severity, int ReturnCode, const char *message,...) {
  int length = 0;
  va_list args;
  char msg[500];
  static int PaDebug = PADEBUG;
  static const char *Tmp = (char*)~0x0;
  const char *lvl[5] = {"_INFO__\0","WARNING\0","*ERROR*\0","*FATAL*\0","_______\0"};

  // Get the PADEBUG variable from the process environement variable
  if ( Tmp == (char*)~0x0 ) {
    Tmp = ::getenv("PADEBUG");
    char Logical[64];
    ::strncpy(Logical,Tmp != 0 ? Tmp : "4",sizeof(Logical));
    Logical[sizeof(Logical)-1]=0;
    ::sscanf(Logical,"%d",&PaDebug);
  }
  // if PADEBUG is zero all messages are printed
  if (Severity < PaDebug) return; 

  va_start(args, message);
  length = ::vsprintf(msg, message, args);
  va_end(args);
  if (Severity>4) Severity = 4; //............................. Maximum accepted
  if (length>499)   { 
    fprintf(stderr,"PubArea::PubAreaPrint: *Error* Input string too long, array overwritten\n");
    return;
  } 
  // Trailing carriage return and zero.
  *(msg + length) = '\n';
  *(msg + length + 1) = '\0';
  fprintf(stdout,"%s %s",lvl[Severity],msg);
  if (ReturnCode)   {
    fprintf(stdout,"%s Errno: 0x%08x Msg: %s\n",
            lvl[0],ReturnCode,lib_rtl_error_message(ReturnCode));
  }
  return;
}

int PubArea::DeletePubArea()  {
  static const char* me="PubArea::DeletePubArea:";
  PubAreaPrint(0,0,"%s called",me);
#if defined(__VMS)
  int status = sys$dgblsc (SEC$M_SYSGBL, m_name, SEC$K_MATALL );
  if (status&1)   {
    PubAreaPrint(0,0,"%s Deleted Global Section",me);
    status = PA_SUCCESS;
  }
  else  {
    PubAreaPrint(3,status,
                 "%s Cannot delete global section",me);
    status = PA_FAILURE;
  }
#elif defined(_OSK)
  int status = 0;
  while(status==0)  {
    PubAreaPrint(4,0,"%s unlinking PubArea...",me);
    status = system("unlink PubArea<>>>/nil");
  }
  status = PA_SUCCESS;
#else
  int status = 0;
#endif
  //........................................... Lets clean pointers just in case
  m_ptr = 0;
  m_header = 0;
  return status;
}

int PubArea::PrintIndex()   {
  if (!m_index)  {
    PubAreaPrint(2,0,"PubArea::PrintHeader: No index exists!");
    return PA_FAILURE;
  }
  int nindex =  m_header->NumIndex;
  if (nindex==0) return PA_SUCCESS;
  PubAreaPrint(4,0,"%4s %2s %8s %8s %12s %s","Slot","Tp","  Size  "," OffSet ",
               "  Process   ","Used");
  PubAreaPrint(4,0,"%4s %2s %8s %8s %12s %s","----","--","--------","--------",
               "------------","----");
  int i = 0;
  PubAreaIndex *index[PA_MAX_INDEX], *indexinc = m_index;
  for (i=0;i<PA_MAX_INDEX;i++)  {  
    index[i]=indexinc; 
    indexinc++;
  }
  for (i=0;i<nindex;i++)  {
    PubAreaPrint(4,0,
                 "%4d %2d %8d %8d %12s %1d",
                 i,index[i]->SlotType,index[i]->SlotSize,index[i]->SlotOffset,
                 index[i]->SlotProcess, index[i]->InUse);
  }
  return PA_SUCCESS;
}

int PubArea::PrintHeader()    {  
  if (!m_header) {
    PubAreaPrint(2,0,"PubArea::PrintHeader: No header exists!");
    return PA_FAILURE;
  }
  PubAreaHeader *tmpheader = m_header;
  PubAreaPrint(4,0,"Name                    : %s",tmpheader->Name);
  PubAreaPrint(4,0,"Stamp                   : %s",tmpheader->PubAreaStamp);
  PubAreaPrint(4,0,"MagicWord               : %4d",tmpheader->MagicWord);
  PubAreaPrint(4,0,"OpSys                   : %s",tmpheader->OpSys);
  PubAreaPrint(4,0,"Node Name               : %s",tmpheader->NodeName);
  PubAreaPrint(4,0,"BufferOffset            : %d bytes",tmpheader->PubAreaBufferOffset);
  PubAreaPrint(4,0,"Area Size               : %d bytes",tmpheader->AreaSize);
  PubAreaPrint(4,0,"Number of index entries : %d",tmpheader->NumIndex);
  PubAreaPrint(4,0,"Locking process         : %s",tmpheader->LockPrcName);
  return  PA_SUCCESS;
}

int PubArea::CheckStamp() {
  if (m_header==0)  {
    PubAreaPrint(3,0,"PubArea::CheckStamp: Header does not exist");
    return PA_FAILURE;
  }
  int res = strcmp(m_header->PubAreaStamp,PA_STAMP);
  if (res!=0)   { 
    PubAreaPrint(3,0,"PubArea::CheckStamp: Stamp does not match");
    PubAreaPrint(0,0,"In public area found %s",m_header->PubAreaStamp);
    PubAreaPrint(0,0,"Current version is   %s",PA_STAMP);
    return PA_FAILURE;
  }
  return PA_SUCCESS;
}

int PubArea::Lock() {
  ::lib_rtl_lock(m_lock);
  m_locked = 1;
  strncpy(m_header->LockPrcName,LocalProcess,sizeof(m_header->LockPrcName));
  m_header->LockPrcName[sizeof(m_header->LockPrcName)-1]=0;
  return PA_SUCCESS;
}

int PubArea::UnLock()   {
  m_locked = 0;
  m_header->LockPrcName[0] = 0;
  ::lib_rtl_unlock(m_lock);
  return PA_SUCCESS;
}

//*** PubArea::GetSlotofType ***
int PubArea::GetSlotofType(int Type, int* context, void* &slot)  {
  if ( ++(*context) < m_header->NumIndex )  {
    PubAreaIndex  *ind  = m_index;
    for ( int i = 0; i < m_header->NumIndex; i++, ind++)     {
      if ( ind->InUse && ((ind->SlotType==Type) || (Type==-1)) )     {
        if ( i >= *context )  {
          slot = (void*)((char*)(m_ptr) + ind->SlotOffset);
          *context = i;
          return PA_SUCCESS;
        }
      }
    }
  }
  *context = -1;
  slot = 0;
  return(PA_NOMORE);
}

//*** PubArea::GetSlotType ***
int PubArea::GetSlotType(void* slot)    {
  if (slot == 0) return (-1);
  int Offset = (char*)(slot) - (char*)(m_ptr);
  if (!m_header->NumIndex)  {
    PubAreaPrint(1,0,"PubArea::GetSlotType: No entries in index!");
    return(-1);
  }
  PubAreaIndex *ind = m_index;
  for (int i=0; i<m_header->NumIndex;i++, ind++)  {
    if (Offset==ind->SlotOffset)    {
      return ind->SlotType;
    }      
  }
  PubAreaPrint(3,0,"PubArea::GetSlotType: Slot: %d not found. Offset: %d",(int*)slot,Offset);
  return(PA_NOMORE);
}

//*** PubArea::GetPAfromVMS ***
int PubArea::GetPAfromVMS(const char *Node)   {
  char source[64] = "";
  size_t buffersize = PA_SIZE_VMS_DEFAULT;
  strcpy(source,Node);
  strcat(source,"::PAGBLSERVER_0\0");
  int status = amsc_send_message("DUMP",4,source,0,0);
  if ( status != AMS_SUCCESS )  { 
    PubAreaPrint(3,status,"PubArea::GetPAfromVMS: Error sending request "
                 "to VMS node %s. Is the server running ?",Node);
    return PA_FAILURE;
  }
  status = amsc_get_message(m_remote,&buffersize,source,0,100,0,0,0);
  if ( status != AMS_SUCCESS )  { 
    PubAreaPrint(3,status,"PubArea::GetPAfromVMS: Error receiving data from %s",Node);
    return PA_FAILURE;
  }
  PubAreaPrint(0,0,"PubArea::GetPAfromVMS: Received %d bytes from %s",buffersize,Node);
  if (buffersize==0) return  PA_FAILURE;
  return PA_SUCCESS;
}

//*** PubArea::GetPAfromOS9 ***
int PubArea::GetPAfromOS9(const char * /*node*/) {
#if 0
  struct dsc$descriptor node_desc;
  node_desc.dsc$a_pointer = node;
  node_desc.dsc$w_length  = strlen(node);

  int status     = 0;
  int buffersize = PA_SIZE_OSK_DEFAULT;
  $DESCRIPTOR(name_desc,"PubArea");

  static char* tmpbuffer = 0;

  //.... if (tmpbuffer==0)  tmpbuffer = new char[PA_SIZE_OSK_DEFAULT];

  tmpbuffer = (char *) m_remote;

  status = os9_read_data_module(&node_desc,
                                (dsc$descriptor*) &name_desc,
                                tmpbuffer,
                                &buffersize);

  if (status==OS9_OVERRUN) return PA_FAILURE;
  if (status==OS9_COMMSFAIL) return PA_FAILURE;
  if (!(status&1))
  {  PubAreaPrint(3,status,
                  "PubArea::GetPAfromVMS: Fic data module scan error");
    return PA_FAILURE;
  }

  // memcpy(m_remote,tmpbuffer,PA_SIZE_OSK_DEFAULT);
#endif
  return PA_SUCCESS;
}

//*** ConvertPubAreafromOS9 ***
int PubArea::ConvertPubAreafromOS9()    {
  //... Create the data structures the first time
  if (m_hdrFmt==0) m_hdrFmt  = new DataStructure("5I,73C");
  if (m_idxFmt==0)  m_idxFmt   = new DataStructure("4I,20C");

  DataStructure::ConvertfromHost(m_header,m_header,sizeof(PubAreaHeader),*m_hdrFmt,DATAFORMAT_OS9);
  int nindex = m_header->NumIndex;
  PubAreaIndex *indexinc = m_index;
  for (int i=0;i<nindex; i++, indexinc++)  {
    DataStructure::ConvertfromHost(indexinc,indexinc,sizeof(PubAreaIndex),*m_idxFmt,DATAFORMAT_OS9);      
    //     indexinc++; //................. Increment index pointer to next index entry
    //
    int SlotType   = indexinc->SlotType; 
    if ((SlotType>=0)&&(SlotType<PA_MAX_INDEX))    {
      if (m_idxBuffer[SlotType]!=0)   {
        int SlotOffset = indexinc->SlotOffset;
        int SlotSize   = indexinc->SlotSize;
        void* Buffer = (void*)((char*)m_ptr + SlotOffset);
        DataStructure::ConvertfromHost(Buffer, 
                                       Buffer, 
                                       SlotSize, 
                                       *(m_idxBuffer[SlotType]), 
                                       DATAFORMAT_OS9);
      }
    }
    //
  }
  return PA_SUCCESS;
}

//*** PubArea::SetSlotTypeFormat ***
int PubArea::SetSlotTypeFormat(int SlotType, char* Format)   {
  static const char* me="PubArea::SetSlotTypeFormat:";
  if ((SlotType<0)||(SlotType>=PA_MAX_INDEX))  {
    PubAreaPrint(3,0,"%s SlotType %d out of range 0-%d",me,SlotType,PA_MAX_INDEX-1);
    return PA_FAILURE;
  }
  if ((Format==0)||(strcmp(Format,"\0")==0))  {
    PubAreaPrint(1,0,"%s Empty format for slot type %d",me,SlotType);
    return PA_FAILURE;
  }
  m_idxBuffer[SlotType]  = new DataStructure(Format);
  return  PA_SUCCESS;
}

//*** PubArea::DumpSlot(void*) ***
int PubArea::DumpSlot(void* slot)   {
  static const char* me="PubArea::DumpSlot:";
  int i = 0;
  //........................................... The input was not a slot pointer
  if (slot == 0)  {
    PubAreaPrint(1,0,"%s Trying to dump a void slot !",me);
    return  PA_FAILURE;
  }

  //............................... Get the offset from the start of the PubArea
  int Offset = (char*)(slot) - (char*)(m_ptr);
  //...................................... Check that the PubArea has some entry
  int nindex =  m_header->NumIndex;
  if (!nindex)  {
    PubAreaPrint(1,0,"PubArea::DumpSlot: No entries in index!");
    return PA_FAILURE;
  }
  PubAreaIndex *index[PA_MAX_INDEX], *indexinc = m_index;
  for (i=0;i<nindex;i++) {index[i]=indexinc; indexinc++;}
  int SlotOffset = 0, size = 0, type = 0;
  for (i=0;i<nindex;i++)  {
    SlotOffset = index[i]->SlotOffset;
    if (Offset==SlotOffset)  {
      size = index[i]->SlotSize;
      type = index[i]->SlotType;

      unsigned char *tmp  = new unsigned char[size+2];
      unsigned char *tmp2 = new unsigned char[size+2];
      memcpy(tmp,(unsigned char*)slot,size);
      memcpy(tmp2,(unsigned char*)slot,size);
      PubAreaPrint(4,0,"\n Dumping slot %d Size %d bytes Type %d Offset %d Addr %d",
                   i,size,type,SlotOffset,(int*)slot);

      if (size==0) return PA_SUCCESS; //.......................... No slot size

      int k = 0, j=0, l=0;
      lib_rtl_output(LIB_RTL_ALWAYS," *** Ascii Dump *** codes 0-31 are dots (.) 127-255 are wildcards (*)\n");
      for (j=0;j<(size);j++)
      {
        if (*tmp<32) lib_rtl_output(LIB_RTL_ALWAYS,".");
        if ((*tmp>31)&&(*tmp<127)) lib_rtl_output(LIB_RTL_ALWAYS,"%c",*tmp);
        if (*tmp>126) lib_rtl_output(LIB_RTL_ALWAYS,"*");
        k++;tmp++;
        if (k>61) {lib_rtl_output(LIB_RTL_ALWAYS,"\n");k=0;}
      }
      k=0;l=0;
      lib_rtl_output(LIB_RTL_ALWAYS,"\n *** Hexadecimal Dump ***\n");
      for (j=0;j<(size);j++)  {
        lib_rtl_output(LIB_RTL_ALWAYS,"%02x",*tmp2);
        k++; l++; tmp2++;
        if (l>3) {lib_rtl_output(LIB_RTL_ALWAYS," ");l=0;} //.... Prints spaces 
        if (k>31) {lib_rtl_output(LIB_RTL_ALWAYS,"\n");k=0;}
      }
      lib_rtl_output(LIB_RTL_ALWAYS,"\n");
      return PA_SUCCESS;
    }      
  }
  PubAreaPrint(3,0,"PubArea::DumpSlot: Slot: %d not found. Offset: %d",
               (int*)slot,Offset);
  return(PA_NOMORE);
}

//******************************
//*** PubArea::DumpSlot(int) ***
//******************************
int PubArea::DumpSlot(int slot)  {
  static const char* me="PubArea::DumpSlot(int):";
  int i = 0;
  //........................................... The input was not a slot pointer
  if (slot < 0)  {
    PubAreaPrint(1,0,"%s Trying to dump a negative slot number (%d)!",me,slot);
    return  PA_FAILURE;
  }
  //...................................... Check that the PubArea has some entry
  int nindex =  m_header->NumIndex;
  if (!nindex)  {
    PubAreaPrint(1,0,"%s No entries in index!",me);
    return PA_FAILURE;
  }
  if (slot>=nindex)  {
    PubAreaPrint(1,0,
                 "%s Slot number (%d) bigger than number of slots (%d)!",me,slot,nindex);
    return PA_FAILURE;
  }
  PubAreaIndex *index[PA_MAX_INDEX], *indexinc = m_index;
  for (i=0;i<nindex;i++) {index[i]=indexinc; indexinc++;}

  void *slotptr = (void*) ((char*)m_header +index[slot]->SlotOffset);
  int size = index[slot]->SlotSize;
  int type = index[slot]->SlotType;

  unsigned char *tmp  = new unsigned char[size+2];
  unsigned char *tmp2 = new unsigned char[size+2];
  memcpy(tmp,(unsigned char*)slotptr,size);
  memcpy(tmp2,(unsigned char*)slotptr,size);
  PubAreaPrint(4,0,"\n Slot number %d Size %d bytes Type %d Offset %d Addr %d",
               i,size,type,index[slot]->SlotOffset,(int*)slotptr);

  if (size==0) return PA_SUCCESS; //.......................... No slot size

  int k = 0, j=0, l=0;
  lib_rtl_output(LIB_RTL_ALWAYS," *** Ascii Dump *** codes 0-31 are dots (.) 127-255 are wildcards (*)\n");
  for (j=0;j<(size);j++)  {
    if (*tmp<32) lib_rtl_output(LIB_RTL_ALWAYS,".");
    if ((*tmp>31)&&(*tmp<127)) lib_rtl_output(LIB_RTL_ALWAYS,"%c",*tmp);
    if (*tmp>126) lib_rtl_output(LIB_RTL_ALWAYS,"*");
    k++;tmp++;
    if (k>63) {lib_rtl_output(LIB_RTL_ALWAYS,"\n");k=0;}
  }
  k=0;l=0;
  lib_rtl_output(LIB_RTL_ALWAYS,"\n *** Hexadecimal Dump ***\n");
  for (j=0;(j<size);j++)  {
    lib_rtl_output(LIB_RTL_ALWAYS,"%02x",*tmp2);
    k++; l++; tmp2++;
    if (l>3) {lib_rtl_output(LIB_RTL_ALWAYS," ");l=0;} //.... Prints spaces 
    if (k>31) {lib_rtl_output(LIB_RTL_ALWAYS,"\n");k=0;}
  }
  lib_rtl_output(LIB_RTL_ALWAYS,"\n");
  return PA_SUCCESS;
}      

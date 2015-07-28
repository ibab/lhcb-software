/*
  A_PUBAREA$SRC:PUBAREA.H;
  Created           :  5-AUG-1996 by A.Pacheco

  This is the file that defines the structures and classes used by the 
  Publishing Area package (PubArea). Mainly there are three structures:

  PubAreaHeader: Structure of the top header information of the publising area
  PubAreaIndex : Structure of the index record for each slot 
  PubArea      : Class handling the publishing area.

*/

#ifndef _PA_H
#define _PA_H

#define PADEBUG 3  //............. Debug value for having output to the terminal

// Defaults are in bytes
#define PA_SIZE_VMS_DEFAULT 409600 //....... Default size for VMS Global Section
#define PA_SIZE_OSK_DEFAULT 20480  //....... Default size for OS9 Global Section

#define PA_STAMP "PA09\0" //.......... Some sort of label which changes when the
                          //....... structure of the publising area changes also
#ifdef _OSK
#define PA_MAX_INDEX 300  //..................... Maximum number of slot entries
#else
#define PA_MAX_INDEX 1000	// 300 is too less! M.Frank
#endif

#include <string>           //............ Needed for string descriptor handling
#include "CPP/FormatCnv.h"  //... Needed for DataStructure conversion OS9 to VMS
#include "CPP/passdef.h"    //.......... File containing return code definitions
#include "RTL/rtl.h"

// *********************
// *** PubAreaHeader ***
// *********************
// Structure of the header in the beginning of the public area
struct PubAreaHeader    {
  int  MagicWord; //........ Magic word to determine if byteswapping is needed
  int  PubAreaBufferOffset;//....................... Offset to the buffer area
  int  AreaSize; //................................. Size of the area in bytes
  int  NumIndex; //..................................... Number of index slots
  char LockPrcName[64]; //.................................... Locking Process
  char PubAreaStamp[5]; //................. Identifier of the Public Area 'PA'
  char OpSys[11]; //.............................. Operating system: VMS or OS9
  char NodeName[64]; //................ Node name where the module was created
  char Name[64];     //............................Name of the publishing area
};

// ********************
// *** PubAreaIndex ***
// ********************
// Structure of the index record of a slot in the publising area. They are
// dynamically accumulated after the header file. There is a maximum of 
// 300 slots which can be increased with no harm by changing PA_MAX_INDEX

struct PubAreaIndex   {
  int SlotType; //...................................  Type of slot to be filled
  int SlotSize; //................................  Size of slot to be allocated
  int SlotOffset; //................................ Pointer to slot buffer area
  int InUse; //.............. Cleared when the slot is empty and ready for reuse
  char SlotProcess[64]; //.................. Name of process allocating the slot
};

// ***************
// *** PubArea ***
// ***************
// Base class of the Publising Area. Creates, deletes and maintains information
// stored in data modules (OS9) or global sections (OpenVMS). 

class PubArea   {
protected:
  lib_rtl_lock_t m_lock;
  void *m_ptr; //.................... Pointer to the start of the publising area
  void *m_remote; //............. Buffer used to retrieve remote publising areas
  std::string m_name; //. Global section/Data module Name of the Publishing Area
  PubAreaHeader *m_header;         //.......Pointer to the header of the PubArea
  PubAreaIndex  *m_index;         //........... Pointer to the first index entry
  int m_locked; //.................. Set when the process has locked the pubarea
  char m_node[64];    //................................. Name of the local node
  DataStructure* m_hdrFmt; //.......... Format of the header used for conversion
  DataStructure* m_idxFmt; //..... Format of the index entry used for conversion
  DataStructure* m_idxBuffer[PA_MAX_INDEX]; //...... Formats of the buffer areas
public:
  explicit PubArea(const char* name);   //.......................... Constructor
  ~PubArea(); //..................................................... Destructor
  int AllocateSlot(int SlotType,int SlotSize, void* &Slot);//....Allocates space
  int FreeSlot(void* SlotPtr); //............................. Deallocates space
  int PrepareSlot(void* SlotPtr); //..................... Initializes slot space
  int LinkPubArea(char* Node); //.............. Gets the first slot in hierarchy
  int LinkPubArea(int Size /* in bytes */); //. Gets the first slot in hierarchy
  int RefreshPubArea();//.............. Refresh the contents of a remote PubArea
  //.............................. Gets by context all the slots of a given type
  int GetSlotofType(int SlotType, int* Context, void* &Slot); //..............
  int ConversionNeeded(); //...... Returns 1 if byte swap is needed for integers
  int CleanProcessArea(); //............. Clean all slots of the current process
  int CreatePubArea(int Size = (-1)/* bytes */); //........... Creates a PubArea
  int DeletePubArea(); //..................................... Deletes a PubArea
  int CleanPubArea(); //..................................... Cleans the pubarea
  int InitPubArea(int Size = (-1)/* bytes */); //........ Initializes the PubArea
  //................. Prints error messages on the screen for debugging purposes
  void PubAreaPrint(int Severity, int ReturnCode, const char *Message,...);
  int PrintHeader(); //....................... Prints the contents of the header
  int PrintIndex(); //......................... Prints the contents of the index
  void* GetPubAreaPtr() {return this->m_ptr;}; //. Returns the base pointer
  int CheckStamp(); //.................... Checks the version of the Public Area
  int Lock(); //........................... Locks the global section/data module
  int UnLock(); //....................... Unlocks the global section/data module
  int GetSlotType(void* SlotPtr);//..................... Gets the type of a slot
  //............................................................ Debugging tools
  int DumpSlot(void* SlotPtr);//..................... Dumps a given slot pointer
  int DumpSlot(int SlotNum);//........................ Dumps a given slot number
  //.................................................. Memory corruption tracers
  int GetPAfromVMS(const char* Node);//...... Requests global section via server
  int GetPAfromOS9(const char* Node);//......... Requests data module via server
  int ConvertPubAreafromOS9();//................................ Data conversion
  //.................................... Data conversion for buffer by slot type
  int SetSlotTypeFormat(int SlotType, char* Format);
  static int ExitHandler(void* param);
};

#endif

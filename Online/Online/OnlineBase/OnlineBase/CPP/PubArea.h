#ifndef _PUBAREA_H
#define _PUBAREA_H
#include "CPP/passdef.h"
#include <cstdlib>
// ***************
// *** PubArea ***
// ***************
// Base class of the Publising Area. Creates, deletes and maintains information
// stored in data modules (OS9) or global sections (OpenVMS). 
class _PubArea      {
public:
#ifdef _pubarea_C
  typedef PubArea PaObject;
#else
#define PubArea _PubArea
  typedef void PaObject;
#endif
protected:  
  /// Pointer to the actual PubArea object  
  PaObject* obj;
private:
  _PubArea(const _PubArea& c) : obj(c.obj) {}  //.............. Copy Constructor
  _PubArea& operator=(const _PubArea&) { return *this; }  //.... Assignment oper
public:  
  explicit _PubArea(const char* name);  //.......................... Constructor
  ~_PubArea(); //.................................................... Destructor
  int AllocateSlot(int SlotType,int SlotSize, void* &Slot);//....Allocates space
  int FreeSlot(void* SlotPtr); //............................. Deallocates space
  int PrepareSlot(void* SlotPtr); //..................... Initializes slot space
  int LinkPubArea(const char* Node); //........ Gets the first slot in hierarchy
  int LinkPubArea(size_t size); //............. Gets the first slot in hierarchy
  int RefreshPubArea();//.............. Refresh the contents of a remote PubArea
  //.............................. Gets by context all the slots of a given type
  int GetSlotofType(int SlotType, int* Context, void* &Slot); //................
  int CleanProcessArea(); //............. Clean all slots of the current process
  int CreatePubArea(int Size = (-1)/* bytes */); //........... Creates a PubArea
  int DeletePubArea(); //..................................... Deletes a PubArea
  int CleanPubArea(); //..................................... Cleans the pubarea 
  int InitPubArea(int Size = (-1) /* bytes */); //...... Initializes the PubArea
  int PrintHeader(); //....................... Prints the contents of the header
  int PrintIndex(); //......................... Prints the contents of the index
  int GetSlotType(void* SlotPtr);//..................... Gets the type of a slot  
  //............................................................ Debugging tools 
  int DumpSlot(void* SlotPtr);//..................... Dumps a given slot pointer
  int DumpSlot(int SlotNum);//........................ Dumps a given slot number  
  int Lock();  
  int UnLock();
  //.................................... Data conversion for buffer by slot type  
  int SetSlotTypeFormat(int SlotType, const char* Format);
};
#endif  // _PUBAREA_H

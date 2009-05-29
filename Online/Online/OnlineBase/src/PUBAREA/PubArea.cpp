#define _pubarea_C
#include "PA.h"
#include "CPP/PubArea.h"

_PubArea::_PubArea(const char* name )	{
  obj = new _PubArea::PaObject(name);
}

_PubArea::~_PubArea()	{
  delete obj;
}

int _PubArea::AllocateSlot(int SlotType,int SlotSize, void* &Slot)  { 
  return obj->AllocateSlot(SlotType, SlotSize, Slot);
}

int _PubArea::FreeSlot(void* SlotPtr)	{
  return obj->FreeSlot(SlotPtr);
}

int _PubArea::PrepareSlot(void* SlotPtr)    {
  return obj->PrepareSlot(SlotPtr);
}

int _PubArea::LinkPubArea(const char* Node)	{
  return obj->LinkPubArea((char*)Node);
}

int _PubArea::LinkPubArea(size_t siz)   {
  return obj->LinkPubArea(siz);
}

int _PubArea::RefreshPubArea()	  {
  return obj->RefreshPubArea();
}

int _PubArea::GetSlotofType(int SlotType, int* Context, void* &Slot)	{
  return obj->GetSlotofType(SlotType, Context, Slot);
}

int _PubArea::CleanProcessArea()    {
  return obj->CleanProcessArea();
}

int _PubArea::CreatePubArea(int Size /* bytes */)   {
  return obj->CreatePubArea(Size);
}

int _PubArea::DeletePubArea()	{
  return obj->DeletePubArea();
}

int _PubArea::CleanPubArea()	{
  return obj->CleanPubArea();
}

int _PubArea::InitPubArea(int Size /* bytes */)	  {
  return obj->InitPubArea(Size);
}

int _PubArea::PrintHeader()   {
  return obj->PrintHeader();
}

int _PubArea::PrintIndex()    {
  return obj->PrintIndex();
}

int _PubArea::GetSlotType(void* SlotPtr)    {
  return obj->GetSlotType(SlotPtr);
}

int _PubArea::DumpSlot(void* SlotPtr)	{
  return obj->DumpSlot(SlotPtr);
}

int _PubArea::DumpSlot(int SlotNum)   {
  return obj->DumpSlot(SlotNum);
}

int _PubArea::Lock()   {
  return obj->Lock();
}

int _PubArea::UnLock()   {
  return obj->UnLock();
}

int _PubArea::SetSlotTypeFormat(int SlotType, const char* Format)   {
  return obj->SetSlotTypeFormat(SlotType, (char*)Format);
}

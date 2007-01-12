#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartRef.h"

StatusCode makeMCTruthLink(const DataObject* from, const DataObject* to);
void* accessMCTruth( void* (*cast)(DataObject*), const DataObject* from);
void* accessMCTruth( void* (*cast)(ContainedObject*), const ContainedObject* from);

StatusCode makeMCTruthLink(const DataObject* from, const DataObject* to) {
  if ( from && to )  {
    IRegistry*   treg = to->registry();
    LinkManager* lmgr = from->linkMgr();
    if ( treg && lmgr )  {
      if ( lmgr->addLink(treg->identifier(), to) >= 0 )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}
void* accessMCTruth( void* (*cast)(DataObject*), const DataObject* from)
{
  void* result = 0;
  if ( 0 != from )  {
    LinkManager* mgr = from->linkMgr();
    if ( mgr )  {
      for ( int i = 0, n = mgr->size(); i < n; i++ )  {
        if ( (result=cast(SmartRef<DataObject>(from, i))) ) {
          return result;
        }
      }
    }
  }
  return 0;
}

void* accessMCTruth( void* (*cast)(ContainedObject*), const ContainedObject* from)  {
  void* result = 0;
  if ( 0 != from )  {
    long  idx = from->index();
    if ( idx != StreamBuffer::INVALID )  {
      const DataObject* parent = from->parent();
      if ( parent )   {
        LinkManager* mgr = parent->linkMgr();
        if ( mgr )  {
          for ( int i = 0, n = mgr->size(); i < n; i++ )  {
            if ( (result=cast(SmartRef<ContainedObject>(from, i, idx))) ) {
              return result;
            }
          }
        }
      }
    }
  }
  return 0;
}

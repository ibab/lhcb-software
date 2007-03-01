// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpIdentList.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
#include "PVSS/Internals.h"

// PVSS include files
#include "DpIdentList.hxx"

void PVSS::pvss_list_create(void*& context)    {
  context = new DpIdentList;
}

bool PVSS::pvss_list_drop(void*& context)    {
  if ( context )  {
    DpIdentList* list = (DpIdentList*)context;
    delete list;
    context = 0;
    return true;
  }
  return false;
}

bool PVSS::pvss_list_add(void* ctxt,const DpIdentifier& dpid)    {
  if ( ctxt )  {
    DpIdentList*  list = (DpIdentList*)ctxt;
    list->append(dpid);
    return true;
  }
  return false;
}

bool PVSS::pvss_list_add_wild(void* ctxt,const char* wildname,DpIdentifier*& array,long& cnt,int typ)  {
  DpIdentList*  list = (DpIdentList*)ctxt;
  if ( ctxt )  {
    if ( pvss_lookup_dpidset(wildname,array,cnt,typ) )  {
      for(long i=0; i<cnt; ++i)  {
        list->append(array[i]);
      }
      return true;
    }
  }
  return false;
}

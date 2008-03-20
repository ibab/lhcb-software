// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpHelpers.cpp,v 1.8 2008-03-20 20:50:44 frankb Exp $
//  ====================================================================
//  DpHelpers.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpHelpers.cpp,v 1.8 2008-03-20 20:50:44 frankb Exp $

// PVSS include files
#include "Manager.hxx"
#include "DpElement.hxx"
#include "MapTableItem.hxx"
#include "ConfigsMapper.hxx"

#include "PVSS/DpID.h"
#include "PVSS/Lock.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Internals.h"
#include "PVSS/Environment.h"
#include "PVSSManager/SyncWaitForAnswer.h"

#include <string>
#include <stdexcept>

static int s_debug = 0;
static SystemNumType s_system = 0;
static PVSS::DevTypeManager* s_devTypeMgr = 0;
static PVSS::DevType* (*s_addDevType)(PVSS::DevTypeManager* m,int id, const char* n) = 0;
static void (*s_addDevTypeElem)(PVSS::DevType* m,int id, int typ, const char* n) = 0;

static void visitDpElement(PVSS::DevTypeManager* m, PVSS::DevType* t, const DpType& p, const DpTypeNode* n, const std::string& par)  {
  if ( n )  {
    // char txt[512];
    DpIdentification *dpIdent = Manager::getDpIdentificationPtr();
    char* elname = "Unknown";
    DpElementId eid = n->getId();
    DpElement elt(eid);
    DpElementType elTyp = elt.getType(&p);
    dpIdent->getElementName(p.getName(), eid, elname,s_system);
    //DpElementType eTyp  = n->getElementType();
    //sprintf(txt,"  %d got elem: par:[%s] elname:\"%s\" pid:%d eid:%d typ=%d %d",
    //	    (int)s_system,par.c_str(),elname ? elname : "None",p.getName(),eid, elTyp,eTyp);
    //pvss_print(PVSS::PRIO_SEVERE,36,txt);
    std::string nam;
    if ( !par.empty() ) nam = par + ".";
    nam += elname;
    size_t nson = n->getSonNumber();
    if ( nson == 0 )  {
      s_addDevTypeElem(t,eid,elTyp,nam.c_str());
    }
    else  {
      for(size_t is = 0; is<nson; ++is )    {
	visitDpElement(m,t,p,p.getTypeNodePtr(n->getSon(is)),nam);
      }
    }
  }
}

static int visitDpType(const DpType& p)  {
  DpIdentification *dpIdent = Manager::getDpIdentificationPtr();
  const DpTypeNode* n = p.getRootTypeNodePtr();
  char* name = "Unknown";
  dpIdent->getTypeName(p.getName(),name,s_system);
  PVSS::DevType* t = s_addDevType(s_devTypeMgr,p.getName(),name);
  delete [] name;
  visitDpElement(s_devTypeMgr,t,p,n,"");
  return 1;
}

/// Set debug flag
void PVSS::pvss_set_debug(int value)  {
  s_debug = value;
}

/// Access debug flag
int PVSS::pvss_debug()  {
  return s_debug;
}

int PVSS::pvss_load_configurations(CfgManager* m, int id, 
    void (*addCfg)(CfgManager* m,int id, const char* n),
    void (*addAttr)(CfgManager* m,int id, const char* n),
    void (* /* addDetail */)(CfgManager* m,int id, const char* n))
{
  size_t cfgNum = 0;
  s_system = id;
  
  const DpConfigNrType *configs = ConfigsMapper::getConfigIds(cfgNum);
  for(size_t i=0; i<cfgNum; ++i)  {
    size_t attrNum = 0;
    DpConfigNrType cfg = configs[i];
    const char* n = ConfigsMapper::getConfigName(cfg);
    const DpAttributeNrType *attrs = ConfigsMapper::getAttributeIds(cfg,attrNum);
    addCfg(m,cfg,n);
    if ( pvss_debug() ) ::printf("PVSS> Adding Config [%d]:%s\n",cfg,n);
    for(size_t j=0; j<attrNum; ++j)  {
      const DpAttributeNrType att = attrs[j];
      n = ConfigsMapper::getAttributeName(cfg,att);
      if ( pvss_debug() ) ::printf("PVSS>         Adding attribute [%ld]:%s\n",att,n);
      addAttr(m, att, n);
    }
    // Forget about details....only very few configs have named details.
  }
  return 1;
}

int PVSS::pvss_load_device_types(PVSS::DevTypeManager* mgr, int id,
  PVSS::DevType* (*addDev)(PVSS::DevTypeManager* m,int id, const char* n),
  void (*addElem)(PVSS::DevType* m,int id, int typ, const char* n))
{
  s_system = id;
  s_devTypeMgr = mgr;
  s_addDevType = addDev;
  s_addDevTypeElem = addElem;
  DpTypeContainer* cont = 0;
  if ( DpIdentification::getDefaultSystem() == s_system )  {
    if ( pvss_debug() ) ::printf("PVSS> Using default system....\n");
    cont = Manager::getTypeContainerPtr();
  }
  else {
    if ( pvss_debug() ) ::printf("PVSS> Using other system:%d....\n",id);
    cont = Manager::getTypeContainerPtr(s_system);
  }
  cont->visitEveryType(visitDpType);
  return 1;
}

void PVSS::pvss_setup_null_dp(const DpID* /* data */, size_t len)   {
  if ( len == sizeof(DpIdentifier) )  {
    ::printf("PVSS> Internal DP length:%d bytes, DpIdentifier size:%d bytes\n",
	     len,sizeof(DpIdentifier));
    return;
  }
  ::printf("PVSS> ERROR: Internal DP length:%d bytes, DpIdentifier size:%d bytes\n",
	   len,sizeof(DpIdentifier));
  throw "Error in size comparison of PVSS DpIdentifier structure";
}

bool PVSS::pvss_lookup_dpid(const char* nam, DpID& id)  {
  return Manager::getId(nam,id) == PVSS_TRUE;
}

bool PVSS::pvss_lookup_name(const DpID& id, char*& nam)  {
  static char* n = 0;
  if ( n ) delete [] n;
  bool res = Manager::getName(id,n) == PVSS_TRUE;
  nam = n;
  return res;
}

bool PVSS::pvss_lookup_dpidset(const char* wildname,DpID*& array,long& cnt,int typ)  {
  DpIdentifier* arr = 0;
  bool res = Manager::getIdSet(wildname,arr,cnt,typ) == PVSS_TRUE;
  array = (DpID*)arr;
  return res;
}

bool PVSS::pvss_create_device(const char* name,int type, int systemId, DevAnswer* a)  {
  Lock lock(pvss_global_lock());
  SyncWaitForAnswer* answer= new SyncWaitForAnswer(a);
  PVSSboolean res = Manager::dpCreate(name, DpTypeId(type), answer, systemId);
  return (PVSS_TRUE == res) ? a ? PVSS::Environment::instance().waitForAnswer(a) : true : false;
}

bool PVSS::pvss_delete_device(const DpID& dpid, int /* systemId */, DevAnswer* a)  {
  Lock lock(pvss_global_lock());
  SyncWaitForAnswer* answer= new SyncWaitForAnswer(a);
  PVSSboolean res = Manager::dpDelete(dpid, answer);
  return (PVSS_TRUE == res) ? a ? PVSS::Environment::instance().waitForAnswer(a) : true : false;
}

/// Access type name by DpElement type identifier
const char* PVSS::pvss_type_name(int typ)  {
  switch(typ)  {
#define _PRT(x)   case DPELEMENT_##x:  return "DPELEMENT_"#x
     _PRT(NOELEMENT);
     _PRT(RECORD);
     _PRT(ARRAY);
     _PRT(DYNCHAR);
     _PRT(DYNUINT);
     _PRT(DYNINT);
     _PRT(DYNFLOAT);
     _PRT(DYNBIT);
     _PRT(DYN32BIT);
     _PRT(DYNTEXT);
     _PRT(DYNTIME);
     _PRT(CHARARRAY);
     _PRT(UINTARRAY);
     _PRT(INTARRAY);
     _PRT(FLOATARRAY);
     _PRT(BITARRAY);
     _PRT(32BITARRAY);
     _PRT(TEXTARRAY);
     _PRT(TIMEARRAY);
     _PRT(CHAR);
     _PRT(UINT);
     _PRT(INT);
     _PRT(FLOAT);
     _PRT(BIT);
     _PRT(32BIT);
     _PRT(TEXT);
     _PRT(TIME);
     _PRT(DPID);
     _PRT(NOVALUE);
     _PRT(DYNDPID);
     _PRT(DYNCHARARRAY);
     _PRT(DYNUINTARRAY);
     _PRT(DYNINTARRAY);
     _PRT(DYNFLOATARRAY);
     _PRT(DYNBITARRAY);
     _PRT(DYN32BITARRAY);
     _PRT(DYNTEXTARRAY);
     _PRT(DYNTIMEARRAY);
     _PRT(DYNDPIDARRAY);
     _PRT(DPIDARRAY);
     _PRT(NOVALUEARRAY);
     _PRT(TYPEREFERENCE);
     _PRT(LANGTEXT);
     _PRT(LANGTEXTARRAY);
     _PRT(DYNLANGTEXT);
     _PRT(DYNLANGTEXTARRAY);
     _PRT(BLOB);
     _PRT(BLOBARRAY);
     _PRT(DYNBLOB);
     _PRT(DYNBLOBARRAY);
#undef _PRT
  default:
    return "DPELEMENT_NOELEMENT";
  }
  return "Unknwon Type";
}

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpHelpers.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  DpHelpers.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
#include "PVSS/Internals.h"

// PVSS include files
#include "Manager.hxx"
#include "DpElement.hxx"
#include "MapTableItem.hxx"
#include "ConfigsMapper.hxx"

#include "PVSS/DevAnswer.h"
#include "PVSS/Environment.h"
#include "PVSSManager/SyncWaitForAnswer.h"

#include <string>

static SystemNumType s_system = 0;
static PVSS::CfgManager* s_cfgMgr = 0;
static PVSS::DevTypeManager* s_devTypeMgr = 0;
static PVSS::DevType* (*s_addDevType)(PVSS::DevTypeManager* m,int id, const char* n) = 0;
static void (*s_addDevTypeElem)(PVSS::DevType* m,int id, int typ, const char* n) = 0;

static DpIdentifier getIdentifier(const std::string& nam)  {
  DpIdentifier dpid;
  if (Manager::getId(nam.c_str(), dpid) == PVSS_FALSE)  {
    throw std::runtime_error("Cannot retrieve datapoint identifier of "+nam);
  }
  return dpid;
}

static void visitDpElement(PVSS::DevTypeManager* m, PVSS::DevType* t, const DpType& p, const DpTypeNode* n, const std::string& par)  {
  if ( n )  {
    DpIdentification *dpIdent = Manager::getDpIdentificationPtr();
    char* elname = "Unknown";
    DpElementId eid = n->getId();
    DpElement elt(eid);
    DpElementType elTyp = elt.getType(&p);
    dpIdent->getElementName(p.getName(), eid, elname);
    std::string nam;
    if ( !par.empty() ) nam = par + ".";
    nam += elname;
    size_t nson = n->getSonNumber();
    if ( nson == 0 )  {
      s_addDevTypeElem(t,eid,elTyp,nam.c_str());
    }
    else  {
      for(size_t is = 0; is<nson; ++is )    {
        visitDpElement(m,t,p,DpElement(n->getSon(is)).getDpTypeNode(&p),nam);
      }
    }
  }
}

static int visitDpType(const DpType& p)  {
  DpIdentification *dpIdent = Manager::getDpIdentificationPtr();
  const DpTypeNode* n = p.getRootTypeNodePtr();
  char* name = "Unknown";
  dpIdent->getTypeName(p.getName(),name);
  PVSS::DevType* t = s_addDevType(s_devTypeMgr,p.getName(),name);
  delete [] name;
  visitDpElement(s_devTypeMgr,t,p,n,"");
  return 1;
}

int PVSS::pvss_load_configurations(CfgManager* m, int id, 
    void (*addCfg)(CfgManager* m,int id, const char* n),
    void (*addAttr)(CfgManager* m,int id, const char* n),
    void (*addDetail)(CfgManager* m,int id, const char* n))
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
    // ::printf("Adding Config [%d]:%s\n",cfg,n);
    for(size_t j=0; j<attrNum; ++j)  {
      const DpAttributeNrType att = attrs[j];
      n = ConfigsMapper::getAttributeName(cfg,att);
      // ::printf("        Adding attribute [%d]:%s\n",att,n);
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
  Manager::getTypeContainerPtr()->visitEveryType(visitDpType);
  return 1;
}

void PVSS::pvss_setup_null_dp(const DpIdentifier* data, size_t len)   {
  if ( len == sizeof(DpIdentifier) )  {
    printf("Internal DP length:%d bytes, DpIdentifier size:%d bytes\n",len,sizeof(DpIdentifier));
    return;
  }
  printf("ERROR: Internal DP length:%d bytes, DpIdentifier size:%d bytes\n",len,sizeof(DpIdentifier));
  throw "Error in size comparison of PVSS DpIdentifier structure";
}

bool PVSS::pvss_lookup_dpid(const char* nam, DpIdentifier& id)  {
  return Manager::getId(nam,id) == PVSS_TRUE;
}

bool PVSS::pvss_lookup_name(const DpIdentifier& id, char*& nam)  {
  static char* n = 0;
  if ( n ) delete [] n;
  bool res = Manager::getName(id,n) == PVSS_TRUE;
  nam = n;
  return res;
}

bool PVSS::pvss_lookup_dpidset(const char* wildname,DpIdentifier*& array,long& cnt,int typ)  {
  return Manager::getIdSet(wildname,array,cnt,typ) == PVSS_TRUE;
}

bool PVSS::pvss_create_device(const char* name,int type, int systemId, DevAnswer* a)  {
  SyncWaitForAnswer* answer= new SyncWaitForAnswer(a);
  PVSSboolean res = Manager::dpCreate(name, DpTypeId(type), answer, systemId);
  return (PVSS_TRUE == res) ? a ? PVSS::Environment::instance().waitForAnswer(a) : true : false;
}

bool PVSS::pvss_delete_device(const DpIdentifier& dpid, int systemId, DevAnswer* a)  {
  SyncWaitForAnswer* answer= new SyncWaitForAnswer(a);
  PVSSboolean res = Manager::dpDelete(dpid, answer);
  return (PVSS_TRUE == res) ? a ? PVSS::Environment::instance().waitForAnswer(a) : true : false;
}


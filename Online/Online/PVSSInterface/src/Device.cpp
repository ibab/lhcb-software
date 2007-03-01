// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/Device.cpp,v 1.1 2007-03-01 10:39:50 frankb Exp $
//  ====================================================================
//  Device.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $

// Framework include files
#include "PVSS/Device.h"
#include "PVSS/DevElement.h"
#include "PVSS/DeviceManager.h"
#include <algorithm>

using namespace PVSS;

/// Initializing constructor
Device::Device(DeviceManager* mgr, const DevType *typ, const std::string& nam)
: DataPoint(mgr->manager(),nam), m_manager(mgr), m_type(typ)
{
}

/// Standard destructor
Device::~Device()   {
}



#if 0
  if ( n )  {
    DpIdentification *dpIdent = Manager::getDpIdentificationPtr();
    char* elname = "Unknown";
    DpElementId eid = n->getId();
    DpElement elt(eid);
    DpElementType elTyp = elt.getType(&p);
    dpIdent->getElementName(p.getName(), eid, elname);
    if ( lvl==0 )  {
      cout << "Device:";
      cout << " ID:"   << p.getName();
      cout << " Root:" << p.getRootElement();
    }
    else  {
      cout << "   ->Son:";
      for(int i=0; i<lvl; ++i) cout << "   ";
    }
  
    cout << " id="            << eid;
    cout << " TypeNode: ID="  << n->getId();
    cout << " Father:"        << n->getFather();
    cout << " Sons:"          << n->getSonNumber() << " Typ:" << elTyp;
    cout << " ["              << pvss_type_name(elTyp).c_str() << "]";
    cout << " Name:"          << par.c_str() << "." << elname;	
    cout << " Configs:";
    std::string nam = par + ".";
    for(DpConfigNrType ic=DPCONFIGNR_NOCONFIGNR; ic<DPCONFIGNR_LAST_ITEM;ic=DpConfigNrType(int(ic)+1))  {
      DpConfig* conf = elt.getConfig(ic);
      if ( conf ) cout << ic << " ";
    }
    cout << endl;

    for(size_t is = 0, nson = n->getSonNumber(); is<nson; ++is )
      printElement(lvl+1,p,DpElement(n->getSon(is)).getDpTypeNode(&p),nam+elname);
    return;
  }
  cout << "Unconfigured DpTypeNode found!" << endl;

  ====

  DpIdentification *dpIdent = Manager::getDpIdentificationPtr();
  const DpTypeNode* r = p.getRootTypeNodePtr();
  printElement(0,p,r,"");
  return 1;
#endif

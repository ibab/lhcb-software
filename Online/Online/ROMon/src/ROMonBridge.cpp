// $Id: ROMonBridge.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonBridge.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $
#ifndef ROMON_ROMONBRIDGE_H
#define ROMON_ROMONBRIDGE_H 1

// Framework include files
#include "CPP/Interactor.h"
#include "ROMon/FMCMonListener.h"

// C++ include files
#include <vector>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  
  class ClusterListener : public FMCMonListener, public ROUpdateHandler {
  protected:
    /// Service container type
    typedef std::map<Item*,int>  Services;
    /// Map of services to be bridged
    Services    m_services;
    /// Prefix for resulting services
    std::string m_prefix;
  public:
    /// Standard constructor with initialization
    ClusterListener(bool verbose, const std::string& name, const std::string& prefix);
    /// Default destructor    
    virtual ~ClusterListener();
    /// Add service to be bridged
    void addService(const std::string& svc);
    /// Update handler
    virtual void update(void* param);
    /// Feed data to DIS when updating data
    static void feed(void* tag, void** buf, int* size, int* first);
  };

  /**@class ROMonBridge ROMonBridge.h GaudiOnline/ROMonBridge.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class ROMonBridge : public CPP::Interactor {
  protected:
    /// Cluster container type definition
    typedef std::vector<RODimListener*> Servers;
    /// Cluster container
    Servers         m_servers;
    /// Process name
    std::string     m_name;
    /// Prefix for resulting service names
    std::string     m_prefix;
    /// Printout level
    long            m_print;

    struct ClusterAdder {
      ROMonBridge* bridge;
      ClusterAdder(ROMonBridge* b) : bridge(b) {}
      void operator()(const std::string& n) const { bridge->addCluster(n); }
    };
  public:
    /// Standard constructor with initialization
    ROMonBridge(int argc, char** argv);
    /// Default destructor
    virtual ~ROMonBridge();
    /// Add cluster data points to bridge
    void addCluster(const std::string& name);
    /// Help printout in case of -h /? or wrong arguments
    static void help();
    /// Interactor override ....
    virtual void handle(const CPP::Event& ev);
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONBRIDGE_H */

// $Id: ROMonBridge.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonBridge.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $

// C++ include files
#include <iostream>
#include <stdexcept>

// Framework includes
extern "C" {
#include "dic.h"
#include "dis.h"
}
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
//#include "ROMon/ROMonBridge.h"
#include "ROMon/Constants.h"
#include "ROMon/PartitionListener.h"
#include "ROMonDefs.h"

#include <algorithm>
#include <memory>

using namespace ROMon;
using namespace std;
typedef FMCMonListener::Descriptor DSC;
typedef RODimListener::Clients     Clients;

/// Standard constructor with initialization
ClusterListener::ClusterListener(bool verbose, const string& sf, const string& pref) 
  : FMCMonListener(verbose), m_prefix(pref) 
{
  m_infoTMO = 0;
  setItem("");
  setMatch("*");
  setUpdateHandler(this);
  string svc0 = "/" + strlower(sf) + "/ROpublish";
  addService(svc0);
  addService(svc0+"/CPU");
  addService(svc0+"/Tasks");
  addService(svc0+"/ROTasks");
  addService(svc0+"/HLTDefer");
  addService(svc0+"/HLT1");
  svc0 = "/" + strupper(sf) + "/TaskSupervisor";
  addService(svc0+"/Summary");
  addService(svc0+"/Status");
  ::lib_rtl_output(LIB_RTL_DEBUG,"[ROMonBridge] Added services for subfarm:%s",sf.c_str());
}

/// Default destructor    
ClusterListener::~ClusterListener()  {
  dim_lock();
  for(Services::iterator i=m_services.begin(); i!=m_services.end(); ++i)
    ::dis_remove_service((*i).second);
  m_services.clear();
  dim_unlock();
}

/// Add service to be bridged
void ClusterListener::addService(const string& svc) {
  string svc_name = m_prefix+svc;
  addHandler(svc,svc);
  Clients::iterator i=m_clients.find(svc);
  if ( i != m_clients.end() )  {
    Item* it = (*i).second;
    int   id = ::dis_add_service((char*)svc_name.c_str(),(char*)"C",0,0,feed,(long)it);
    m_services.insert(make_pair(it,id));
    ::lib_rtl_output(LIB_RTL_VERBOSE,"[ROMonBridge] Added service:%s",svc_name.c_str());
  }
}

/// Update handler
void ClusterListener::update(void* param) {
  Item* it = (Item*)param;
  Services::const_iterator j=m_services.find(it);
  if ( j != m_services.end() ) {
    int id = (*j).second;
    ::lib_rtl_output(LIB_RTL_VERBOSE,"[ROMonBridge] Update Service:%d",id);
    ::dis_update_service(id);
  }
}

/// Feed data to DIS when updating data
void ClusterListener::feed(void* tag, void** buff, int* size, int* ) {
  static const char* data = "";
  Item* it = *(Item**)tag;
  if ( it ) {
    Descriptor* d = it->data<Descriptor>();
    if ( d  && d->data ) {
      *buff = (void*)(d->data ? d->data : data);
      *size = d->actual;
      return;
    }
  }
  *buff = (void*)data;
  *size = 0;
}

/// Standard constructor
ROMonBridge::ROMonBridge(int argc, char** argv) : m_print(LIB_RTL_WARNING)  {
  string PUBLISHING_NODE = "ECS03", from=PUBLISHING_NODE, to=PUBLISHING_NODE;
  RTL::CLI cli(argc, argv, ROMonBridge::help);
  m_name = "/"+RTL::nodeNameShort()+"/"+RTL::processName();
  cli.getopt("publish",2, m_prefix="/Farm");
  cli.getopt("print",2,m_print);
  cli.getopt("from",2, from);
  cli.getopt("to",2, to);

  ::lib_rtl_install_printer(ro_rtl_print,(void*)m_print);
  ::dic_set_dns_node((char*)from.c_str());
  ::dis_set_dns_node((char*)to.c_str());
  PartitionListener p(this,"Subfarms","*",true);
  ::dis_start_serving((char*)m_name.c_str());
}

/// Default destructor
ROMonBridge::~ROMonBridge() {
}

/// Add cluster data points to bridge
void ROMonBridge::addCluster(const string& sf) {
  m_servers.push_back(new ClusterListener(m_print<LIB_RTL_INFO, sf, m_prefix));
  //::lib_rtl_output(LIB_RTL_ALWAYS,"[ROMonBridge] Added subfarm:%s",sf.c_str());
}

/// Interactor override ....
void ROMonBridge::handle(const CPP::Event& ev) {
  typedef vector<string> StringV;
  switch(ev.eventtype) {
  case TimeEvent:
    //if (ev.timer_data == ??? ) {}
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_CONNECT: {
      auto_ptr<StringV> farms((StringV*)ev.data);
      for_each(farms->begin()+1,farms->end(),ClusterAdder(this));
      ::dis_start_serving((char*)m_name.c_str());
      return;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// Help printout in case of -h /? or wrong arguments
void ROMonBridge::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_bridge -opt [-opt]\n"
                   "     -from=<string>     Node name from which the datapoints should be consumed.\n"
                   "     -to=<string>       Node to which these data points should be published.\n"
                   "     -print=<integer>   Printout value and verbosity.\n"
                   "     -publish=<string>  Prefix for published services.\n\n");
}

/// Main entry point to start the application
extern "C" int romon_bridge(int argc, char** argv) {
  ROMonBridge mon(argc,argv);
  print_startup("Process information COLLECTOR");
  IocSensor::instance().run();
  return 1;
}


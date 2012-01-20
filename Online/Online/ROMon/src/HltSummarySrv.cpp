// $Id: HltSummarySrv.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/HltSummarySrv.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $
#ifndef ROMON_HLTSUMMARYSRV_H
#define ROMON_HLTSUMMARYSRV_H 1

// Framework include files
#include "CPP/Interactor.h"
#include "ROMon/FMCMonListener.h"

// C++ include files
#include <vector>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  
  class HltSummaryListener : public FMCMonListener, public ROUpdateHandler {
  protected:
    /// Pointer to parent
    Interactor* m_parent;
  public:
    /// Standard constructor with initialization
    HltSummaryListener(Interactor* parent, const std::string& name, bool verbose);
    /// Default destructor    
    virtual ~HltSummaryListener();
    /// Update handler
    virtual void update(void* param);
    /// Feed data to DIS when updating data
    //static void feed(void* tag, void** buf, int* size, int* first);
  };

  /**@class HltSummarySrv HltSummarySrv.h GaudiOnline/HltSummarySrv.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class HltSummarySrv : public Interactor {
  protected:
    /// Cluster container type definition
    typedef std::vector<HltSummaryListener*> Servers;
    /// Cluster container
    Servers         m_servers;
    /// Process name
    std::string     m_name;
    /// String buffer with run information result
    std::string     m_run_result;
    /// String buffer with summary result
    std::string     m_summary_result;
    /// Run summary DIM service ID
    int             m_runSvc;
    /// Run summary DIM service ID
    int             m_sumSvc;
    /// Property for time delay between publications
    int             m_delay;
    /// Printout level
    long            m_print;

    struct ClusterAdder {
      HltSummarySrv* bridge;
      ClusterAdder(HltSummarySrv* b) : bridge(b) {}
      void operator()(const std::string& n) const { bridge->addCluster(n); }
    };
  public:
    /// Standard constructor with initialization
    HltSummarySrv(int argc, char** argv);
    /// Default destructor
    virtual ~HltSummarySrv();
    /// Add cluster data points to bridge
    void addCluster(const std::string& name);
    /// Publish collected data to DIM services
    void collectData();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
    /// Interactor override ....
    virtual void handle(const Event& ev);
  };
}      // End namespace ROMon
#endif /* ROMON_HLTSUMMARYSRV_H */

// $Id: HltSummarySrv.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/HltSummarySrv.cpp,v 1.2 2010-10-14 13:30:09 frankb Exp $

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
#include "CPP/TimeSensor.h"
//#include "ROMon/HltSummarySrv.h"
#include "ROMon/CPUMon.h"
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
HltSummaryListener::HltSummaryListener(Interactor* parent, const std::string& sf, bool verbose)
  : FMCMonListener(verbose), m_parent(parent)
{
  m_infoTMO = 0;
  setItem("");
  setMatch("/RO/hlt*");
  setUpdateHandler(this);
  string svc = "/RO/" + strlower(sf) + "/ROpublish/HLTDefer";
  addHandler(sf,svc);
  ::lib_rtl_output(LIB_RTL_DEBUG,"[HltSummarySrv] Added services for subfarm:%s",sf.c_str());
}

/// Default destructor    
HltSummaryListener::~HltSummaryListener()  {
}

/// Update handler
void HltSummaryListener::update(void* /* param */)   {
  //IocSensor::instance().send(m_parent,CMD_UPDATE,param);
}
#if 0
/// Feed data to DIS when updating data
void HltSummaryListener::feed(void* tag, void** buff, int* size, int* /* first */) {
  static const char* data = "";
  Item* it = *(Item**)tag;
  if ( it ) {
    Descriptor* d = it->data<Descriptor>();
    if ( d ) {
      if ( d->data ) {
	*buff = (void*)(d->data ? d->data : data);
	*size = d->actual;
	return;
      }
    }
  }
  *buff = (void*)data;
  *size = 0;
}
#endif
/// Feed data to DIS when updating data
static void feed(void* tag, void** buff, int* size, int* /* first */) {
  static const char* data = "";
  string* s = *(string**)tag;
  if ( !s->empty() )  {
    *buff = (void*)s->c_str();
    *size = s->length()+1;
    return;
  }
  *buff = (void*)data;
  *size = 0;
}

/// Standard constructor
HltSummarySrv::HltSummarySrv(int argc, char** argv) : m_delay(5), m_print(LIB_RTL_WARNING)  {
  RTL::CLI cli(argc, argv, HltSummarySrv::help);
  m_name = "/"+RTL::nodeNameShort()+"/"+RTL::processName();
  cli.getopt("print",2,m_print);
  cli.getopt("delay",2,m_delay);

  ::lib_rtl_install_printer(ro_rtl_print,(void*)m_print);
  PartitionListener p(this,"Subfarms","hlt*",true);
  m_runSvc = ::dis_add_service((char*)"/HLT/Deferred/Runs",(char*)"C",0,0,feed,(long)&m_run_result);
  m_sumSvc = ::dis_add_service((char*)"/HLT/Deferred/Summary",(char*)"C",0,0,feed,(long)&m_summary_result);
  ::dis_start_serving((char*)m_name.c_str());
  TimeSensor::instance().add(this,m_delay,(void*)CMD_UPDATE);
}

/// Default destructor
HltSummarySrv::~HltSummarySrv() {
  ::dis_remove_service(m_runSvc);
  ::dis_remove_service(m_sumSvc);
}

/// Add cluster data points to bridge
void HltSummarySrv::addCluster(const string& sf) {
  m_servers.push_back(new HltSummaryListener(this, sf, m_print<LIB_RTL_INFO));
}

/// Publish collected data to DIM services
void HltSummarySrv::collectData() {
  typedef DeferredHLTSubfarmStats _S;
  typedef RODimListener::Clients  _C; 
  typedef RODimListener::Item     _I;
  typedef _S::Nodes               _N;
  typedef _S::Node::Runs          _R;
  map<int,int> tot_runs;
  long tot_nodes = 0, tot_files = 0;
  long long tot_blocks = 0, free_blocks = 0;
  char txt[132];

  DimLock lock;
  for(Servers::const_iterator i=m_servers.begin(); i!=m_servers.end();++i) {
    const _C& clients = (*i)->clients();
    for(_C::const_iterator ic=clients.begin(); ic != clients.end(); ++ic) {
      _I* it = (*ic).second;
      DSC* d = it->data<DSC>();
      if ( d->data ) {
	const _S* stats = (_S*)d->data;
	const _N& nodes = *(stats->nodes());
	for (_N::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
	  const _R& runs = (*n).runs;
	  const Diskspace& disk = (*n).localdisk;
	  for (_R::const_iterator ri=runs.begin(); ri!=runs.end(); ri=runs.next(ri))  {
	    int run = (*ri).first;
	    int files = (*ri).second;
	    tot_runs[run] += files;
	    tot_files += files;
	  }
	  long tot = disk.numBlocks;
	  long fr  = disk.freeBlocks;
	  if ( (tot-fr) < 1 ) fr = tot;
	  tot_blocks += tot;
	  free_blocks += fr;
	  ++tot_nodes;
	  //::lib_rtl_output(LIB_RTL_DEBUG,"Node:%s Total blocks:%lld Free:%lld  %7.2f %%",
	  //		   (*n).name,tot,fr,100.f*float(fr)/float(tot));
	}
      }
    }
  }
  if ( tot_blocks <= 0 ) tot_blocks = 1;
  ::lib_rtl_output(LIB_RTL_DEBUG,"Nodes:%ld Runs:%ld Files:%ld Total blocks:%lld Free:%lld  %7.2f %%",
		   tot_nodes, tot_runs.size(),tot_files,tot_blocks,free_blocks,
		   100.f*float(free_blocks)/float(tot_blocks));

  m_run_result = "";
  for(map<int,int>::const_iterator j=tot_runs.begin();j!=tot_runs.end();++j) {
    ::sprintf(txt,"%d/%d",(*j).first,(*j).second);
    m_run_result += string(txt)+"|";
  }
  if ( tot_runs.size()>0 ) m_run_result = m_run_result.substr(0,m_run_result.length()-1);
  ::sprintf(txt,"%ld|%ld|%ld|%lld|%lld",tot_nodes,long(tot_runs.size()),
	    tot_files,tot_blocks,free_blocks);
  m_summary_result = txt;
}

/// Interactor override ....
void HltSummarySrv::handle(const Event& ev) {
  typedef vector<string> StringV;
  switch(ev.eventtype) {
  case TimeEvent:
    //if (ev.timer_data == ??? ) {}
    if (ev.timer_data == (void*)CMD_UPDATE ) {
      collectData();
      ::dis_update_service(m_sumSvc);
      ::dis_update_service(m_runSvc);
      TimeSensor::instance().add(this,m_delay,(void*)CMD_UPDATE);
    }
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_UPDATE:
      //::lib_rtl_output(LIB_RTL_VERBOSE,"[HltSummarySrv] Update Service:%p",ev.data);
      return;
    case CMD_CONNECT: {
      auto_ptr<StringV> farms((StringV*)ev.data);
      for_each(farms->begin(),farms->end(),ClusterAdder(this));
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
void HltSummarySrv::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_bridge -opt [-opt]\n"
                   "     -print=<integer>   Printout value and verbosity.\n");
}

/// Main entry point to start the application
extern "C" int romon_hltsummary_server(int argc, char** argv) {
  HltSummarySrv mon(argc,argv);
  print_startup("Deferred HLT summary PUBLISHER");
  TimeSensor::instance().run();
  return 1;
}


// $Id: CPUMonCollector.cpp,v 1.3 2008-06-25 22:53:23 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/CPUMonCollector.cpp,v 1.3 2008-06-25 22:53:23 frankb Exp $

// C++ include files
#include <iostream>

// Framework includes
#include "dic.hxx"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "ROMon/CPUMon.h"
#include "ROMon/CPUMonOstream.h"
#include "ROMon/CPUMonCollector.h"
#include "dic.hxx"

#include "ROMonDefs.h"
using namespace ROMon;

/// Standard constructor
CPUMonCollector::CPUMonCollector(int argc, char** argv) 
  : ROMonGblBuffer(argc,argv,CPUMonCollector::help), m_needUpdate(true)
{
  std::string match = "*";
  RTL::CLI cli(argc, argv, CPUMonCollector::help);
  cli.getopt("match",3,match);
  m_verbose = true;
  m_ncpu.setMatch(match+"/cpu/info");
  m_ncpu.setItem("cpuN");
  m_ncpu.setVerbose(m_verbose);
  m_info.setMatch(match+"/cpu/info");
  m_info.setVerbose(m_verbose);
  m_info.setItem("data");
  m_stat.setMatch(match+"/cpu/stat");
  m_stat.setItem("data");
  m_stat.setVerbose(m_verbose);
  m_ncpu.setUpdateHandler(this);
  m_info.setUpdateHandler(this);
  m_stat.setUpdateHandler(this);
  m_ncpu.setType(ONCE_ONLY);
  m_ncpu.start();
  m_info.start();
  m_stat.start();
}

/// Help printout in case of -h /? or wrong arguments
void CPUMonCollector::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"cpumon_collect -opt [-opt]\n");
  ROMonGblBuffer::help();
}

/// Start monitoring activity
int CPUMonCollector::monitor() {
  CPUMonData gbl(data());
  if ( gbl.ptr ) {
    typedef RODimListener::Clients Clients;
    bool exec = true;
    while(exec)    {
      CPUfarm* f = gbl.farm;
      if ( f && m_needUpdate ) {  // Take GBL lock while updating
	RTL::Lock lock(m_lock);
	CPUfarm::Nodes& nodes = f->reset()->nodes;
	const Clients& cl_ncpu = m_ncpu.clients();
	const Clients& cl_stat = m_stat.clients();
	const Clients& cl_info = m_info.clients();
	::strncpy(f->name,RTL::nodeName().c_str(),sizeof(f->name));
	f->name[sizeof(f->name)-1] = 0;
	f->type = CPUfarm::TYPE;
	f->time = ::time(0);
	dim_lock();
	m_needUpdate = false;
	CPUfarm::Nodes::iterator it = nodes.reset();
	for(Clients::const_iterator ic = cl_ncpu.begin(); ic != cl_ncpu.end(); ++ic) {
	  const std::string& node = (*ic).first;
	  Clients::const_iterator is = cl_stat.find(node);
	  Clients::const_iterator ii = cl_info.find(node);
	  if ( is != cl_stat.end() && ii != cl_info.end() ) {
	    typedef FMCMonListener::Descriptor DSC;
	    DSC* dsc_info = (*ii).second->data<DSC>();
	    DSC* dsc_stat = (*is).second->data<DSC>();
	    const float* ds = (float*)dsc_stat->data;
	    const char*  di = dsc_info->data;
	    if ( ((char*)it) > gbl.str+m_section_size ) {
	      log() << "Global section memory too small.....exiting" << std::endl;
	      break;
	    }
	    ::strncpy((*it).name,node.c_str(),sizeof((*it).name));
	    (*it).name[sizeof((*it).name)-1] = 0;
	    ::strncpy((*it).family,di,sizeof((*it).family));
	    (*it).family[sizeof((*it).family)-1] = 0;
	    (*it).ctxtRate = ds[0];
	    (*it).time     = dsc_stat->time;
	    (*it).millitm  = dsc_info->millitm;
	    if ( (*ic).second->data<DSC>()->data ) {
	      int ncpu = *(int*)(*ic).second->data<DSC>()->data;
	      if ( ncpu < 64 ) { // Must be reasonable
		CPUset::Cores::iterator corIt = (*it).cores.reset();
		for (int icpu=0; icpu<ncpu; ++icpu) {
		  if ( ((char*)corIt) > gbl.str+m_section_size ) {
		    log() << "Global section memory too small.....exiting" << std::endl;
		    break;
		  }
		  ::memcpy(&(*corIt).stats,ds+1,sizeof(CPU::Stat));
		  const char* vendor_id = di;
		  const char* family    = vendor_id + ::strlen(vendor_id)+1;
		  const char* model     = family    + ::strlen(family)+1;
		  const char* mod_name  = model     + ::strlen(model)+1;
		  const char* stepping  = mod_name  + ::strlen(mod_name)+1;
		  const char* clock     = stepping  + ::strlen(stepping)+1;
		  const char* cache     = clock     + ::strlen(clock)+1;
		  const char* physId    = cache     + ::strlen(cache)+1;
		  const char* sibblings = physId    + ::strlen(physId)+1;
		  const char* bogo      = sibblings + ::strlen(sibblings)+1;
		  di = bogo + ::strlen(bogo)+1;
		  (*corIt).clock        = ::atof(clock);
		  (*corIt).cache        = ::atoi(cache);
		  (*corIt).bogomips     = ::atof(bogo);
		  corIt = (*it).cores.add(corIt);
		}
	      }
	    }
	    it = nodes.add(it);
	  }
	}
	dim_unlock();
	f->fixup();
	if ( m_print ) {
	  log() << "========================" << ::lib_rtl_timestr() 
		<< "========================" << std::endl
		<< *f << std::endl;
	}
      }
      ::lib_rtl_sleep(m_delay);
    }
    return 1;
  }
  log() << "CPUMon gbl " << sectionName() << " invalid....exiting" << std::endl;
  return 0;
}

/// Update handler
void CPUMonCollector::update() {
  m_needUpdate = true;
}

extern "C" int cpumon_collect(int argc, char** argv) {
  CPUMonCollector mon(argc,argv);
  print_startup("CPU information COLLECTOR");
  return mon.monitor();
}


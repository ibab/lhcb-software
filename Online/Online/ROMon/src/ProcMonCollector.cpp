// $Id: ProcMonCollector.cpp,v 1.5 2008-11-13 12:13:32 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ProcMonCollector.cpp,v 1.5 2008-11-13 12:13:32 frankb Exp $

// C++ include files
#include <iostream>

// Framework includes
#include "dic.hxx"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "ROMon/CPUMon.h"
#include "ROMon/CPUMonOstream.h"
#include "ROMon/ProcMonCollector.h"

#include "ROMonDefs.h"
using namespace ROMon;

/// Standard constructor
ProcMonCollector::ProcMonCollector(int argc, char** argv) 
  : ROMonGblBuffer(argc,argv,ProcMonCollector::help), m_needUpdate(true)
{
  std::string match = "*";
  RTL::CLI cli(argc, argv, ProcMonCollector::help);
  cli.getopt("match",3,match);
  m_verbose = true;
  m_ntasks.setMatch(match+"/ps");
  m_ntasks.setItem("ntasks");
  m_ntasks.setVerbose(m_verbose);
  m_info.setMatch(match+"/ps");
  m_info.setItem("data");
  m_info.setVerbose(m_verbose);
  m_ntasks.setUpdateHandler(this);
  m_info.setUpdateHandler(this);
  m_ntasks.start();
  m_info.start();
}

/// Help printout in case of -h /? or wrong arguments
void ProcMonCollector::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"cpumon_collect -opt [-opt]\n");
  ROMonGblBuffer::help();
}

/// Start monitoring activity
int ProcMonCollector::monitor() {
  CPUMonData gbl(data());
  if ( gbl.ptr ) {
    typedef RODimListener::Clients Clients;
    bool exec = true;
    while(exec)    {
      ProcFarm* f = gbl.procFarm;
      if ( f && m_needUpdate ) {  // Take GBL lock while updating
        RTL::Lock lock(m_lock);
        ProcFarm::Nodes& nodes = f->reset()->nodes;
        const Clients& cl_ntasks = m_ntasks.clients();
        const Clients& cl_info = m_info.clients();
        ro_get_node_name(f->name,sizeof(f->name));
        f->type = ProcFarm::TYPE;
        f->time = ::time(0);
        dim_lock();
        m_needUpdate = false;
        ProcFarm::Nodes::iterator it = nodes.reset();
        for(Clients::const_iterator ic = cl_ntasks.begin(); ic != cl_ntasks.end(); ++ic) {
          const std::string& node = (*ic).first;
          Clients::const_iterator is = cl_ntasks.find(node);
          Clients::const_iterator ii = cl_info.find(node);
          if ( is != cl_ntasks.end() && ii != cl_info.end() ) {
            typedef FMCMonListener::Descriptor DSC;
            DSC* dsc_info = (*ii).second->data<DSC>();
            DSC* dsc_stat = (*is).second->data<DSC>();
            const char*  di = dsc_info->data;
            if ( ((char*)it) > gbl.str+m_section_size ) {
              log() << "Global section memory too small.....exiting" << std::endl;
              break;
            }
            ::strncpy((*it).name,node.c_str(),sizeof((*it).name));
            (*it).name[sizeof((*it).name)-1] = 0;
            (*it).time     = dsc_stat->time;
            (*it).millitm  = dsc_info->millitm;
            if ( (*ic).second->data<DSC>()->data ) {
              int ntask = *(int*)(*ic).second->data<DSC>()->data;
              Procset::Processes::iterator prcIt = (*it).processes.reset();
              for (int itsk=0; itsk<ntask; ++itsk) {
#define defineItem(x) const char* x = ptr + strlen(ptr)+1; ptr = x;
                const char* utgid = di;
                const char* ptr   = utgid;
                defineItem(tid);
                defineItem(tgid);
                defineItem(ppid);
                defineItem(pgid);
                defineItem(nlwp);
                defineItem(usr);
                defineItem(grp);
                defineItem(tty);
                defineItem(cmd);
                defineItem(sch);
                defineItem(prio);
                defineItem(rtprio);
                defineItem(nice);
                defineItem(psr);
                defineItem(stat);
                defineItem(cpu);
                defineItem(mem);
                defineItem(vsize);
                defineItem(lock);
                defineItem(rss);
                defineItem(data);
                defineItem(stack);
                defineItem(exe);
                defineItem(lib);
                defineItem(share);
                defineItem(ign);
                defineItem(pend);
                defineItem(catched);
                defineItem(block);
                defineItem(start);
                defineItem(elaps);
                defineItem(cputim);
                defineItem(cmdline);
                di = cmdline + ::strlen(cmdline)+1;
                if ( strncmp(utgid,"N/A",3) != 0 && strcmp(tid,tgid)==0 ) {
                  //std::cout << node << " -> " << utgid << " " << usr << " " << grp << " " << cpu << " " << mem << " " 
                  //            << tid << " " << ppid << " " << nlwp << " " << exe << std::endl;
                  if ( ((char*)prcIt) > gbl.str+m_section_size ) {
                    log() << "Global section memory too small.....exiting" << std::endl;
                    break;
                  }
                  Process& p = (*prcIt);
                  ::strncpy(p.utgid,utgid,sizeof(p.utgid));
                  p.utgid[sizeof(p.utgid)-1]=0;
                  ::strncpy(p.owner,usr,sizeof(p.owner));
                  p.owner[sizeof(p.owner)-1]=0;
                  p.cpu     = (float)::atof(cpu);
                  p.mem     = (float)::atof(mem);
                  p.vsize   = (float)::atoi(vsize);
                  p.rss     = (float)::atoi(rss);
                  p.pid     = ::atoi(tgid);
                  p.ppid    = ::atoi(ppid);
                  p.threads = ::atoi(nlwp);
                  prcIt = (*it).processes.add(prcIt);
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
  log() << "ProcMon gbl " << sectionName() << " invalid....exiting" << std::endl;
  return 0;
}

/// Update handler
void ProcMonCollector::update() {
  m_needUpdate = true;
}

extern "C" int procmon_collect(int argc, char** argv) {
  ProcMonCollector mon(argc,argv);
  print_startup("Process information COLLECTOR");
  return mon.monitor();
}


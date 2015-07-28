// $Id: ROMonTaskCollector.cpp,v 1.5 2008-11-13 12:13:33 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonTaskCollector.cpp,v 1.5 2008-11-13 12:13:33 frankb Exp $

// Framework includes
#include "dic.hxx"
#include "dis.hxx"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMonOstream.h"
#include "ROMon/ROMonTaskCollector.h"

using namespace ROMon;
using namespace std;

/// Standard constructor
ROMonTaskCollector::ROMonTaskCollector(int argc, char** argv) 
  : ROMonGblBuffer(argc,argv,ROMonTaskCollector::help), m_mbmInfo(0), m_fsmInfo()
{
  m_fsmInfo.setVerbose(m_verbose);
  m_fsmInfo.start();
}

/// Help printout in case of -h /? or wrong arguments
void ROMonTaskCollector::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_collect -opt [-opt]\n");
  ROMonGblBuffer::help();
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -verbose          Make printouts to stdout.\n");  
}

/// Start monitoring activity
int ROMonTaskCollector::monitor() {
  ROMonData gbl(data());
  if ( gbl.ptr ) {
    typedef RODimFSMListener::Clients Clients;
    bool exec = true;
    while(exec)    {
      if ( gbl.node ) {  // Take GBL lock while updating
        RTL::Lock lock(m_lock);
        m_mbmInfo.handle(gbl.node, m_section_size);
        Node::Tasks* tasks = gbl.node->tasks();
        Node::Tasks::iterator it = tasks->reset();
        const Clients& cl = m_fsmInfo.clients();
        gbl.node->type = Node::TYPE;
        if ( ((char*)it) > gbl.str+m_section_size ) {
          log() << "Global section memory too small.....exiting" << endl;
          break;
        }
        else {
          ::dim_lock();
          for(Clients::const_iterator ic = cl.begin(); ic != cl.end(); ++ic) {
            const FSMTask* t = (*ic).second->data<FSMTask>();
            if ( ((char*)it) > gbl.str+m_section_size ) {
              log() << "Global section memory too small.....exiting" << endl;
              ::dim_unlock();
              break;
            }
            if ( t->processID != -1 ) {
              ::memcpy(it,t,sizeof(FSMTask));
              it = tasks->add(it);
            }
          }
          ::dim_unlock();
          gbl.node->fixup();
        }
      }
      if ( m_print ) {
        log() << "========================" << ::lib_rtl_timestr() 
              << "========================" << endl
              << *gbl.node << endl;
      }
      ::lib_rtl_sleep(m_delay);
    }
    return 1;
  }
  log() << "ROMon gbl invalid....exiting." << endl;
  return 0;
}

extern "C" int romon_collect(int argc, char** argv) {
  ROMonTaskCollector romon(argc,argv);
  print_startup("information COLLECTOR");
  return romon.monitor();
}


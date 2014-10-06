/*
 * AligOnlIterator.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#include "GaudiKernel/Service.h"
#include "GaudiAlg/GaudiTool.h"

#include "OnlineAlign/IAlignSys.h"
#include "OnlineAlign/IAlignUser.h"
#include "RTL/rtl.h"

#include "ASDCollector.h"

#include "GaudiKernel/ToolHandle.h"
#include "AlignmentInterfaces/IAlignUpdateTool.h"
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"

//#include <stdio.h>
//#include <stdlib.h>

class AlignOnlineIterator : public GaudiTool, virtual public LHCb::IAlignIterator
{
 public:
  AlignOnlineIterator(const std::string &  type, const std::string &  name, const IInterface *  parent  );
  //      StatusCode init(std::string, std::string);
  StatusCode i_start();
  StatusCode i_run();
  StatusCode initialize();
  StatusCode finalize();
  StatusCode stop();
  //      StatusCode de_init();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
 private:
  LHCb::IAlignDrv *m_parent;
  lib_rtl_thread_t m_thread;
  std::string m_PartitionName;
  ToolHandle<Al::IAlignUpdateTool> m_alignupdatetool ;
  ToolHandle<IWriteAlignmentConditionsTool> m_xmlwriter ;
  unsigned int m_maxIteration ;
  ASDCollector m_asdCollector ;
};


DECLARE_TOOL_FACTORY(AlignOnlineIterator)

static AlignOnlineIterator *IteratorInstance;

extern "C"
{
  int AlignOnlineIteratorThreadFunction(void *t)
  {
    AlignOnlineIterator *f = (AlignOnlineIterator*)t;
    IteratorInstance = f;
    f->i_run();
    return 1;
  }

}

AlignOnlineIterator::AlignOnlineIterator(const std::string &  type, 
				 const std::string &  name, 
				 const IInterface *  parent  )
  : GaudiTool(type,name,parent),
    m_alignupdatetool("Al::AlignUpdateTool"),
    m_xmlwriter("WriteMultiAlignmentConditionsTool")
{
  declareInterface<LHCb::IAlignIterator>(this) ;
  declareProperty("PartitionName",   m_PartitionName= "LHCbA") ;
  declareProperty("MaxIteration", m_maxIteration = 10 ) ;
  declareProperty("ASDDir", m_asdCollector.m_dir ) ;
  declareProperty("ASDFilePattern", m_asdCollector.m_filePatt ) ;

  IInterface *p=(IInterface*)parent;
  StatusCode sc = p->queryInterface(LHCb::IAlignDrv::interfaceID(),(void**)(&m_parent));
}

StatusCode AlignOnlineIterator::initialize()
{  
  debug() << "AlignOnlineIterator::initialize()" << endreq ;

  StatusCode sc = GaudiTool::initialize() ;
  if(sc.isSuccess() ) {
    sc = m_alignupdatetool.retrieve() ;
    if(!sc.isSuccess() ) 
      error() << "Cannot retrieve alignupdatetool" << endreq ;
  }

  if(sc.isSuccess() ) {
    sc = m_xmlwriter.retrieve() ;
    if(!sc.isSuccess() ) 
      error() << "Cannot retrieve xmlwriter" << endreq ;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode AlignOnlineIterator::finalize()
{
  m_alignupdatetool.release().ignore() ;
  m_xmlwriter.release().ignore() ;
  return GaudiTool::finalize() ;
}

StatusCode AlignOnlineIterator::i_run()
{
  StatusCode sc = StatusCode::SUCCESS ;
  // only called once
  unsigned int iteration(0) ;
  while( iteration < m_maxIteration ) {
    debug() << "Iteration " << ++iteration << endreq ;

    // 1. writes a little file with number of iteration step
    debug() << "calling parent->writeReference()" << endreq ;
    m_parent->writeReference();
 
    // 2. write the xml
    debug() << "writing xml files" << endreq ;
    sc = m_xmlwriter->write() ;
    if( !sc.isSuccess() ) {
      error() << "Error writing xml files" << endreq ;
      break ;
    }
    
    // 3. start the analyzers and wait 
    debug() << "wait for analyzers" << endreq ;
    m_asdCollector.setTime() ;
    m_parent->doContinue();
    m_parent->waitRunOnce();
    
    // 4. read ASDs and compute new constants
    debug() << "Collecting ASD files" << endreq ;
    Al::IAlignUpdateTool::ConvergenceStatus convergencestatus ;
    Al::Equations equations ;
    m_asdCollector.collectASDs(equations) ;
    debug() << "Collected ASDs: numevents = " << equations.numEvents() << endreq ;

    // std::vector<std::string> filenames ;
    // for(auto filename : filenames ) {
    //   Al::Equations tmp(0) ;
    //   tmp.readFromFile( filename.c_str() ) ;
    //   debug() << "Adding derivatives from input file: " << filename << " " << tmp.numHits() << " "
    // 	      << tmp.totalChiSquare()  << endreq ;
    //   if( equations.nElem()==0 )
    // 	equations = tmp ;
    //   else
    // 	equations.add( tmp ) ;
    // }
    debug() << "Calling AlignUpdateTool" << endreq ;
    
    StatusCode sc = m_alignupdatetool->process( equations, convergencestatus ) ;
    if( !sc.isSuccess() ) {
      error() << "Error calling alignupdate tool" << endreq ;
      break ;
    }

    // 5. break the loop if converged
    if( convergencestatus == Al::IAlignUpdateTool::Converged ) {
      // write the xml one last time ?
      sc = m_xmlwriter->write() ;
      break ;
    }
  }
  
  //fflush(stdout);
  m_parent->doStop();
  return StatusCode::SUCCESS;
}

StatusCode AlignOnlineIterator::i_start()
{
  // called only once
  // don't touch this
  ::lib_rtl_start_thread(AlignOnlineIteratorThreadFunction,this,&m_thread);
  return StatusCode::SUCCESS;
}

StatusCode AlignOnlineIterator::stop()
{
  // called only once
  ::lib_rtl_delete_thread(m_thread);
  return StatusCode::SUCCESS;
}


StatusCode AlignOnlineIterator::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (LHCb::IAlignIterator::interfaceID().versionMatch(riid))  {
    *ppvIF = (IAlignIterator*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return GaudiTool::queryInterface(riid, ppvIF);
}


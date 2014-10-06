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
  declareProperty("PartitionName",   m_PartitionName= "LHCbA");
  IInterface *p=(IInterface*)parent;
  StatusCode sc = p->queryInterface(LHCb::IAlignDrv::interfaceID(),(void**)(&m_parent));
}

StatusCode AlignOnlineIterator::initialize()
{  
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
  // only called once

  while( 1 ) {
    
    // 1. writes a little file with number of iteration step
    debug() << "calling parent->writeReference()" << endreq ;
    m_parent->writeReference();
 
    // 2. write the xml
    debug() << "writing xml files" << endreq ;
    m_xmlwriter->write() ;
    
    // 3. break the loop if converged
    // if( convergencestatus == Converged ) break ;

    // 4. start the analyzers and wait 
    debug() << "wait for analyzers" << endreq ;
    m_parent->doContinue();
    m_parent->waitRunOnce();
    
    // 5. read ASDs and compute new constants
    Al::IAlignUpdateTool::ConvergenceStatus convergencestatus ;
    Al::Equations equations ;
    std::vector<std::string> filenames ;
    for(auto filename : filenames ) {
      Al::Equations tmp(0) ;
      tmp.readFromFile( filename.c_str() ) ;
      if( equations.numHits()==0 ) 
	equations = tmp ;
      else
	equations.add( tmp ) ;
      warning() << "Adding derivatives from input file: " << filename << " " << tmp.numHits() << " "
		<< tmp.totalChiSquare()  << endreq ;
    }
    StatusCode sc = m_alignupdatetool->process( equations, convergencestatus ) ;
    
    // 6. break the loop if converged
    if( convergencestatus == Al::IAlignUpdateTool::Converged ) {
      // write the xml one last time ?
      m_xmlwriter->write() ;
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


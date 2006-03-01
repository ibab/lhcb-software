// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MessageSenderAlg.cpp,v 1.1 2006-03-01 19:20:33 frankb Exp $
//	====================================================================
//  MessageSenderAlg.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"

using MBM::Producer;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class MessageSenderAlg MessageSenderAlg.cpp
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MessageSenderAlg : public Algorithm   {
    Producer*   m_prod;
    int         m_partitionID;
    std::string m_buffer;
    std::string m_procName;
  public:
    /// Standard algorithm constructor
    MessageSenderAlg(const std::string& name, ISvcLocator* pSvcLocator)
    :	Algorithm(name, pSvcLocator),  m_prod(0)  {
      declareProperty("Buffer",      m_buffer);
      declareProperty("ProcessName", m_procName);
      declareProperty("PartitionID", m_partitionID);
    }

    /// Standard Destructor
    virtual ~MessageSenderAlg()      {
    }
    /// Initialize
    virtual StatusCode initialize()   {
      if ( m_procName.empty() )  {
        char txt[64];
        ::lib_rtl_get_process_name(txt, sizeof(txt));
        m_procName = txt;
      }
      m_prod = new Consumer(m_buffer,m_procName,m_partitionID);
      return StatusCode::SUCCESS;
    }
    /// Finalize
    virtual StatusCode finalize()     {    
      if ( m_prod ) delete m_prod;
      m_prod = 0;
      m_procName = "";
      return StatusCode::SUCCESS;
    }
    /// Execute procedure
    virtual StatusCode execute()    {
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MessageSenderAlg);

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/DecisionSetterAlg.cpp,v 1.9 2006-01-12 12:56:34 frankb Exp $
//	====================================================================
//  DecisionSetterAlg.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiOnline/MEPManager.h"
#include "MDF/RawEventDescriptor.h"
#include "MBM/Producer.h"

using namespace MBM;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class DecisionSetterAlg 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class DecisionSetterAlg : public Algorithm   {
    IMEPManager*  m_mepMgr;
    MEPID         m_mepID;
    Producer*     m_prod;
  public:
    /// Standard algorithm constructor
    DecisionSetterAlg(const std::string& name, ISvcLocator* pSvcLocator)
    :	Algorithm(name, pSvcLocator), m_mepMgr(0), m_mepID(MEP_INV_DESC), m_prod(0) {                                
    }
    /// Standard Destructor
    virtual ~DecisionSetterAlg()      {                                 }
    /// Initialize
    virtual StatusCode initialize()   {
      StatusCode status = service("MEPManager",m_mepMgr);
      if ( !status.isSuccess() )   {
        return status;
      }
      m_mepID = m_mepMgr->mepID();
      m_prod = new Producer(m_mepID->resBuffer,m_mepID->processName,m_mepID->partitionID);
      mbm_register_free_event(m_mepID->resBuffer,  0, 0);
      mbm_register_alloc_event(m_mepID->resBuffer, 0, 0);
      return StatusCode::SUCCESS;
    }

    /// Finalize
    virtual StatusCode finalize()     {    
      if ( m_mepMgr )  {
        m_mepMgr->release();
        m_mepMgr = 0;
      }
      if ( m_prod )  {
        delete m_prod;
        m_prod = 0;
      }
      return StatusCode::SUCCESS;
    }
    /// Execute procedure
    virtual StatusCode execute()    {
      SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
      if ( evt )  {
        IRegistry* reg = evt->registry();
        if ( reg )   {
          GenericAddress* addr = dynamic_cast<GenericAddress*>(reg->address());
          RawEventDescriptor* src = (RawEventDescriptor*)addr->ipar()[1];
          size_t len = sizeof(RawEventHeader)+(src->numberOfFragments()-1)*sizeof(int);
          if ( m_prod->getSpace(len) == MBM_NORMAL ) {
            EventDesc& e = m_prod->event();
            e.type    = src->eventType();
            e.len     = len;
            ::memcpy(e.data, src->header(), len);
            ::memcpy(e.mask, src->triggerMask(), sizeof(e.mask));
            // Add here optional data to the output record....

            if ( m_prod->sendEvent() == MBM_NORMAL )  {
              return StatusCode::SUCCESS;
            }
          }
        }
      }
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,DecisionSetterAlg);

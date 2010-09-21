// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/DecisionSetterAlg.cpp,v 1.18 2010-09-21 14:28:25 frankb Exp $
//  ====================================================================
//  DecisionSetterAlg.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiOnline/MEPManager.h"
#include "MDF/RawEventDescriptor.h"
#include "MDF/RawDataAddress.h"
#include "MBM/Producer.h"
#include <cstring>

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
    :  Algorithm(name, pSvcLocator), m_mepMgr(0), m_mepID(MEP_INV_DESC), m_prod(0) {                                
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
      // mbm_register_free_event(m_mepID->evtBuffer,  0, 0);
      // mbm_register_free_event(m_mepID->resBuffer,  0, 0);
      // mbm_register_alloc_event(m_mepID->resBuffer, 0, 0);
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
      //static int cnt = 0;
      //if ( !((++cnt%10)==0) ) return StatusCode::SUCCESS;
      SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
      if ( evt )  {
        IRegistry* reg = evt->registry();
        if ( reg )   {
          RawDataAddress* addr = dynamic_cast<RawDataAddress*>(reg->address());
          RawEventDescriptor* src = (RawEventDescriptor*)addr->ipar()[0];

          size_t len = sizeof(RawEventHeader)+(src->numberOfFragments()-1)*sizeof(int);
          if ( m_prod->getSpace(len) == MBM_NORMAL ) {
            EventDesc& e = m_prod->event();
            e.type    = src->eventType();
            e.len     = len;
            ::memcpy(e.data, src->header(), len);
            ::memcpy(e.mask, src->triggerMask(), sizeof(e.mask));
            // Add here optional data to the output record....
            // MEPEVENT* me = (MEPEVENT*)(int*)(m_mepID->mepStart + src->header()->m_begin);
            // ::printf("MEP: %d   count:%d Pattern:%08X Valid:%d ",
            //  me->mepBufferID, me->refCount, me->magic, me->valid);
            if ( m_prod->sendEvent() == MBM_NORMAL )  {
              // ::printf(" Sent:%d\n",me->refCount);
              return StatusCode::SUCCESS;
            }
          }
        }
      }
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,DecisionSetterAlg)

// $Header:  $
//  ====================================================================
//  OdinPrescaler.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: $
// Framework include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/OnlineRunInfo.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <cstdlib>
#include <vector>
#include <ctime>

/*
 *  LHCb namespace declaration
 */
namespace LHCb {

  /**@class OdinPrescaler OdinPrescaler.cpp
   *
   * Allows to prescale events given a certain trigger type mask.
   * If the mask is exactly ZERO, it is ignored. Otherwise only
   * a configurable fraction of events, with a non-zero and mask
   * with the ODIN triggertype pass.
   *
   *
   * @author:  M.Frank
   * @version: 1.0
   */
  class GAUDI_API OdinPrescaler: public Algorithm   {
  public:
    /// Standard Algorithm Constructor(s)
    OdinPrescaler(const std::string& nam, ISvcLocator* svc) 
      : Algorithm(nam,svc), m_seed(0)
    {
      declareProperty("AcceptRate", m_rate=1.0,
                      "Fraction of the events allowed to pass the filter");
      declareProperty("DownscaleTriggerTypes", m_downScale, 
                      "Downscaled ODIN trigger types");
      declareProperty("PassthroughTriggerTypes", m_passThrough, 
                      "Downscaled ODIN trigger types");
      declareProperty("BankLocation",m_bankLocation=RawEventLocation::Default);
    }

    /// Default Destructor
    virtual ~OdinPrescaler( ) {}

    /// Algorithm overload: Initialize the algorithm
    virtual StatusCode initialize() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Start the algorithm
    virtual StatusCode start() {
      m_seed = InterfaceID::hash32((RTL::processName()+"@"+RTL::nodeName()).c_str());
      ::srand(m_seed);
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Finalize the algorithm
    virtual StatusCode finalize() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Event execution routine
    virtual StatusCode execute() {
      double frac = double(::rand()) / double(RAND_MAX);
      //double frac = double(::rand_r(&m_seed)) / double(RAND_MAX);
      std::vector<int>::const_iterator vi;
      MsgStream log(msgSvc(),name());
      DataObject* pDO = 0;

      log << MSG::DEBUG << "Fraction:" << frac << " Rate:" << m_rate << endmsg;
      StatusCode sc = eventSvc()->retrieveObject(m_bankLocation,pDO);
      if ( sc.isSuccess() ) {
        typedef std::vector<RawBank*> _V;
        RawEvent *raw = (RawEvent*)pDO;
        const _V& bnks = raw->banks(RawBank::ODIN);
        for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
          RawBank* bank = *i;
          const OnlineRunInfo* info = bank->begin<OnlineRunInfo>();
          int triggerType = info->triggerType;
#if 0
          int readoutType = info->readoutType;
          int eventType   = info->EventType;
#endif
          if ( (vi=std::find(m_passThrough.begin(),m_passThrough.end(),triggerType)) != 
               m_passThrough.end() )  {
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
          else if ( (vi=std::find(m_downScale.begin(),m_downScale.end(),triggerType)) != 
                    m_downScale.end() )  {
            setFilterPassed(m_rate>=frac);
            return StatusCode::SUCCESS;
          }
          else if ( m_downScale.empty() && m_passThrough.empty() ) {
            setFilterPassed(m_rate>=frac);
            return StatusCode::SUCCESS;
          }
          else {
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
        }
      }
      return StatusCode::SUCCESS;
    }

  private:
    /// Property: Raw bank location
    std::string      m_bankLocation;
    /// Property: Percentage of events that should be passed
    double           m_rate;
    /// Property: Odin event type to be filtered
    std::vector<int> m_downScale;
    /// Property: Odin event type for forced pass-through
    std::vector<int> m_passThrough;
    /// Random number seed.
    unsigned int m_seed;
  };
}

#include "GaudiKernel/AlgFactory.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,OdinPrescaler)

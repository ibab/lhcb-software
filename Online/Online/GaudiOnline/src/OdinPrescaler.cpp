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

// C/C++ include files
#include <cstdlib>
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
      : Algorithm(nam,svc)
    {
      ::srand((unsigned int)::time(0));
      declareProperty("AcceptRate", m_rate=1.0,
		      "Fraction of the events allowed to pass the filter");
      declareProperty("TriggerType", m_trgType=0, 
		      "Required ODIN trigger type; if 0 -> ignored");
      declareProperty("BankLocation",m_bankLocation=RawEventLocation::Default);
    }

    /// Default Destructor
    virtual ~OdinPrescaler( ) {}

    /// Algorithm overload: Initialize the algorithm
    virtual StatusCode initialize() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Finalize the algorithm
    virtual StatusCode finalize() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Event execution routine
    virtual StatusCode execute() {
      MsgStream log(msgSvc(),name());
      DataObject* pDO = 0;
      double frac = double(rand()) / double(RAND_MAX);
      log << MSG::DEBUG << "Trg type:" << m_trgType << " Fraction:" << frac
	  << " Rate:" << m_rate << endmsg;
      if ( m_trgType == 0 ) {
	if ( frac > m_rate ) {
	  setFilterPassed(false);
	}
	return StatusCode::SUCCESS;
      }
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
	  if ( (triggerType&m_trgType) != 0 ) {
	    if ( frac > m_rate ) {
	      setFilterPassed( false );
	      return StatusCode::SUCCESS;
	    }
	  }
	}
      }
      return StatusCode::SUCCESS;
    }


  private:
    /// Raw bank location
    std::string m_bankLocation;
    /// Percentage of events that should be passed
    double m_rate;
    /// Odin event type to be filtered
    int m_trgType;
  };
}

#include "GaudiKernel/AlgFactory.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,OdinPrescaler)

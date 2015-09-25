// $Header:  $
//  ====================================================================
//  OdinCheckAlg.cpp
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

/*
 *  LHCb namespace declaration
 */
namespace LHCb {

  /**@class OdinCheckAlg OdinCheckAlg.cpp
   *
   * Check the ODIN bank in each event for consistency
   *
   * @author:  M.Frank
   * @version: 1.0
   */
  class GAUDI_API OdinCheckAlg: public Algorithm   {
  public:
    /// Standard Algorithm Constructor(s)
    OdinCheckAlg(const std::string& nam, ISvcLocator* svc) 
      : Algorithm(nam,svc)
    {
      declareProperty("BankLocation",m_bankLocation=RawEventLocation::Default);
    }

    /// Default Destructor
    virtual ~OdinCheckAlg( ) {}

    /// Algorithm overload: Initialize the algorithm
    virtual StatusCode initialize() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Start the algorithm
    virtual StatusCode start() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Finalize the algorithm
    virtual StatusCode finalize() {
      return StatusCode::SUCCESS; 
    }

    /// Algorithm overload: Event execution routine
    virtual StatusCode execute() {
      typedef std::vector<RawBank*> _V;
      DataObject* pDO = 0;
      StatusCode sc = eventSvc()->retrieveObject(m_bankLocation,pDO);
      if ( !sc.isSuccess() ) {
	MsgStream err(msgSvc(),name());
	err << MSG::ERROR << "No RawEvent object found!" << endmsg;
      }
      const _V& bnks = ((RawEvent*)pDO)->banks(RawBank::ODIN);
      if ( bnks.empty() )   {
	MsgStream err(msgSvc(),name());
	err << MSG::ERROR << "Missing ODIN banks for this event!" << endmsg;
      }
      if ( bnks.size() != 1 )   {
	MsgStream err(msgSvc(),name());
	err << MSG::ERROR << "Wrong number of ODIN banks (" << bnks.size() 
	    << ") for this event!" << endmsg;
      }
      for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
	RawBank* bank = *i;
	bool ok = true;
	if ( !bank )  {
	  MsgStream err(msgSvc(),name());
	  err << MSG::ERROR << "INVALID ODIN bank pointer (0)." << endmsg;
	  continue;
	}
	if ( bank->type() != RawBank::ODIN )   {
	  MsgStream err(msgSvc(),name());
	  err << MSG::ERROR << "Wrong bank type of ODIN bank: " << bank->type() << endmsg;
	  ok = false;
	}
	if ( bank->magic() != RawBank::MagicPattern )  {
	  MsgStream err(msgSvc(),name());
	  err << MSG::ERROR << "Wrong magic word of ODIN bank: " 
	      << std::hex << bank->magic() << std::dec << endmsg;
	  ok = false;
	}
	if ( ok && outputLevel() <= MSG::INFO )   {
	  MsgStream log(msgSvc(),name());
          const OnlineRunInfo* info = bank->begin<OnlineRunInfo>();
	  log << MSG::INFO << "ODIN OK. Run:" << info->Run << " Event:" << info->L0ID
	      << " Orbit:" << info->Orbit << " BunchID:" << info->bunchID << endmsg;
	}
      }
      return StatusCode::SUCCESS;
    }

  private:
    /// Property: Raw bank location
    std::string      m_bankLocation;
  };
}

#include "GaudiKernel/AlgFactory.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,OdinCheckAlg)

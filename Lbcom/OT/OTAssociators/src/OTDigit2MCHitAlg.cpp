// $Id: OTDigit2MCHitAlg.cpp,v 1.1 2003-06-10 09:04:16 jvantilb Exp $

// Event
#include "Event/OTDigit.h"
#include "Event/MCOTDigit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCTruth.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "OTDigit2MCHitAlg.h"
#include "OTAssociators/OTDigit2MCHitAsct.h"

/** @file OTDigit2MCHitAlg.cpp 
 *
 *  Implementation of the OTDigit2MCHitAlg class
 *  
 *  @author J. van Tilburg
 *  @date   05/06/2003
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTDigit2MCHitAlg>          s_factory ;
const        IAlgFactory& OTDigit2MCHitAlgFactory = s_factory ; 

OTDigit2MCHitAlg::OTDigit2MCHitAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTDigit2MCHitLocation );
  declareProperty( "SpillOver", m_spillOver  = false );
  declareProperty( "associatorName", m_nameAsct = "OTDigit2MCDepositAsct" );
}

OTDigit2MCHitAlg::~OTDigit2MCHitAlg() 
{
  // destructor
}

StatusCode OTDigit2MCHitAlg::initialize() 
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialize" << endmsg;

  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if( sc.isFailure() || 0 == m_hAsct) {
    msg << MSG::FATAL << "Unable to retrieve Associator tool" << endmsg;
    return sc;
  }
 
  return StatusCode::SUCCESS;
}


StatusCode OTDigit2MCHitAlg::execute() 
{
  typedef Relation1D<OTDigit, MCHit> Table;

  // get OTDigits
  SmartDataPtr<OTDigits> digitCont(eventSvc(), OTDigitLocation::Default);
  if (0 == digitCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTDigits" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // get the MCHits for the event, in case you do not 
  // want to make links to spillover
  SmartDataPtr<MCHits> mcHits(eventSvc(), MCHitLocation::OTHits);
  if( 0 == mcHits){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Could not find MCHits in " 
        << MCHitLocation::OTHits << endmsg;
    return StatusCode::FAILURE;
  }
  m_mcHits = mcHits;

  // create an association table 
  Table* aTable = new Table();

  // loop and link OTDigits to MC truth
  OTDigits::const_iterator iterDigit;
  for(iterDigit = digitCont->begin(); 
      iterDigit != digitCont->end(); ++iterDigit){
    std::vector<MCHit*> hitVector;
    associateToTruth(*iterDigit, hitVector);
    std::vector<MCHit*>::iterator iHit = hitVector.begin();
    while (iHit != hitVector.end()) {
      aTable->relate(*iterDigit, *iHit);
      ++iHit;
    }
  } // loop iterDigit

  // register table in store
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "     *** Could not register " << outputData()
        << endmsg;
    delete aTable;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode OTDigit2MCHitAlg::finalize() 
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endmsg;

  // Release tools
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}


StatusCode OTDigit2MCHitAlg::associateToTruth(const OTDigit* aDigit,
                                              std::vector<MCHit*>& hitVector) 
{
  // make link to truth to MCHit
  // retrieve table
  OTDigit2MCDepositAsct::DirectType* aTable = m_hAsct->direct();
  if (0 == aTable){
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find table" << endmsg;
    return StatusCode::FAILURE;
  }
 
  OTDigit2MCDepositAsct::MCDeposits range = aTable->relations(aDigit);
  if ( !range.empty() ) {
    OTDigit2MCDepositAsct::MCDepositsIterator iterDep;
    for (iterDep = range.begin(); iterDep != range.end(); ++iterDep) {      
      MCOTDeposit* aDeposit = iterDep->to();
      if (0 != aDeposit) {
        MCHit* aHit = aDeposit->mcHit();
        if (0 != aHit) {
          if (m_spillOver || m_mcHits == aHit->parent() ) 
            hitVector.push_back(aHit);
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

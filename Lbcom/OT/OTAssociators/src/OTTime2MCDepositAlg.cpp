// $Id: OTTime2MCDepositAlg.cpp,v 1.2 2004-11-10 12:59:57 jnardull Exp $

// Event
#include "Event/MCTruth.h"

// local
#include "OTTime2MCDepositAlg.h"
#include "OTAssociators/OTTime2MCDepositAsct.h"

/** @file OTTime2MCDepositAlg.cpp 
 *
 *  Implementation of the OTTime2MCDepositAlg class
 *  
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTTime2MCDepositAlg>          s_factory ;
const        IAlgFactory& OTTime2MCDepositAlgFactory = s_factory ; 

OTTime2MCDepositAlg::OTTime2MCDepositAlg( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTTime2MCDepositLocation );
  declareProperty( "acceptTime", m_acceptTime  = 7.8*ns );
}

OTTime2MCDepositAlg::~OTTime2MCDepositAlg() {
  // destructor
}

StatusCode OTTime2MCDepositAlg::initialize() 
{
  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCDepositAlg::execute() 
{

  StatusCode sc;

 // get OTTimes
  SmartDataPtr<OTTimes> timeCont(eventSvc(), OTTimeLocation::Default);
  if (0 == timeCont){ 
    return Error ("Failed to find OTTimes");
  }

  // Retrieve MCOTTime
  SmartDataPtr<MCOTTimes>
    mcTime ( eventDataService(), MCOTTimeLocation::Default);
  if ( !mcTime ) {
    msg () << "Could not find MCOTTime in " << MCOTTimeLocation::Default 
           << endreq;
    return StatusCode::FAILURE;
  }
  sc  = setMCTruth(timeCont, mcTime);
  if ( !sc.isSuccess() )  {
    return Error (" Failed to set the MC truth link ",sc);
  }

  // create an association table 
  OTTime2MCDepositAsct::Table* aTable = new OTTime2MCDepositAsct::Table();
  sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    msg () << "Could not register " << outputData() << endreq;
    delete aTable;
    return StatusCode::FAILURE;
  } 

  // loop and link OTTimes to MC truth
  OTTimes::const_iterator iterTime;
  for( iterTime = timeCont->begin(); iterTime != timeCont->end(); ++iterTime ) {
    std::vector<MCOTDeposit*> depVector;
    associateToTruth( *iterTime, depVector );
    std::vector<MCOTDeposit*>::iterator iDep = depVector.begin();
    while ( iDep != depVector.end() ) {
      aTable->relate(*iterTime, *iDep );
      ++iDep;
    }
  } // loop iterTime

  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCDepositAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode 
OTTime2MCDepositAlg::associateToTruth( const OTTime* aTime,
                                       std::vector<MCOTDeposit*>& depVector ) {

  // link time to truth
  const MCOTTime* mcTime = mcTruth<MCOTTime>(aTime);
  if (0 != mcTime) {

    // link to deposits
    SmartRefVector<MCOTDeposit> depCont = mcTime->deposits();
    if ( 0 == depCont.size()){
      msg () << " Deposits Size" << depCont.size() << endreq;
      return StatusCode::FAILURE;
    }
    
    unsigned tdcTime = aTime->tdcTime();

    SmartRefVector<MCOTDeposit>::iterator iterDep;
    for ( iterDep = depCont.begin(); iterDep != depCont.end(); ++iterDep ) {
      if( (*iterDep)->tdcTime() < tdcTime + m_acceptTime ) {
        depVector.push_back( *iterDep );
      }
    }
  }
  return StatusCode::SUCCESS;
}    


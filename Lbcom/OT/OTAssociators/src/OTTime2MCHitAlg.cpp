// $Id: OTTime2MCHitAlg.cpp,v 1.2 2004-11-10 12:59:57 jnardull Exp $

// local
#include "OTTime2MCHitAlg.h"
#include "OTAssociators/OTTime2MCHitAsct.h"

/** @file OTTime2MCHitAlg.cpp 
 *
 *  Implementation of the OTTime2MCHitAlg class
 *  
 *  @author J. Nardulli and J. van Tilburg
 *  @date   15/06/2004
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTTime2MCHitAlg>          s_factory ;
const        IAlgFactory& OTTime2MCHitAlgFactory = s_factory ; 

OTTime2MCHitAlg::OTTime2MCHitAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTTime2MCHitLocation );
  declareProperty( "SpillOver", m_spillOver  = false );
  declareProperty( "associatorName", m_nameAsct = "OTTime2MCDepositAsct" );
}

OTTime2MCHitAlg::~OTTime2MCHitAlg() 
{
  // destructor
}

StatusCode OTTime2MCHitAlg::initialize() 
{
  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if( sc.isFailure() || 0 == m_hAsct) {
    return Error ("Unable to retrieve Associator tool",sc);
  } 
  return StatusCode::SUCCESS;
}


StatusCode OTTime2MCHitAlg::execute() 
{
  // get OTTimes
  SmartDataPtr<OTTimes> timeCont(eventSvc(), OTTimeLocation::Default);
  if (0 == timeCont){ 
    return Error ("Failed to find OTTimes");
  }
  
  //Get the MCHits for the event, in case you don't want to make links to spill.
  SmartDataPtr<MCHits> mcHits(eventSvc(), MCHitLocation::OTHits);
  if( 0 == mcHits){
    msg() << "Could not find MCHits in " << MCHitLocation::OTHits 
          << endreq;
    return StatusCode::FAILURE;
  }
  m_mcHits = mcHits;

  // create an association table and register table in store
  OTTime2MCHitAsct::Table* aTable = new OTTime2MCHitAsct::Table();
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    msg() << "Could not register " << outputData() << endreq;
    delete aTable;
    return StatusCode::FAILURE;
  }

  // loop and link OTTimes to MC truth
  OTTimes::const_iterator iterTime;
  for ( iterTime = timeCont->begin(); 
        iterTime != timeCont->end(); ++iterTime){
    std::vector<MCHit*> hitVector;
    associateToTruth(*iterTime, hitVector);
    std::vector<MCHit*>::iterator iHit = hitVector.begin();
    while (iHit != hitVector.end()) {
      aTable->relate(*iterTime, *iHit);
      ++iHit;
    }
  } // loop iterTime

  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCHitAlg::finalize() 
{
  // Release tool
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}

StatusCode 
OTTime2MCHitAlg::associateToTruth(const OTTime* aTime,
                                   std::vector<MCHit*>& hitVector)
 
{
  // make link to truth to MCHit and retrieve table
  OTTime2MCDepositAsct::DirectType* aTable = m_hAsct->direct();
  if (0 == aTable){
    return Error ("Failed to find table");
  }
 
  OTTime2MCDepositAsct::MCDeposits range = aTable->relations(aTime);
  if ( !range.empty() ) {
    OTTime2MCDepositAsct::MCDepositsIterator iterDep;
    for (iterDep = range.begin(); iterDep != range.end(); ++iterDep) {      
      MCOTDeposit* aDeposit = iterDep->to();
      if (0 != aDeposit) {
        MCHit* aHit = aDeposit->mcHit();
        if (0 != aHit) {
          if( m_spillOver || (m_mcHits == aHit->parent()) ) {
            hitVector.push_back( aHit );
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

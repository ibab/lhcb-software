// $Id: OTTime2MCHitAlg.cpp,v 1.3 2004-11-23 14:21:17 cattanem Exp $

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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_hAsct = tool<OTTime2MCDepositAsct::IAsct>( m_nameAsct );

  return StatusCode::SUCCESS;
}


StatusCode OTTime2MCHitAlg::execute() 
{
  // get OTTimes
  OTTimes* timeCont = get<OTTimes>( OTTimeLocation::Default );
  
  //Get the MCHits for the event, in case you don't want to make links to spill.
  m_mcHits = get<MCHits>( MCHitLocation::OTHits );

  // create an association table and register table in store
  OTTime2MCHitAsct::Table* aTable = new OTTime2MCHitAsct::Table();
  put( aTable, outputData() );

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

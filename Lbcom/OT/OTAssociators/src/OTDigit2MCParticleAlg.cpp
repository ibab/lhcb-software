// $Id: OTDigit2MCParticleAlg.cpp,v 1.1 2003-06-10 09:04:16 jvantilb Exp $

// Event
#include "Event/OTDigit.h"
#include "Event/MCOTDigit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "OTDigit2MCParticleAlg.h"
#include "OTAssociators/OTDigit2MCParticleAsct.h"

/** @file OTDigit2MCParticleAlg.cpp 
 *
 *  Implementation of the OTDigit2MCParticleAlg class
 *  
 *  @author J. van Tilburg
 *  @date   05/06/2003
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTDigit2MCParticleAlg>          s_factory ;
const        IAlgFactory& OTDigit2MCParticleAlgFactory = s_factory ; 

OTDigit2MCParticleAlg::OTDigit2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTDigit2MCParticleLocation );
  declareProperty( "associatorName", m_nameAsct = "OTDigit2MCHitAsct" );
}

OTDigit2MCParticleAlg::~OTDigit2MCParticleAlg() {
  // destructor
}

StatusCode OTDigit2MCParticleAlg::initialize() {

  MsgStream msg(msgSvc(), name());
  StatusCode sc;
  msg << MSG::DEBUG << "==> Initialize" << endmsg;

  sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if( sc.isFailure() || 0 == m_hAsct) {
    msg << MSG::FATAL << "Unable to retrieve Associator tool" << endmsg;
    return sc;
  } 
  return StatusCode::SUCCESS;
}


StatusCode OTDigit2MCParticleAlg::execute() {

  typedef Relation1D<OTDigit, MCParticle>    Table;

  // get OTDigits
  SmartDataPtr<OTDigits> digitCont(eventSvc(), OTDigitLocation::Default);
  if (0 == digitCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTDigits" << endmsg;
    return StatusCode::FAILURE;
  }

  // create an association table 
  Table* aTable = new Table();

  // loop and link OTDigits to MC truth
  OTDigits::const_iterator iterDigit;
  for(iterDigit = digitCont->begin(); 
      iterDigit != digitCont->end(); ++iterDigit){
    std::vector<MCParticle*> partVector;
    associateToTruth(*iterDigit, partVector);
    std::vector<MCParticle*>::iterator iPart = partVector.begin();
    while (iPart != partVector.end()) {
      aTable->relate(*iterDigit, *iPart);
      ++iPart;
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

StatusCode OTDigit2MCParticleAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endmsg;

  // Release tools
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}

StatusCode 
OTDigit2MCParticleAlg::associateToTruth(const OTDigit* aDigit,
                                        std::vector<MCParticle*>& partVector) {
  // make truth link to MCParticle
  StatusCode sc = StatusCode::SUCCESS;

  // retrieve table
  OTDigit2MCHitAsct::DirectType* aTable = m_hAsct->direct();
  if (0 == aTable){
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find table" << endmsg;
    return StatusCode::FAILURE;
  }
 
  OTDigit2MCHitAsct::MCHits range = aTable->relations(aDigit);
  if ( !range.empty() ) {
    OTDigit2MCHitAsct::MCHitsIterator iterHit;
    for (iterHit = range.begin(); iterHit != range.end(); ++iterHit) {
      MCHit* aHit = iterHit->to();
      if (0 != aHit) {
        MCParticle* aParticle = aHit->mcParticle();
        if (0 != aParticle) partVector.push_back(aParticle);
      }
    }
  }

  return sc;
}

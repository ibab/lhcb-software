
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Event/MCParticle.h"
#include "Event/ParticleID.h"

#include "RichMaPMTSignal.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMaPMTSignal>  s_factory ;
const         IAlgFactory& RichMaPMTSignalFactory = s_factory ;

// Standard constructor, initializes variables
RichMaPMTSignal::RichMaPMTSignal( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

  declareProperty( "HitLocation",
                   m_RichHitLocation =  MCRichHitLocation::Default );
  declareProperty( "PrevLocation",
                   m_RichPrevLocation = "Prev/" + MCRichHitLocation::Default );
  declareProperty( "PrevPrevLocation",
                   m_RichPrevPrevLocation = "PrevPrev/" + MCRichHitLocation::Default );
  declareProperty( "NextLocation",
                   m_RichNextLocation = "Next/" + MCRichHitLocation::Default );
  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );

}

RichMaPMTSignal::~RichMaPMTSignal() {};

StatusCode RichMaPMTSignal::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  // detector tool
  if ( !toolSvc()->retrieveTool("RichDetInterface" , m_DetInterface ) ) {
    log << MSG::FATAL << "    Unable to create RichDetInterface" << endreq;
    return StatusCode::FAILURE;
  }

  m_BunchSpace = 25.0;

  log << MSG::DEBUG
      << " Using MaPMT signal algorithm" << endreq;
  log << MSG::WARNING
      << " Dividing energy by 10 to fix problem in GaussRICH. Remove when fixed."
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichMaPMTSignal::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Execute" << endreq;

  // Form a new container of MCRichSummedDeposits
  mcSummedDeposits = new MCRichSummedDeposits();

  // Form a new container of MCRichDeposits
  mcDeposits = new MCRichDeposits();

  // Process main and spillover events
  ProcessEvent( m_RichHitLocation, 0    );
  ProcessEvent( m_RichPrevLocation, -25 );
  ProcessEvent( m_RichPrevPrevLocation, -50 ) ;
  ProcessEvent( m_RichNextLocation, 25 ) ;

  // Register new containers to Gaudi data store
  if ( !eventSvc()->registerObject(m_RichSummedDepositLocation,mcSummedDeposits) ) {
    log << MSG::ERROR << "Failed to register MCRichSummedDeposits at "
        << m_RichSummedDepositLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully registered " << mcSummedDeposits->size()
        << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq;
  }
  if ( !eventSvc()->registerObject(m_RichDepositLocation,mcDeposits) ) {
    log << MSG::ERROR << "Failed to register MCRichDeposits at "
        << m_RichDepositLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully registered " << mcDeposits->size()
        << " MCRichDeposits at " << m_RichDepositLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichMaPMTSignal::ProcessEvent( std::string hitLoc,
                                          double tofOffset ) {

  MsgStream log(msgSvc(), name());

  // Load hits
  SmartDataPtr<MCRichHits> hits( eventSvc(), hitLoc );
  if ( !hits ) {
    log << MSG::DEBUG << "Cannot locate MCRichHits at " << hitLoc << endreq;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::DEBUG << "Successfully located " << hits->size()
        << " MCRichHits at " << hitLoc << endreq;
  }

  for ( MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit ) {

    // Select hits
    if ( !(*iHit)->chargedTrack() ) {

      RichSmartID id(0);
      StatusCode sc = m_DetInterface->smartID( (*iHit)->entry(), id );
      // Is hit in active pixel
      if ( sc.isSuccess() && 0 != id.index() ) {

        // Then create a new deposit
        MCRichDeposit* newDeposit = new MCRichDeposit();
        mcDeposits->insert( newDeposit );
        newDeposit->setParentHit( *iHit );
        newDeposit->setEnergy( (*iHit)->energy()/10 );
        newDeposit->setTime( tofOffset + (*iHit)->timeOfFlight() );

        // Add to the set of other deposits in the pixel
        MCRichSummedDeposit * dep = mcSummedDeposits->object(id);
        if ( !dep ) {
          dep = new MCRichSummedDeposit();
          mcSummedDeposits->insert( dep, id );
        }
        dep->addToDeposits(newDeposit);

      } // active hit if

    } // hit selection

  } // hit loop

  return StatusCode::SUCCESS;
}


StatusCode RichMaPMTSignal::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  if ( m_DetInterface ) { toolSvc()->releaseTool(m_DetInterface); m_DetInterface=0; }

  return StatusCode::SUCCESS;
}

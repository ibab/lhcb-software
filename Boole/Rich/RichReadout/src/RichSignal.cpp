
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Event/MCParticle.h"
#include "Event/ParticleID.h"

#include "RichSignal.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSignal>  s_factory ;
const         IAlgFactory& RichSignalFactory = s_factory ;

// Standard constructor, initializes variables
RichSignal::RichSignal( const std::string& name,
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
  declareProperty( "NextNextLocation",
                   m_RichNextNextLocation = "NextNext/" + MCRichHitLocation::Default );
  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );

}

RichSignal::~RichSignal() {};

StatusCode RichSignal::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
    msg << MSG::FATAL << "Unable to create Random generator" << endreq;
    return StatusCode::FAILURE;
  }

  // detector tool
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_DetInterface ) ) {
    msg << MSG::FATAL << "Unable to create RichDetInterface" << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG
      << " Using HPD signal algorithm" << endreq;
  msg << MSG::WARNING
      << " Dividing energy by 10 to fix problem in GaussRICH. Remove when fixed."
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::execute() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Execute" << endreq;

  // Form a new containers of MCRichSummedDeposits and MCRichDeposits
  mcSummedDeposits = new MCRichSummedDeposits();
  if ( !eventSvc()->registerObject(m_RichSummedDepositLocation,mcSummedDeposits) ) {
    msg << MSG::ERROR << "Failed to register MCRichSummedDeposits at "
        << m_RichSummedDepositLocation << endreq;
    return StatusCode::FAILURE;
  }
  mcDeposits = new MCRichDeposits();
  if ( !eventSvc()->registerObject(m_RichDepositLocation,mcDeposits) ) {
    msg << MSG::ERROR << "Failed to register MCRichDeposits at "
        << m_RichDepositLocation << endreq;
    return StatusCode::FAILURE;
  }

  // Process main and spillover events
  ProcessEvent( m_RichHitLocation,       0  );
  ProcessEvent( m_RichPrevLocation,     -25 );
  ProcessEvent( m_RichPrevPrevLocation, -50 );
  ProcessEvent( m_RichNextLocation,      25 );
  ProcessEvent( m_RichNextNextLocation,  50 );

  // Debug Printout
  msg << MSG::DEBUG << "Created " << mcSummedDeposits->size()
      << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq;
  msg << MSG::DEBUG << "Created " << mcDeposits->size()
      << " MCRichDeposits at " << m_RichDepositLocation << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::ProcessEvent( std::string hitLoc,
                                     double tofOffset ) {

  MsgStream msg(msgSvc(), name());

  // Load hits
  SmartDataPtr<MCRichHits> hits( eventSvc(), hitLoc );
  if ( !hits ) {
    msg << MSG::DEBUG << "Cannot locate MCRichHits at " << hitLoc << endreq;
    return StatusCode::SUCCESS;
  } else {
    msg << MSG::DEBUG << "Successfully located " << hits->size()
        << " MCRichHits at " << hitLoc << endreq;
  }

  for ( MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit ) {

    // Select hits
    if ( !(*iHit)->chargedTrack() ) {

      RichSmartID id(0);
      // Is hit in active pixel
      StatusCode sc = m_DetInterface->smartID( (*iHit)->entry(), id );
      if ( sc.isSuccess() && 0 != id.index() ) {

        // Find out if we already have a hit for this super-pixel
        MCRichSummedDeposit * dep = mcSummedDeposits->object(id);
        if ( tofOffset < -1 && dep ) {
          // Toss a coin to see if we add this hit to the existing deposits
          // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
          // already existing hit
          int iRan = (int)(m_rndm()*8.);
          if ( iRan != 0 ) continue;
        }

        // Then create a new deposit
        MCRichDeposit* newDeposit = new MCRichDeposit();
        mcDeposits->insert( newDeposit );
        newDeposit->setParentHit( *iHit );
        // Divide energy by 10 to fix Gauss energy problem
        newDeposit->setEnergy( (*iHit)->energy()/10 );
        newDeposit->setTime( tofOffset + (*iHit)->timeOfFlight() );

        // Add to the set of other deposits in the pixel
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


StatusCode RichSignal::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  if ( m_DetInterface ) { toolSvc()->releaseTool(m_DetInterface); m_DetInterface=0; }

  // finalize randomn number generator
  m_rndm.finalize();

  return StatusCode::SUCCESS;
}


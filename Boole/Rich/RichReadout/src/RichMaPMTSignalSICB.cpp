
#include "RichMaPMTSignalSICB.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMaPMTSignalSICB>  s_factory ;
const         IAlgFactory& RichMaPMTSignalSICBFactory = s_factory ;

// Standard constructor, initializes variables
RichMaPMTSignalSICB::RichMaPMTSignalSICB( const std::string& name,
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

RichMaPMTSignalSICB::~RichMaPMTSignalSICB() {};

StatusCode RichMaPMTSignalSICB::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  if ( !toolSvc()->retrieveTool( "MaPMTDetTool", m_mapmtDet ) ) {
    log << MSG::FATAL << "Unable to create MaPMTDetTool" << endreq;
    return StatusCode::FAILURE;
  }

  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
    log << MSG::FATAL << "Unable to create Random generator" << endreq;
    return StatusCode::FAILURE;
  }

  m_BunchSpace = 25.0;

  log << MSG::DEBUG
      << " Using MaPMT signal algorithm for SICB" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichMaPMTSignalSICB::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Execute" << endreq;

  // Form a new containers of MCRichSummedDeposits and MCRichDeposits
  mcSummedDeposits = new MCRichSummedDeposits();
  mcDeposits       = new MCRichDeposits();

  // Process main and spillover events with TOF offsets
  ProcessEvent( m_RichHitLocation,       0  );
  ProcessEvent( m_RichPrevLocation,     -25 );
  ProcessEvent( m_RichPrevPrevLocation, -50 );
  ProcessEvent( m_RichNextLocation,      25 );

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

StatusCode RichMaPMTSignalSICB::ProcessEvent( std::string hitLoc,
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

  // loop over hits
  for ( MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit ) {

    // Select normal hits
    // if( (*iHit)->radiator() <= 2 || !(*iHit)->scatteredPhoton() ) {
    if ( !(*iHit)->chargedTrack() ) {

      // positions on MaPMT window and correlated pixelID
      RichSmartID id;
      if ( m_mapmtDet->validateCdfHit((*iHit)->entry(), id) ) {

        // Then create a new deposit
        MCRichDeposit* newDeposit = new MCRichDeposit();
        mcDeposits->insert( newDeposit );
        newDeposit->setParentHit( *iHit );

        // Hit energy
        newDeposit->setEnergy( SimpleEnergy() );

        // TOF information
        double tof = tofOffset + (*iHit)->timeOfFlight()/1e9;
        if ( (*iHit)->rich() == Rich::Rich2 ) tof -= 30; // Global shift for Rich2
        newDeposit->setTime( tof );

        // Add to the set of other deposits in the pixel
        MCRichSummedDeposit * dep = mcSummedDeposits->object(id);
        if ( !dep ) {
          dep = new MCRichSummedDeposit();
          mcSummedDeposits->insert( dep, id );
        }
        dep->addToDeposits(newDeposit);

      } // id OK

    } // normal hit if

    // select charged track hits
    // Needs adding for MaPMTs
    //if ( (*iHit)->chargedTrack() ) {
    //} // end charged track hit if

  } // loop over MC Hits

  return StatusCode::SUCCESS;
}

StatusCode RichMaPMTSignalSICB::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endreq;

  // release pixel finder
  if (m_mapmtDet) { toolSvc()->releaseTool(m_mapmtDet); m_mapmtDet = NULL; }

  // finalize randomn number generator
  m_rndm.finalize();

  return StatusCode::SUCCESS;
}

double RichMaPMTSignalSICB::SimpleEnergy() {

  double energy = 0.015;
  if ( m_rndm() <= 0.2 ) {
    return m_rndm()*energy;
  } else {
    return energy;
  }

  return 0; // shouldn't happen
}

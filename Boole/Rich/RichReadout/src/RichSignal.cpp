
#include "RichSignal.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSignal>  s_factory;
const         IAlgFactory& RichSignalFactory = s_factory ;

// Standard constructor, initializes variables
RichSignal::RichSignal( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ) {

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
  declareProperty( "LHCBackgroundLocation",
                   m_lhcBkgLocation = "LHCBackground/" + MCRichHitLocation::Default );

  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );

  declareProperty( "UseSpillover",     m_doSpillover = true );
  declareProperty( "UseLHCBackground", m_doLHCBkg = true );

}

RichSignal::~RichSignal() {};

StatusCode RichSignal::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Initialize base class
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
    msg << MSG::FATAL << "Unable to create Random generator" << endreq;
    return StatusCode::FAILURE;
  }

  // detector tool
  acquireTool( "RichSmartIDTool", m_smartIDTool );

  msg << MSG::WARNING
      << "Dividing energy by 10 to fix problem in GaussRICH. Remove when fixed."
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

  // Process main event
  ProcessEvent( m_RichHitLocation,       0  );

  // if requested, process spillover events
  if ( m_doSpillover ) {
    ProcessEvent( m_RichPrevLocation,     -25 );
    ProcessEvent( m_RichPrevPrevLocation, -50 );
    ProcessEvent( m_RichNextLocation,      25 );
    // not needed yet
    //ProcessEvent( m_RichNextNextLocation,  50 );
  }

  // if requested, process LHC background
  if ( m_doLHCBkg ) { ProcessEvent( m_lhcBkgLocation, 0 ); }

  // Debug Printout
  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG << "Created " << mcSummedDeposits->size()
        << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq;
    msg << MSG::DEBUG << "Created " << mcDeposits->size()
        << " MCRichDeposits at " << m_RichDepositLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::ProcessEvent( const std::string & hitLoc,
                                     const double tofOffset ) const {

  // Load hits
  SmartDataPtr<MCRichHits> hits( eventSvc(), hitLoc );
  if ( !hits ) {
    if ( msgLevel(MSG::DEBUG) ) {
      MsgStream msg(msgSvc(), name());
      msg << MSG::DEBUG << "Cannot locate MCRichHits at " << hitLoc << endreq;
    }
    return StatusCode::SUCCESS;
  }
  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Successfully located " << hits->size()
        << " MCRichHits at " << hitLoc << endreq;
  }

  for ( MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit ) {

    RichSmartID tempID;
    // Is hit in active pixel
    if ( (m_smartIDTool->smartID((*iHit)->entry(),tempID)).isSuccess() 
         && tempID.isValid() ) {

      // For the time being strip sub-pixel information
      RichSmartID id = tempID.pixelID();
      
      // Find out if we already have a hit for this super-pixel
      MCRichSummedDeposit * dep = mcSummedDeposits->object(id);
      if ( tofOffset < -1 && dep ) {
        // Toss a coin to see if we add this hit to the existing deposits
        // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
        // already existing hit
        if ( 0 != static_cast<int>(m_rndm()*8.) ) continue;
      }

      // Then create a new deposit
      MCRichDeposit* newDeposit = new MCRichDeposit();
      mcDeposits->insert( newDeposit );
      newDeposit->setParentHit( *iHit );

      // energy
      newDeposit->setEnergy( (*iHit)->energy() );

      // TOF
      double tof = tofOffset + (*iHit)->timeOfFlight();
      // Global shift for Rich2.
      if ( (*iHit)->rich() == Rich::Rich2 ) tof -= 40;
      newDeposit->setTime( tof );

      // Add to the set of other deposits in the pixel
      if ( !dep ) {
        dep = new MCRichSummedDeposit();
        mcSummedDeposits->insert( dep, id );
      }
      dep->addToDeposits(newDeposit);

    } // active hit if

  } // hit loop

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_smartIDTool );

  // finalize randomn number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}


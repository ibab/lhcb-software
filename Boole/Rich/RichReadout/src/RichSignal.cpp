
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

  debug() << "Initialize :-" << endreq;

  // Initialize base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
    return Error( "Unable to create Random generator" );
  }

  // detector tool
  acquireTool( "RichSmartIDTool", m_smartIDTool );

  debug() << " Processing spillover events flag = " << m_doSpillover << endreq
          << " Processing LHC background flag   = " << m_doLHCBkg << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::execute() {

  debug() << "Execute" << endreq;

  // Form a new containers of MCRichSummedDeposits and MCRichDeposits
  mcSummedDeposits = new MCRichSummedDeposits();
  put( mcSummedDeposits, m_RichSummedDepositLocation );
  mcDeposits = new MCRichDeposits();
  put( mcDeposits, m_RichDepositLocation );

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
  debug() << "Created " << mcSummedDeposits->size()
          << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq
          << "Created " << mcDeposits->size()
          << " MCRichDeposits at " << m_RichDepositLocation << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::ProcessEvent( const std::string & hitLoc,
                                     const double tofOffset ) const {

  // Load hits
  SmartDataPtr<MCRichHits> hits( eventSvc(), hitLoc );
  if ( !hits ) {
    debug() << "Cannot locate MCRichHits at " << hitLoc << endreq;
    return StatusCode::SUCCESS;
  }
  debug() << "Successfully located " << hits->size()
          << " MCRichHits at " << hitLoc << endreq;

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

      // Hit energy
      newDeposit->setEnergy( (*iHit)->energy() );

      // TOF
      double tof = tofOffset + (*iHit)->timeOfFlight();
      // Global shift for Rich2.
      if ( Rich::Rich2 == (*iHit)->rich() ) tof -= 40;
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

  debug() << "Finalize" << endreq;

  // finalize randomn number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}


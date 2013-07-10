
//===============================================================================
/** @file RichSignal.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSignal
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSignal.h"

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( Signal )

// Standard constructor, initializes variables
Signal::Signal( const std::string& name,
                ISvcLocator* pSvcLocator )
  : Rich::AlgBase   ( name, pSvcLocator ),
    m_mcDeposits    ( NULL ),
    m_smartIDTool   ( NULL ),
    m_truth         ( NULL ),
    m_timeShift ( Rich::NRiches )
{

  declareProperty( "HitLocation",
                   m_RichHitLocation = LHCb::MCRichHitLocation::Default );
  declareProperty( "PrevLocation",
                   m_RichPrevLocation = "Prev/" + LHCb::MCRichHitLocation::Default );
  declareProperty( "PrevPrevLocation",
                   m_RichPrevPrevLocation = "PrevPrev/" + LHCb::MCRichHitLocation::Default );
  declareProperty( "NextLocation",
                   m_RichNextLocation = "Next/" + LHCb::MCRichHitLocation::Default );
  declareProperty( "NextNextLocation",
                   m_RichNextNextLocation = "NextNext/" + LHCb::MCRichHitLocation::Default );
  declareProperty( "LHCBackgroundLocation",
                   m_lhcBkgLocation = "LHCBackground/" + LHCb::MCRichHitLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = LHCb::MCRichDepositLocation::Default );

  m_timeShift[Rich::Rich1] = 0;
  m_timeShift[Rich::Rich2] = 40;
  declareProperty( "TimeCalib", m_timeShift );

  declareProperty( "UseSpillover",     m_doSpillover = true );
  declareProperty( "UseLHCBackground", m_doLHCBkg = true );
  declareProperty( "CheckSmartIDs", m_testSmartIDs = false );

}

Signal::~Signal() {}

StatusCode Signal::initialize()
{
  // Initialize base class
  StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  sc = m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) );
  if ( sc.isFailure() )
  {
    return Error( "Unable to initialise random number generator" );
  }

  // tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, NULL, true );
  acquireTool( "RichMCTruthTool", m_truth, NULL, true       );

  return sc;
}

StatusCode Signal::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endmsg;

  // Form new container of MCRichDeposits
  m_mcDeposits = new LHCb::MCRichDeposits();
  put( m_mcDeposits, m_RichDepositLocation );

  // Process main event
  // must be done first (so that first associated hit is signal)
  StatusCode sc = ProcessEvent( m_RichHitLocation, 0, 0 );

  // if requested, process spillover events
  if ( m_doSpillover )
  {
    sc = sc && ProcessEvent( m_RichPrevPrevLocation, -50, -2 );
    sc = sc && ProcessEvent( m_RichPrevLocation,     -25, -1 );
    sc = sc && ProcessEvent( m_RichNextLocation,      25,  1 );
    sc = sc && ProcessEvent( m_RichNextNextLocation,  50,  2 );
  }

  // if requested, process LHC background
  if ( m_doLHCBkg ) { sc = sc && ProcessEvent( m_lhcBkgLocation, 0, 0 ); }

  // Debug Printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created overall " << m_mcDeposits->size()
            << " MCRichDeposits at " << m_RichDepositLocation << endmsg;
  }

  return sc;
}

StatusCode Signal::ProcessEvent( const std::string & hitLoc,
                                 const double tofOffset,
                                 const int eventType ) const
{

  // Load hits
  if ( exist<LHCb::MCRichHits>(hitLoc) )
  {
    ++counter( "Found MCRichHits at " + hitLoc );
  }
  else { return StatusCode::SUCCESS; }
  LHCb::MCRichHits * hits = get<LHCb::MCRichHits>( hitLoc );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully located " << hits->size()
            << " MCRichHits at " << hitLoc << " : Pointer=" << hits << endmsg;
  }

  unsigned int nDeps(0);
  for ( LHCb::MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit )
  {

    // Get RichSmartID from MCRichHit (stripping sub-pixel info for the moment)
    const LHCb::RichSmartID id = (*iHit)->sensDetID().pixelID();

    // Run some checks
    if ( UNLIKELY(m_testSmartIDs) )
    {
      LHCb::RichSmartID tempID;
      const bool ok = (m_smartIDTool->smartID((*iHit)->entry(),tempID)).isSuccess();
      if      ( !ok )
      { 
        Warning( "Failed to compute RichSmartID from MCRichHit entry point" ).ignore(); 
      }
      else if ( id != tempID.pixelID() )
      { 
        Warning( "RichSmartID mis-match. Enable DEBUG for more details." ).ignore(); 
        if ( msgLevel(MSG::DEBUG) ) 
        {
          debug() << "Original " << id << endmsg;
          debug() << "New      " << tempID.pixelID() << endmsg;
        }
      }
      if ( msgLevel(MSG::VERBOSE) )
      {
        Gaudi::XYZPoint detectP;
        const StatusCode sc = m_smartIDTool->globalPosition( id, detectP );
        if ( sc.isFailure() )
        {
          Warning( "Problem translating RichSmartID into global position" ).ignore();
        }
        else
        {
          verbose() << id << endmsg;
          verbose() << " -> Hit   pos : " << (*iHit)->entry() << endmsg;
          verbose() << " -> Digit pos : " << detectP << endmsg;
        }
      }
    }

    // Create a new deposit
    LHCb::MCRichDeposit* dep = new LHCb::MCRichDeposit();
    m_mcDeposits->insert( dep );
    ++nDeps;

    // Set RichSmartID
    dep->setSmartID( id );

    // set parent hit
    dep->setParentHit( *iHit );

    // Hit energy
    dep->setEnergy( (*iHit)->energy() );

    // TOF
    const double tof = tofOffset + (*iHit)->timeOfFlight() - m_timeShift[(*iHit)->rich()];
    dep->setTime( tof );

    // get history from hit
    LHCb::MCRichDigitHistoryCode hist = (*iHit)->mcRichDigitHistoryCode();

    // add event type to history
    if      (  0 == eventType ) { hist.setSignalEvent(true);   }
    else if ( -1 == eventType ) { hist.setPrevEvent(true);     }
    else if ( -2 == eventType ) { hist.setPrevPrevEvent(true); }
    else if (  1 == eventType ) { hist.setNextEvent(true);     }
    else if (  2 == eventType ) { hist.setNextNextEvent(true); }

    // Update history in dep
    dep->setHistory( hist );

  } // hit loop

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << nDeps << " MCRichDeposits for " << hitLoc << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode Signal::finalize()
{
  // finalize random number generator
  const StatusCode sc = m_rndm.finalize();
  if ( sc.isFailure() ) Warning( "Failed to finalise random number generator" ).ignore();

  // finalize base class
  return Rich::AlgBase::finalize();
}


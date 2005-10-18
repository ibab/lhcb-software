
//===============================================================================
/** @file RichSignal.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSignal
 *
 *  CVS Log :-
 *  $Id: RichSignal.cpp,v 1.4 2005-10-18 12:43:06 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSignal.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSignal>  s_factory;
const         IAlgFactory& RichSignalFactory = s_factory ;

// Standard constructor, initializes variables
RichSignal::RichSignal( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichAlgBase        ( name, pSvcLocator ),
    m_mcSummedDeposits ( 0 ),
    m_mcDeposits       ( 0 )
{

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

StatusCode RichSignal::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  // tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
  acquireTool( "RichMCTruthTool", m_truth, 0, true       );

  return sc;
}

StatusCode RichSignal::execute()
{
  debug() << "Execute" << endreq;

  // Form new containers of MCRichSummedDeposits and MCRichDeposits
  m_mcSummedDeposits = new MCRichSummedDeposits();
  put( m_mcSummedDeposits, m_RichSummedDepositLocation );
  m_mcDeposits = new MCRichDeposits();
  put( m_mcDeposits, m_RichDepositLocation );

  // Process main event
  // must be done first (so that first associated hit is signal)
  ProcessEvent( m_RichHitLocation, 0, 0 );

  // if requested, process spillover events
  if ( m_doSpillover )
  {
    ProcessEvent( m_RichPrevPrevLocation, -50, -2 );
    ProcessEvent( m_RichPrevLocation,     -25, -1 );
    ProcessEvent( m_RichNextLocation,      25,  1 );
    ProcessEvent( m_RichNextNextLocation,  50,  2 );
  }

  // if requested, process LHC background
  if ( m_doLHCBkg ) { ProcessEvent( m_lhcBkgLocation, 0, 0 ); }

  // Debug Printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created overall " << m_mcDeposits->size()
            << " MCRichDeposits at " << m_RichDepositLocation << endreq
            << "Created overall " << m_mcSummedDeposits->size()
            << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::ProcessEvent( const std::string & hitLoc,
                                     const double tofOffset,
                                     const int eventType ) const
{

  // Load hits
  if ( !exist<MCRichHits>(hitLoc) )
  {
    //if ( msgLevel(MSG::DEBUG) )
    //{
    //  debug() << "Cannot locate MCRichHits at " << hitLoc << endreq;
    //}
    return Warning( "Cannot locate MCRichHits at " + hitLoc, 
                    StatusCode::SUCCESS, 0 );
  }
  MCRichHits * hits = get<MCRichHits>( hitLoc );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully located " << hits->size()
            << " MCRichHits at " << hitLoc << " : Pointer=" << hits << endreq;
  }

  unsigned int nDeps(0), nSumDeps(0);
  for ( MCRichHits::const_iterator iHit = hits->begin(); 
        iHit != hits->end(); ++iHit ) 
  {

    RichSmartID tempID;
    // Is hit in active pixel
    if ( (m_smartIDTool->smartID((*iHit)->entry(),tempID)).isSuccess()
         && tempID.pixelDataAreValid() ) {

      // For the time being strip sub-pixel information
      const RichSmartID id = tempID.pixelID();

      // Find out if we already have a hit for this super-pixel
      MCRichSummedDeposit * sumDep = m_mcSummedDeposits->object(id);
      if ( tofOffset < -1 && sumDep ) 
      {
        // Toss a coin to see if we add this hit to the existing deposits
        // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
        // already existing hit
        if ( 0 != static_cast<int>(m_rndm()*8.) ) continue;
      }

      // Then create a new deposit
      MCRichDeposit* dep = new MCRichDeposit();
      m_mcDeposits->insert( dep );
      ++nDeps;

      // set parent hit
      dep->setParentHit( *iHit );

      // Hit energy
      dep->setEnergy( (*iHit)->energy() );

      // TOF
      double tof = tofOffset + (*iHit)->timeOfFlight();
      // Global shift for Rich2.
      if ( Rich::Rich2 == (*iHit)->rich() ) tof -= 40;
      dep->setTime( tof );

      // Add to the set of other deposits in the pixel
      if ( !sumDep )
      {
        sumDep = new MCRichSummedDeposit();
        m_mcSummedDeposits->insert( sumDep, id );
        ++nSumDeps;
      }
      sumDep->addToDeposits(dep);

      // summed energy
      sumDep->setSummedEnergy( sumDep->summedEnergy() + dep->energy() );

      // store type event type
      if      (  0 == eventType ) { dep->setSignalEvent(true);   sumDep->setSignalEvent(true);   }
      else if ( -1 == eventType ) { dep->setPrevEvent(true);     sumDep->setPrevEvent(true);     }
      else if ( -2 == eventType ) { dep->setPrevPrevEvent(true); sumDep->setPrevPrevEvent(true); }
      else if (  1 == eventType ) { dep->setNextEvent(true);     sumDep->setNextEvent(true);     }
      else if (  2 == eventType ) { dep->setNextNextEvent(true); sumDep->setNextNextEvent(true); }

      // store history in summed deposit
      if ( !m_truth->isBackground( *iHit ) ) 
      {
        if      ( Rich::Aerogel == (*iHit)->radiator() ) { sumDep->setAerogelHit(true); }
        else if ( Rich::C4F10   == (*iHit)->radiator() ) { sumDep->setC4f10Hit(true);   }
        else if ( Rich::CF4     == (*iHit)->radiator() ) { sumDep->setCf4Hit(true);     }
      } 
      if ( (*iHit)->scatteredPhoton() ) { sumDep->setScatteredHit(true);  }
      if ( (*iHit)->chargedTrack()    ) { sumDep->setChargedTrack(true);  }
      if ( (*iHit)->backgroundHit()   ) { sumDep->setBackgroundHit(true); }

    } // active hit if

  } // hit loop

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << nDeps << " MCRichDeposits and " << nSumDeps
            << " MCRichSummedDeposits for " << hitLoc << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::finalize()
{
  // finalize random number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}


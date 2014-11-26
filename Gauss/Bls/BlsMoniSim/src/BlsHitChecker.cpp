// $Id: BlsHitChecker.cpp,v 1.1.1.2 2010-03-10 17:38:47 vtalanov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "Event/MCParticle.h"
#include "Event/MCHeader.h"

// from STL
#include <map>
#include <string>

// local
#include "BlsHitChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BlsHitChecker
//
// 2010-02-06 : Vadim Talanov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BlsHitChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

BlsHitChecker::BlsHitChecker( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
                     m_blsMCHits ( ),
               m_blsHitsLocation ( "MC/Bls/Hits" ),
                        m_blsAOn ( false ),
                        m_blsCOn ( false ),
               m_blsHTitlePrefix ( "BLS: " ),
                 m_blsHEntryXMin ( -150.0 * Gaudi::Units::mm),
                 m_blsHEntryXMax ( +150.0 * Gaudi::Units::mm),
               m_blsHEntryXNbins ( 300 ),
                 m_blsHEntryYMin ( -150.0 * Gaudi::Units::mm),
                 m_blsHEntryYMax ( +150.0 * Gaudi::Units::mm),
               m_blsHEntryYNbins ( 300 ),
                 m_blsHEntryZMin ( -2200.0 * Gaudi::Units::mm),
                 m_blsHEntryZMax ( -1900.0 * Gaudi::Units::mm),
               m_blsHEntryZNbins ( 300 ),
           m_blsHEntryTimeOffset ( 0.0 * Gaudi::Units::ns ),
              m_blsHEntryTimeMin ( -50.0 * Gaudi::Units::ns ),
              m_blsHEntryTimeMax ( +50.0 * Gaudi::Units::ns ),
            m_blsHEntryTimeNbins ( 100 ),
             m_blsHTrackEnDepMin ( 0.0 ),
             m_blsHTrackEnDepMax ( 50.0 ),
           m_blsHTrackEnDepNbins ( 500 ),
            m_blsHTrackLengthMin ( 0.0 ),
            m_blsHTrackLengthMax ( 7.0 ),
          m_blsHTrackLengthNbins ( 70 ),
         m_blsHEventNumTracksMin ( 0.0 ),
         m_blsHEventNumTracksMax ( 50.0 ),
       m_blsHEventNumTracksNbins ( 50 ),
               m_blsHEventNumMin ( 0.0 ),
               m_blsHEventNumMax ( 1000.0 ),
             m_blsHEventNumNbins ( 1000 ),
                       m_blsHits ( 0 ),
                     m_blsTracks ( 0 )
{
  declareProperty( "HitsLocation", m_blsHitsLocation );
  declareProperty( "BlsAOn", m_blsAOn );
  declareProperty( "BlsCOn", m_blsCOn );
  declareProperty( "HistogramTitlePrefix", m_blsHTitlePrefix );
  declareProperty( "EntryXMin", m_blsHEntryXMin );
  declareProperty( "EntryXMax", m_blsHEntryXMax );
  declareProperty( "EntryXNbins", m_blsHEntryXNbins );
  declareProperty( "EntryYMin", m_blsHEntryYMin );
  declareProperty( "EntryYMax", m_blsHEntryYMax );
  declareProperty( "EntryYNbins", m_blsHEntryYNbins );
  declareProperty( "EntryZMin", m_blsHEntryZMin );
  declareProperty( "EntryZMax", m_blsHEntryZMax );
  declareProperty( "EntryZNbins", m_blsHEntryZNbins );
  declareProperty( "EntryTimeOffset", m_blsHEntryTimeOffset );
  declareProperty( "EntryTimeMin", m_blsHEntryTimeMin );
  declareProperty( "EntryTimeMax", m_blsHEntryTimeMax );
  declareProperty( "EntryTimeNbins", m_blsHEntryTimeNbins );
  declareProperty( "TrackEnDepMin", m_blsHTrackEnDepMin );
  declareProperty( "TrackEnDepMax", m_blsHTrackEnDepMax );
  declareProperty( "TrackEnDepNbins", m_blsHTrackEnDepNbins );
  declareProperty( "TrackLengthMin", m_blsHTrackLengthMin );
  declareProperty( "TrackLengthMax", m_blsHTrackLengthMax );
  declareProperty( "TrackLengthNbins", m_blsHTrackLengthNbins );
  declareProperty( "EventNumTracksMin", m_blsHEventNumTracksMin );
  declareProperty( "EventNumTracksMax", m_blsHEventNumTracksMax );
  declareProperty( "EventNumTracksNbins", m_blsHEventNumTracksNbins );
  declareProperty( "EventNumMin", m_blsHEventNumMin );
  declareProperty( "EventNumMax", m_blsHEventNumMax );
  declareProperty( "EventNumNbins", m_blsHEventNumNbins );
 }

//=============================================================================
// Destructor
//=============================================================================

BlsHitChecker::~BlsHitChecker() {} 

//=============================================================================
// Initialization
//=============================================================================

StatusCode BlsHitChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) )
    debug() << "BlsHitChecker::initialize()" << endmsg;
  return StatusCode::SUCCESS;
} // end of: BlsHitChecker::initialize() {

//=============================================================================
// Main execution
//=============================================================================

StatusCode BlsHitChecker::execute() {
  if ( msgLevel(MSG::DEBUG) )
    debug() << "BlsHitChecker::execute()" << endmsg;
  if ( ! exist < LHCb::MCHits > ( m_blsHitsLocation ) ) {
    // Something happened - m_blsHitsLocation points to nowhere?...
    error() << "There are no MCHits at "
            << m_blsHitsLocation
            << " in TES!" << endmsg;
    // This is critical - tell the world about this error
    return StatusCode::FAILURE;
  } // end of: if ( ! exist < LHCb::MCHits > ( m_blsHitsLocation ) )
  // Get MC hits in the scintillator volume of BLSs
  m_blsMCHits = get < LHCb::MCHits > ( m_blsHitsLocation );
  // Just return if the number of hits in all BLSs is zero
  if ( 0 == m_blsMCHits->size() )
    return StatusCode::SUCCESS;
  // Get event header for event number (also used much later)
  const LHCb::MCHeader* myMCHeader =
    get < LHCb::MCHeader > ( LHCb::MCHeaderLocation::Default );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "m_blsHitsLocation: "
            << m_blsMCHits->size()
            << " hit(s) found at event no. "
            << myMCHeader->evtNumber() << endmsg;
  // Define a multimap for Particle/Hit pairs
  t_blsMCParticle2MCHitMultimap myMultimap;
  // Loop over all hits got from m_blsHitsLocation
  for ( LHCb::MCHits::iterator It = m_blsMCHits->begin();
        It != m_blsMCHits->end();
        It++ ) {
    // Get MC Particle that created this hit
    const LHCb::MCParticle* myMCParticle = (*It)->mcParticle();
    // If DEBUG is switched on
    if ( msgLevel(MSG::DEBUG) ) {
      // Print some data for hit and particle
      LHCb::ParticleID myParticleId = myMCParticle->particleID();
      Gaudi::LorentzVector myParticleMomentum = myMCParticle->momentum();
      const LHCb::MCVertex* myMCVertex = myMCParticle->originVertex();
      debug () << "Particle ID and momentum: "
               << myParticleId.pid() << " "
               << myParticleMomentum.e()/Gaudi::Units::GeV << " "
               << myMCParticle << " "
               << (*It) << " "
               << endmsg;
      debug () << "Entry point: "
               << (*It)->entry().x()/Gaudi::Units::mm << " "
               << (*It)->entry().y()/Gaudi::Units::mm << " "
               << (*It)->entry().z()/Gaudi::Units::mm
               << endmsg;
      debug () << "Exit point: "
               << (*It)->exit().x()/Gaudi::Units::mm << " "
               << (*It)->exit().y()/Gaudi::Units::mm << " "
               << (*It)->exit().z()/Gaudi::Units::mm
               << endmsg;
      debug () << "Vertex position: "
               << myMCVertex->position().x()/Gaudi::Units::mm << " " 
               << myMCVertex->position().y()/Gaudi::Units::mm << " " 
               << myMCVertex->position().z()/Gaudi::Units::mm << " " 
               << myMCVertex->type()
               << endmsg;
    } // end of: if ( msgLevel(MSG::DEBUG) )
    // Positive X goes to positive BLS and negative X to negative BLS
    if ( ( (*It)->entry().x() > 0 && m_blsCOn ) ||
         ( (*It)->entry().x() < 0 && m_blsAOn ) ) {
      // Insert a pair of Particle-Hit into the map
      myMultimap.insert
        ( t_blsMCParticle2MCHitMultimap::value_type ( myMCParticle, *It ) );
    } // end of: if ( ( (*It)->entry().x() > 0 && m_blsCOn ) ||
  } // end of: for ( LHCb::MCHits::iterator It = m_blsMCHits->begin();
  // Just return if no hits were found in a particular BLS
  if ( myMultimap.empty() ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "No hits in this BLS found at event no. "
              << myMCHeader->evtNumber()
              << endmsg;
    return StatusCode::SUCCESS;
  } // end of: if ( myMultimap.empty() ) {
  // Calculate hits/tracks and energy deposited in event
  unsigned short myEventNumHits = 0;
  unsigned short myEventNumTracks = 0;
  double myEventEnDep = 0.0;
  // String for histogram titles
  std::string myTitle;
  // Scan Particle<->Hit map to get quantities per single track
  for ( t_blsMCParticle2MCHitMultimap::iterator anIt = myMultimap.begin();
        anIt != myMultimap.end();
        ) {
    // Count new track
    myEventNumTracks++;
    // Calculate energy deposited by a single track and this track length
    double myTrackEnDep = 0.0;
    double myTrackLength = 0.0;
    // For each unique key (== MCParticle) in a map collect hits it has created
    const LHCb::MCParticle* myKey = (*anIt).first;
    for ( t_blsMCParticle2MCHitMultimap::iterator oneMoreIt =
            myMultimap.lower_bound ( myKey );
          oneMoreIt != myMultimap.upper_bound ( myKey );
          oneMoreIt++ ) {
      // Count new hit
      myEventNumHits++;
      const LHCb::MCParticle* myMCParticle = (*oneMoreIt).first;
      LHCb::MCHit* myMCHit = (*oneMoreIt).second;
      // If DEBUG is switched on
      if ( msgLevel(MSG::DEBUG) ) {
        debug() << "Particle2Hit map: "
                << myMCParticle << " "
                << myMCHit << " "
                << myMCHit->energy()/Gaudi::Units::MeV << " "
                << myMCHit->time()/Gaudi::Units::ns
                << endmsg;
      } // end of: if ( msgLevel(MSG::DEBUG) )
      // Add properties for current hit (== step) to track
      myTrackEnDep += myMCHit->energy();
      myTrackLength += myMCHit->pathLength();
      // Make histograms for a per hit quantities
      myTitle = m_blsHTitlePrefix
        + "Hit entry point in scintillator [mm], XY plane";
      plot2D( myMCHit->entry().x()/Gaudi::Units::mm,
              myMCHit->entry().y()/Gaudi::Units::mm,
              "HitEntryXY",
              myTitle,
              m_blsHEntryXMin/Gaudi::Units::mm,
              m_blsHEntryXMax/Gaudi::Units::mm,
              m_blsHEntryYMin/Gaudi::Units::mm,
              m_blsHEntryYMax/Gaudi::Units::mm,
              m_blsHEntryXNbins,
              m_blsHEntryYNbins );
      myTitle = m_blsHTitlePrefix
        + "Hit entry point in scintillator [mm], ZX plane";
      plot2D( myMCHit->entry().z()/Gaudi::Units::mm,
              myMCHit->entry().x()/Gaudi::Units::mm,
              "HitEntryZX",
              myTitle,
              m_blsHEntryZMin/Gaudi::Units::mm,
              m_blsHEntryZMax/Gaudi::Units::mm,
              m_blsHEntryXMin/Gaudi::Units::mm,
              m_blsHEntryXMax/Gaudi::Units::mm,
              m_blsHEntryZNbins,
              m_blsHEntryXNbins );
      myTitle = m_blsHTitlePrefix
        + "Hit entry point in scintillator [mm], ZY plane";
      plot2D( myMCHit->entry().z()/Gaudi::Units::mm,
              myMCHit->entry().y()/Gaudi::Units::mm,
              "HitEntryZY",
              myTitle,
              m_blsHEntryZMin/Gaudi::Units::mm,
              m_blsHEntryZMax/Gaudi::Units::mm,
              m_blsHEntryYMin/Gaudi::Units::mm,
              m_blsHEntryYMax/Gaudi::Units::mm,
              m_blsHEntryZNbins,
              m_blsHEntryYNbins );
      myTitle = m_blsHTitlePrefix
        + "Hit energy deposited in scintillator [MeV]";
      plot( ( myMCHit->time() + m_blsHEntryTimeOffset )/Gaudi::Units::ns,
            "HitEnDepTime",
            myTitle,
            m_blsHEntryTimeMin,
            m_blsHEntryTimeMax,
            m_blsHEntryTimeNbins,
            myMCHit->energy()/Gaudi::Units::MeV);
      // This can as well be: anIt = upper_bound(myKey)
      anIt++;
    } // end of: for ( t_blsMCParticle2MCHitMultimap::iterator oneMoreIt
    // Add energy deposited by a single track to total energy deposited in event
    myEventEnDep += myTrackEnDep;
    // Make histograms for a per track quantities
    myTitle = m_blsHTitlePrefix
      + "Energy deposited per track [MeV]";
    plot( myTrackEnDep/Gaudi::Units::MeV,
          "TrackEnDep",
          myTitle,
          m_blsHTrackEnDepMin,
          m_blsHTrackEnDepMax,
          m_blsHTrackEnDepNbins);
    myTitle = m_blsHTitlePrefix
      + "Track length [cm]";
    plot( myTrackLength/Gaudi::Units::cm,
          "TrackLength",
          myTitle,
          m_blsHTrackLengthMin,
          m_blsHTrackLengthMax,
          m_blsHTrackLengthNbins);
    myTitle = m_blsHTitlePrefix
      + "Energy deposited per track unit length [MeV/cm]";
    plot( (myTrackEnDep/Gaudi::Units::MeV)/(myTrackLength/Gaudi::Units::cm),
          "TrackEnDepPerUnitLength",
          myTitle,
          m_blsHTrackEnDepMin,
          m_blsHTrackEnDepMax,
          m_blsHTrackEnDepNbins);
  } // end of: for ( t_blsMCParticle2MCHitMultimap::iterator anIt
  // If DEBUG is switched on
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Event no. "
            << myMCHeader->evtNumber()
            << ": Hits/Tracks = "
            << myEventNumHits
            << "/"
            << myEventNumTracks
            << ", EnDep = "
            << myEventEnDep/Gaudi::Units::MeV
            << endmsg;
  } // end of: if ( msgLevel(MSG::DEBUG) )
  // Make histograms for a per event quantities
  myTitle = m_blsHTitlePrefix
    + "Number of hits per event ";
  plot( myEventNumHits,
        "EventNumHits",
        myTitle,
        m_blsHEventNumTracksMin,
        m_blsHEventNumTracksMax,
        m_blsHEventNumTracksNbins );
  myTitle = m_blsHTitlePrefix
      + "Number of tracks per event ";
  plot( myEventNumTracks,
        "EventNumTracks",
        myTitle,
        m_blsHEventNumTracksMin,
        m_blsHEventNumTracksMax,
        m_blsHEventNumTracksNbins );
  myTitle = m_blsHTitlePrefix
    + "Energy deposited per event [MeV]";
  plot( myEventEnDep/Gaudi::Units::MeV,
        "EventEnDep",
        myTitle,
        m_blsHTrackEnDepMin,
        m_blsHTrackEnDepMax,
        m_blsHTrackEnDepNbins);
  // Count number of hits/tracks per run
  m_blsHits += myEventNumHits;
  m_blsTracks += myEventNumTracks;
  // Plot global per run quantities
  myTitle = m_blsHTitlePrefix
      + "Number of hits in event";
  plot( myMCHeader->evtNumber(),
        "RunNumHits",
        myTitle,
        m_blsHEventNumMin,
        m_blsHEventNumMax,
        m_blsHEventNumNbins,
        myEventNumHits);
  myTitle = m_blsHTitlePrefix
      + "Number of tracks in event";
  plot( myMCHeader->evtNumber(),
        "RunNumTracks",
        myTitle,
        m_blsHEventNumMin,
        m_blsHEventNumMax,
        m_blsHEventNumNbins,
        myEventNumTracks);
  myTitle = m_blsHTitlePrefix
    + "Energy deposited in event [MeV]";
  plot( myMCHeader->evtNumber(),
        "RunEnDep",
        myTitle,
        m_blsHEventNumMin,
        m_blsHEventNumMax,
        m_blsHEventNumNbins,
        myEventEnDep/Gaudi::Units::MeV);
  // Final global return
  return StatusCode::SUCCESS;
} // end of: BlsHitChecker::execute()

//=============================================================================
//  Finalize
//=============================================================================

StatusCode BlsHitChecker::finalize() {
  if ( msgLevel(MSG::INFO) )
    // Print numbers for Hits and Tracks to the common output
    info() << "Hits/Tracks: "
           << m_blsHits
           << "/"
           << m_blsTracks << endmsg; 
  return GaudiAlgorithm::finalize();  // must be called after all other actions
} // end of: BlsHitChecker::finalize()

//=============================================================================

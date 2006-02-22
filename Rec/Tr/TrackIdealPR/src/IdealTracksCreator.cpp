// Include files
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCbDefinitions
#include "Kernel/TrackTypes.h"

//from XxxDet
#include "VeloDet/DeVelo.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"

// from MCEvent
#include "Event/MCHit.h"

// from RecEvent
#include "Event/RecHeader.h"

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"

// Local
#include "IdealTracksCreator.h"

using namespace Gaudi;
using namespace LHCb;

static const AlgFactory<IdealTracksCreator>    s_factory;
const IAlgFactory& IdealTracksCreatorFactory = s_factory;

/** @file IdealTracksCreator.cpp
 *
 *  Implementation of IdealTracksCreator.
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @author M. Needham
 *  @author R.M. van der Eijk (28-5-2002) 
 *  @author J. van Tilburg (02-07-2003)
 *  @date   27-7-1999
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IdealTracksCreator::IdealTracksCreator( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : GaudiAlgorithm( name, pSvcLocator )
//  , m_otTim2MCHit(0)
//  , m_itClus2MCP(0)
//  , m_veloClus2MCP(0)
  , m_velo(0)
  , m_ttTracker(0)
  , m_itTracker(0)
  , m_otTracker(0)
  , m_trackSelector(0)
  , m_stateCreator(0)
{
  /// default job Options
  declareProperty( "AddOTTimes",      m_addOTTimes      = true );
  declareProperty( "AddSTClusters",   m_addSTClusters   = true );
  declareProperty( "AddVeloClusters", m_addVeloClusters = true );
  declareProperty( "InitState",       m_initState       = true );
  declareProperty( "InitStateUpstream",    m_initStateUpstream = true );
  declareProperty( "TrueStatesAtMeasZPos", m_trueStatesAtMeas = false );
  declareProperty( "TracksOutContainer",
                   m_tracksOutContainer = TrackLocation::Ideal );
  declareProperty( "VeloGeometryPath",
                   m_veloPath = "/dd/Structure/LHCb/Velo" );
  declareProperty( "TTGeometryPath",
                   m_ttTrackerPath = DeSTDetLocation::location("TT") );
  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = DeSTDetLocation::location("IT") );
  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default );
  declareProperty( "STPositionTool",
                   m_stPositionToolName = "STOfflinePosition" );
  declareProperty( "MinNHits", m_minNHits = 6     );
  declareProperty( "ErrorX2",  m_errorX2  = 4.0   );
  declareProperty( "ErrorY2",  m_errorY2  = 400.0 );
  declareProperty( "ErrorTx2", m_errorTx2 = 6.e-5 );
  declareProperty( "ErrorTy2", m_errorTy2 = 1.e-4 );
  declareProperty( "ErrorP",   m_errorP   = 0.15  );
};

//=============================================================================
// Destructor
//=============================================================================
IdealTracksCreator::~IdealTracksCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode IdealTracksCreator::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  info() << "==> Initialize" << endreq;

  // Load Geometry from XmlDDDB
  // --------------------------
  m_velo      = getDet<DeVelo>( m_veloPath );

  m_ttTracker = getDet<DeSTDetector>( m_ttTrackerPath );
  m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );

  m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );

  // Retrieve the STClusterPosition tool
  m_stPositionTool = tool<ISTClusterPosition>( m_stPositionToolName );

  // Retrieve the TrackCriteriaSelector tool
  m_trackSelector = tool<ITrackCriteriaSelector>( "TrackCriteriaSelector",
                                                  "select", this );

  // Retrieve the IdealStateCreator tool
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator" );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode IdealTracksCreator::execute()
{
  debug() << "==> Execute" << endreq;

  StatusCode sc;

  // Event header info
  const RecHeader* recHdr = get<RecHeader>( RecHeaderLocation::Default );
  debug() << "Event " << recHdr -> evtNumber() << endreq;

  // Retrieve the MCParticle container
  const MCParticles* particles = get<MCParticles>( MCParticleLocation::Default );

  /// Retrieve the container for the OTTimes
  const OTTimes* otTimes = get<OTTimes>( OTTimeLocation::Default );
  debug() << "- retrieved " <<  otTimes -> size() << " OTTimes." << endreq;

  // Make container for tracks
  Tracks* tracksCont = new Tracks();

  // create the association (linker) table and register it in the TES
  LinkerWithKey<Track,MCParticle> linkTable( evtSvc(), msgSvc(),
                                             m_tracksOutContainer );

  debug() << "Starting loop over the "
          << particles -> size() << " MCParticles ... " << endreq;

  // loop over MCParticles
  // =====================
  MCParticles::const_iterator iPart;
  for ( iPart = particles -> begin(); particles -> end() != iPart; ++iPart ) {
    MCParticle* mcParticle = *iPart;
    verbose() << endreq
              << "- MCParticle of type "
              << m_trackSelector -> trackType( mcParticle )
              << " , (key # " << mcParticle -> key() << ")" << endreq
              << "    - momentum = " << mcParticle -> momentum() << " MeV" 
              << endreq
              << "    - P        = " << mcParticle -> momentum().Vect().R()
              << " MeV" <<endreq
              << "    - charge   = "
              << ( mcParticle -> particleID().threeCharge() / 3 ) << endreq;
    if ( m_trackSelector -> select( mcParticle ) ) {
      debug() << endreq
              << "Selected MCParticle of type "
              << m_trackSelector -> trackType( mcParticle )
              << " , (key # " << mcParticle -> key() << ")" << endreq
              << "    - momentum = " << mcParticle -> momentum() << " MeV"
              << endreq
              << "    - P        = " << mcParticle -> momentum().Vect().R()
              << " MeV" <<endreq
              << "    - charge   = "
              << ( mcParticle -> particleID().threeCharge() / 3 ) << endreq;

      // Make a new track
      // ----------------
      Track* track = new Track();

      m_trackSelector -> setTrackType( mcParticle, track );

      // Check whether a Velo track is backward
      if ( Track::Velo == track -> type() ) {
        const double pz = mcParticle -> momentum().pz();
        if ( pz < 0.0 ) track -> setFlag( Track::Backward, true );
      }

      // Add Velo clusters
      // -----------------
      if ( m_addVeloClusters == true ) {
        debug() << "... adding VeloXxxClusters" << endreq;
        sc = addVeloClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add velo R clusters" << endreq;
          return StatusCode::FAILURE;
        }
      }

      // Add ST clusters
      // ---------------
      if ( m_addSTClusters == true ) {
        debug() << "... adding STClusters" << endreq;
        sc = addSTClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add inner tracker clusters" << endreq;
          return StatusCode::FAILURE;
        }        
      }
      
      // Add OTTimes
      // -----------
      if ( m_addOTTimes == true && otTimes != 0 ) {
        debug() << "... adding OTTimes" << endreq;
        sc = addOTTimes( otTimes, mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add outer tracker OTTimes" << endreq;
          return StatusCode::FAILURE;
        }
      }

      // Check if the track contains enough hits
      // ---------------------------------------
      if ( (int) track -> nMeasurements() < m_minNHits) {
        debug() << " -> track deleted. Had only " << track -> nMeasurements()
                << " hits" << endreq;
        delete track;
        continue; // go to next track
      }

      // Initialize a seed state
      // -----------------------
      if ( m_initState ) {
        if ( m_initStateUpstream ) {
          std::vector<Measurement*>::const_reverse_iterator rbeginM =
            track -> measurements().rbegin();
          sc = this -> initializeState( (*rbeginM)->z(),
                                        track, mcParticle );
        }
        else {
          std::vector<Measurement*>::const_iterator beginM =
            track -> measurements().begin();
          sc = this -> initializeState( (*beginM)->z(),
                                        track, mcParticle );
        }
        if ( !sc.isSuccess() ) {
          delete track;
          continue; // go to next track
        }
      }

      // Set some of the track properties
      // --------------------------------
      track -> setStatus( Track::PatRecMeas );
      track -> setFlag( Track::Unique, true );
      track -> setHistory( Track::TrackIdealPR );

      // Add true states at each measurement 
      // ===================================
      if ( m_trueStatesAtMeas ) {
        deleteStates( track );
        sc = StatusCode::SUCCESS;
        std::vector<Measurement*>::const_iterator iMeas =
          track -> measurements().begin();
        std::vector<Measurement*>::const_iterator endM =
          track -> measurements().end();
        while ( sc.isSuccess() && iMeas != endM ) {
          State* tempState;
          sc = m_stateCreator -> createState( mcParticle,
                                              (*iMeas)->z(),
                                              tempState );
          if ( sc.isSuccess() ) track -> addToStates( *tempState );
          else delete tempState;
          ++iMeas;
        }
        if ( sc.isFailure() ) {
          delete track;
          continue;
        }        
      }

      debug() << "At the end: states at z-positions: ";
      const std::vector<State*>& allstates = track->states();
      for ( unsigned int it = 0; it < allstates.size(); ++it ) {
        debug() << allstates[it] -> z() << " ";
      }
      debug() << endreq;
      for ( unsigned int it2 = 0; it2 < allstates.size(); ++it2 ) {
        debug() << "state vectors:" << endreq
                << allstates[it2] -> stateVector() << endreq;
      }
      debug() << endreq;

      // Add the track to the Tracks container and fill the associatin table
      // -------------------------------------------------------------------
      tracksCont -> add( track );
      linkTable.link( mcParticle, track, 1. );

      // debugging Track ...
      // -------------------
      debug()
        << "-> Track with key # " << track -> key() << endreq
        << "  * charge         = " << track -> charge() << endreq
        << "  * is Invalid     = " << track -> checkFlag( Track::Invalid ) 
        << endreq
        << "  * is Unique      = " << track -> checkFlag( Track::Unique ) 
        << endreq
        << "  * is of type     = " << track -> type() << endreq
        << "  * is Backward    = " << track -> checkFlag( Track::Backward ) 
        << endreq
        << "  * # measurements = " << track -> nMeasurements() << endreq;
      
      // print the measurements
      const std::vector<Measurement*>& meas = track -> measurements();
      for ( std::vector<Measurement*>::const_iterator itMeas = meas.begin();
            itMeas != meas.end(); ++itMeas ) {
        debug() << "  - measurement of type " << (*itMeas) -> type() << endreq
                << "  - z        = " << (*itMeas) -> z() << " mm" << endreq
                << "  - LHCbID   = " << (*itMeas) -> lhcbID()  << endreq;
        // continue according to type ...  
        if ( (*itMeas) -> lhcbID().isOT() ) {
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().otID() 
                  << endreq;
        }
        else if ( (*itMeas) -> lhcbID().isST() ) {
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().stID() 
                  << endreq;
        }
        else if ( (*itMeas) -> lhcbID().isVelo() ) {    
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().veloID() 
                  << endreq;
        }
      }

    } // is selected
  } // looping over MCParticles

  debug() << "Created " << tracksCont -> size() << " tracks." << endreq;

  // Store the Tracks in the TES
  // ===========================
  return put( tracksCont, m_tracksOutContainer );
};

//=============================================================================
// Finalize
//=============================================================================
StatusCode IdealTracksCreator::finalize()
{
  debug() << "==> Finalize" << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
};

//=============================================================================
// Add outer tracker clusters
//=============================================================================
StatusCode IdealTracksCreator::addOTTimes( const OTTimes* times,
                                           MCParticle* mcPart,
                                           Track* track )
{
  unsigned int nOTMeas = 0;

  typedef LinkerTool<OTTime,MCHit> asctTool;
  typedef asctTool::DirectType     table;

  asctTool associator( evtSvc(), OTTimeLocation::Default+"2MCHits" );

  const table* tbl = associator.direct();
  if( !tbl )
    return Error( "Empty table with associations" );

  // Loop over outer tracker clusters
  OTTimes::const_iterator itOTTime = times -> begin();
  while ( itOTTime != times->end() ) {
    const OTTime* aTime = *itOTTime;
    // retrieve MCHit associated with this cluster
    table::Range range = tbl -> relations( aTime );
    for( table::iterator iTim = range.begin(); iTim != range.end(); ++iTim ) {
      const MCHit* aMCHit = iTim -> to();
      const MCParticle* aParticle = aMCHit -> mcParticle();
      if ( aParticle == mcPart ) {
        // get the ambiguity from the MCHit
        const OTChannelID channel = aTime -> channel();
        const DeOTModule* module  = m_otTracker -> module( channel );
        const XYZPoint entryP     = aMCHit -> entry();
        if ( aMCHit -> displacement().z() == 0. ) continue; // curling track
        const double tx = aMCHit -> dxdz();
        const double ty = aMCHit -> dydz();
        // input in global frame: entryP, tx, ty
        double mcDist   = module -> distanceToWire( channel.straw(),
                                                    entryP, tx, ty );
        int ambiguity = 1;
        if ( mcDist < 0.0 ) ambiguity = -1;
        // Get the tu from the MCHit
        double angle = module -> stereoAngle();
        double tu = tx * cos(angle) + ty * sin(angle);
        OTMeasurement otTim = OTMeasurement( *aTime, *m_otTracker,
                                             ambiguity, tu );
        track -> addToLhcbIDs( otTim.lhcbID() );
        track -> addToMeasurements( otTim );
        ++nOTMeas;
        debug() << " - added OTMeasurement, ambiguity = "
                << ambiguity << endreq;
      }
    }
    // next cluster
    ++itOTTime;
  } // loop over outer tracker clusters

  debug() << "- " << nOTMeas << " OTMeasurements added" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addSTClusters( MCParticle* mcPart,
                                              Track* track )
{
  unsigned int nSTMeas = 0;

  // Retrieve STClusters from inverse relation
  typedef LinkerTool<STCluster,MCParticle> asctTool;

  typedef asctTool::InverseType invTable;

  asctTool ttAssociator( evtSvc(), STClusterLocation::TTClusters );
  asctTool itAssociator( evtSvc(), STClusterLocation::ITClusters );

  // for the STClusters in TT
  const invTable* invTTTable = ttAssociator.inverse();
  if( !invTTTable )
    return Error( "Empty table with associations STCluster(TT)-MCParticle " );
  invTable::Range ttRange = invTTTable -> relations( mcPart );
  for( invTable::iterator it = ttRange.begin(); it != ttRange.end(); ++it ) {
    const STCluster* aCluster = it -> to();
    STMeasurement meas =
      STMeasurement( *aCluster, *m_itTracker, *m_stPositionTool );
        track -> addToLhcbIDs( meas.lhcbID() );
    track -> addToMeasurements( meas );
    ++nSTMeas;
  }

  // for the STClusters in IT
  const invTable* invITTable = itAssociator.inverse();
  if( !invITTable )
    return Error( "Empty table with associations STCluster(IT)-MCParticle " );
  invTable::Range itRange = invITTable -> relations( mcPart );
  for( invTable::iterator it = itRange.begin(); it != itRange.end(); ++it ) {
    const STCluster* aCluster = it -> to();
    STMeasurement meas =
      STMeasurement( *aCluster, *m_itTracker, *m_stPositionTool );
        track -> addToLhcbIDs( meas.lhcbID() );
    track -> addToMeasurements( meas );
    ++nSTMeas;
  }

  debug() << "- " << nSTMeas << " STMeasurements added" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addVeloClusters( MCParticle* mcPart, 
                                                Track* track )
{
  unsigned int nVeloRMeas   = 0;
  unsigned int nVeloPhiMeas = 0;

  // Retrieve VeloClusters from inverse relation
  typedef LinkerTool<VeloCluster,MCParticle> asctTool;

  typedef asctTool::InverseType invTable;

  asctTool associator( evtSvc(), VeloClusterLocation::Default );
  const invTable* invTbl = associator.inverse();
  if( !invTbl )
    return Error( "Empty table with associations" );

  invTable::Range range = invTbl -> relations( mcPart );

  for( invTable::iterator it = range.begin(); it != range.end(); ++it ) {
    const VeloCluster* aCluster = it -> to();

    double z = m_velo -> zSensor( aCluster -> channelID().sensor() );

    double phi = 999.;
    double r   = -999.;
    State* tempState;
    StatusCode sc = m_stateCreator -> createState( mcPart, z, tempState );
    if ( sc.isSuccess() ) {
      TrackVector vec = tempState -> stateVector();
      r = sqrt( vec[0]*vec[0] + vec[1]*vec[1] );
      phi = atan2( vec[1], vec[0] );
    }
    delete tempState;

    // Check if VeloCluster is of type R or Phi
    if ( m_velo -> isRSensor( aCluster -> channelID().sensor() ) ) {
      VeloRMeasurement meas = VeloRMeasurement( *aCluster, *m_velo, phi );
      track -> addToLhcbIDs( meas.lhcbID() );
      track -> addToMeasurements( meas );
      ++nVeloRMeas;
    }
    else {
      VeloPhiMeasurement meas = VeloPhiMeasurement( *aCluster, *m_velo, r );
      track -> addToLhcbIDs( meas.lhcbID() );
      track -> addToMeasurements( meas );
      ++nVeloPhiMeas;
    }
  }
  
  debug() << "- " << nVeloRMeas << " / " << nVeloPhiMeas
          << " Velo R/Phi Measurements added" << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Initialize seed state
//=============================================================================
StatusCode IdealTracksCreator::initializeState( double z,
                                                Track* track,
                                                MCParticle* mcPart )
{
  State* state;
  StatusCode sc = m_stateCreator -> createState( mcPart, z, state );
  if ( sc.isSuccess() ) {
    // set covariance matrix to a somewhat larger value for the fit
    TrackMatrix& cov = state -> covariance();
    cov(0,0) = m_errorX2;
    cov(1,1) = m_errorY2;
    cov(2,2) = m_errorTx2;
    cov(3,3) = m_errorTy2;
    cov(4,4) = pow( m_errorP * state->qOverP(), 2. );

    track -> addToStates( *state );

    double qP = state -> qOverP();

    debug() << "- State added:" << endreq
            << "  - position = " << state -> position() << " mm" <<endreq
            << "  - momentum = " << state -> momentum() << " MeV" <<endreq
            << "  - P        = " << state -> p() << " MeV" <<endreq
            << "  - charge   = " << ( qP != 0. ? int(fabs(qP)/qP) : 0  ) 
            << endreq;
  }
  else {
    delete state;
    debug() << "-> unable to add a State!" << endreq;
  }
  
  return sc;
};

//=============================================================================
// Delete all states on the track
//=============================================================================
StatusCode IdealTracksCreator::deleteStates( Track* track )
{
  std::vector<State*> tmpStates = track -> states();
  for ( std::vector<State*>::iterator it  = tmpStates.begin();
        it != tmpStates.end(); ++it ) {
    track -> removeFromStates( (*it) );
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================

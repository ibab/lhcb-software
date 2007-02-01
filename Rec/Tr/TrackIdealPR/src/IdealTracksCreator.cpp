// $Id: IdealTracksCreator.cpp,v 1.33 2007-02-01 10:15:10 wouter Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCbDefinitions
#include "Kernel/TrackTypes.h"

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

//from XxxDet
#include "VeloDet/DeVelo.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"

// from RecEvent
#include "Event/RecHeader.h"

// from MCEvent
#include "Event/MCHit.h"

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/StateTraj.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedFrom.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// Local
#include "IdealTracksCreator.h"

using namespace Gaudi;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( IdealTracksCreator );

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
  , m_velo(0)
  , m_ttTracker(0)
  , m_itTracker(0)
  , m_otTracker(0)
  , m_trackSelector(0)
  , m_stateCreator(0)
{
  /// default job Options
  declareProperty( "AddVeloClusters", m_addVeloClusters = true           );
  declareProperty( "AddTTClusters",   m_addTTClusters   = true           );
  declareProperty( "AddITClusters",   m_addITClusters   = true           );
  declareProperty( "AddOTTimes",      m_addOTTimes      = true           );
  declareProperty( "AddMeasurements", m_addMeasurements = true           );
  declareProperty( "InitState",       m_initState       = true           );
  declareProperty( "InitStateUpstreamFit", m_initStateUpstreamFit = true );
  declareProperty( "TrueStatesAtMeasZPos", m_trueStatesAtMeas = false    );
  declareProperty( "TracksOutContainer",
                   m_tracksOutContainer = TrackLocation::Ideal           );
  declareProperty( "VeloGeometryPath",
                   m_veloPath = DeVeloLocation::Default                  );
  declareProperty( "TTGeometryPath",
                   m_ttTrackerPath = DeSTDetLocation::location("TT")     );
  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = DeSTDetLocation::location("IT")     );
  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default       );
  declareProperty( "TTClusterPositionTool",
                   m_ttPositionToolName = "STOfflinePosition"            );
  declareProperty( "ITClusterPositionTool",
                   m_itPositionToolName =
                   "STOfflinePosition/ITClusterPosition"                 );
  declareProperty( "VeloPositionTool",
                   m_veloPositionToolName = "VeloClusterPosition"        );
  declareProperty( "TrackSelectorTool",
                   m_selectorToolName = "TrackCriteriaSelector"          );
  declareProperty( "MinNHits", m_minNHits = 6                            );

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

  // Retrieve the STClusterPosition tools
  m_ttPositionTool = tool<ISTClusterPosition>( m_ttPositionToolName );
  m_itPositionTool = tool<ISTClusterPosition>( m_itPositionToolName );

  // Retrieve the VeloClusterPosition tool
  m_veloPositionTool = tool<IVeloClusterPosition>( m_veloPositionToolName );

  // Retrieve the Track selector tool
  m_trackSelector = tool<ITrackCriteriaSelector>( m_selectorToolName,
                                                  "TrackSelectorTool", this );
  
  // Retrieve the IdealStateCreator tool
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator" );

  // Retrieve the magnetic field and the poca tool
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

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
  const MCParticles* particles = get<MCParticles>(MCParticleLocation::Default);

  // Make container for tracks
  Tracks* tracksCont = new Tracks();

  // To store the Tracks in the TES
  put( tracksCont, m_tracksOutContainer );

  // create the association (linker) table and register it in the TES
  LinkerWithKey<MCParticle,Track> linkTable( evtSvc(), msgSvc(),
                                             m_tracksOutContainer );

  debug() << "Starting loop over the "
          << particles -> size() << " MCParticles ... " << endreq;

  // loop over MCParticles
  // =====================
  MCParticles::const_iterator iPart;
  for ( iPart = particles -> begin(); particles -> end() != iPart; ++iPart ) {
    MCParticle* mcParticle = *iPart;
    verbose() << "- MCParticle of type "
              << m_trackSelector -> trackType( mcParticle )
              << " , (key # " << mcParticle -> key() << ")" << endreq
              << "    - vertex = " << mcParticle -> originVertex() ->position()
              << endreq
              << "    - momentum = " << mcParticle -> momentum() << " MeV" 
              << endreq
              << "    - P        = " << mcParticle -> p()
              << " MeV" <<endreq
              << "    - PID   = "
              << ( mcParticle -> particleID().pid() ) << endreq
              << "    - charge   = "
              << ( mcParticle -> particleID().threeCharge() / 3 ) << endreq;
    if ( m_trackSelector -> select( mcParticle ) ) {
      debug() << endreq
              << "Selected MCParticle of type "
              << m_trackSelector -> trackType( mcParticle )
              << " , (key # " << mcParticle -> key() << ")" << endreq
              << "    - momentum = " << mcParticle -> momentum() << " MeV"
              << endreq
              << "    - P        = " << mcParticle -> p()
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

      // Add TT clusters
      // ---------------
      if ( m_addTTClusters == true ) {
        debug() << "... adding TTClusters" << endreq;
        sc = addTTClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add TT tracker clusters" << endreq;
          return StatusCode::FAILURE;
        }        
      }

      // Add IT clusters
      // ---------------
      if ( m_addITClusters == true ) {
        debug() << "... adding ITClusters" << endreq;
        sc = addITClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add IT tracker clusters" << endreq;
          return StatusCode::FAILURE;
        }        
      }
      
      // Add OTTimes
      // -----------
      if ( m_addOTTimes == true ) {
        debug() << "... adding OTTimes" << endreq;
        sc = addOTTimes( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add outer tracker OTTimes" << endreq;
          return StatusCode::FAILURE;
        }
      }

      // Check if the track contains enough hits
      // ---------------------------------------
      if ( (int) track -> nLHCbIDs() < m_minNHits) {
        debug() << " -> track deleted. Had only " << track -> nLHCbIDs()
                << " LHCbID's" << endreq;
        delete track;
        continue; // go to next track
      }

      // Initialize a seed state
      // -----------------------
      if ( m_initState && !m_trueStatesAtMeas ) {
        sc = this -> initializeState( mcParticle, track );
        if ( !sc.isSuccess() ) {
          debug() << " -> track deleted as unable to initialize state"
                  << endreq;
          delete track;
          continue; // go to next track
        }
      }

      // Set some of the track properties
      // --------------------------------
      track -> setHistory( Track::TrackIdealPR );
      if ( m_addMeasurements ) track -> setPatRecStatus( Track::PatRecMeas );
      else                     track -> setPatRecStatus( Track::PatRecIDs  );

      // Add true states at each measurement 
      // ===================================
      if ( m_trueStatesAtMeas ) {
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

      // Add the track to the Tracks container and fill the association table
      // -------------------------------------------------------------------
      tracksCont -> add( track );
      linkTable.link( track, mcParticle, 1. );

      // debugging Track ...
      // -------------------
      debug()
        << "-> Track with key # " << track -> key() << endreq
        << "  * charge         = " << track -> charge() << endreq
        << "  * is Invalid     = " << track -> checkFlag( Track::Invalid ) 
        << endreq
        << "  * is Unique      = " << !track -> checkFlag( Track::Clone ) 
        << endreq
        << "  * is of type     = " << track -> type() << endreq
        << "  * is Backward    = " << track -> checkFlag( Track::Backward ) 
        << endreq
        << "  * # LHCbID's     = " << track -> nLHCbIDs()
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

  return StatusCode::SUCCESS;
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
StatusCode IdealTracksCreator::addOTTimes( const MCParticle* mcPart,
					   Track* track ) const
{
  unsigned int nOTMeas = 0;

  LinkedFrom<OTTime,MCParticle>
    otLink( evtSvc(), msgSvc(), OTTimeLocation::Default );
  if ( otLink.notFound() ) {
    return Error( "Unable to retrieve OTTime to MCParticle Linker table" );
  }
  else {
    const OTTime* aTime = otLink.first( mcPart );
    while ( NULL != aTime ) {
      OTMeasurement meas =
        OTMeasurement( *aTime, *m_otTracker );
      
      track -> addToLhcbIDs( meas.lhcbID() );
      
      StatusCode sc;
      if ( m_addMeasurements ) { // Add the measurement to the track
        // Set the reference vector
        State* tempState;
        sc = m_stateCreator -> createState( mcPart, meas.z(), tempState );
        if ( sc.isSuccess() ) {
          meas.setRefVector( tempState -> stateVector() );
          
          // Get the ambiguity using the Poca tool
          XYZVector distance;
          XYZVector bfield;
          m_pIMF -> fieldVector( tempState->position(), bfield );
          StateTraj stateTraj = StateTraj( meas.refVector(), meas.z(), bfield );
          double s1 = 0.0;
          double s2 = (meas.trajectory()).arclength( stateTraj.position(s1) );
          StatusCode msc = m_poca -> minimize( stateTraj, s1, meas.trajectory(),
                                               s2, distance, 20*Gaudi::Units::mm );
          if( msc.isFailure() ) {
            warning() << "TrajPoca minimize failed in addOTTimes." << endreq;
          }
          int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;
          meas.setAmbiguity( ambiguity );
          
          // Set the z-position of the measurement
          meas.setZ( stateTraj.position(s1).z() );
          
          // Reset using the updated z-position
          State* myTempState;
          sc = m_stateCreator->createState(mcPart, meas.z(), myTempState);
          if ( sc.isSuccess() ) meas.setRefVector( myTempState -> stateVector() );
          
          track -> addToMeasurements( meas );
          
          delete myTempState;
        }
        if ( sc.isFailure() )
          Warning( "Failed to calculate ref. info. and ambiguity for OTMeasurement" );
        
        delete tempState;
      } // if ( m_addMeasurements ) 
      
      ++nOTMeas;      
      aTime = otLink.next();
    } // loop over OTTimes
  } // if ( otLink.notFound() )

  debug() << "- " << nOTMeas << " OTMeasurements added" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addTTClusters( const MCParticle* mcPart,
                                              Track* track ) const
{
  unsigned int nTTMeas = 0;

  LinkedFrom<STCluster,MCParticle>
    ttLink( evtSvc(), msgSvc(), STClusterLocation::TTClusters );
  if ( ttLink.notFound() ) {
    return Error( "Unable to retrieve STCluster-TT to MCParticle Linker table");
  }
  else {
    const STCluster* aCluster = ttLink.first( mcPart );
    while ( NULL != aCluster ) {
      STMeasurement meas =
        STMeasurement( *aCluster, *m_ttTracker, *m_ttPositionTool );
      track -> addToLhcbIDs( meas.lhcbID() );
      if ( m_addMeasurements ) { // Add the measurement to the track
        // Set the reference vector
        State* tempState;
        StatusCode sc = m_stateCreator->createState(mcPart,meas.z(),tempState);
        if ( sc.isSuccess() ) {
          meas.setRefVector( tempState -> stateVector() ); 
          track -> addToMeasurements( meas );
        }
        else {
          Warning( "Failed to calculate ref. info. for STMeasurement in TT" );
        }
        delete tempState;
      }
      ++nTTMeas;      
      aCluster = ttLink.next();
    }
  }

  debug() << "- " << nTTMeas << " STMeasurements in TT added" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addITClusters( const MCParticle* mcPart,
                                              Track* track ) const
{
  unsigned int nITMeas = 0;

  LinkedFrom<STCluster,MCParticle>
    itLink( evtSvc(), msgSvc(), STClusterLocation::ITClusters );
  if ( itLink.notFound() ) {
    return Error( "Unable to retrieve STCluster-IT to MCParticle Linker table");
  }
  else {
    const STCluster* aCluster = itLink.first( mcPart );
    while ( NULL != aCluster ) {
      STMeasurement meas =
        STMeasurement( *aCluster, *m_itTracker, *m_itPositionTool );
      
      track -> addToLhcbIDs( meas.lhcbID() );
      
      StatusCode sc;
      if ( m_addMeasurements ) { // Add the measurement to the track
        // Set the reference vector
        State* tempState;
        sc = m_stateCreator->createState(mcPart,meas.z(),tempState);
        if ( sc.isSuccess() ) {
          meas.setRefVector( tempState -> stateVector() );
          
          // Set the z-position of the measurement
          XYZVector distance;
          XYZVector bfield;
          m_pIMF -> fieldVector( tempState->position(), bfield );
          StateTraj stateTraj = StateTraj( meas.refVector(), meas.z(), bfield );
          double s1 = 0.0;
          double s2 = (meas.trajectory()).arclength( stateTraj.position(s1) );
          StatusCode msc = m_poca->minimize(stateTraj, s1, meas.trajectory(), s2,
                                            distance, 20*Gaudi::Units::mm);
          if( msc.isFailure() ) {
            warning() << "TrajPoca minimize failed in addITClusters." << endreq;
          }
          meas.setZ( stateTraj.position(s1).z() );
          
          // Reset using the updated z-position
          State* myTempState;
          sc = m_stateCreator->createState(mcPart, meas.z(), myTempState);
          if ( sc.isSuccess() ) meas.setRefVector( myTempState -> stateVector() );

          track -> addToMeasurements( meas );
          
          delete myTempState;
        }
        if ( sc.isFailure() )
          Warning( "Failed to calculate ref. info. for STMeasurement in IT" );
        
        delete tempState;
      } // if ( m_addMeasurements ) 
      
      ++nITMeas;      
      aCluster = itLink.next();
    }
  }

  debug() << "- " << nITMeas << " STMeasurements in IT added" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addVeloClusters( const MCParticle* mcPart, 
                                                Track* track ) const
{
  unsigned int nVeloRMeas   = 0;
  unsigned int nVeloPhiMeas = 0;

  LinkedFrom<VeloCluster,MCParticle>
    veloLink( evtSvc(), msgSvc(), VeloClusterLocation::Default );
  if ( veloLink.notFound() ) {
    return Error( "Unable to retrieve VeloCluster to MCParticle Linker table");
  }
  else {
    const VeloCluster* aCluster = veloLink.first( mcPart );
    while ( NULL != aCluster ) {
      // Get the reference vector
      const DeVeloSensor* sensor =
        m_velo -> sensor( aCluster -> channelID().sensor() );
      double z = sensor->z();
      // Check if VeloCluster is of type R or Phi
      if ( sensor -> isR() || sensor -> isPileUp() ) {
        VeloRMeasurement meas = VeloRMeasurement( *aCluster, *m_velo, 
                                                  *m_veloPositionTool );
        track -> addToLhcbIDs( meas.lhcbID() );
        if ( m_addMeasurements ) { // Add the measurement to the track
          State* tempState;
          StatusCode sc = m_stateCreator -> createState( mcPart, z, tempState);
          if ( sc.isSuccess() ) meas.setRefVector( tempState->stateVector()); 
          else Warning( "Failed to calculate ref. info. for VeloMeasurement" );
          track -> addToMeasurements( meas );
          delete tempState;
        }
        ++nVeloRMeas;
      } else {
        VeloPhiMeasurement meas = VeloPhiMeasurement( *aCluster, *m_velo ,
                                                      *m_veloPositionTool );
        track -> addToLhcbIDs( meas.lhcbID() );
        if ( m_addMeasurements ) { // Add the measurement to the track
          State* tempState;
          StatusCode sc = m_stateCreator -> createState( mcPart, z, tempState);
          if ( sc.isSuccess() ) meas.setRefVector( tempState->stateVector() );
          else Warning( "Failed to calculate ref. info. for VeloMeasurement" );
          track -> addToMeasurements( meas );
          delete tempState;
        }
        
        ++nVeloPhiMeas;
      }
      aCluster = veloLink.next();
    }
  }
  
  debug() << "- " << nVeloRMeas << " / " << nVeloPhiMeas
          << " Velo R/Phi Measurements added" << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Initialize seed state
//=============================================================================
StatusCode IdealTracksCreator::initializeState( const MCParticle* mcPart,
						Track* track) const
{
  double zseed(0) ;
  if( !track->measurements().empty()) {
    std::vector<LHCb::Measurement*>::const_iterator imeas = track->measurements().begin() ;
    zseed = (*imeas)->z() ;
    for(++imeas; imeas != track->measurements().end(); ++imeas) {
      double thisz = (*imeas)->z() ;
      if ( m_initStateUpstreamFit && thisz > zseed ) zseed = thisz;
      if (!m_initStateUpstreamFit && thisz < zseed ) zseed = thisz;
    }
  }
  
  State* state(0);
  StatusCode sc = m_stateCreator -> createState( mcPart, zseed, state );
  if ( sc.isSuccess() ) {
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

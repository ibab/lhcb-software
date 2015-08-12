// Include files
// -------------
// from Gaudi
#include "GaudiKernel/RndmGenerators.h"

// from TrackEvent
#include "Event/TrackTypes.h"

// from MCEvent
#include "Event/MCHit.h"

// from TrackEvent
#include "Event/StateParameters.h"
#include "Event/StateVector.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedFrom.h"

// from TrackInterfaces
#include "MCInterfaces/IMCReconstructible.h"

// Local
#include "IdealTracksCreator.h"

#include "Map.h"
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

using namespace Gaudi;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( IdealTracksCreator )

/** @file IdealTracksCreator.cpp
 *
 *  Implementation of IdealTracksCreator.
 *
 *  @author M. Needham (make faster + more flexible)
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
  , m_trackSelector(0)
  , m_stateCreator(0)
  , m_itLinker(0,0,"")
  , m_ttLinker(0,0,"")
  , m_otLinker(0,0,"")
  , m_veloLinker(0,0,"")
  , m_muonLinker(0,0,"")
{
  /// default job Options
  declareProperty( "AddVeloClusters", m_addVeloClusters = true           );
  declareProperty( "AddTTClusters",   m_addTTClusters   = true           );
  declareProperty( "AddITClusters",   m_addITClusters   = true           );
  declareProperty( "AddOTTimes",      m_addOTTimes      = true           );
  declareProperty( "AddMuonHits",      m_addMuonHits      = false        );
  declareProperty( "InitState",       m_initState       = true           );
  declareProperty( "InitStateUpstreamFit", m_initStateUpstreamFit = true );
  declareProperty( "TrueStatesAtMCHits", m_trueStatesAtMCHits = false    );
  declareProperty( "TracksOutContainer",
                   m_tracksOutContainer = TrackLocation::Ideal           );
  declareProperty( "TrackSelectorTool",
                   m_selectorToolName =  "MCReconstructible"        );
  declareProperty( "MinNHits", m_minNHits = 6                            );
  std::vector<int> tmp = boost::assign::list_of(1)(3)(4)(5)(6);
  declareProperty( "TrackTypes", m_tracktypes = tmp);

  declareProperty("ITEff", m_itEff = 1.0);
  declareProperty("OTEff", m_otEff = 1.0);
  declareProperty("TTEff", m_ttEff = 1.0);
  declareProperty("VeloEff", m_veloEff = 1.0);
  declareProperty("smearP", m_smearP = 0.0);

}

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

  info() << "==> Initialize" << endmsg;

  m_trackSelector = tool<IMCReconstructible>(m_selectorToolName,"TrackSelectorTool",this);

  // Retrieve the IdealStateCreator tool
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator" );

  sc = randSvc()->generator(Rndm::Flat(0.,1.),m_uniformDist.pRef());
  if ( sc.isFailure() ) {
    return Warning("Failed to get flat random number generator", 
                   StatusCode::FAILURE); 
  }

  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussDist.pRef());
  if ( sc.isFailure() ) {
    return Warning("Failed to get flat gauss random generator", 
                   StatusCode::FAILURE); 
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode IdealTracksCreator::execute()
{

  StatusCode sc;

  debug() << "==> Execute" << endmsg;

  // init the linker tables
  sc = initEvent();
  if (sc.isFailure()){
    return Error("Failed to finder linkers", StatusCode::FAILURE);
  }  


  // Retrieve the MCParticle container
  const MCParticles* particles = get<MCParticles>(MCParticleLocation::Default);

  // Make container for tracks
  Tracks* tracksCont = new Tracks(); tracksCont->reserve(500);

  // To store the Tracks in the TES
  put( tracksCont, m_tracksOutContainer );

  // create the association (linker) table and register it in the TES
  LinkerWithKey<MCParticle,Track> linkTable( evtSvc(), msgSvc(),
                                             m_tracksOutContainer );

  debug() << "Starting loop over the "
          << particles -> size() << " MCParticles ... " << endmsg;

  // loop over MCParticles
  // =====================

  const TrackIdealPR::CatMap& theMap = TrackIdealPR::catToType();

  MCParticles::const_iterator iPart;
  for ( iPart = particles -> begin(); particles -> end() != iPart; ++iPart ) {
    MCParticle* mcParticle = *iPart;

    if ( msgLevel( MSG::DEBUG) ){
      printMCParticle(mcParticle);
    }

    // find out what type it is reconstructible as....
    IMCReconstructible::RecCategory cat = m_trackSelector->reconstructible(mcParticle);
    
    // map to track type
    Track::Types aType = theMap.find(cat)->second;

    if (wanted(aType) == true ) {
   
      if ( msgLevel( MSG::DEBUG) ){
	debug() << "Selected particle ";
        printMCParticle(mcParticle);
      }
      // Make a new track
      // ----------------
      Track* track = new Track();
      track->setType(aType);

      // Check whether a Velo track is backward
      if ( Track::Velo == track -> type() ) {
        const double pz = mcParticle -> momentum().pz();
        if ( pz < 0.0 ) track -> setFlag( Track::Backward, true );
      }

      // Add Velo clusters
      // -----------------
      if ( m_addVeloClusters == true ) {
        debug() << "... adding VeloXxxClusters" << endmsg;
        sc = addVeloClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add velo R clusters" << endmsg;
          return StatusCode::FAILURE;
        }
      }

      // Add TT clusters
      // ---------------
      if ( m_addTTClusters == true ) {
        debug() << "... adding TTClusters" << endmsg;
        sc = addTTClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add TT tracker clusters" << endmsg;
          return StatusCode::FAILURE;
        }        
      }

      // Add IT clusters
      // ---------------
      if ( m_addITClusters == true ) {
        debug() << "... adding ITClusters" << endmsg;
        sc = addITClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add IT tracker clusters" << endmsg;
          return StatusCode::FAILURE;
        }        
      }
      
      // Add OTTimes
      // -----------
      if ( m_addOTTimes == true ) {
        debug() << "... adding OTTimes" << endmsg;
        sc = addOTTimes( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add outer tracker OTTimes" << endmsg;
          return StatusCode::FAILURE;
        }
      }

      // Add Muon hits
      // -----------
      if ( m_addMuonHits == true ) {
        debug() << "... adding OTTimes" << endmsg;
        sc = addMuonHits( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add outer tracker OTTimes" << endmsg;
          return StatusCode::FAILURE;
        }
      }
     
      // Check if the track contains enough hits
      // ---------------------------------------
      if ( (int) track -> nLHCbIDs() < m_minNHits) {
        debug() << " -> track deleted. Had only " << track -> nLHCbIDs()
                << " LHCbID's" << endmsg;
        delete track;
        continue; // go to next track
      }

      // Initialize a seed state
      // -----------------------
      if ( m_initState && !m_trueStatesAtMCHits ) {
        sc = this -> initializeState( mcParticle, track );
        if ( !sc.isSuccess() ) {
          debug() << " -> track deleted as unable to initialize state"
                  << endmsg;
          delete track;
          continue; // go to next track
        }
      }

      // Set some of the track properties
      // --------------------------------
      track -> setHistory( Track::TrackIdealPR );
      track -> setPatRecStatus( Track::PatRecIDs ) ;

      // Add true states at each measurement 
      // ===================================
      if ( m_trueStatesAtMCHits ) {
        sc = m_stateCreator -> addMCHitStates(*mcParticle, *track) ;
        if ( sc.isFailure() ) {
          delete track;
          continue;
        }        
      }

      // Add the track to the Tracks container and fill the association table
      // -------------------------------------------------------------------
      tracksCont -> add( track );
      linkTable.link( track, mcParticle, 1. );

      if ( msgLevel( MSG::DEBUG) ){
        printTrack(track);
      }

      // debugging Track ...
      // -------------------
    } // is selected
  } // looping over MCParticles

  debug() << "Created " << tracksCont -> size() << " tracks." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode IdealTracksCreator::finalize()
{
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Add outer tracker clusters
//=============================================================================
StatusCode IdealTracksCreator::addOTTimes( const MCParticle* mcPart,
					   Track* track ) const
{
  unsigned int nOTMeas = 0;
  std::vector<OTChannelID> channels = m_otLinker.keyRange(mcPart);
  for( std::vector<OTChannelID>::const_iterator ichan = channels.begin() ;
       ichan != channels.end(); ++ichan)
    // possible inefficiency
    if (m_uniformDist->shoot() < m_otEff){
      track -> addToLhcbIDs(LHCb::LHCbID(*ichan)) ;
      ++nOTMeas;      
    }
  
  debug() << "- " << nOTMeas << " OTMeasurements added" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addTTClusters( const MCParticle* mcPart,
                                              Track* track ) const
{
  unsigned int nTTMeas = 0;

  const STCluster* aCluster = m_ttLinker.first( mcPart );
  while ( NULL != aCluster ) {

    // possible inefficiency 
    if (m_uniformDist->shoot() < m_ttEff){

      LHCbID id = LHCbID(aCluster->channelID());
      track->addToLhcbIDs(id);
      ++nTTMeas;
	}
    aCluster = m_ttLinker.next();
  }  // loop cluster

  debug() << "- " << nTTMeas << " STMeasurements in TT added" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addITClusters( const MCParticle* mcPart,
                                              Track* track ) const
{
  unsigned int nITMeas = 0;

  const STCluster* aCluster = m_itLinker.first( mcPart );
  while ( NULL != aCluster ) {

    // possible inefficiency
    if (m_uniformDist->shoot() < m_itEff){

      LHCbID id = LHCbID(aCluster->channelID());      
      track->addToLhcbIDs(id);
      ++nITMeas;      
    }
    aCluster = m_itLinker.next();
  }
  debug() << "- " << nITMeas << " STMeasurements in IT added" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addVeloClusters( const MCParticle* mcPart, 
                                                Track* track ) const
{
  unsigned int nVeloMeas   = 0;
  const VeloCluster* aCluster = m_veloLinker.first( mcPart );
  while ( NULL != aCluster ) {

    // possible inefficiency
    if (m_uniformDist->shoot() < m_veloEff){

      LHCbID id = LHCbID(aCluster->channelID());
      if (id.isVeloR() || id.isVeloPhi()){
        // not pileup
        track->addToLhcbIDs(id);
        ++nVeloMeas;
      } // is r or phi
    }
    aCluster = m_veloLinker.next();
  }  // while
 
  debug() << "- " << nVeloMeas << " / " 
          << " Velo Measurements added" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addMuonHits( const MCParticle* mcPart, 
                                                Track* track ) const
{
  unsigned int nMuonMeas   = 0;
  const MuonCoord* aCluster = m_muonLinker.first( mcPart );
  while ( NULL != aCluster ) {

    LHCbID id = LHCbID(aCluster->key());
    track->addToLhcbIDs(id);
    ++nMuonMeas;
    aCluster = m_muonLinker.next();
  }  // while
 
  debug() << "- " << nMuonMeas << " / " 
          << " Muon Measurements added" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialize seed state
//=============================================================================
StatusCode IdealTracksCreator::initializeState( const MCParticle* mcPart,
						Track* track) const
{

  // try to mimic the pattern recognition
  if (track->hasVelo() == true) { 
    State state; state.setLocation(State::EndVelo);
    StatusCode sc = m_stateCreator->createState(mcPart,
                                     800.0 , 
                                     state );
    if ( sc.isSuccess()){
      // smear p
      smearQOverP(state);
      track->addToStates(state);
    }
    else {
      return StatusCode::FAILURE;
    }
  }

  if (track->hasT() == true){
    State state; state.setLocation(State::AtT);
    StatusCode sc = m_stateCreator->createState(mcPart,
                                     StateParameters::ZEndT , 
                                     state );
    if ( sc.isSuccess()){
      smearQOverP(state);
      track->addToStates(state);
    }
    else {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode IdealTracksCreator::initEvent() const{

  if (m_addITClusters == true){
    m_itLinker = STLinker( evtSvc(), msgSvc(), STClusterLocation::ITClusters );
    if ( m_itLinker.notFound() ) {
      return Error( "Unable to retrieve ITCluster to MCParticle Linker table");
    }
  }

  if (m_addTTClusters == true){
    m_ttLinker = STLinker( evtSvc(), msgSvc(), STClusterLocation::TTClusters );
    if ( m_ttLinker.notFound() ) {
      return Error( "Unable to retrieve TTCluster to MCParticle Linker table");
    }
  }

  if (m_addOTTimes == true){
    m_otLinker = OTLinker( evtSvc(), msgSvc(), OTTimeLocation::Default );
    if ( m_otLinker.notFound() ) {
      return Error( "Unable to retrieve OTTimes to MCParticle Linker table");
    }
  }

  if (m_addVeloClusters == true){
    m_veloLinker = VeloLinker( evtSvc(), msgSvc(), VeloClusterLocation::Default );
    if ( m_veloLinker.notFound() ) {
      return Error( "Unable to retrieve VeloCluster to MCParticle Linker table");
    }
  }  

   if (m_addMuonHits == true){
    m_muonLinker = MuonLinker( evtSvc(), msgSvc(), MuonCoordLocation::MuonCoords );
    if ( m_muonLinker.notFound() ) {
      return Error( "Unable to retrieve Muon to MCParticle Linker table");
    }
  } 

  return StatusCode::SUCCESS;
}

void IdealTracksCreator::printMCParticle(const MCParticle* mcParticle) const{

  debug() << "- MCParticle of type "
	  << " , (key # " << mcParticle -> key() << ")" << endmsg
	  << "    - vertex = " << mcParticle -> originVertex() ->position()
	  << endmsg
	  << "    - momentum = " << mcParticle -> momentum() << " MeV" 
	  << endmsg
	  << "    - P        = " << mcParticle -> p()
	  << " MeV" <<endmsg
	  << "    - PID   = "
	  << ( mcParticle -> particleID().pid() ) << endmsg
	  << "    - charge   = "
	  << ( mcParticle -> particleID().threeCharge() / double(3) ) << endmsg;
}

void IdealTracksCreator::printTrack(const Track* track) const{

  debug()
      << "-> Track with key # " << track -> key() << endmsg
      << "  * charge         = " << track -> charge() << endmsg
      << "  * is Invalid     = " << track -> checkFlag( Track::Invalid ) 
      << endmsg
      << "  * is Unique      = " << !track -> checkFlag( Track::Clone ) 
      << endmsg
      << "  * is of type     = " << track -> type() << endmsg
      << "  * is Backward    = " << track -> checkFlag( Track::Backward ) 
      << endmsg
      << "  * # LHCbID's     = " << track -> nLHCbIDs()
      << "  * # measurements = " << track -> nMeasurements() << endmsg;
      
  // print the measurements
  LHCb::Track::MeasurementContainer meas = track->measurements();
  for ( LHCb::Track::MeasurementContainer::const_iterator itMeas = meas.begin();
	itMeas != meas.end(); ++itMeas ) {
       debug() << "  - measurement of type " << (*itMeas) -> type() << endmsg
              << "  - z        = " << (*itMeas) -> z() << " mm" << endmsg
              << "  - LHCbID   = " << (*itMeas) -> lhcbID()  << endmsg;
        // continue according to type ...  
    if ( (*itMeas) -> lhcbID().isOT() ) {
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().otID() 
                  << endmsg;
    }
    else if ( (*itMeas) -> lhcbID().isST() ) {
       debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().stID() 
                  << endmsg;
    }
    else if ( (*itMeas) -> lhcbID().isVelo() ) {    
      debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().veloID() 
                << endmsg;
      }
   }
}

bool IdealTracksCreator::wanted(const Track::Types& aType) const{

  bool selected = true;
  if ( aType == Track::TypeUnknown ||
      ( !m_tracktypes.empty() &&
         std::find( m_tracktypes.begin(), m_tracktypes.end(),  
                    aType ) == m_tracktypes.end() ) ) selected = false;
  return selected;
}

void IdealTracksCreator::smearQOverP(State& aState) const{

  const double smearedValue = aState.qOverP()*(
     1.0 + (m_gaussDist->shoot() * m_smearP   ));
  aState.setQOverP(smearedValue);
}

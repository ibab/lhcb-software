// $Id: IdealTracksCreator.cpp,v 1.38 2007-06-27 15:14:35 mneedham Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// from TrackEvent
#include "Event/TrackTypes.h"

// from MCEvent
#include "Event/MCHit.h"

// from TrackFitEvent
#include "Event/Measurement.h"
#include "Event/OTMeasurement.h"
#include "Event/StateTraj.h"
#include "Event/StateParameters.h"
#include "Event/StateVector.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedFrom.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "MCInterfaces/IMCReconstructible.h"

// Local
#include "IdealTracksCreator.h"

#include "Map.h"
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

using namespace Gaudi;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( IdealTracksCreator );

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
  declareProperty( "AddMeasurements", m_addMeasurements = true           );
  declareProperty( "InitState",       m_initState       = true           );
  declareProperty( "InitStateUpstreamFit", m_initStateUpstreamFit = true );
  declareProperty( "TrueStatesAtMeasZPos", m_trueStatesAtMeas = false    );
  declareProperty( "TracksOutContainer",
                   m_tracksOutContainer = TrackLocation::Ideal           );
  declareProperty( "TrackSelectorTool",
                   m_selectorToolName =  "MCReconstructible"        );
  declareProperty( "MinNHits", m_minNHits = 6                            );
  declareProperty( "TrackTypes", m_tracktypes = boost::assign::list_of(1)(3)(4)(5)(6));

  declareProperty("ITEff", m_itEff = 1.0);
  declareProperty("OTEff", m_otEff = 1.0);
  declareProperty("TTEff", m_ttEff = 1.0);
  declareProperty("VeloEff", m_veloEff = 1.0);
  declareProperty("smearP", m_smearP = 0.0);

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

  m_trackSelector = tool<IMCReconstructible>(m_selectorToolName,"TrackSelectorTool",this);

  // Retrieve the IdealStateCreator tool
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator" );

  // Retrieve the poca tool
  m_poca = tool<ITrajPoca>( "TrajPoca" );
  m_measProvider = tool<IMeasurementProvider>("MeasurementProvider");

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
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode IdealTracksCreator::execute()
{

  StatusCode sc;

  debug() << "==> Execute" << endreq;

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
          << particles -> size() << " MCParticles ... " << endreq;

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

      // Add Muon hits
      // -----------
      if ( m_addMuonHits == true ) {
        debug() << "... adding OTTimes" << endreq;
        sc = addMuonHits( mcParticle, track );
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
          State tempState;
          sc = m_stateCreator -> createState(mcParticle,
                                             (*iMeas)->z(),
                                             tempState );
          if ( sc.isSuccess() ) track -> addToStates( tempState );
          ++iMeas;
        }
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
  const OTTime* aTime = m_otLinker.first( mcPart );
  while ( NULL != aTime ) {

    // possible inefficiency
    if (m_uniformDist->shoot() < m_otEff){

      LHCbID id = LHCbID(aTime->channel()); 
      track -> addToLhcbIDs(id);
      
      if ( m_addMeasurements ) { // Add the measurement to the track

        StatusCode sc;
        Measurement* meas = m_measProvider->measurement(id);
        if (meas != 0){

        // Set the reference vector
        StateVector tempState;
        sc = m_stateCreator -> createStateVector( mcPart, meas->z(), tempState );
        if ( sc.isSuccess() ) {
            meas->setRefVector( tempState.parameters() );
          
            // Get the ambiguity using the Poca tool
            XYZVector distance;
            static XYZVector bfield = XYZVector();
            StateTraj stateTraj = StateTraj( meas->refVector(), meas->z(), bfield );
            double s1 = 0.0;
            double s2 = (meas->trajectory()).arclength( stateTraj.position(s1) );
            StatusCode msc = m_poca -> minimize(stateTraj, s1, meas->trajectory(),
                                              s2, distance, 20*Gaudi::Units::mm );
            if( msc.isFailure() ) {
              warning() << "TrajPoca minimize failed in addOTTimes." << endreq;
            }
            int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;
            OTMeasurement* otMeas = dynamic_cast<OTMeasurement*>(meas);
            otMeas->setAmbiguity( ambiguity );
          
            // Set the z-position of the measurement
            meas->setZ( stateTraj.position(s1).z() );
          
            // Reset using the updated z-position
            StateVector myTempState;
            sc = m_stateCreator->createStateVector(mcPart, meas->z(), myTempState);
            if ( sc.isSuccess() ) meas->setRefVector( myTempState.parameters() );
          
            track -> addToMeasurements( *meas );
            delete meas; 
    	  }
          else {
            Warning( "Failed to calculate ref. info. and ambiguity for OTMeasurement" );
	  }
        } // meas        
      } // if ( m_addMeasurements ) 
      
      ++nOTMeas;      
    }
    aTime = m_otLinker.next();
  } // loop over OTTimes

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

  const STCluster* aCluster = m_ttLinker.first( mcPart );
  while ( NULL != aCluster ) {

    // possible inefficiency 
    if (m_uniformDist->shoot() < m_ttEff){

      LHCbID id = LHCbID(aCluster->channelID());
      track->addToLhcbIDs(id);

      if ( m_addMeasurements ) { // Add the measurement to the track
        // Set the reference vector
        Measurement* meas = m_measProvider->measurement(id);
        if (meas != 0){
          StateVector tempState;
          StatusCode sc = m_stateCreator->createStateVector(mcPart,
                                                        meas->z(),tempState);
          if ( sc.isSuccess() ) {
            meas->setRefVector( tempState.parameters() ); 
            track -> addToMeasurements( *meas );
            delete meas; 
          }
          else {
            Warning( "Failed to calculate ref. info. for STMeasurement in TT" );
          }
        } // meas
      }
      ++nTTMeas;
	}
    aCluster = m_ttLinker.next();
  }  // loop cluster

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

  const STCluster* aCluster = m_itLinker.first( mcPart );
  while ( NULL != aCluster ) {

    // possible inefficiency
    if (m_uniformDist->shoot() < m_itEff){

      LHCbID id = LHCbID(aCluster->channelID());      
      track->addToLhcbIDs(id);
      
      if ( m_addMeasurements ) { // Add the measurement to the track
 
        Measurement* meas = m_measProvider->measurement(id);
        if (meas != 0){
      
          StatusCode sc;
          // Set the reference vector
          StateVector tempState;
          sc = m_stateCreator->createStateVector(mcPart,meas->z(),tempState);
          if ( sc.isSuccess() ) {
            meas->setRefVector( tempState.parameters() );
          
            // Set the z-position of the measurement
            XYZVector distance;
            static XYZVector bfield = XYZVector();
            StateTraj stateTraj = StateTraj( meas->refVector(), meas->z(), bfield );
            double s1 = 0.0;
            double s2 = (meas->trajectory()).arclength( stateTraj.position(s1) );
            StatusCode msc = m_poca->minimize(stateTraj, s1, meas->trajectory(), s2,
                                              distance, 0.002*Gaudi::Units::mm);
            if( msc.isFailure() ) {
              warning() << "TrajPoca minimize failed in addITClusters." << endreq;
            }
            meas->setZ( stateTraj.position(s1).z() );
          
            // Reset using the updated z-position
            StateVector myTempState;
            sc = m_stateCreator->createStateVector(mcPart, meas->z(), myTempState);
            if ( sc.isSuccess() ) meas->setRefVector( myTempState.parameters() );
            track -> addToMeasurements( *meas );                 
            delete meas;  
          }
          else {
            Warning( "Failed to calculate ref. info. for STMeasurement in IT" );
	  }
        } // meas != 0           
      } // if ( m_addMeasurements )    
      ++nITMeas;      
    }
    aCluster = m_itLinker.next();
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
        if ( m_addMeasurements ) { // Add the measurement to the track
          Measurement* meas = m_measProvider->measurement(id);
          if (meas != 0){
            StateVector tempState;
            StatusCode sc = m_stateCreator->createStateVector(mcPart, meas->z(), tempState);
            if ( sc.isSuccess()) {
              meas->setRefVector( tempState.parameters());
	    }		     
            else {
              Warning( "Failed to calculate ref. info. for VeloMeasurement" );
	    }
            track->addToMeasurements(*meas);
            delete meas;		         
	  }
        } // add  measurements	
      } // is r or phi
    }
    aCluster = m_veloLinker.next();
  }  // while
 
  debug() << "- " << nVeloMeas << " / " 
          << " Velo Measurements added" << endreq;
  
  return StatusCode::SUCCESS;
};

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

    if ( m_addMeasurements ) { // Add the measurement to the track
      Measurement* meas = m_measProvider->measurement(id);
      if (meas != 0){
        StateVector tempState;
        StatusCode sc = m_stateCreator->createStateVector(mcPart, meas->z(), tempState);
        if ( sc.isSuccess()) {
          meas->setRefVector( tempState.parameters());
	}		     
        else {
          Warning( "Failed to calculate ref. info. for VeloMeasurement" );
	}
        track->addToMeasurements(*meas);
          delete meas;		         
	}
      } // add  measurements	
    aCluster = m_muonLinker.next();
  }  // while
 
  debug() << "- " << nMuonMeas << " / " 
          << " Muon Measurements added" << endreq;
  
  return StatusCode::SUCCESS;
};

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
                                     StateParameters::ZAtT , 
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
};

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
    if ( m_ttLinker.notFound() ) {
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
}

void IdealTracksCreator::printTrack(const Track* track) const{

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
   const std::vector<Measurement*>& meas = track->measurements();
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

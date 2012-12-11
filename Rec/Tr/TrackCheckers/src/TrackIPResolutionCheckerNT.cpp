// Include files 

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/MCParticle.h"
#include "Kernel/HitPattern.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// Boost
#include <boost/foreach.hpp>

/** @class TrackMonitor TrackMonitor.h "TrackCheckers/TrackMonitor"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class PrResCheckerNT : public GaudiTupleAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  PrResCheckerNT( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~PrResCheckerNT();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm initialize */
  virtual StatusCode initialize();

 private:

private:
  //std::string m_tracksInContainer;    ///< Input Tracks container location
};

DECLARE_ALGORITHM_FACTORY( PrResCheckerNT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrResCheckerNT::PrResCheckerNT(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator )
{
  //declareProperty( "TracksInContainer", m_tracksInContainer = LHCb::TrackLocation::Default  );
}

//=============================================================================
// Destructor
//=============================================================================
PrResCheckerNT::~PrResCheckerNT() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode PrResCheckerNT::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode PrResCheckerNT::execute()
{

  std::string m_tracklocation = LHCb::TrackLocation::Velo;
  if ( true ) m_tracklocation = LHCb::TrackLocation::VP;

  // get the list of tracks
  const LHCb::Tracks* tracks = get<LHCb::Tracks>(m_tracklocation);

  // get the linker table track -> mcparticle
  LinkedTo<LHCb::MCParticle> linker( evtSvc(), msgSvc(), m_tracklocation );  

  // create the ntuple
  Tuple theTuple = nTuple( "tracks" , "" , CLID_ColumnWiseTuple );

  // loop over the tracks
  BOOST_FOREACH(const LHCb::Track* aTrack, *tracks) {
    
    theTuple->column("p", aTrack->p()/Gaudi::Units::GeV );
    theTuple->column("probChi2", aTrack->probChi2() );
    theTuple->column("chi2",aTrack->chi2() );
    theTuple->column("ndof",aTrack->nDoF() );
    theTuple->column("type",aTrack->type() ) ;
    theTuple->column("z", aTrack->firstState().z() ) ;
    theTuple->column("x", aTrack->firstState().x() ) ;
    theTuple->column("y", aTrack->firstState().y() ) ;
    theTuple->column("tx", aTrack->firstState().tx() ) ;
    theTuple->column("ty", aTrack->firstState().ty() ) ;
    theTuple->column("qop", aTrack->firstState().qOverP() ) ;
    theTuple->column("veloChi2", aTrack->info(LHCb::Track::FitVeloChi2,0)) ;
    theTuple->column("veloNdof", aTrack->info(LHCb::Track::FitVeloNDoF,0)) ;
    theTuple->column("TChi2", aTrack->info(LHCb::Track::FitTChi2,0)) ;
    theTuple->column("TNdof", aTrack->info(LHCb::Track::FitTNDoF,0)) ;
    
    LHCb::HitPattern hitpattern(aTrack->lhcbIDs()) ;
    theTuple->column("numVeloStations",hitpattern.numVeloStations()) ;
    theTuple->column("numVeloStationsOverlap",hitpattern.numVeloStationsOverlap()) ;
    theTuple->column("numITStationsOverlap",hitpattern.numITStationsOverlap()) ;
    theTuple->column("numITOTStationsOverlap",hitpattern.numITOTStationsOverlap()) ;
    theTuple->column("numVeloHoles",hitpattern.numVeloHoles()) ;
    theTuple->column("numTHoles",hitpattern.numTHoles()) ;
    theTuple->column("numTLayers",hitpattern.numTLayers()) ;
    theTuple->column("numVeloStations",hitpattern.numVeloStations()) ;
    theTuple->column("numVeloClusters",hitpattern.numVeloR()+hitpattern.numVeloPhi()) ;

    int waslinked = false ;
    const LHCb::MCParticle* mcparticle = linker.first(aTrack) ;
    if(mcparticle && mcparticle->originVertex()) {
      waslinked= true ;
      
      Gaudi::XYZPoint trueorigin = mcparticle->originVertex()->position() ;
      const LHCb::State& state   = aTrack->firstState() ;
      theTuple->column("truex",trueorigin.x()) ;
      theTuple->column("truey",trueorigin.y()) ;
      theTuple->column("truez",trueorigin.z()) ;
      theTuple->column("truemom",mcparticle->p()) ;
      theTuple->column("truepid",mcparticle->particleID().pid()) ;
      const LHCb::MCParticle* mother = mcparticle->originVertex()->mother() ;
      theTuple->column("truemotherpid", mother? mother->particleID().pid() : 0 ) ;
      theTuple->column("truepx",mcparticle->momentum().Px()) ;
      theTuple->column("truepy",mcparticle->momentum().Py()) ;
      theTuple->column("truepz",mcparticle->momentum().Pz()) ;
      theTuple->column("truept",mcparticle->momentum().Pt()) ;
      theTuple->column("trueeta",mcparticle->momentum().eta()) ;   
      theTuple->column("truephi",mcparticle->momentum().phi()) ;

      double dz = trueorigin.z() - state.z() ;
      double IPx = (state.x() + dz* state.tx()) - trueorigin.x() ;
      double IPy = (state.y() + dz* state.ty()) - trueorigin.y() ;
      theTuple->column("IPx",IPx) ;
      theTuple->column("IPy",IPy) ;
    }
    
    theTuple->column("waslinked",waslinked) ;
    
    theTuple->write();
  }

  return StatusCode::SUCCESS ;
  
}


// Include files 
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Kernel/HitPattern.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// Boost
#include <boost/foreach.hpp>

/** @class TrackIPResolutionChecker TrackIPResolutionChecker.cpp "TrackCheckers/TrackIPResolutionChecker"
 * 
 * Class for track monitoring
 *  @author W. Hulsbergen and P. Tsopelas
 *  @date   21-01-2013
 */                 
                                                           
class TrackIPResolutionChecker : public GaudiHistoAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackIPResolutionChecker( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackIPResolutionChecker();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm initialize */
  virtual StatusCode initialize();

 private:

private:
  std::string m_tracklocation ;    ///< Input Tracks container location
};

DECLARE_ALGORITHM_FACTORY( TrackIPResolutionChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackIPResolutionChecker::TrackIPResolutionChecker(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator )
{
  declareProperty( "TracksInContainer", m_tracklocation = LHCb::TrackLocation::VP );
}

//=============================================================================
// Destructor
//=============================================================================
TrackIPResolutionChecker::~TrackIPResolutionChecker() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackIPResolutionChecker::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}


//=============================================================================
// Execute
//=============================================================================
StatusCode TrackIPResolutionChecker::execute()
{
  
  // get the list of tracks
  const LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_tracklocation);

  // get the linker table track -> mcparticle
  LinkedTo<LHCb::MCParticle> linker( evtSvc(), msgSvc(), m_tracklocation );  

  // loop over the tracks
  BOOST_FOREACH(const LHCb::Track* track, tracks) {
    const LHCb::MCParticle* mcparticle = linker.first(track) ;
    if( mcparticle && mcparticle->originVertex() &&
	mcparticle->momentum().Pz()>0 &&
	mcparticle->momentum().Pt()> 200 ) {
      
      double invtrueptGeV = Gaudi::Units::GeV/mcparticle->momentum().Pt() ;
	
      plot(track->lhcbIDs().size(),"NumLHCBIDs","Number of LHCbIDs",-0.5,40.5,41) ;
      Gaudi::XYZPoint trueorigin = mcparticle->originVertex()->position() ;
      plot(trueorigin.z(),"TrueOriginZ","True origin z",-100,100) ;
      plot(trueorigin.x(),"TrueOriginX","True origin x",-100,100) ;
      plot(trueorigin.y(),"TrueOriginX","True origin y",-100,100) ;

      // for now, the track state is simply firststate
      const LHCb::State& state = track->firstState() ;

      double tx = state.tx() ;
      double ty = state.ty() ;
      double dz = trueorigin.z() - state.z() ;
      double IPx = (state.x() + dz* state.tx()) - trueorigin.x() ;
      double IPy = (state.y() + dz* state.ty()) - trueorigin.y() ;
      double IP3D = std::sqrt( (IPx*IPx+IPy*IPy)/(1 + tx*tx + ty*ty) ) ;
      plot2D( invtrueptGeV, IPx, "IPXVsInvTruePtH2", "IPx versus 1/pt_true", 0, 0.3, -0.5, 0.5, 50, 200 ) ;
      plot2D( invtrueptGeV, IPy, "IPYVsInvTruePtH2", "IPx versus 1/pt_true", 0, 0.3, -0.5, 0.5, 50, 200 ) ;
      plot2D( invtrueptGeV, IP3D, "IP3DVsInvTruePtH2", "IP versus 1/pt_true", 0, 0.3, -0.5, 0.5, 50, 200 ) ;
    }
  }

  return StatusCode::SUCCESS ;
}



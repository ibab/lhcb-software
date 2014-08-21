// $Id: TrackSeedT0Alg.cpp,v 1.3 2009-10-09 10:35:59 wouter Exp $
// Include files
#include <GaudiAlg/GaudiHistoAlg.h>

#include <sstream>
#include <boost/foreach.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// Event
#include <Event/Track.h>
#include <Event/State.h>
#include <Event/FitNode.h>
#include <Event/OTMeasurement.h>

// local
class TrackSeedT0Alg : public GaudiHistoAlg
{
public: 
  TrackSeedT0Alg(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TrackSeedT0Alg( ); ///< Destructor
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  
  // Properties
  std::string m_trackLocation;
  std::string m_caloFwdTrackLocation;
  std::string m_caloBwdTrackLocation;
  bool m_useCaloTiming ;
  bool m_useFittedDistance ;
  bool m_rejectTimeOutliers ;
  double m_maxTimeResidual ;
  
  const LHCb::Track* getCaloTrack();
};

//-----------------------------------------------------------------------------
// Implementation file for class : TrackSeedT0Alg
//
// 2008-09-11 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackSeedT0Alg )

using std::stringstream;

//=============================================================================
TrackSeedT0Alg::TrackSeedT0Alg(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default) ;
  declareProperty("UseCaloTiming", m_useCaloTiming = false ) ;
  declareProperty("RejectTimeOutliers", m_rejectTimeOutliers = false ) ;
  declareProperty("UseFittedDistance", m_useFittedDistance = false ) ;
  declareProperty("MaxTimeResidual", m_maxTimeResidual = 20 ) ;
  declareProperty("CaloFwdTrackLocation", m_caloFwdTrackLocation = LHCb::TrackLocation::CaloCosmicsForward);
  declareProperty("CaloBwdTrackLocation", m_caloBwdTrackLocation = LHCb::TrackLocation::CaloCosmicsBackward);
}

//=============================================================================
TrackSeedT0Alg::~TrackSeedT0Alg()
{

}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackSeedT0Alg::initialize()
{
   StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   if( m_useCaloTiming ) {
     error() << "Cannot use calo timing at the moment, because need to migrate to CaloTool. (wdh 2009/09/06)." << endreq ;
     return StatusCode::FAILURE ; 
   }

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

struct NodeT0
{
  size_t node ;
  double t0 ;
  NodeT0(size_t n,double t) : node(n),t0(t) {}
} ;

inline bool CompareNodeT0( const NodeT0& lhs, const NodeT0& rhs )
{
  return lhs.t0 < rhs.t0 ;
}

StatusCode TrackSeedT0Alg::execute()
{
   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   // Reference z-position for the event t0. Must match the one in the projector.
   const double zref = 12800 ;

   // First get the calo tracks.
   double calt0 = 0 ;
   double calz0 = zref ;
   // should try and catch here ... this should still work if calo isn't there ...
   //if( m_useCaloTiming ) {
   const LHCb::Track* calotrack = getCaloTrack(); 
   if( calotrack ) {
     //calt0 = calotrack->info(LHCb::Track::CaloTimeInfo, 0);
     calt0 = 0 ;
     calz0 = calotrack->firstState().z() ;
     debug() << "Seeding track with calo t0=" << calt0 << " z0=" << calz0 << endmsg ;
   } else {
     Warning("No Calo track found. Initializing event t0 with 0",StatusCode::SUCCESS,0) ;
   }
   //}

   // Get the Tracks.
   LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);
   for (LHCb::Tracks::iterator itrk = tracks->begin(); itrk != tracks->end(); ++itrk) {
     const LHCb::State& trkstate = (*itrk)->firstState() ;
     double slope  = std::sqrt(1 + trkstate.tx()*trkstate.tx() + trkstate.ty()*trkstate.ty()) ;
     bool forward  = trkstate.ty() < 0 ;
     double dtofdz = (forward ? 1 : -1) * slope / Gaudi::Units::c_light ;
     
     double tof = (calz0-zref) * dtofdz ;
     double caloEventT0 = calt0 - tof ;
     (*itrk)->addInfo(1001, caloEventT0) ;

     // let;s also calculate an estimate from the drift time
     // average. even better is from the residuals, but we do that
     // later
     double otEventT0(0) ;
     {
       double tsum(0) ;
       size_t nmeas(0) ;
       std::vector< NodeT0 > t0vec ;
       size_t nodeindex(0) ;
       BOOST_FOREACH( const LHCb::Node* node, (*itrk)->nodes()) {
	 ++nodeindex ;
	 if( node->type() == LHCb::Node::HitOnTrack &&
	     node->measurement().type() == LHCb::Measurement::OT ) {
	   const LHCb::OTMeasurement* meas = dynamic_cast<const LHCb::OTMeasurement*>(&(node->measurement())) ;
	   double drifttime = meas->calibratedTime() - meas->propagationTimeFromY( node->state().y()) ;
	   // this is where we could use the distance between track and straw
	   double dist = 0.5*2.45 ;
	   if( m_useFittedDistance ) {
	     const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(node) ;
	     dist = - fitnode->unbiasedResidual() ; // works only if fit made without drifttime!!
	     plot(dist,"dist","distance to wire",-5,5) ;
	   }
	   double expectedtime = meas->module().driftTimeWithError(std::abs(dist)).val ;
	   double tof = (meas->z() - zref) * dtofdz ;
	   double thist0 = (drifttime - expectedtime) - tof ;
	   tsum += thist0 ;
	   t0vec.push_back( NodeT0(nodeindex,thist0) ) ;
	   ++nmeas ;
	 }
       }
       if( t0vec.size()>0 ) {
	 // make a truncated mean. how? choose mean in interval with fixed width
	 std::sort( t0vec.begin(), t0vec.end(), CompareNodeT0 ) ;
	 // 	 std::cout << " t0s: " ;
	 // 	 for( size_t i=0; i<t0vec.size(); ++i) 
	 // 	   std::cout << std::setprecision(2) << t0vec[i].t0 << ", " ;
	 // 	 std::cout << std::endl ;

	 const double window = m_maxTimeResidual ;
	 size_t bestpos(0), numaccepted(1) ;
	 for( size_t ipos=0; 
	      ipos<t0vec.size() && ipos + numaccepted < t0vec.size() ;
	      ++ipos) {
	   size_t jpos = ipos+1 ;
	   while( jpos<t0vec.size() &&
		  t0vec[jpos].t0 - t0vec[ipos].t0 < window ) ++jpos ;
	   if( jpos - ipos > numaccepted ) {
	     bestpos     = ipos ;
	     numaccepted = jpos - ipos ;
	   }
	 }
	 //std::cout << "best pos: " << bestpos << std::endl ;
	 //if( double(numaccepted)/t0vec.size() < 0.6 ) std::cout << "PROBLEM" << std::endl ;

	 plot( double(numaccepted)/t0vec.size(),"acceptfrac","fraction of accepted for t0",0,1) ;

	 // take the mean of what is left
	 double tsum(0) ;
	 for( size_t ipos = bestpos; ipos<bestpos+numaccepted; ++ipos)
	   tsum += t0vec[ipos].t0 ;
	 otEventT0 = tsum/numaccepted ;
	   // now reject all times that are too far away.
	 size_t numrejected(0) ;
	 for( size_t ipos = 0; ipos<t0vec.size(); ++ipos) {
	   double residual = t0vec[ipos].t0 - otEventT0 ;
	   plot1D( residual,"timeres","time res", -50,50) ;
	   if( m_rejectTimeOutliers && std::abs(t0vec[ipos].t0 - otEventT0) > m_maxTimeResidual ) {
	     const_cast<LHCb::Node*>((*itrk)->nodes()[t0vec[ipos].node])->setType( LHCb::Node::Outlier) ;
	     ++numrejected ; 
	   }
	 }
	 plot( double(numrejected),"noutliers","number of outliers",-0.5,20.5,21) ;
	 plot( double(numrejected)/t0vec.size(),"fracoutliers","fraction of outliers",0,1) ;
       }
     }
     (*itrk)->addInfo(1002, otEventT0) ;
     

     double eventT0 = m_useCaloTiming ? caloEventT0 : otEventT0 ;
     // now udpate all reference states
     for( LHCb::Track::StateContainer::const_iterator istate = (*itrk)->states().begin(); 
	  istate != (*itrk)->states().end(); ++istate )
       (const_cast<LHCb::State*>(*istate))->setQOverP(eventT0) ;
     // it could also be that this track was fitted already. in that case we update all nodes
     BOOST_FOREACH( const LHCb::Node* node, (*itrk)->nodes()) 
       (const_cast<LHCb::State&>(node->state())).setQOverP(eventT0) ;
   }
   
   return StatusCode::SUCCESS;
}
		
		//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackSeedT0Alg::finalize()
{

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

   return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
const LHCb::Track* TrackSeedT0Alg::getCaloTrack()
{
  const LHCb::Tracks* forwardTracks = 0;
  const LHCb::Tracks* backwardTracks = 0;
  if (exist<LHCb::Tracks>(m_caloFwdTrackLocation)) {
    forwardTracks = get<LHCb::Tracks>(m_caloFwdTrackLocation);
  } else {
    std::stringstream s ;
    s << "Cannot find container for CaloFwdTracks at "
      << m_caloFwdTrackLocation << std::endl ;
    Warning(s.str(),StatusCode::FAILURE,1) ;
  }
  if (exist<LHCb::Tracks>(m_caloBwdTrackLocation)) {
    backwardTracks = get<LHCb::Tracks>(m_caloBwdTrackLocation);
  } else {
    std::stringstream s ;
    s << "Cannot find container for CaloBwdTracks at "
      << m_caloBwdTrackLocation << std::endl ;
    Warning(s.str(),StatusCode::FAILURE,1) ;
  }
    
  const LHCb::Track* track(0) ;
  if( forwardTracks && !forwardTracks->empty()) {
    track = *(forwardTracks->begin()) ;
  }
  if( !track && backwardTracks && !backwardTracks->empty()) {
    track = *(backwardTracks->begin()) ;
  }
  return track ;
}

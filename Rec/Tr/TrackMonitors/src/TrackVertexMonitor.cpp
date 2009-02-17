#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"

class TrackVertexMonitor : public GaudiHistoAlg 
{
public:

   /** Standard construtor */
  TrackVertexMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackVertexMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();
  
private:
  std::string m_trackContainerName;
  std::string m_pvContainerName;
  double m_ipmax ;
  double m_dzmax ;
  double m_rpvmax ;
  double m_zpvmin ;
  double m_zpvmax ;
  double m_maxLongTrackChisqPerDof ;
  size_t m_nprbins ;

  ToolHandle<ITrackVertexer> m_vertexer ;

  AIDA::IHistogram1D* m_trackTransverseIP ;
  AIDA::IProfile1D* m_trackTransverseIPVsPhi ;
  AIDA::IProfile1D* m_trackTransverseIPVsEta ;
  AIDA::IHistogram1D* m_trackLongitudinalIP ;
  AIDA::IProfile1D* m_trackLongitudinalIPVsPhi ;
  AIDA::IProfile1D* m_trackLongitudinalIPVsEta ;

  AIDA::IHistogram1D* m_twoprongMass ;
  AIDA::IHistogram1D* m_twoprongDoca ;
  AIDA::IHistogram1D* m_twoprongDocaPull ;
  AIDA::IProfile1D* m_twoprongDocaVsEta ; 
  AIDA::IProfile1D* m_twoprongDocaVsPhi ;

} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackVertexMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVertexMonitor::TrackVertexMonitor( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_vertexer("TrackVertexer")
{
  declareProperty( "TrackContainer", m_trackContainerName = LHCb::TrackLocation::Default  );
  declareProperty( "PVContainer", m_pvContainerName = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "MaxIP", m_ipmax = 0.5*Gaudi::Units::mm ) ;
  declareProperty( "MaxDz", m_dzmax =   5*Gaudi::Units::mm ) ;
  declareProperty( "MaxRPV", m_rpvmax = 1*Gaudi::Units::mm ) ;
  declareProperty( "MinZPV", m_zpvmin = -20*Gaudi::Units::cm ) ;
  declareProperty( "MaxZPV", m_zpvmax =  20*Gaudi::Units::cm ) ;
  declareProperty( "MaxLongTrackChisqPerDof", m_maxLongTrackChisqPerDof
 = 5 ) ;
  declareProperty( "NumProfileBins", m_nprbins = 20 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackVertexMonitor::~TrackVertexMonitor() 
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackVertexMonitor::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;              // error printed already by GaudiAlgorithm
  sc = m_vertexer.retrieve() ;

  // book histograms
  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  // impact parameters of the hightest Pt track wrt the vertex
  m_trackTransverseIP   = book1D("fast track transverse IP",-m_ipmax,m_ipmax) ;
  m_trackLongitudinalIP = book1D("fast track longitudinal IP",-m_ipmax,m_ipmax) ;
  m_trackTransverseIPVsPhi = bookProfile1D("fast track transverse IP vs phi",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_trackTransverseIPVsEta = bookProfile1D("fast track transverse IP vs eta",2.0,5.0,m_nprbins) ;
  m_trackLongitudinalIPVsPhi = bookProfile1D("fast track longitudinal IP vs phi",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;
  m_trackLongitudinalIPVsEta = bookProfile1D("fast track longitudinal IP vs eta",2.0,5.0,m_nprbins) ;

  // impact parameter and vertex chisquare of the two highest Pt tracks
  m_twoprongMass      = book1D("twoprong mass",0,20*Gaudi::Units::GeV) ;
  m_twoprongDoca      = book1D("twoprong doca",-m_ipmax,m_ipmax) ;
  m_twoprongDocaPull  = book1D("twoprong doca pull",-5,5) ;
  m_twoprongDocaVsEta = bookProfile1D("twoprong doca vs vs eta",2.0,5.0,m_nprbins) ;
  m_twoprongDocaVsPhi = bookProfile1D("twoprong doca vs phi",-Gaudi::Units::pi,Gaudi::Units::pi,m_nprbins) ;

  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackVertexMonitor::finalize() 
{
  return m_vertexer.release() && GaudiHistoAlg::finalize() ;
}


std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
{
  std::vector<const LHCb::Track*> rc(tracks.size()) ;
  std::copy(tracks.begin(),tracks.end(),rc.begin()) ;
  return rc ;
}

template< class TrackContainer, class Predicate >
std::vector<const LHCb::Track*> myselect( const TrackContainer& tracks, Predicate selector) 
{
  std::vector<const LHCb::Track*> rc ;
  for( typename TrackContainer::const_iterator itrack = tracks.begin() ;
       itrack != tracks.end(); ++itrack) 
    if( selector(*itrack) ) rc.push_back(*itrack) ;
  return rc ;
}

struct TrackTypePredicate
{
  int m_type ;
  TrackTypePredicate(int atype) : m_type(atype) {} 
  bool operator()(const LHCb::Track* track) const { return track->type()==m_type ; }
} ;

struct TrackFlagPredicate
{
  LHCb::Track::Flags m_flag ;
  bool m_positive ;
  TrackFlagPredicate(LHCb::Track::Flags flag, bool positive=true) : m_flag(flag), m_positive(positive) {} 
  bool operator()(const LHCb::Track* track) const { return m_positive ? track->checkFlag(m_flag) : !track->checkFlag(m_flag) ; }
} ;

struct TrackTXPredicate
{
  int m_sign ;
  TrackTXPredicate(int asign) : m_sign(asign) {}
  bool operator()(const LHCb::Track* track) const { return track->firstState().tx()*m_sign>0 ; }
} ;

struct TrackChisqPredicate
{
  double m_maxchisq ;
  TrackChisqPredicate(double maxChisqPerDof) : m_maxchisq(maxChisqPerDof) {}
  bool operator()(const LHCb::Track* track) const { return track->chi2PerDoF() < m_maxchisq  ; }
} ;

#include "Event/TrackStateVertex.h"
#include "Event/TwoProngVertex.h"

StatusCode TrackVertexMonitor::execute()
{ 
  if (!exist<LHCb::RecVertices>(m_pvContainerName)) 
    return Warning( m_pvContainerName+" not found", StatusCode::SUCCESS, 0);
  const LHCb::RecVertices* pvcontainer = get<LHCb::RecVertices>( m_pvContainerName ) ;
  //const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackContainerName );
  
  TrackTypePredicate isLong( LHCb::Track::Long ) ;
  TrackFlagPredicate isBackward( LHCb::Track::Backward ) ;
  TrackFlagPredicate isForward( LHCb::Track::Backward,false ) ;
  
  // lists needed
  // - primary vertices
  // - all tracks
  // - long tracks
  // - backward tracks
  // for now I'll just create the track lists from the Best container
  typedef std::vector<const LHCb::Track*> TrackVector ;

  // number of primary vertices
  plot(pvcontainer->size(),"NumPrimaryVertices",-0.5,10.5,11) ;

  for( LHCb::RecVertices::const_iterator ipv = pvcontainer->begin() ;
       ipv != pvcontainer->end(); ++ipv ) {
    const LHCb::RecVertex* pv = *ipv ;
    TrackVector tracks = myconvert(pv->tracks()) ;
    TrackVector forwardtracks = myselect(tracks,isForward) ;
    TrackVector backwardtracks =  myselect(tracks,isBackward) ;
    TrackVector longtracks =  myselect(tracks,isLong) ;
    TrackVector goodlongtracks = myselect(tracks,TrackChisqPredicate(m_maxLongTrackChisqPerDof)) ;
    
    // number of tracks per primary vertex
    plot( tracks.size(), "NumTracksPerPV",-0.5,99.5,50) ;
    // number of long tracks per primary vertex
    plot( longtracks.size(), "NumLongTracksPerPV",-0.5,99.5,50) ;
    // number of backward tracks per primary vertex
    plot( backwardtracks.size(), "NumBackTracksPerPV",-0.5,99.5,50) ;
    // chisquare
    plot( pv->chi2() / pv->nDoF(), "PV chisquare per dof",0,20) ;
    // position
    plot( pv->position().x(), "PV x position",-m_rpvmax,m_rpvmax) ;
    plot( pv->position().y(), "PV y position",-m_rpvmax,m_rpvmax) ;
    plot( pv->position().z(), "PV z position", m_zpvmin,m_zpvmax) ;
    if( fabs( pv->position().y() ) < m_rpvmax ) 
      profile1D( pv->position().z(), pv->position().y(),"PV y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
    if( fabs( pv->position().x() ) < m_rpvmax ) 
      profile1D( pv->position().z(), pv->position().x(),"PV x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
    
    // refit the primary vertex with only the long tracks
    if(longtracks.size()>=2) {
      LHCb::RecVertex* longvertex = m_vertexer->fit( longtracks ) ;
      if(longvertex) plot( longvertex->chi2() / longvertex->nDoF(), "PV long chisquare per dof",0,20) ;
      delete longvertex ;
    }
    
    // now split the primary vertex in left and right tracks
    TrackVector lefttracks = myselect(tracks,TrackTXPredicate(+1)) ;
    TrackVector righttracks =  myselect(tracks,TrackTXPredicate(-1)) ;
    if( lefttracks.size() >= 2 && righttracks.size() >= 2 ) {
      // fit two vertices
      LHCb::RecVertex* leftvertex  = m_vertexer->fit( lefttracks ) ;
      LHCb::RecVertex* rightvertex = m_vertexer->fit( righttracks ) ;
      if( leftvertex && rightvertex) {
	// draw the difference
	Gaudi::XYZVector dx = leftvertex->position() - rightvertex->position() ;

	plot( dx.x(), "PV left-right delta x",-m_ipmax,m_ipmax) ;
	plot( dx.y(), "PV left-right delta y",-m_ipmax,m_ipmax) ;
	plot( dx.z(), "PV left-right delta z",-m_dzmax,m_dzmax) ;
	if( fabs( dx.y() ) < m_ipmax ) 
	  profile1D( pv->position().z(), dx.y(),"PV left-right delta y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
	if( fabs( dx.x() ) < m_ipmax ) 
	  profile1D( pv->position().z(), dx.x(),"PV left-right delta x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
	
	// draw the pull of the difference
	Gaudi::SymMatrix3x3 cov = leftvertex->covMatrix() + rightvertex->covMatrix() ;
	plot( dx.x()/std::sqrt(cov(0,0)), "PV left-right delta x pull",-5,5) ;
	plot( dx.y()/std::sqrt(cov(1,1)), "PV left-right delta y pull",-5,5) ;
	plot( dx.z()/std::sqrt(cov(2,2)), "PV left-right delta z pull",-5,5) ;
	// draw the chisquares
	plot( leftvertex->chi2() / leftvertex->nDoF(), "PV left chisquare per dof",0,20) ;
	plot( rightvertex->chi2() / rightvertex->nDoF(), "PV right chisquare per dof",0,20) ;
      }
      delete leftvertex ;
      delete rightvertex ;
    }
    
    if( forwardtracks.size() >= 2 && backwardtracks.size() >= 2 ) {
      // fit two vertices
      LHCb::RecVertex* forwardvertex  = m_vertexer->fit( forwardtracks ) ;
      LHCb::RecVertex* backwardvertex = m_vertexer->fit( backwardtracks ) ;
      if( forwardvertex && backwardvertex) {
	Gaudi::XYZVector dx = forwardvertex->position() - backwardvertex->position() ;
	
	// draw the difference
	plot( dx.x(), "PV forward-backward delta x",-m_ipmax,m_ipmax) ;
	plot( dx.y(), "PV forward-backward delta y",-m_ipmax,m_ipmax) ;
	plot( dx.z(), "PV forward-backward delta z",-m_dzmax,m_dzmax) ;
	if( fabs( dx.y() ) < m_ipmax ) 
	  profile1D( pv->position().z(), dx.y(),"PV forward-backward delta y versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
	if( fabs( dx.x() ) < m_ipmax ) 
	  profile1D( pv->position().z(), dx.x(),"PV forward-backward delta x versus z",m_zpvmin,m_zpvmax,m_nprbins) ;
	
	// draw the pull of the difference
	Gaudi::SymMatrix3x3 cov = forwardvertex->covMatrix() + backwardvertex->covMatrix() ;
	plot( dx.x()/std::sqrt(cov(0,0)), "PV forward-backward delta x pull",-5,5) ;
	plot( dx.y()/std::sqrt(cov(1,1)), "PV forward-backward delta y pull",-5,5) ;
	plot( dx.z()/std::sqrt(cov(2,2)), "PV forward-backward delta z pull",-5,5) ;
	// draw the chisquares
	plot( forwardvertex->chi2() / forwardvertex->nDoF(), "PV forward chisquare/dof",0,20) ;
	plot( backwardvertex->chi2() / backwardvertex->nDoF(), "PV backward chisquare/dof",0,20) ;

      }
      delete forwardvertex ;
      delete backwardvertex ;
    }
    
    // do something with IP of highest momentum track, as function of phi and eta
    // first sort tracks by type and IP
    if( goodlongtracks.size()>=2 && tracks.size()>=4 ) {
      using namespace boost::lambda;
      std::sort(goodlongtracks.begin(), goodlongtracks.end(), 
		bind(&LHCb::State::pt,bind(&LHCb::Track::firstState,*_1)) <
		bind(&LHCb::State::pt,bind(&LHCb::Track::firstState,*_2)) ) ;
      const LHCb::Track* firsttrack = goodlongtracks.back() ;
      goodlongtracks.pop_back() ;

      // now pick a 2nd track that makes the highest possible invariant mass with this one
      double highestmass2(0) ;
      const LHCb::Track* secondtrack(0) ;
      Gaudi::XYZVector firstp3 = firsttrack->firstState().momentum() ;
      for( TrackVector::const_iterator it = goodlongtracks.begin();
	   it != goodlongtracks.end(); ++it) {
	Gaudi::XYZVector p3 = (*it)->firstState().momentum() ;
	double mass2= p3.r() * firstp3.r() - p3.Dot( firstp3 ) ;
	if(secondtrack==0 || highestmass2 < mass2 ) {
	  highestmass2 = mass2 ;
	  secondtrack = *it ;
	}
      }
      
      // recompute the vertex without these tracks
      TrackVector::iterator newend = tracks.end() ;
      newend = std::remove(tracks.begin(),newend,firsttrack) ;
      newend = std::remove(tracks.begin(),newend,secondtrack) ;
      tracks.erase(newend,tracks.end()) ;
      LHCb::RecVertex* restvertex  = m_vertexer->fit( tracks ) ;
      if( restvertex && firsttrack->nStates()!=0 ) {
	const LHCb::State& firststate = firsttrack->firstState() ;
	double dz  = restvertex->position().z() - firststate.z() ;
	double dx  = firststate.x() + dz * firststate.tx() - restvertex->position().x()  ;
	double dy  = firststate.y() + dz * firststate.ty() - restvertex->position().y() ;
	double nt  = std::sqrt( firststate.tx()*firststate.tx() + firststate.ty()*firststate.ty() ) ;
	// transverse and longitudinal impact parameter
	double iptrans = (dx * firststate.ty() - dy * firststate.tx())/nt ;
	double iplong  = (dx * firststate.tx() + dy * firststate.ty())/nt ;
	double phi = firstp3.phi() ;
	double eta = firstp3.eta() ;
	
	m_trackTransverseIP->fill(iptrans ) ;
	m_trackLongitudinalIP->fill(iplong ) ;
	// apply a cut for the profiles
	if( fabs(iptrans) < m_trackTransverseIP->axis().upperEdge() &&
	    fabs(iplong) < m_trackLongitudinalIP->axis().upperEdge() ) {
	  m_trackTransverseIPVsEta->fill(eta,iptrans) ;
	  m_trackTransverseIPVsPhi->fill(phi,iptrans) ;
	  m_trackLongitudinalIPVsEta->fill(eta,iplong) ;
	  m_trackLongitudinalIPVsPhi->fill(phi,iplong) ;
	}
      
	// compute doca of two tracks
	Gaudi::XYZVector dx3 = firsttrack->firstState().position() - secondtrack->firstState().position() ;
	Gaudi::XYZVector n3  = firsttrack->firstState().slopes().Cross( secondtrack->firstState().slopes() ) ;
	double doca = dx3.Dot(n3) / n3.R() ;
	m_twoprongMass->fill(std::sqrt(highestmass2)) ;
	m_twoprongDoca->fill(doca) ;
	if( fabs(doca) < m_twoprongDoca->axis().upperEdge() ) {
	  m_twoprongDocaVsEta->fill(eta,doca) ;
	  m_twoprongDocaVsPhi->fill(phi,doca) ;
	}
	// the easiest way to compute the pull is with a vertex fit
	LHCb::TwoProngVertex* twoprong = m_vertexer->fit(firsttrack->firstState(),secondtrack->firstState()) ;
	if(twoprong) {
	  m_twoprongDocaPull->fill(std::sqrt(twoprong->chi2()) * (doca>0 ? 1 : -1)) ;
	  delete twoprong ;
	}
      }

      delete restvertex ;
    }
  }
  return StatusCode::SUCCESS ;
}

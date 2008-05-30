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
  ToolHandle<ITrackVertexer> m_vertexer ;
  
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
#include "Event/TrackStateVertex.h"
#include "Event/TwoProngVertex.h"

StatusCode TrackVertexMonitor::execute()
{ 
  
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
    
    // number of tracks per primary vertex
    plot( tracks.size(), "NumTracksPerPV",-0.5,99.5,50) ;
    // number of long tracks per primary vertex
    plot( std::count_if( tracks.begin(),  tracks.end(), isLong ), "NumLongTracksPerPV",-0.5,99.5,50) ;
    // number of backward tracks per primary vertex
    plot( std::count_if( tracks.begin(),  tracks.end(), isBackward ), "NumBackTracksPerPV",-0.5,99.5,50) ;
    // chisquare
    plot( pv->chi2() / pv->nDoF(), "PV chisquare/dof",0,20) ;
    // position
    plot( pv->position().x(), "PV x position",-5*Gaudi::Units::mm, 5*Gaudi::Units::mm) ;
    plot( pv->position().y(), "PV y position",-5*Gaudi::Units::mm, 5*Gaudi::Units::mm) ;
    plot( pv->position().z(), "PV z position",-20*Gaudi::Units::cm, 20*Gaudi::Units::cm) ;
    plot2D( pv->position().z(), pv->position().y(),"PV y versus z",
	    -20*Gaudi::Units::cm, 20*Gaudi::Units::cm,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
    plot2D( pv->position().z(), pv->position().x(),"PV x versus z",
	    -20*Gaudi::Units::cm, 20*Gaudi::Units::cm,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	  
    // refit the primary vertex to see if we get about the same chisquare
    LHCb::RecVertex* revertex = m_vertexer->fit( tracks ) ;
    if(revertex) plot( revertex->chi2() / revertex->nDoF(), "PV refit chisquare/dof",0,20) ;
    delete revertex ;

    // refit the primary vertex with only the long tracks
    if(longtracks.size()>=2) {
      LHCb::RecVertex* longvertex = m_vertexer->fit( longtracks ) ;
      if(longvertex) plot( longvertex->chi2() / longvertex->nDoF(), "PV long chisquare/dof",0,20) ;
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
	plot( dx.x(), "PV left-right delta x",-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot( dx.y(), "PV left-right delta y",-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot( dx.z(), "PV left-right delta z",-5*Gaudi::Units::mm, 5*Gaudi::Units::mm) ;
	plot2D( pv->position().z(), dx.x(),"PV left-right delta x versus z",
		-20*Gaudi::Units::cm, 20*Gaudi::Units::cm,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot2D( pv->position().z(), dx.y(),"PV left-right delta y versus z",
		-20*Gaudi::Units::cm, 20*Gaudi::Units::cm,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;

	// draw the pull of the difference
	Gaudi::SymMatrix3x3 cov = leftvertex->covMatrix() + rightvertex->covMatrix() ;
	plot( dx.x()/std::sqrt(cov(0,0)), "PV left-right delta x pull",-5,5) ;
	plot( dx.y()/std::sqrt(cov(1,1)), "PV left-right delta y pull",-5,5) ;
	plot( dx.z()/std::sqrt(cov(2,2)), "PV left-right delta z pull",-5,5) ;
	// draw the chisquares
	plot( leftvertex->chi2() / leftvertex->nDoF(), "PV left chisquare/dof",0,20) ;
	plot( rightvertex->chi2() / rightvertex->nDoF(), "PV right chisquare/dof",0,20) ;
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
	plot( dx.x(), "PV forward-backward delta x",-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot( dx.y(), "PV forward-backward delta y",-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot( dx.z(), "PV forward-backward delta z",-5*Gaudi::Units::mm, 5*Gaudi::Units::mm) ;
	plot2D( pv->position().z(), dx.x(),"PV forward-backward delta x versus z",
		-20*Gaudi::Units::cm, 20*Gaudi::Units::cm,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot2D( pv->position().z(), dx.y(),"PV forward-backward delta y versus z",
		-20*Gaudi::Units::cm, 20*Gaudi::Units::cm,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	
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
    if( longtracks.size()>=2 && tracks.size()>=4 ) {
      using namespace boost::lambda;
      std::sort(longtracks.begin(), longtracks.end(), 
		bind(&LHCb::State::pt,bind(&LHCb::Track::firstState,*_1)) <
		bind(&LHCb::State::pt,bind(&LHCb::Track::firstState,*_2)) ) ;
      const LHCb::Track* firsttrack = longtracks.back() ;
      longtracks.pop_back() ;

      // now pick a 2nd track that makes the highest possible invariant mass with this one
      double highestmass2(0) ;
      const LHCb::Track* secondtrack(0) ;
      Gaudi::XYZVector firstp3 = firsttrack->firstState().momentum() ;
      for( TrackVector::const_iterator it = longtracks.begin();
	   it != longtracks.end(); ++it) {
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
	double phi = firstp3.phi() ;
	double eta = firstp3.eta() ;
	plot2D( phi, dx , "fast track delta x vs phi",
		-Gaudi::Units::pi,Gaudi::Units::pi,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot2D( eta, dx , "fast track delta x vs eta",
		2,4,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot2D( phi, dy , "fast track delta y vs phi",
		-Gaudi::Units::pi,Gaudi::Units::pi,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
	plot2D( eta, dy , "fast track delta y vs eta",
		2,4,-1*Gaudi::Units::mm, 1*Gaudi::Units::mm) ;
      } 

      // Create a vertex from the two tracks. Compute the mass
      // (assuming 0 rest mass for daughters). Compute decay length.
      //LHCb::TwoProngVertex* tpvtxA = m_vertexer->fit( firsttrack->firstState(), secondtrack->firstState() ) ;
      //delete tpvtxA ;
      
      delete restvertex ;
    }
    // do something with IP of highest momentum to one-but-highest momentum track as function of phi

  }
  return StatusCode::SUCCESS ;
}

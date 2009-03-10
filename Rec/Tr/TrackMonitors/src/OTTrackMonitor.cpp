#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"

class OTTrackMonitor : public GaudiHistoAlg 
{
public:

   /** Standard construtor */
  OTTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~OTTrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  const ITrajPoca* m_pocatool ;
  ToolHandle<ITrackProjector> m_projector ;
  std::string m_trackLocation;
  double m_maxUnbiasedChisqPerDofGoodTracks ;
  int m_granularity ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTTrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTTrackMonitor::OTTrackMonitor( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_projector("TrajOTProjector")
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty( "Projector", m_projector ) ;
  declareProperty( "MaxUnbiasedChisqPerDofGoodTracks", m_maxUnbiasedChisqPerDofGoodTracks = 2 ) ;
  declareProperty( "Granularity", m_granularity = 1 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
OTTrackMonitor::~OTTrackMonitor() 
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode OTTrackMonitor::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  m_pocatool = tool<ITrajPoca>("TrajPoca") ;
  m_projector.retrieve().ignore() ;
  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTTrackMonitor::finalize() 
{
  m_projector.release().ignore() ;
  return GaudiHistoAlg::finalize() ;
}

//=========================================================================
// 
//=========================================================================
StatusCode OTTrackMonitor::execute()
{ 
  setHistoTopDir("Track/") ;
  const int numUniqueModule = 432 ;

  const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackLocation ) ;
  for( LHCb::Tracks::const_iterator itr = tracks->begin() ;
       itr != tracks->end(); ++itr) {
    
    double sumtimeresidual(0) ;
    double sumn(0) ;
    for( LHCb::Track::NodeContainer::const_iterator inode = (*itr)->nodes().begin() ;
	 inode != (*itr)->nodes().end(); ++inode ) 
      if( ( (*inode)->type() == LHCb::Node::HitOnTrack ||
	    (*inode)->type() == LHCb::Node::Outlier )
	  &&
	  (*inode)->measurement().type() == LHCb::Measurement::OT ) { 
	const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(*inode) ;

	const LHCb::OTMeasurement* meas =
	  static_cast<const LHCb::OTMeasurement*>(&(*inode)->measurement()) ;
	LHCb::OTChannelID channel = meas->channel() ;
	int uniquelayer = (channel.station()-1) * 4 + channel.layer() ;
	int uniquequarter = uniquelayer*4 + channel.quarter() ;
	int uniquemodule  = uniquequarter*9 + channel.module() - 1 ;

	// directory name
	char tmpprefix[256] = "" ;
	switch( m_granularity ) {
	case 1: sprintf(tmpprefix,"station%d/",channel.station()) ; break ;
	case 2: sprintf(tmpprefix,"layer%d/",uniquelayer) ; break ;
	default: {} ;
	}
	std::string nameprefix(tmpprefix) ;
	
	// Now, this gives access to the unbaised residual:
	double residual = fitnode->unbiasedResidual() ;
	// But we cannot obtain the unbiased 'radius', because the hit
	// was used for the time-of-flight (at last on cosmics). This
	// is what has been causing all the trouble. Therefore, first
	// get the full unbiased state: 
	// double radius = meas->driftRadiusWithErrorFromY(ypos).val ;
	bool isoutlier((*inode)->type() == LHCb::Node::Outlier ) ;
	LHCb::State ubstate = isoutlier ? fitnode->state() : fitnode->unbiasedState() ;

	// project it:
	LHCb::FitNode unbiasednode(*fitnode) ;
	unbiasednode.setState( ubstate ) ;
	unbiasednode.setRefVector( ubstate.stateVector() ) ;
	m_projector->projectReference(unbiasednode) ;

	// and only now get the time-of-flight
	double drifttime = meas->driftTimeFromY(ubstate.y()) ;
	double radius   = meas->driftRadiusWithErrorFromY(ubstate.y()).val ;
	double trkdist = unbiasednode.pocaVector().Dot( ubstate.position() - fitnode->measurement().trajectory().beginPoint() ) ;
	double trktime = meas->module().driftTimeWithError(fabs(trkdist)).val ;
	double drifttimeresidual = drifttime - trktime ;
	double resscalefactor = std::sqrt(fitnode->errMeasure()/fitnode->errUnbiasedResidual()) ;
	double respull = fitnode->residual()/fitnode->errResidual() ;
	
	// plots by module
	profile1D( uniquemodule, drifttimeresidual, "timeresvsmodule", "unbiased drifttime residual versus module", -0.5,numUniqueModule-0.5,numUniqueModule) ;
	profile1D( uniquemodule, residual, "resvsmodule", "unbiased residual versus module", -0.5,numUniqueModule-0.5,numUniqueModule) ;
	plot(uniquemodule,"moduleocch1","number of HOTs per module",-0.5,numUniqueModule-0.5,numUniqueModule) ;
	
	// residuals distributions, by layer
	plot(drifttime,nameprefix + "drifttime","drifttime",-25,75) ;
	plot(radius,nameprefix + "driftradius","driftradius",0,5) ;
	plot(trkdist,nameprefix + "trkdist","unbiased distance",-5,5) ;
	plot(drifttimeresidual*resscalefactor,nameprefix + "drifttimeresidual","drifttime residual (rms unbiased)",-25,25) ;
	plot(residual*resscalefactor,nameprefix + "residual","drifttime residual (rms unbiased)",-2,2) ;
	plot(respull,nameprefix + "residualpull","residual pull",-5,5) ;
	
	// same but for 'good' tracks
	double unbiasedchi2 = ((*itr)->chi2() - respull * respull)/((*itr)->nDoF()-1) ;
	bool goodtrack = unbiasedchi2 < m_maxUnbiasedChisqPerDofGoodTracks ;
	if( goodtrack ) {
	  plot(drifttime,nameprefix + "drifttimegood","drifttime (good tracks)",-25,75) ;
	  plot(trkdist,nameprefix + "trkdistgood","unbiased distance (good tracks)",-5,5) ;
	  plot(drifttimeresidual*resscalefactor,nameprefix + "drifttimeresidualgood","drifttime residual (rms unbiased, good tracks)",-25,25) ;
	  plot(residual*resscalefactor,nameprefix + "residualgood","drifttime residual (rms unbiased, good tracks)",-2,2) ;
	  plot(respull,nameprefix + "residualpullgood","residual pull (good tracks)",-5,5) ;
	}
	
	// 2D occupancy plot, quite a beast. should be forbidden in monitoring!
	plot2D((*inode)->state().x(),(*inode)->state().y(),nameprefix + "xyh2","position of track at hit",-3000,3000,-3000,3000,20,20) ;
	plot2D(trkdist,drifttime,nameprefix + "rt","drifttime versus unbiased distance",-5,5,-25,75) ;
	
	sumtimeresidual += drifttimeresidual ;
	++sumn ;
      }
    if(sumn>0)  plot( sumtimeresidual/sumn,"avtimeres","average time residual on track",-50,50) ;
  }
  return StatusCode::SUCCESS ;
}

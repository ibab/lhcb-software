#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "TrackInterfaces/IPitchResTool.h"

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
  void setNormalization(const std::string& hisname)  ;
  // this should be moved to OTChannelID
  inline int uniqueLayer( const LHCb::OTChannelID& channelID) {
    return  (channelID.station()-1) * 4 + channelID.layer() ;
  }
  inline int uniqueQuarter( const LHCb::OTChannelID& channelID) {
    return uniqueLayer( channelID) * 4 + channelID.quarter() ;
  }
  inline int uniqueModule( const LHCb::OTChannelID& channelID) {
    return uniqueQuarter(channelID)*9 + channelID.module() - 1 ;
  }
private:
  const ITrajPoca* m_pocatool ;
  ToolHandle<ITrackProjector> m_projector ;
  ToolHandle<IOTRawBankDecoder> m_decoder ;
  const IPitchResTool *m_pitchtool;
  const DeOTDetector* m_otdet ;
  std::string m_trackLocation;
  double m_maxUnbiasedChisqPerDofGoodTracks ;
  int m_granularity ;
  size_t m_numEvents ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTTrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTTrackMonitor::OTTrackMonitor( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_projector("TrajOTProjector"),
    m_decoder("OTRawBankDecoder"),
    m_numEvents(0)
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
  m_decoder.retrieve().ignore() ;
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_pitchtool = tool<IPitchResTool>("PitchResTool");

  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTTrackMonitor::finalize() 
{
  // rescale the occupancy histogram
  m_projector.release().ignore() ;
  m_decoder.release().ignore() ;
  return GaudiHistoAlg::finalize() ;
}

//=========================================================================
// 
//=========================================================================
StatusCode OTTrackMonitor::execute()
{ 
  setHistoTopDir("OT/") ;
  ++m_numEvents ;
  const int numUniqueModule = 432 ;
  const double cellRadius = 2.5 ;
  const int    numDistBins  = 25 ;

  const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackLocation ) ;
  for( LHCb::Tracks::const_iterator itr = tracks->begin() ;
       itr != tracks->end(); ++itr) 
    if( (*itr)->fitStatus() == LHCb::Track::Fitted && (*itr)->nDoF()>1 ) {
      double sumtimeresidual(0) ;
      double sumn(0) ;
      // calculate the pitch residuals by M. Schiller
      std::vector<std::pair<int,double> > pitchRes = m_pitchtool->calcPitchResiduals( *itr );
      for (int i = 0; i < pitchRes.size(); ++i) {
        std::ostringstream os;
        os << "Layer " << pitchRes[i].first << " pitch residuals";
        plot(pitchRes[i].second, os.str(), -3., 3., 120);
      }

      LHCb::Track::ConstNodeRange nodes = (*itr)->nodes() ;
      for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
	   inode != nodes.end(); ++inode ) 
	if( ( (*inode)->type() == LHCb::Node::HitOnTrack ||
	      (*inode)->type() == LHCb::Node::Outlier )
	    && (*inode)->measurement().type() == LHCb::Measurement::OT ) { 
	  const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(*inode) ;
	  
	  const LHCb::OTMeasurement* meas =
	    static_cast<const LHCb::OTMeasurement*>(&(*inode)->measurement()) ;
	  LHCb::OTChannelID channel = meas->channel() ;
	  int uniquelayer   = uniqueLayer(channel) ;
	  //int uniquequarter = uniqueQuarter(channel) ;
	  int uniquemodule  = uniqueModule(channel) ;
	  
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
	  double trktime = meas->module().driftTimeWithError(std::abs(trkdist)).val ;
	  double drifttimeresidual = drifttime - trktime ;
	  double resscalefactor = std::sqrt(fitnode->errMeasure()/fitnode->errUnbiasedResidual()) ;
	  double respull = fitnode->residual()/fitnode->errResidual() ;
	 
	  // make sure all residuals have the same sign in global coordinates
	  if( meas->trajectory().direction(0).y() < 0 ) {
	    residual *= -1 ;
	    respull  *= -1 ;
	  }
	  
	  // plots by module
	  profile1D( uniquemodule, drifttimeresidual, "timeresvsmodule", "unbiased drifttime residual versus module", 
		     -0.5,numUniqueModule-0.5,numUniqueModule) ;
	  profile1D( uniquemodule, residual, "resvsmodule", "unbiased residual versus module", 
		     -0.5,numUniqueModule-0.5,numUniqueModule) ;
	  profile1D( uniquemodule, respull, "respullvsmodule", "residual pull versus module", 
		     -0.5,numUniqueModule-0.5,numUniqueModule) ;
	  plot(uniquemodule,"moduleHotOccupancy","number of HOTs per module",
	       -0.5,numUniqueModule-0.5,numUniqueModule) ;
	  if( isoutlier) 
	    plot(uniquemodule,"moduleOutlierOccupancy","number of outliers per module",
		 -0.5,numUniqueModule-0.5,numUniqueModule) ;



	  
	  // residuals distributions, by layer
	  plot(drifttime,nameprefix + "drifttime","drifttime",-25,75) ;
	  plot(radius,nameprefix + "driftradius","driftradius",0,5) ;
	  plot(trkdist,nameprefix + "trkdist","unbiased distance",-5,5) ;
	  plot(drifttimeresidual*resscalefactor,nameprefix + "drifttimeresidual","drifttime residual (rms unbiased)",-20,20) ;
	  plot(residual*resscalefactor,nameprefix + "residual","residual (rms unbiased)",-2,2) ;
	  plot(respull,nameprefix + "residualpull","residual pull",-5,5) ;
	  profile1D(std::abs(trkdist),drifttimeresidual, "timeresvsdistance", "unbiased drifttime residual versus distance", 
		    0,cellRadius,numDistBins) ;
	  profile1D(std::abs(trkdist),respull, "respullvsdistance", "residual pull versus distance", 
		    0,cellRadius,numDistBins) ;
	  
	  // same but for 'good' tracks
	  double unbiasedchi2 = ((*itr)->chi2() - respull * respull)/((*itr)->nDoF()-1) ;
	  bool goodtrack = unbiasedchi2 < m_maxUnbiasedChisqPerDofGoodTracks ;
	  if( goodtrack ) {
	    plot(drifttime,nameprefix + "drifttimegood","drifttime (good tracks)",-25,75) ;
	    plot(trkdist,nameprefix + "trkdistgood","unbiased distance (good tracks)",-5,5) ;
	    // remove hits outside cell radius
	    if ( std::abs(trkdist) < cellRadius ) {
	      plot(drifttimeresidual*resscalefactor,nameprefix + "drifttimeresidualgood","drifttime residual (rms unbiased, good tracks)",-20,20) ;
	      plot(residual*resscalefactor,nameprefix + "residualgood","residual (rms unbiased, good tracks)",-2,2) ;
	      plot(respull,nameprefix + "residualpullgood","residual pull (good tracks)",-5,5) ;
	      profile1D(std::abs(trkdist),drifttimeresidual, "timeresvsdistancegood", "unbiased drifttime residual versus distance", 
			0,cellRadius) ;
	      profile1D(std::abs(trkdist),respull, "respullvsdistancegood", "residual pull versus distance", 
			0,cellRadius) ;
	      if(!isoutlier) {
		sumtimeresidual += drifttimeresidual ;
		++sumn ;
	      }
	    }
	  }
	  
	  // 2D occupancy plot, quite a beast. should be forbidden in monitoring!
	  plot2D((*inode)->state().x(),(*inode)->state().y(),nameprefix + "xyh2","position of track at hit",-3000,3000,-3000,3000,20,20) ;
	  plot2D(trkdist,drifttime,nameprefix + "rt","drifttime versus unbiased distance",-cellRadius,cellRadius,-25,75) ;
	  
	}
      if(sumn>5)  {
	plot( sumtimeresidual/sumn,"avtimeres","average time residual on track",-10,10) ;
	plot2D( (*itr)->p()/1000,sumtimeresidual/sumn,"avtimeresvsmom","average time residual versus momentum",0,20,-5,5,40,50) ;
      }
    }
  
  // for normalization issues, also want raw HIT occupancy by
  // module.
  for( DeOTDetector::Modules::const_iterator imodule = m_otdet->modules().begin() ;
       imodule != m_otdet->modules().end(); ++imodule) {
    LHCb::OTChannelID modid = (*imodule)->elementID() ;
    size_t numhits = m_decoder->decodeModule( modid ).size() ;
    plot(uniqueModule(modid),"moduleHitOccupancy","number of hits per module",
	 -0.5,numUniqueModule-0.5,numUniqueModule,numhits) ;
  }

  // set this on every event
  setNormalization( "moduleHotOccupancy" ) ;
  setNormalization( "moduleHitOccupancy" ) ;
  setNormalization( "moduleOutlierOccupancy" ) ;
  
  return StatusCode::SUCCESS ;
}


void OTTrackMonitor::setNormalization(const std::string& hisname) 
{
  TH1* h1 = Gaudi::Utils::Aida2ROOT::aida2root(histo(hisname) ) ;
  if( h1 ) h1->SetEntries( m_numEvents ) ;
}





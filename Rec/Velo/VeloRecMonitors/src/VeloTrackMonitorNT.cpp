// Include files 

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

// from Event
#include "Event/ODIN.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/FitNode.h"
#include "Event/VeloCluster.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "VeloDet/DeVelo.h"
#include "Kernel/VeloChannelID.h"
#include "Event/RecVertex.h"
#include "TrackKernel/TrackStateVertex.h"
#include "TrackInterfaces/ITrackVertexer.h"
//#include <boost/lambda/bind.hpp>
//#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

//from TrackInterfaces
#include "TrackInterfaces/IVeloExpectation.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackSelector.h"

// Det

// gsl
//#include "gsl/gsl_math.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"

                                                                                
// Boost
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#include <functional>
using std::bind;
using namespace std::placeholders;
#else
#include <boost/lambda/bind.hpp>
using namespace boost::lambda;
#endif
#include <boost/lambda/lambda.hpp>

#include "VeloTrackMonitorNT.h"

using namespace LHCb;
using namespace Gaudi;
using namespace std;
 
typedef IVeloClusterPosition::Direction Direction;

DECLARE_NAMESPACE_ALGORITHM_FACTORY(Velo, VeloTrackMonitorNT)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloTrackMonitorNT::VeloTrackMonitorNT(const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator ),
  m_allString("ALL"),
  m_clusterLoc ( VeloClusterLocation::Default ),
  m_asctLocation ( m_clusterLoc+"2MCHits" ),
  m_rawClusters(0),
  m_veloDet(0),
  m_expectTool(0),
  m_clusterTool(0),
  m_asctTable ( 0 ),
  m_vertexer(NULL),
  m_tracksFitter(0)
{
  declareProperty( "TracksInContainer", m_tracksInContainer = LHCb::TrackLocation::Default );
  declareProperty( "PVContainer", m_pvContainerName = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "VeloClusterLocation",m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "Fitter", m_fitterName = "TrackMasterFitter" );
  //one entry per event
  declareProperty("EventNtuple", m_evntuple = true);
  //one entry per track
  declareProperty("TrackNtuple", m_trntuple = true);
  //only clusters on track
  declareProperty("ClusterOnTrackNtuple", m_clntuple = true);
  //all clusters
  declareProperty("ClusterNtuple", m_allclntuple = false);
  declareProperty("EtaStudyNtuple", m_etastudy = false);
  declareProperty("RunWithMC", m_runWithMC=false);
  
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloTrackMonitorNT::~VeloTrackMonitorNT() {} 

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }
  //sc = m_vertexer.retrieve() ;
  m_vertexer = tool<ITrackVertexer>( "TrackVertexer" );

  m_veloDet = getDet<DeVelo>(  DeVeloLocation::Default ) ;

  m_expectTool = tool<IVeloExpectation>( "VeloExpectation");

  m_clusterTool=tool<IVeloClusterPosition>("VeloClusterPosition");

  m_tracksFitter = tool<ITrackFitter>( m_fitterName, "Fitter", this );

  return StatusCode::SUCCESS;
}

 //=============================================================================
StatusCode Velo::VeloTrackMonitorNT::finalize() 
{
  return GaudiTupleAlg::finalize();
}


namespace {

  std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
  {
    std::vector<const LHCb::Track*> rc ;
    rc.reserve( tracks.size() ) ;
    BOOST_FOREACH(const LHCb::Track* t, tracks )
      if( t ) rc.push_back( t ) ;
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
  };

  struct TrackVeloSidePredicate
  {
    int m_sign ;
    TrackVeloSidePredicate(int asign) : m_sign(asign) {}
    bool operator()(const LHCb::Track* track) const { 
      return track->firstState().tx()*m_sign*(track->checkFlag(LHCb::Track::Backward)? -1 : 1 )>0 ; }
  };
  
  struct TrackChisqPredicate
  {
    double m_maxchisq ;
    TrackChisqPredicate(double maxChisqPerDof) : m_maxchisq(maxChisqPerDof) {}
    bool operator()(const LHCb::Track* track) const { return track->chi2PerDoF() < m_maxchisq  ; }
  };
}

//=============================================================================
// Execute
//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::execute()
{
  
 
  LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  if ( NULL != odin ){
     if( msgLevel(MSG::DEBUG) ) 
      debug()<< "Run "     << odin->runNumber()
             << ", Event " << odin->eventNumber() << endmsg;
    m_runodin=odin->runNumber();
    m_eventodin=odin->eventNumber();
    m_bunchid= odin->bunchId();
    m_evTimeGps= odin->gpsTime();
   
  } else {
    Warning("No ODIN Header").ignore();
    m_runodin=0;
    m_eventodin=0;
    m_bunchid= 0;
    m_evTimeGps= 0;
  }

  //in case of multiple PV events,choose the one at the smallest radius
  int n_pv = -1;
  double pvx=0.;
  double pvy=0.;
  double pvz=0.;
  double pvchi2=0.;
  double pvndof=0.;
  int pvntr=0;
  // get the input data
  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>(m_tracksInContainer);
  if ( NULL == tracks ) {
    debug() << "No VeloTracks container found for this event !" << endmsg;
    return StatusCode::SUCCESS;
  }

 //count number of backward tracks
  int n_back=0;
  for (LHCb::Tracks::const_iterator iterT = tracks->begin(); iterT != tracks->end(); ++iterT){
    if( (*iterT)->checkFlag( LHCb::Track::Backward ))
      n_back+=1;
  } // iterT


  //std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // histograms per track

  m_rawClusters = getIfExists<LHCb::VeloClusters>(m_clusterCont);
  if( NULL == m_rawClusters ){
    return Error( " ==> There is no VeloClusters in TES " );
  }
  else{
    if (m_allclntuple){
      FillVeloAllClNtuple(tracks); 
    }
  }

  const LHCb::RecVertices* pvcontainer = getIfExists<LHCb::RecVertices>( m_pvContainerName ) ;

  if ( NULL == pvcontainer ) {
     debug() << "No Vertex container found for this event !" << endmsg;
    //return StatusCode::SUCCESS;
  }
  else{
    n_pv = pvcontainer->size();
    int it_npv=1;

    for( LHCb::RecVertices::const_iterator ipv = pvcontainer->begin() ;
         ipv != pvcontainer->end(); ++ipv ) {
      const LHCb::RecVertex*pv = *ipv ;
      if (m_evntuple && !m_etastudy)
        FillVeloEvNtuple(tracks,n_pv ,pv);
      if ((it_npv==1) || (sqrt(pvx*pvx+pvy*pvy)> pv->position().rho() && it_npv>1)){
        pvx= pv->position().x();
        pvy= pv->position().y();
        pvz= pv->position().z();
        pvchi2=pv->chi2();
        pvndof=pv->nDoF();
        pvntr=pv->tracks().size();
      }
      it_npv +=1;
    }
  }


 
  
  if(m_runWithMC){
    debug()<< "Run with MC truth " << endmsg;
    // create the ass table if run over MC data
    AsctTool associator(evtSvc(), m_asctLocation);
    debug()<< "associator " << endmsg;
    m_asctTable=associator.direct();
    debug()<< "associator,direct" << endmsg;
    if(!m_asctTable){
      return ( Error("Empty table with associations", StatusCode::FAILURE) );
    }
    debug()<< "asct table exist " << endmsg;
  }

  for (LHCb::Tracks::const_iterator iterT = tracks->begin(); iterT != tracks->end(); ++iterT){
    StatusCode cluNTStatus;
    if(m_clntuple) cluNTStatus=FillVeloClNtuple(**iterT,n_pv, n_back, pvx,pvy,pvz,pvchi2,pvndof,pvntr);
    if(cluNTStatus.isSuccess()){
      cluNTStatus.ignore();
    }else{
      return ( cluNTStatus );
    }
    if (m_trntuple)
      FillVeloTrNtuple(**iterT,n_pv, n_back, pvx,pvy,pvz,pvchi2,pvndof,pvntr);
  } // iterT

  
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::FillVeloClNtuple(const LHCb::Track& track,
                                                      int n_pv,int n_back, 
                                                      double pvx, double pvy, double pvz,
                                                      double pvchi2, double pvndof, int pvntr)   


{
  Tuple tuple=nTuple("VeloClNtuple", "Clusters on track",CLID_ColumnWiseTuple );
  long long int tzero= 1270064494071424ll; //there is an ll at the end, so that C++ knows this has to be a long long
  float eventTimeMicroSecGps = (float) (((m_evTimeGps  - tzero)/1000000.));
  debug()<< "Cluster information ntuple " << endmsg;


  if(m_runWithMC){
    debug()<< "Run with MC truth " << endmsg;
    // create the ass table if run over MC data
    AsctTool associator(evtSvc(), m_asctLocation);
    m_asctTable=associator.direct();
    eventTimeMicroSecGps=0;

    if(!m_asctTable){
      return ( Error("Empty table with associations", StatusCode::FAILURE) );
    }
    debug()<< "asct table exist " << endmsg;
  }
  
  if( msgLevel(MSG::DEBUG) ) 
    debug()<< "Cluster information ntuple " << endmsg;

  const LHCb::State& firststate = track.firstState() ;
  const LHCb::State& laststate = track.closestState(760) ;
 

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nVeloHits = std::count_if(ids.begin(), ids.end(),
                                               bind(&LHCbID::isVelo,_1));

  double z0=firststate.position().z()+20;
  double z1=laststate.position().z()-20;
  if( msgLevel(MSG::DEBUG) ) 
    debug() << " z0="<<z0<<" z1="<<z1 <<endmsg;
  
  if( msgLevel(MSG::DEBUG) )  
    debug() << "Track node size="<<  track.nodes().size()<<endmsg;
  if( msgLevel(MSG::DEBUG) )  
    debug() << "Track lhcbids size="<<  track.lhcbIDs().size()<<endmsg;

  if( nVeloHits>0 ) {
    std::string names[] = { "VeloR","VeloPhi" } ;
    double chi2=track.info(LHCb::Track::FitVeloChi2,0);//chi2();
    double theta = track.firstState().slopes().theta();
    double slx = track.firstState().tx();
    double sly = track.firstState().ty();
    //double x0 = track.firstState().x();
    //double y0 = track.firstState().y();
    double ndof=track.info(LHCb::Track::FitVeloNDoF,0) ;//nDoF();
    float adcpertrack=0.;
    double p=track.p()/Gaudi::Units::GeV ;
    double type=track.type();
    unsigned int m_sideLeft=0, m_sideRight=0;
    Gaudi::XYZVector global3dDir=Gaudi::XYZVector(slx, sly, 1.);
    LHCb::Track::ConstNodeRange nodes = track.nodes();
    for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
        inode != nodes.end(); ++inode) {
      if(( (*inode)->type() == LHCb::Node::HitOnTrack )
         && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
               ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
        LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
        LHCb::VeloCluster *cluster;
        cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID()));
        //for ( it = trackIDs.begin(); it != trackIDs.end(); it++ ) {
        //LHCb::VeloCluster *cluster;
        //cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject( (it)->channelID() ); 
        adcpertrack+=(float)cluster->totalCharge();
        LHCb::VeloChannelID chan  = nodeID.veloID();
        
        const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
        if (sensor->isLeft()) 
          m_sideLeft+=1;
        else
          m_sideRight+=1;
      } 
    }
    
  
    if(nVeloHits>0) adcpertrack = adcpertrack/nVeloHits;

    for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
         inode != nodes.end(); ++inode) {
      if( msgLevel(MSG::DEBUG) )  
        debug() << " node type="<< (*inode)->type() <<endmsg;
      if( msgLevel(MSG::DEBUG) )  
        debug() << " errResidual2=" << (*inode)->errResidual2() <<
          " tolerance="<<TrackParameters::lowTolerance<<endmsg;
    
      if( (*inode)->type() == LHCb::Node::HitOnTrack 
          // discard extremely small fraction of hits with zero error
          // on residual.
          && (*inode)->errResidual2() > TrackParameters::lowTolerance 
          //check that it is a Velo node
          && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
                ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
        size_t mtype = (*inode)->measurement().type() - 1  ;
        // factor for unbiasing the rms (not the mean!)
        double f = std::sqrt( (*inode)->errMeasure2()/(*inode)->errResidual2()) ;
        double residual=(*inode)->residual();
        double residual_x = 0;
        double residual_y = 0;
        double resolution = f*(*inode)->residual();
        double fracPosTool=0., fracPos=0.;
        // now get the pitch ...
        double pitch(1) ;
        double cl=0;
        double pntx=0.,pnty=0.,pntz=0.,pntphi=0.,pntr=0.,pntphi_loc =0.,pntr_loc =0.;;
        double pntx_mc=0.,pnty_mc=0.,pntz_mc=0.,pntphi_mc=0.,pntr_mc=0.;
        double adcsum=0;
        double projAngle=0., projAngle_mc=0.;
        Direction localDirection, localDirection_mc;
        double anglerho=0;
        double angleofstriplocalframe = 0;
        double angleoftracklocalframe = 0;
        double pitch_mc=0.;
        //evaluation of unbiased residual (refitting the track)
        double unbiasres;
        double unbiasres_chi2 ;
        LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
        LHCb::VeloCluster *clus=
          (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID()));
        const DeVeloSensor* sensor = m_veloDet->sensor(clus->channelID().sensor() ); 
        //define the cluster tool with track information to set properly th`e errors
        SiPositionInfo<LHCb::VeloChannelID> toolInfo;
        toolInfo=m_clusterTool->position(clus, track.firstState().position(),
                                         std::pair<double,double>( track.firstState().tx(), track.firstState().ty()));
        fracPosTool=toolInfo.fractionalPosition;
        fracPos=clus->interStripFraction();

        const std::vector< LHCb::LHCbID >& trackIDs = track.lhcbIDs();
        LHCb::Track* tr_clone = track.cloneWithKey();
        std::vector< LHCb::LHCbID >::const_iterator it;
        for ( it = trackIDs.begin(); it != trackIDs.end(); it++ ) {
          if ( it->isVeloR() ) 
            if (( it->veloID().sensor() == clus->channelID().sensor() ) ||
                ( it->veloID().sensor() == (clus->channelID().sensor()-64))) 
              tr_clone->removeFromLhcbIDs( *it );
          if ( it->isVeloPhi() ) 
            if (( it->veloID().sensor() == clus->channelID().sensor() ) ||
                ( it->veloID().sensor() == (clus->channelID().sensor()+64))) 
              tr_clone->removeFromLhcbIDs( *it );
        }
        // fit the remaining track
        tr_clone->setFitResult(0);

        StatusCode sc = m_tracksFitter -> fit( *tr_clone );
        if ( !sc.isSuccess() ) {
          debug() << "Track fit failed. Continuing with next track." << endmsg;
          continue;
        }
      
        double slx_mc=0., sly_mc=0.;
        
        
        Gaudi::XYZPoint clusterGlobal = (*inode)->state().position();
        
        Gaudi::XYZPoint trackInterceptGlobal= extrapolateToZ(tr_clone,clusterGlobal.z());
        Gaudi::XYZPoint trackInterceptLocal = 
          (*inode)->measurement().detectorElement()->geometry()->toLocal( trackInterceptGlobal);
        Gaudi::XYZPoint clusterLocal = 
          (*inode)->measurement().detectorElement()->geometry()->toLocal( clusterGlobal);
        residual_x = clusterLocal.x()-trackInterceptLocal.x();
        //if (clusterLocal.y()>0)
        residual_y = clusterLocal.y()-trackInterceptLocal.y();
        //std::cout << "resx: "<<residual_x<<"     resy: "<< residual_y << std::endl;
        
          //else
          //residual_y = trackInterceptLocal.y()-clusterLocal.y();
        sensor->residual( trackInterceptGlobal,tr_clone->slopes(), clus->channelID(), fracPosTool, unbiasres, unbiasres_chi2 );
        double err_mc=0.;
        double r_clu=0., r_hit=0., reso_mc=0.;
        double trfracpos=0.;
        
        
        
        if( (*inode)->measurement().type() == LHCb::Measurement::VeloPhi ){

          // this part is using reco info only - Phi clusters
          const DeVeloPhiType* phiDet = m_veloDet->phiSensor( clus->channelID().sensor());
          Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
          pntx=globalPoint.x();
          pnty=globalPoint.y();
          pntz=globalPoint.z();
          pntphi=globalPoint.Phi();
          pntr=globalPoint.Rho();
          
                    
          Gaudi::XYZPoint localPoint =(*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint ) ;
          pitch =  phiDet->phiPitch( localPoint.Rho() ) ;
          cl=phiDet->phiOfStrip(unsigned(clus->channelID().strip()),fracPosTool,localPoint.Rho());
          // work out the fraction wrt the central strip of the cluster
          // to evaluate the fraction always wrt the same strip
          double TrPhi=cl*localPoint.Rho()-unbiasres;
          if ((pnty*localPoint.y())<0) 
            TrPhi=cl*localPoint.Rho()+unbiasres;
          double PhiPitch =  phiDet->phiPitch( localPoint.Rho() ) ;
          trfracpos=(TrPhi-localPoint.Rho()*phiDet->phiOfStrip(unsigned(clus->channelID().strip()),0.,localPoint.Rho()))/PhiPitch;
          adcsum=clus->totalCharge();
          localDirection=localTrackDirection(global3dDir, phiDet);
          unsigned int centreStrip=clus->channelID().strip();
          projAngle=projAnglePhi(localDirection, phiDet, centreStrip);
          pntr_loc = localPoint.Rho();
          pntphi_loc  = localPoint.Phi();  

          //define the cluster tool with track information to set properly the errors
          toolInfo=m_clusterTool->position(clus, track.firstState().position(),
                                           std::pair<double,double>( track.firstState().tx(), track.firstState().ty()));

          
          //Evaluation of rho angle between tangent of the strip and the track in the local plane of the sensor
          //angle of the strip in the local frame
          angleofstriplocalframe = phiDet ->angleOfStrip(centreStrip, fracPosTool)+ TMath::Pi()/2.;
          angleoftracklocalframe = atan(localDirection.second/localDirection.first);
          anglerho=angleoftracklocalframe-angleofstriplocalframe;

          // this part is used when MC info is available - reso study, Phi clusters
          if(m_runWithMC){
            const MCHit* aHit=getAssocMCHit(clus);
            if(0==aHit) continue;
            Gaudi::XYZPoint midPoint=aHit->midPoint();
            Gaudi::XYZPoint localMidPoint=phiDet->globalToLocal(midPoint);
            pntx_mc=localMidPoint.x();
            pnty_mc=localMidPoint.y();
            pntz_mc=localMidPoint.z();
            pntphi_mc=localMidPoint.Phi();
            pntr_mc=localMidPoint.Rho();            
            double offset=0.;
            StatusCode channelStat;
            VeloChannelID entryChann;
            channelStat=phiDet->pointToChannel(midPoint, entryChann, offset, pitch_mc);
            LHCb::VeloChannelID cluCentChann=clus->channelID();
            IVeloClusterPosition::Direction globalDir_mc;
            slx_mc=aHit->dxdz();
            sly_mc=aHit->dydz();
            globalDir_mc=std::make_pair(slx_mc, sly_mc);
            IVeloClusterPosition::toolInfo fullInfo_mc;
            fullInfo_mc=m_clusterTool->position(clus, midPoint, globalDir_mc);
            err_mc=fullInfo_mc.fractionalError;
            Gaudi::XYZVector global3dDir_mc=Gaudi::XYZVector(slx_mc, sly_mc, 1.);
            localDirection_mc=localTrackDirection(global3dDir_mc, phiDet);
            projAngle_mc=projAnglePhi(localDirection_mc, phiDet, centreStrip);
            auto traj=phiDet->trajectory(cluCentChann, fracPos);
            Gaudi::XYZPoint localBeg=phiDet->globalToLocal(traj->beginPoint());
            Gaudi::XYZPoint localEnd=phiDet->globalToLocal(traj->endPoint());
            double traj_x1=localBeg.x();
            double traj_y1=localBeg.y();
            double traj_x2=localEnd.x();
            double traj_y2=localEnd.y();
            double del_x=traj_x2-traj_x1;
            double del_y=traj_y2-traj_y1;
            double C=del_x*traj_y1-del_y*traj_x1;
            reso_mc=del_y*localMidPoint.x()-del_x*localMidPoint.y()+C;
            reso_mc/=sqrt(del_x*del_x+del_y*del_y);

          }
          
        }else{

          // this part is using reco info only - R clusters
          const DeVeloRType* rDet = m_veloDet->rSensor( clus->channelID().sensor() );
          Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
          pntx=globalPoint.x();
          pnty=globalPoint.y();
          pntz=globalPoint.z();
          pntphi=globalPoint.Phi();
          pntr=globalPoint.Rho();
          /*//define the cluster tool with track information to set properly the errors
          toolInfo=m_clusterTool->position(clus, track.firstState().position(),
                                           std::pair<double,double>( track.firstState().tx(), track.firstState().ty()));
          fracPosTool=toolInfo.fractionalPosition;
          fracPos=clus->interStripFraction();*/
          pitch = rDet->rPitch( unsigned(clus->channelID().strip()),fracPosTool ) ;
          cl=rDet->rOfStrip(unsigned(clus->channelID().strip()),fracPosTool);
          adcsum=clus->totalCharge();
          // work out the fraction wrt the central strip of the cluster
          // to evaluate the fraction always wrt the same strip
          double TrRadius =cl-unbiasres;
          unsigned int closestStripTr=clus->channelID().strip();
          trfracpos = (TrRadius-rDet->rOfStrip(closestStripTr))/pitch;
          Gaudi::XYZPoint localPoint = 
            (*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint );
          localDirection=localTrackDirection(global3dDir, rDet);
          projAngle=projAngleR(localDirection, localPoint);
          pntr_loc = localPoint.Rho();
          pntphi_loc  = localPoint.Phi();  

          //Evaluation of rho angle between tangent of the strip and the track in the local plane of the sensor
          //angle of the strip in the local frame
          angleofstriplocalframe = pntphi_loc ;
          angleoftracklocalframe = atan(localDirection.second/localDirection.first);
          anglerho=angleoftracklocalframe-angleofstriplocalframe;

          // this part is used when MC info is available - reso study, R clusters
          if(m_runWithMC){
            const MCHit* aHit=getAssocMCHit(clus);
            if(0==aHit) continue;
            Gaudi::XYZPoint midPoint=aHit->midPoint();
            Gaudi::XYZPoint localMidPoint=rDet->globalToLocal(midPoint);
            pntx_mc=localMidPoint.x();
            pnty_mc=localMidPoint.y();
            pntz_mc=localMidPoint.z();
            pntphi_mc=localMidPoint.Phi();
            pntr_mc=localMidPoint.Rho();
            double offset=0.;
            StatusCode channelStat;
            VeloChannelID entryChann;
            channelStat=rDet->pointToChannel(midPoint, entryChann, offset, pitch_mc);
            LHCb::VeloChannelID cluCentChann=clus->channelID();
            IVeloClusterPosition::Direction globalDir_mc;
            slx_mc=aHit->dxdz();
            sly_mc=aHit->dydz();
            globalDir_mc=std::make_pair(slx_mc, sly_mc);
            IVeloClusterPosition::toolInfo fullInfo_mc;
            fullInfo_mc=m_clusterTool->position(clus, midPoint, globalDir_mc);
            err_mc=fullInfo_mc.fractionalError;
            Gaudi::XYZVector global3dDir_mc=Gaudi::XYZVector(slx_mc, sly_mc, 1.);
            localDirection_mc=localTrackDirection(global3dDir_mc, rDet);
            projAngle_mc=projAngleR(localDirection_mc, localMidPoint);
            r_clu=rDet->rOfStrip(cluCentChann.strip(), fracPos);
            r_hit=rDet->rOfStrip(entryChann.strip(), offset);
            reso_mc=r_hit-r_clu;
            
          }
          
        }
        
        int clSize =0, SensNumber=-1;
        //int sensLeft=-1, SensRtype=-1;
        double zSensPos=0.;

        if (names[mtype]=="VeloR"){
          clSize=clus->size();
          const DeVeloSensor* sensor = m_veloDet->sensor( clus->channelID().sensor() );
          clSize=clus->size();
          zSensPos = sensor->z();
          //sensLeft = sensor->isLeft();
          //SensRtype=1;
          SensNumber=clus->channelID().sensor();
        }else if (names[mtype]=="VeloPhi"){
          const DeVeloPhiType* sensor = m_veloDet->phiSensor( clus->channelID().sensor() );
          clSize=clus->size();
          zSensPos = sensor->z();
          //sensLeft = sensor->isLeft();            
          //SensRtype=0;
          SensNumber=clus->channelID().sensor();
        }
        if (!m_etastudy){
          
          //if(type==3 
             //&& fabs(pntx)<1.5 
          //   && p>5 && chi2/ndof<5 && 
          //   (m_sideRight > 15 || m_sideLeft>15))
          //  {
              
          //event information
          tuple->column( "run",m_runodin);
          tuple->column( "evt",m_eventodin);
          //tuple->column( "bunchid",m_bunchid);
          tuple->column( "evTimeSec",eventTimeMicroSecGps); //(long) m_evTimeGps));
          tuple->column( "pvx",      pvx);
          tuple->column( "pvy",      pvy);
          tuple->column( "pvz",      pvz);
          tuple->column( "npv",      n_pv);
          tuple->column( "pvchi2",      pvchi2);
          tuple->column( "pvndof",      pvndof);
          tuple->column( "pvntr",      pvntr);
          tuple->column( "nbacktr",      n_back);
          //track information
          tuple->column( "ndof", ndof);
          tuple->column( "chi2", chi2);
          tuple->column( "theta", theta);
          tuple->column( "slx", slx);
          tuple->column( "sly", sly);
          //tuple->column( "x0",      x0);
          //tuple->column( "y0",      y0);
          //tuple->column( "nhits", nVeloHits);
          tuple->column( "nhitsr", m_sideRight);
          tuple->column( "nhitsl" , m_sideLeft);
          tuple->column( "p", p);
          tuple->column( "type", type);
          //tuple->column( "adcpertrack",adcpertrack);
          //tuple->column( "side",sensLeft);
          //cluster information
          tuple->column( "pitch",pitch);
          //tuple->column( "Rtype",SensRtype);
          tuple->column( "clSize",clSize);
          tuple->column( "adcsum",adcsum);
          tuple->column( "station", zSensPos);
          tuple->column( "sensnum", SensNumber );
          tuple->column( "pntx",pntx);
          tuple->column( "pnty",pnty);
          tuple->column( "pntz",pntz);
          tuple->column( "pntr",pntr);
          tuple->column( "pntphi",pntphi);
          tuple->column( "pntr_loc",pntr_loc);
          tuple->column( "pntphi_loc",pntphi_loc);
          //tuple->column( "unbiaspntx",trackInterceptGlobal.x());
          //tuple->column( "unbiaspnty",trackInterceptGlobal.y());
          //tuple->column( "unbiaspntz",trackInterceptGlobal.z());
          //tuple->column( "unbiaspntphi",trackInterceptGlobal.Phi());
          //tuple->column( "unbiaspntr",trackInterceptGlobal.Rho());
          tuple->column( "projectedAngle", projAngle);
          tuple->column( "rho", anglerho);
          tuple->column( "rhostrip", angleofstriplocalframe);
          tuple->column( "rhotrack", angleoftracklocalframe);
          tuple->column( "reso",resolution);
          //kalman unbiased residual
          //tuple->column( "unres",(*inode)->unbiasedResidual()); 
          tuple->column( "res",residual); 
          tuple->column( "res_x",residual_x); 
          tuple->column( "res_y",residual_y); 
          //tuple->column( "clerr",toolInfo.fractionalError*pitch);
          tuple->column( "eres",(*inode)->errResidual());
          //tuple->column( "unbiasres",unbiasres);
          //tuple->column( "unbiasreschi2",unbiasres_chi2);

          if(m_runWithMC){
            tuple->column("reso_mc", reso_mc);
            tuple->column("fracPos", fracPos);
            tuple->column("fracPosTool", fracPosTool);
            tuple->column("pitch_mc", pitch_mc);
            tuple->column("slx_mc", slx_mc);
            tuple->column("sly_mc", sly_mc);
            tuple->column("projectedAngle_mc", projAngle_mc);
            tuple->column("err_mc", err_mc);
            tuple->column("pntx_mc", pntx_mc);
            tuple->column("pnty_mc", pnty_mc);
            tuple->column("pntz_mc", pntz_mc);
            tuple->column("pntr_mc", pntr_mc);
            tuple->column("r_clu", r_clu);
            tuple->column("pntphi_mc", pntphi_mc);
          }
          tuple->write();
          //}
        
          
          
        }
        
        
        else{
          //ntuple for eta study
          if (type==3 && p>=1 && clSize == 2){
            //event information
            tuple->column( "run",m_runodin);
            tuple->column( "evt",m_eventodin);
            //track information
            tuple->column( "ndof", ndof);
            tuple->column( "chi2", chi2);
            tuple->column( "theta", theta);
            tuple->column( "slx", slx);
            tuple->column( "sly", sly);
            tuple->column( "nhits", nVeloHits);
            tuple->column( "nhitsr", m_sideRight);
            tuple->column( "nhitsl" , m_sideLeft);
            tuple->column( "p", p);
            //cluster information
            tuple->column( "pitch",pitch);
            tuple->column( "clSize",clSize);
            tuple->column( "adcsum",adcsum);
            tuple->column( "sensnum", SensNumber );
            tuple->column( "pntx",pntx);
            tuple->column( "pnty",pnty);
            tuple->column( "pntz",pntz);
            tuple->column( "pntr",pntr);
            tuple->column( "pntphi",pntphi);
            tuple->column( "unbiaspntx",trackInterceptGlobal.x());
            tuple->column( "unbiaspnty",trackInterceptGlobal.y());
            tuple->column( "unbiaspntz",trackInterceptGlobal.z());
            tuple->column( "unbiaspntphi",trackInterceptGlobal.Phi());
            tuple->column( "unbiaspntr",trackInterceptGlobal.Rho());
            tuple->column( "projectedAngle", projAngle);
            tuple->column( "reso",resolution);
            tuple->column( "clerr",toolInfo.fractionalError*pitch);
            tuple->column( "unbiasres",unbiasres);
            tuple->column( "unbiasreschi2",unbiasres_chi2);
            tuple->column( "clpos",cl);
            tuple->column( "clstripn",clus->channelID().strip());
            tuple->column( "fracpos",fracPosTool);
            tuple->column( "Trfracpos",trfracpos);
            
            // strip information
            vector< float > stripADC ;
            vector< int > stripNum ;
            for(int strip=0; strip<clSize; ++strip){
              stripADC.push_back(clus->stripValues()[strip].second);
              stripNum.push_back(clus->stripValues()[strip].first);
            }
            tuple->farray( "stripadc",stripADC,"stripN",4);
            tuple->farray( "stripNum",stripNum,"stripN",4);
            tuple->write();
          }
        }
        
                  

        delete tr_clone;
        
      }
      //low tolerance
    }//end of loop on the node
  } //track.nodes.size>0
  if( msgLevel(MSG::DEBUG) )  
    debug() << "Filled cluster information ntuple " << endmsg;
  return ( StatusCode::SUCCESS );
}

//=============================================================================
void Velo::VeloTrackMonitorNT::FillVeloTrNtuple(const LHCb::Track& track,
                                                int n_pv,int n_back, 
                                                double pvx, double pvy, double pvz,
                                                double pvchi2, double pvndof, int pvntr)
{
  Tuple tuple=nTuple("VeloTrNtuple", "Track ntuple",CLID_ColumnWiseTuple );
  long long int tzero= 1270064494071424ll; //there is an ll at the end, so that C++ knows this has to be a long long
  float eventTimeMicroSecGps = (float) (((m_evTimeGps  - tzero)/1000000.));


  if(m_runWithMC)
    eventTimeMicroSecGps=0;


  if( msgLevel(MSG::DEBUG) )  
    debug() << "Track information ntuple " << endmsg;
 

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nVeloHits = std::count_if(ids.begin(), ids.end(),
                                               bind(&LHCbID::isVelo,_1));
  if (nVeloHits>0){
    unsigned int nExpectedHits = m_expectTool->nExpected(track);
    double pseudoEff=((double) nVeloHits)/((double) nExpectedHits);
  
    float m_adcpertrack=0.;
    unsigned int m_sideLeft=0, m_sideRight=0;
    double zmin= +800.;
    double zmax= -350.;
    unsigned int sensnrmin=105;
    unsigned int sensnrmax=0;
    unsigned int sensnpmin=105;
    unsigned int sensnpmax=64;
    std::vector<double> N_exp;    
    std::vector<double> N_rec;
    for (int j=0; j<106; j++){
      N_rec.push_back(0.);
      N_exp.push_back(0.);
    }

    LHCb::Track::ConstNodeRange nodes = track.nodes();
    for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
        inode != nodes.end(); ++inode) {
      if(( (*inode)->type() == LHCb::Node::HitOnTrack )
         && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
               ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
        LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
        LHCb::VeloCluster *cluster;
        cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID()));
        LHCb::VeloChannelID chan  = nodeID.veloID();
        int sensorID = chan.sensor();
        const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
        m_adcpertrack+=(float)cluster->totalCharge();
        if (sensor->isLeft()) 
          m_sideLeft=1;
        else
          m_sideRight=1;
        N_rec[sensorID]+=1;
        if( msgLevel(MSG::DEBUG) )  
          debug()<<"z pos=" << sensor->z() <<" sensor" << chan.sensor()<<endmsg;
        if (sensor->z() <zmin)
          zmin = sensor->z();
        if ((chan.sensor()<sensnrmin) && (chan.sensor()<42))
          sensnrmin=chan.sensor();
        if ( (chan.sensor()<sensnpmin) && (chan.sensor()>=64))
          sensnpmin = chan.sensor();
        if (sensor->z() > zmax)
          zmax = sensor->z();
        if ((chan.sensor()>sensnrmax) && (chan.sensor()<42))
          sensnrmax=chan.sensor();
        if ( (chan.sensor()>sensnpmax) && (chan.sensor()>=64))
          sensnpmax = chan.sensor();
      }//if node velo
    }//loop on the node
  
    if (nVeloHits>0)
      m_adcpertrack = m_adcpertrack/nVeloHits;
    unsigned int sensnmin=105, sensnmax=0;
    sensnmax= ((sensnpmax-64)>sensnrmax) ? (sensnpmax-64) : sensnrmax ;
    sensnmin= ((sensnpmin-64)<sensnrmin) ? (sensnpmin-64) : sensnrmin ;
    if( msgLevel(MSG::DEBUG) )  
      debug()<<" zmin=" <<zmin <<" zmax="<<zmax<<" sensnmin="
             <<sensnmin<<" sensnmax="<<sensnmax<<endmsg;
    if( msgLevel(MSG::DEBUG) )  
      debug()<<" sensnrmin="<<sensnrmin<<" sensnrmax="<<sensnrmax<<endmsg;
    if( msgLevel(MSG::DEBUG) )  
      debug()<<" sensnpmin="<<sensnpmin<<" sensnpmax="<<sensnpmax<<endmsg;

    for(unsigned int j=(sensnmin); j<=sensnmax; j++){
      if ((j<=41) || ((j>=64)&&(j<=105))){
        if( msgLevel(MSG::DEBUG) )  
          debug()<<"sensor number "<<j<<endmsg;
        bool nExpectedHits_sensorR = m_expectTool -> isInside(track, j);
        if(nExpectedHits_sensorR == true) 
          N_exp[j]+=1;
        if( msgLevel(MSG::DEBUG) )  
          debug()<<j <<" ==> N_exp "<<N_exp[j]<<endmsg;
        bool nExpectedHits_sensorP = m_expectTool -> isInside(track, j+64);
        if(nExpectedHits_sensorP == true) 
          N_exp[j+64]+=1;
        if( msgLevel(MSG::DEBUG) )  
          debug()<<j+64<<" ==> N_exp "<<N_exp[j]<<endmsg;
      }
    }//end of loop over sensors

    if( msgLevel(MSG::DEBUG) )  
      debug()<<"Pseudoefficiency per sensor " <<endmsg;
    //Pseudoefficiency for each sensor
    //--------------------------------
    double pseudoEfficiency_sens=0;
    double N_expTot=0,N_recTot=0;
    
    for(unsigned int i=(sensnmin); i<=sensnmax; i++){
      if( msgLevel(MSG::DEBUG) )  
        debug()<<"N_recTot " << N_recTot <<endmsg;

      N_expTot+=N_exp[i+64];
      N_recTot+=N_rec[i+64];
      N_expTot+=N_exp[i];
      N_recTot+=N_rec[i];
    
      if( msgLevel(MSG::DEBUG) )  
        debug()<<i<<" N_rec "<<N_rec[i]<<" N_exp "<<N_exp[i]<<endmsg;
      if(N_exp[i]>=1){
        pseudoEfficiency_sens = N_rec[i] / N_exp[i];
        if( msgLevel(MSG::DEBUG) )  
          debug()<<i<<" ==> pseudoefficiency "<<pseudoEfficiency_sens<<endmsg;    
        if(N_rec[i] < N_exp[i]){
          const DeVeloSensor* sensor_h = m_veloDet->sensor(i); 
          Gaudi::XYZPoint trackInterceptGlobal_h= extrapolateToZ(&track, sensor_h->z());
          //Gaudi::XYZPoint trackInterceptHalf_h = sensor_h->globalToVeloHalfBox ( trackInterceptGlobal_h );
          Gaudi::XYZPoint trackInterceptLocal_h(0,0,0) ;      
          trackInterceptLocal_h    = sensor_h -> globalToLocal( trackInterceptGlobal_h );
        } 
      }
      if( msgLevel(MSG::DEBUG) )  
        debug()<<i+64<<" N_rec "<<N_rec[i+64]<<" N_exp "<<N_exp[i+64]<<endmsg;
      pseudoEfficiency_sens = N_exp[i+64] != 0 ? N_rec[i+64] / N_exp[i+64] : 0.;
      if( msgLevel(MSG::DEBUG) )  
        debug()<<i+64<<" ==> pseudoefficiency "<<pseudoEfficiency_sens<<endmsg;
    }//loop on sensors


    double pseudoEfficiencyInterp = ((float)N_recTot)/ ((float)N_expTot);

    unsigned int m_side;
    if (m_sideRight==1 && m_sideLeft==1)
      m_side=2; //overlap
    else if (m_sideRight==1)
      m_side=1; //right-A side
    else 
      m_side=0; //left-C side
  
    if( nVeloHits>0 ) {
      double chi2=track.info(LHCb::Track::FitVeloChi2,0);//chi2();
      double theta = track.firstState().slopes().theta();
      double slx = track.firstState().tx();
      double sly = track.firstState().ty();
      //double x0 = track.firstState().x();
      //double y0 = track.firstState().y();
      //double z0 = track.firstState().z();
      double ndof=track.info(LHCb::Track::FitVeloNDoF,0) ;//nDoF();
      double type=track.type();
      double p=track.p()/Gaudi::Units::GeV ;
 
      tuple->column( "run",m_runodin);
      tuple->column( "evt",m_eventodin);
      tuple->column( "evTimeSec",eventTimeMicroSecGps); //(long) m_evTimeGps));
      //tuple->column( "bunchid",m_bunchid);
      tuple->column( "ndof", ndof);
      tuple->column( "chi2", chi2);
      tuple->column( "theta", theta);
      tuple->column( "slx", slx);
      tuple->column( "sly", sly);
      //tuple->column( "x0",      x0);
      //tuple->column( "y0",      y0);
      //tuple->column( "z0",      z0);
      tuple->column( "p", p);
      tuple->column( "type", type);
      tuple->column( "pvx",      pvx);
      tuple->column( "pvy",      pvy);
      tuple->column( "pvz",      pvz);
      tuple->column( "npv",      n_pv);
      tuple->column( "pvchi2",      pvchi2);
      tuple->column( "pvndof",      pvndof);
      tuple->column( "pvntr",      pvntr);
      tuple->column( "nbacktr",      n_back);
      tuple->column( "nhits", nVeloHits);
      tuple->column( "nhitsr", m_sideRight);
      tuple->column( "nhitsl" , m_sideLeft);
      tuple->column( "nexpecthits", nExpectedHits);
      //tuple->column( "adcpertrack", m_adcpertrack);
      tuple->column( "Trside",    m_side);
      tuple->column( "Eff", pseudoEff);
      tuple->column( "EffInterp", pseudoEfficiencyInterp);
      tuple->write();
    }
  }
  
  if( msgLevel(MSG::DEBUG) )  
    debug() << "Filled Track information ntuple " << endmsg;
}

//=============================================================================
void Velo::VeloTrackMonitorNT::FillVeloEvNtuple(LHCb::Tracks* tracks,int n_pv,
                                                const LHCb::RecVertex* pv)
{
  
  Tuple tuple=nTuple("VeloTrEvNtuple", "Event ntuple",CLID_ColumnWiseTuple );


  long long int tzero= 1270064494071424ll; //there is an ll at the end, so that C++ knows this has to be a long long
  float eventTimeMicroSecGps = (float) (((m_evTimeGps  - tzero)/1000000.));
  debug()<< "Cluster information ntuple " << endmsg;


  if(m_runWithMC)
    eventTimeMicroSecGps=0;


  if( msgLevel(MSG::DEBUG) )  
    debug() << "Event information ntuple " << endmsg;

  int lefttracknum=0,righttracknum=0,troverlapnum=0;
  //  TrackVector lefttracks, righttracks;
  
  int tottrcl=0, tottrrcl=0;
  LHCb::Tracks::const_iterator itTrg;
  for(LHCb::Tracks::const_iterator itTrg=tracks->begin();itTrg!=tracks->end();itTrg++){
    LHCb::Track* track = (*itTrg);
    const std::vector< LHCb::LHCbID >& ids = track->lhcbIDs();
    tottrcl  += std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVelo,_1));
    tottrrcl += std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVeloR,_1));
    unsigned int m_sideLeft=0, m_sideRight=0;
    LHCb::Track::ConstNodeRange nodes = track->nodes();
    for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
        inode != nodes.end(); ++inode) {
      if(( (*inode)->type() == LHCb::Node::HitOnTrack )
         && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
               ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
        LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
        LHCb::VeloCluster *cluster;
        cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID())); 
        const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
        if ( cluster->firstChannel().sensor()<=105){
          if (sensor->isLeft()) 
            m_sideLeft=1;
          else
            m_sideRight=1;
        }
      }
      
    }
    if (m_sideRight==1 && m_sideLeft==1)
      troverlapnum+=1;
    else if (m_sideRight==1)
      righttracknum+=1;
    else 
      lefttracknum+=1;
  }
  
  int totcl= 0;
  int totrcl=0;
  for(LHCb::VeloClusters::const_iterator cluIt=m_rawClusters->begin(); 
      cluIt!=m_rawClusters->end(); cluIt++){
    if (((*cluIt)->channelID().sensor()<42)||
        (((*cluIt)->channelID().sensor()>=64) 
         && ((*cluIt)->channelID().sensor()<=105))){
      totcl+=1;
      if ((*cluIt)->isRType())
        totrcl+=1;
    }
  }

  double pvlx=0.,pvly=0.,pvlz=0.;
  double pvrx=0.,pvry=0.,pvrz=0.;
  int pvlntr=0,pvrntr=0;
  typedef std::vector<const LHCb::Track*> TrackVector ;

  int selectmethod = 1; // 0: Select Left/Righttracks by direction of slope, 1: Select Left/Righttracks by number of hits in Velo Half
  // now split the primary vertex in left and right tracks
  TrackVector pvtracks = myconvert(pv->tracks()) ;
  
  //TrackVector lefttracks = myselect(pvtracks,TrackVeloSidePredicate(+1)) ;
  //TrackVector righttracks =  myselect(pvtracks,TrackVeloSidePredicate(-1)) ; 
  TrackVector lefttracks, righttracks;
  if (selectmethod==0){
    lefttracks = myselect(pvtracks,TrackVeloSidePredicate(+1)) ;
    righttracks =  myselect(pvtracks,TrackVeloSidePredicate(-1)) ;
  }
  else if (selectmethod ==1){
    for (unsigned int i = 0; i < pvtracks.size(); ++i){
      unsigned int m_sideLeft=0, m_sideRight=0;
      LHCb::Track::ConstNodeRange nodes = pvtracks.at(i)->nodes();
      for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
        inode != nodes.end(); ++inode) {
       
        if(( (*inode)->type() == LHCb::Node::HitOnTrack )
           && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
                 ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
          LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
          LHCb::VeloCluster *cluster;
          cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID()));
          const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
          if (sensor->isLeft()) 
            m_sideLeft+=1;
          else
            m_sideRight+=1;

        }
        
      }
      
      if (m_sideRight == 0) lefttracks.push_back(pvtracks.at(i));
      else if (m_sideLeft == 0) righttracks.push_back(pvtracks.at(i));
      
    }
    
    
  }
  
  unsigned int SenNuml= 200.;
  vector<double> lx1vec, ly1vec, lz1vec, slxlvec, slylvec, SenNumlvec;
  double lx1 = 0., ly1 = 0.,lz1 = 0.;
  vector<double> rx1vec, ry1vec, rz1vec, slxrvec, slyrvec, SenNumrvec;
  double Rx1 = 0., Ry1 = 0.,Rz1 = 0.;
  unsigned int SenNumR = 200.;
  if( lefttracks.size() >= 10 && righttracks.size() >= 10 ) {
    // fit two vertices
    LHCb::RecVertex* leftvertex  = m_vertexer->fit( lefttracks ) ;
    LHCb::RecVertex* rightvertex = m_vertexer->fit( righttracks ) ;
    if( leftvertex && rightvertex) {
      pvlx=leftvertex->position().x();
      pvly=leftvertex->position().y();  
      pvlz=leftvertex->position().z();
      pvlntr=lefttracks.size();
      pvrx=rightvertex->position().x();
      pvry=rightvertex->position().y();  
      pvrz=rightvertex->position().z();
      pvrntr=righttracks.size();
    }
  


 //  double DistlMax = 100000.;
//   double lx1 = 0., ly1 = 0.,lz1 = 0.;

//   for(TrackVector::const_iterator itTrg= lefttracks.begin();itTrg!= lefttracks.end();itTrg++){

//    const LHCb::State& statel = (*itTrg)->closestState(pvlz);
//    double statelx = statel.position().x();
//    double stately = statel.position().y();
//    double statelz = statel.position().z();
//    double Distl = sqrt((statelx-pvlx)*(statelx-pvlx)+(stately-pvly)*(stately-pvly)+(statelz-pvlz)*(statelz-pvlz));

//    if(Distl<DistlMax){
//      DistlMax = Distl;
//      lx1 = statelx;
//      ly1 = stately;
//      lz1 = statelz;
//     }
//  }



  

for(TrackVector::const_iterator itTrg= lefttracks.begin();itTrg!= lefttracks.end();itTrg++){

  double DistlMax = 100000.;
  LHCb::Track::ConstNodeRange nodes = (*itTrg)->nodes();
    for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
        inode != nodes.end(); ++inode) {
      if(( (*inode)->type() == LHCb::Node::HitOnTrack )
         && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
               ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
        
        
    double statelx = (*inode)->position().x();
    double stately = (*inode)->position().y();
    double statelz = (*inode)->position().z();
    double Distl = sqrt((statelx-pvlx)*(statelx-pvlx)+(stately-pvly)*(stately-pvly)+(statelz-pvlz)*(statelz-pvlz));

        if(Distl<DistlMax){
        DistlMax = Distl;
        lx1 = statelx;
        ly1 = stately;
        lz1 = statelz;

        LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
        LHCb::VeloCluster *cluster;
        cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID())); 
        const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
        SenNuml = sensor->sensorNumber();

          }
      }
      
    }
    lx1vec.push_back(lx1);
    ly1vec.push_back(ly1);
    lz1vec.push_back(lz1);
    double slx = (*itTrg)->firstState().tx();
    double sly = (*itTrg)->firstState().ty();
    slxlvec.push_back(slx);
    slylvec.push_back(sly);    
    SenNumlvec.push_back(SenNuml);
    
 }

 


for(TrackVector::const_iterator itTrg= righttracks.begin();itTrg!= righttracks.end();itTrg++){

 double DistRMax = 100000.;
  LHCb::Track::ConstNodeRange nodes = (*itTrg)->nodes();
    for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
        inode != nodes.end(); ++inode) {
      if(( (*inode)->type() == LHCb::Node::HitOnTrack )
         && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
               ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {


    double stateRx = (*inode)->position().x();
    double stateRy = (*inode)->position().y();
    double stateRz = (*inode)->position().z();
    double DistR = sqrt((stateRx-pvrx)*(stateRx-pvrx)+(stateRy-pvry)*(stateRy-pvry)+(stateRz-pvrz)*(stateRz-pvrz));

        if(DistR<DistRMax){
        DistRMax = DistR;
        Rx1 = stateRx;
        Ry1 = stateRy;
        Rz1 = stateRz;

        LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
        LHCb::VeloCluster *cluster;
        cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID())); 
        const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
        SenNumR = sensor->sensorNumber();
    
          }
      }
    }
    
    rx1vec.push_back(Rx1);
    ry1vec.push_back(Ry1);
    rz1vec.push_back(Rz1);
    double slx = (*itTrg)->firstState().tx();
    double sly = (*itTrg)->firstState().ty();
    slxrvec.push_back(slx);
    slyrvec.push_back(sly);
    SenNumrvec.push_back(SenNumR);
 }
  }
  

   
  
  tuple->column( "run",m_runodin);
  tuple->column( "evt",m_eventodin);
  tuple->column( "bunchid",m_bunchid);
  tuple->column( "evTimeSec",eventTimeMicroSecGps); 
  unsigned int m_trnum=(tracks->size());
  tuple->column( "SenNuml",SenNuml);
  tuple->farray( "lx1vec",lx1vec.begin(),lx1vec.end(), "Lenl",100);
  tuple->farray( "ly1vec",ly1vec.begin(),ly1vec.end(), "Lenl",100);
  tuple->farray( "lz1vec",lz1vec.begin(),lz1vec.end(), "Lenl",100);
  tuple->farray( "slxlvec",slxlvec.begin(),slxlvec.end(), "Lenl", 100);
  tuple->farray( "slylvec",slylvec.begin(),slylvec.end(), "Lenl", 100);
  tuple->farray( "SenNumlvec",SenNumlvec.begin(),SenNumlvec.end(),"Lenl", 100);
  tuple->column( "SenNumR",SenNumR);
  tuple->farray( "rx1vec",rx1vec.begin(),rx1vec.end(), "Lenr",100);
  tuple->farray( "ry1vec",ry1vec.begin(),ry1vec.end(), "Lenr",100);
  tuple->farray( "rz1vec",rz1vec.begin(),rz1vec.end(), "Lenr",100);
  tuple->farray( "slxrvec",slxrvec.begin(),slxrvec.end(), "Lenr", 100);
  tuple->farray( "slyrvec",slyrvec.begin(),slyrvec.end(), "Lenr", 100);
  tuple->farray( "SenNumrvec",SenNumrvec.begin(),SenNumrvec.end(),"Lenr", 100);
  tuple->column( "trnum",m_trnum);
  tuple->column( "lefttr",  lefttracknum);
  tuple->column( "righttr",  righttracknum);
  tuple->column( "overlaptr",  troverlapnum );   
  tuple->column( "totcl", totcl);
  tuple->column( "totrcl", totrcl);
  tuple->column( "tottrcl", tottrcl);
  tuple->column( "tottrrcl", tottrrcl);
  tuple->column( "pvx",      pv->position().x());
  tuple->column( "pvy",      pv->position().y());
  tuple->column( "pvz",      pv->position().z());
  tuple->column( "npv",      n_pv);
  tuple->column( "pvchi2",      pv->chi2());
  tuple->column( "pvndof",      pv->nDoF());
  tuple->column( "pvntr",      pv->tracks().size());
  tuple->column( "pvlx",      pvlx);
  tuple->column( "pvly",      pvly);
  tuple->column( "pvlz",      pvlz);
  tuple->column( "pvlntr",      pvlntr);
  tuple->column( "pvrx",      pvrx);
  tuple->column( "pvry",      pvry);
  tuple->column( "pvrz",      pvrz);
  tuple->column( "pvrntr",    pvrntr);
  tuple->write();

  if( msgLevel(MSG::DEBUG) )  
    debug() << "Filled Event information ntuple " << endmsg;
 
 

}	  


//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::FillVeloAllClNtuple(LHCb::Tracks* tracks)


{
  Tuple tuple=nTuple("VeloAllClNtuple", "All Clusters",CLID_ColumnWiseTuple );
  long long int tzero= 1270064494071424ll; //there is an ll at the end, so that C++ knows this has to be a long long
  float eventTimeMicroSecGps = (float) (((m_evTimeGps  - tzero)/1000000.));

  debug()<< "All Cluster information ntuple " << endmsg;
  
  LHCb::VeloClusters::const_iterator itVC;

  for ( itVC = m_rawClusters -> begin(); itVC != m_rawClusters -> end(); ++itVC ) { 
    //cout<<" qui"<<endl;
    
    LHCb::VeloCluster* cluster = (*itVC);
    const DeVeloSensor* sensor = m_veloDet->sensor( cluster->channelID().sensor() );

    double adcsum=cluster->totalCharge();
    double  station= sensor->z();
    double clr=0.;
    double clphi=0. ;
    int Rtype=cluster->isRType();
    double pitch=0.;
    if (Rtype){
      const DeVeloRType* rDet = m_veloDet->rSensor( cluster->channelID().sensor() );
      pitch = rDet->rPitch( unsigned(cluster->channelID().strip()) ) ;
      clr=rDet->globalROfStrip(unsigned(cluster->channelID().strip()));
     }
    else{
      const DeVeloPhiType* phiDet = m_veloDet->phiSensor( cluster->channelID().sensor());
      if (phiDet)
        clphi=phiDet->globalPhiOfStrip(unsigned(cluster->channelID().strip()));
    }
    
    int sensnum=cluster-> channelID().sensor();
    int clSize=cluster->size();
    unsigned int stripNumber   = cluster -> channelID().strip();
    unsigned int chipChannel   = sensor -> StripToChipChannel( stripNumber ); // 0 -> 2047
    unsigned int activeLink = chipChannel/32;

    if (sensnum<120){
      //to exclude the PU
      tuple->column( "run",m_runodin);
      tuple->column( "evt",m_eventodin);
      tuple->column( "bunchid",m_bunchid);
      tuple->column( "evTimeSec",eventTimeMicroSecGps); //(long) m_evTimeGps));
      unsigned int m_trnum=(tracks->size());
      tuple->column( "trnum",m_trnum);
      unsigned int m_totcl=(m_rawClusters->size());
      tuple->column( "totcl",m_totcl);
      tuple->column( "pitch",pitch);
      tuple->column( "Rtype",Rtype);
      tuple->column( "clSize",clSize);
      tuple->column( "adcsum",adcsum);
      tuple->column( "station", station);
      tuple->column( "sensnum", sensnum );
      tuple->column( "stripnum", stripNumber );
      tuple->column( "channel", chipChannel );
      tuple->column( "link", activeLink );
      tuple->column( "clr",clr);
      tuple->column( "clphi",clphi);
      tuple->write();
    }
    
        
  }
  
  if( msgLevel(MSG::DEBUG) )  
    debug() << "Filled cluster information ntuple " << endmsg;
  return ( StatusCode::SUCCESS );
}



//========================================================
// Linear extrapolator from a track to a given z position.
//========================================================
Gaudi::XYZPoint Velo::VeloTrackMonitorNT::extrapolateToZ(const LHCb::Track *track, double toZ)
{
  // get state parameters
  Gaudi::XYZPoint coord = track->position();
  Gaudi::XYZVector slope = track->slopes();
  Gaudi::XYZPoint result;
  double deltaZ=toZ-coord.z();
  result.SetXYZ(coord.x()+slope.x()*deltaZ, coord.y()+slope.y()*deltaZ, toZ);
  
  return result;
}

double Velo::VeloTrackMonitorNT::projAngleR(const Direction& locDirection,
                                      const Gaudi::XYZPoint& aLocPoint)
{
  if( msgLevel(MSG::DEBUG) )  
    debug()<< " --> projAngleR() " <<endmsg;
  //
  Gaudi::XYZVector parallel2Track;
  //Direction locDir=localTrackDirection(m_trackDir, sensor);
  //-- track angle
  double alphaOfTrack=angleOfInsertion(locDirection, parallel2Track);
  //-- vector normal to the strip - sensor type specific
  Gaudi::XYZVector perp2RStrip(aLocPoint.x(), aLocPoint.y(), 0.);
  double cosTrackOnNormal=parallel2Track.Dot(perp2RStrip.Unit());
  //-- projection of track on normal to local strip
  double trackOnNormal=fabs(cosTrackOnNormal);
  //-- projection of track on Z axis
  double trackOnZ=cos(alphaOfTrack);
  double projectedAngle=atan(trackOnNormal/trackOnZ);
  //
  return ( projectedAngle );
}

double Velo::VeloTrackMonitorNT::projAnglePhi(const Direction& locDirection,
                                        const DeVeloPhiType* phiSensor,
                                        unsigned int centreStrip)
{
  if( msgLevel(MSG::DEBUG) )  
    debug()<< " --> projAnglePhi() " <<endmsg;
  //
  Gaudi::XYZVector parallel2Track;
  std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> strip;
  strip=phiSensor->localStripLimits(centreStrip);
  Gaudi::XYZVector perp2PhiStrip((strip.first.y()-strip.second.y()),
                                 (strip.second.x()-strip.first.x()), 0.);
  double alphaOfTrack=angleOfInsertion(locDirection, parallel2Track);
  double cosTrackOnNormal=parallel2Track.Dot(perp2PhiStrip.Unit());
  double trackOnNormal=fabs(cosTrackOnNormal);
  double trackOnZ=cos(alphaOfTrack);
  double projectedAngle=atan(trackOnNormal/trackOnZ);
  //
  return ( projectedAngle );
}

Direction Velo::VeloTrackMonitorNT::localTrackDirection(const Gaudi::XYZVector& gloTrackDir,
                                                  const DeVeloSensor* sensor) const
{
  if( msgLevel(MSG::DEBUG) )  
    debug()<< " --> localTrackDirection() " <<endmsg;
  //-- translate global slopes into local
  using namespace std;
  Direction locTrackDir;
  if(sensor->isLeft()&&(!sensor->isDownstream())){
    locTrackDir=make_pair(gloTrackDir.x(), gloTrackDir.y());
  }else if(sensor->isLeft()&&sensor->isDownstream()){
    locTrackDir=make_pair((-1)*gloTrackDir.x(), gloTrackDir.y());
  }else if(sensor->isRight()&&sensor->isDownstream()){
    locTrackDir=make_pair(gloTrackDir.x(), (-1)*gloTrackDir.y());
  }else if(sensor->isRight()&&(!sensor->isDownstream())){
    locTrackDir=make_pair((-1)*gloTrackDir.x(), (-1)*gloTrackDir.y());
  }
  //
  return ( locTrackDir );
}

double Velo::VeloTrackMonitorNT::angleOfInsertion(const Direction& localSlopes,
                                            Gaudi::XYZVector& parallel2Track) const
{
  if( msgLevel(MSG::DEBUG) )  
    debug()<< " --> angleOfInsertion() " <<endmsg;
  //
  const Gaudi::XYZVector ZVersor(0., 0., 1.);
  Gaudi::XYZVector perpPi1(1., 0., -(localSlopes.first));
  Gaudi::XYZVector perpPi2(0., 1., -(localSlopes.second));
  // vector parallel to the track
  Gaudi::XYZVector parallel=perpPi1.Cross(perpPi2);
  double modParallel=sqrt(parallel.Mag2());
  // and normalized parallel to track
  Gaudi::XYZVector normParallel(parallel.x()/modParallel,
                                parallel.y()/modParallel,
                                parallel.z()/modParallel
                                );
  parallel2Track=normParallel;
  double cosOfInsertion=parallel2Track.Dot(ZVersor);
  double alphaOfInsertion=acos(cosOfInsertion);
  //
  return ( alphaOfInsertion );
      
}

const LHCb::MCHit* Velo::VeloTrackMonitorNT::getAssocMCHit(const LHCb::VeloCluster* clus) const
{
  if( msgLevel(MSG::DEBUG) )  
    debug()<< " --> getAssocMCHit() " <<endmsg;
  //
  Range range=m_asctTable->relations(clus);
  
  if(range.begin()==range.end()){
    if( msgLevel(MSG::DEBUG) )  
      debug()<< " No MCHits associated " <<endmsg;
    return ( 0 );
  }
  const MCHit* aHit=(range.begin())->to();

  return ( aHit );
}

//============================================================================
VeloChannelID Velo::VeloTrackMonitorNT::weightedMean(const VeloCluster* cluster, double& isp)
{
  debug()<< " ==> weightedMean() " <<endmsg;
  //
  StatusCode sc;
  int strips=cluster->size();
  int intDistance=0;
  double centre=0., totalCharge=0.;
  // ask velo about pointer to sensor using cluster
  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVeloSensor* sensor=m_veloDet->sensor(sensorNumber);
  for(int strip=0; strip<strips; ++strip){
    sc=sensor->channelDistance(cluster->channels()[0], 
                               cluster->channels()[strip], intDistance);
    sc.ignore();
    debug()<< " int distance: " << intDistance <<endmsg;
    centre+=(static_cast<float>(intDistance+100))*
      cluster->stripValues()[strip].second;
    totalCharge+=cluster->stripValues()[strip].second;
  }
  centre/=totalCharge;
  centre-=100;
  intDistance=static_cast<int>(LHCb::Math::round(centre));
  debug()<< " centre: " << centre <<endmsg;
  isp=centre-intDistance;
  VeloChannelID intDistanceID;
  sc=sensor->neighbour(cluster->channels()[0], intDistance, intDistanceID);
  //
  if(sc.isSuccess()){
    return ( intDistanceID );
  }else{
    return ( VeloChannelID(0) );
  }
}
//-


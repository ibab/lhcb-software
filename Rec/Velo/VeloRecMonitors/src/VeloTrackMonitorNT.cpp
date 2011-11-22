// Include files 

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

//from TrackInterfaces
#include "TrackInterfaces/IVeloExpectation.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackSelector.h"

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

                                                                                
// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>


#include "VeloTrackMonitorNT.h"

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;
using namespace std;
 
typedef IVeloClusterPosition::Direction Direction;

DECLARE_NAMESPACE_ALGORITHM_FACTORY(Velo, VeloTrackMonitorNT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloTrackMonitorNT::VeloTrackMonitorNT(const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator ),
  m_allString("ALL"),
  m_clusterLoc ( VeloClusterLocation::Default ),
  m_asctLocation ( m_clusterLoc+"2MCHits" ),
  m_asctTable ( 0 )
{
  declareProperty( "TracksInContainer", m_tracksInContainer = LHCb::TrackLocation::Default );
  declareProperty("EventNtuple", m_trntuple = true);
  declareProperty("TrackNtuple", m_evntuple = true);
  declareProperty("ClusterOnTrackNtuple", m_clntuple = true);
  declareProperty("RunWithMC", m_runWithMC=false);
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloTrackMonitorNT::~VeloTrackMonitorNT() {}; 

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_veloDet = getDet<DeVelo>(  DeVeloLocation::Default ) ;

  m_expectTool = tool<IVeloExpectation>( "VeloExpectation");

  m_clusterTool=tool<IVeloClusterPosition>("VeloClusterPosition");

  return StatusCode::SUCCESS;
};

//=============================================================================
// Execute
//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::execute()
{
  
 
  if ( exist<LHCb::ODIN>( LHCb::ODINLocation::Default )){
    LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    if( msgLevel(MSG::DEBUG) ) debug()<< "Run "     << odin->runNumber()
            << ", Event " << odin->eventNumber() << endmsg;
    m_runodin=odin->runNumber();
    m_eventodin=(long unsigned int)odin->eventNumber();
    m_bunchid= odin->bunchId();
    
  } else {
    Warning("No ODIN Header").ignore();
    m_runodin=0;
    m_eventodin=0;
    m_bunchid= 0;
  }

  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_tracksInContainer);

  //std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // histograms per track

  if(!exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default)){
    return Error( " ==> There is no VeloClusters in TES " );
  }else{
    m_rawClusters=get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
  }
  if (m_evntuple)
    FillVeloEvNtuple(tracks);
  for (LHCb::Tracks::const_iterator iterT = tracks->begin(); iterT != tracks->end(); ++iterT){
      if((*iterT)->checkType(Track::Velo) == true){
        StatusCode cluNTStatus;
        if(m_clntuple) cluNTStatus=FillVeloClNtuple(**iterT);
        if(cluNTStatus.isSuccess()){
          cluNTStatus.ignore();
        }else{
          return ( cluNTStatus );
        }
        if (m_trntuple)
          FillVeloTrNtuple(**iterT);
      }
  } // iterT

  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode Velo::VeloTrackMonitorNT::FillVeloClNtuple(const LHCb::Track& track)   
{
  Tuple tuple=nTuple("VeloClNtuple", "Clusters on track",CLID_ColumnWiseTuple );

  if(m_runWithMC){
    // create the ass table if run over MC data
    AsctTool associator(evtSvc(), m_asctLocation);
    m_asctTable=associator.direct();
    if(!m_asctTable){
      return ( Error("Empty table with associations", StatusCode::FAILURE) );
    }
  }
  
  if( msgLevel(MSG::DEBUG) ) debug()<< "Cluster information ntuple " << endmsg;

  const LHCb::State& firststate = track.firstState() ;
  const LHCb::State& laststate = track.closestState(760) ;
 

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nVeloHits = std::count_if(ids.begin(), ids.end(),
                                               bind(&LHCbID::isVelo,_1));

  double z0=firststate.position().z()+20;
  double z1=laststate.position().z()-20;
  if( msgLevel(MSG::DEBUG) ) debug() << " z0="<<z0<<" z1="<<z1 <<endmsg;
  
  if( msgLevel(MSG::DEBUG) ) debug() << "Track node size="<<  track.nodes().size()<<endmsg;
  if( msgLevel(MSG::DEBUG) ) debug() << "Track lhcbids size="<<  track.lhcbIDs().size()<<endmsg;

  if( nVeloHits>0 ) {
    std::string names[] = { "VeloR","VeloPhi" } ;
    double chi2=track.chi2();
    double theta = track.firstState().slopes().theta();
    double slx = track.firstState().tx();
    double sly = track.firstState().ty();
    double x0 = track.firstState().x();
    double y0 = track.firstState().y();
    double ndof=track.nDoF();
    float adcpertrack=0.;
    Gaudi::XYZVector global3dDir=Gaudi::XYZVector(slx, sly, 1.);
    //const std::vector< LHCb::LHCbID >& trackIDs = track.lhcbIDs();
    //std::vector< LHCb::LHCbID >::const_iterator it;
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
      } 
    }
    if(nVeloHits>0) adcpertrack = adcpertrack/nVeloHits;

    for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
         inode != nodes.end(); ++inode) {
      if( msgLevel(MSG::DEBUG) ) debug() << " node type="<< (*inode)->type() <<endmsg;
      if( msgLevel(MSG::DEBUG) ) debug() << " errResidual2=" << (*inode)->errResidual2() <<
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
        double resolution = f*(*inode)->residual();
        // now get the pitch ...
        double pitch(1) ;
        double pntx=0.,pnty=0.,pntz=0.,pntphi=0.,pntr=0.;
        double pntx_mc=0.,pnty_mc=0.,pntz_mc=0.,pntphi_mc=0.,pntr_mc=0.;
        double adcsum=0;
        double projAngle=0., projAngle_mc=0.;
        Direction localDirection, localDirection_mc;

        double pitch_mc=0.;
        SiPositionInfo<LHCb::VeloChannelID> toolInfo;
        double fracPosTool=0., fracPos=0.;
        double slx_mc=0., sly_mc=0.;
        double err_mc=0.;
        double r_clu=0., r_hit=0., reso_mc=0.;
        
        if( (*inode)->measurement().type() == LHCb::Measurement::VeloPhi ){

          // this part is using reco info only - Phi clusters

          const LHCb::VeloCluster* clus = static_cast<LHCb::VeloPhiMeasurement&>
            ((*inode)->measurement()).cluster() ;
          const DeVeloPhiType* phiDet = m_veloDet->phiSensor( clus->channelID().sensor());
          Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
          pntx=globalPoint.x();
          pnty=globalPoint.y();
          pntz=globalPoint.z();
          pntphi=globalPoint.Phi();
          pntr=globalPoint.Rho();
          Gaudi::XYZPoint localPoint = 
            (*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint ) ;
          pitch =  localPoint.Rho() * phiDet->phiPitch( unsigned(clus->strip(0)) ) ;
          adcsum=clus->totalCharge();
          localDirection=localTrackDirection(global3dDir, phiDet);
          unsigned int centreStrip=clus->channelID().strip();
          projAngle=projAnglePhi(localDirection, phiDet, centreStrip);

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
            toolInfo=m_clusterTool->position(clus);
            fracPosTool=toolInfo.fractionalPosition;
            fracPos=clus->interStripFraction();
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
            auto_ptr<Trajectory> traj=phiDet->trajectory(cluCentChann, fracPos);
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

          const LHCb::VeloCluster* clus = 
            static_cast<LHCb::VeloRMeasurement&>((*inode)->measurement()).cluster() ;
          const DeVeloRType* rDet = m_veloDet->rSensor( clus->channelID().sensor() );
          Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
          pntx=globalPoint.x();
          pnty=globalPoint.y();
          pntz=globalPoint.z();
          pntphi=globalPoint.Phi();
          pntr=globalPoint.Rho();
          pitch = rDet->rPitch( unsigned(clus->strip(0)) ) ;
          adcsum=clus->totalCharge();
          Gaudi::XYZPoint localPoint = 
            (*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint );
          localDirection=localTrackDirection(global3dDir, rDet);
          projAngle=projAngleR(localDirection, localPoint);

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
            toolInfo=m_clusterTool->position(clus);
            fracPosTool=toolInfo.fractionalPosition;
            fracPos=clus->interStripFraction();
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
        int clSize =0, sensLeft=-1, SensRtype=-1,SensNumber=-1;
        double zSensPos=0.;
        if (names[mtype]=="VeloR"){
          const LHCb::VeloCluster* clus = 
            static_cast<LHCb::VeloRMeasurement&>((*inode)->measurement()).cluster() ;
          clSize=clus->size();
          const DeVeloSensor* sensor = m_veloDet->sensor( clus->channelID().sensor() );
          clSize=clus->size();
          zSensPos = sensor->z();
          sensLeft = sensor->isLeft();
          SensRtype=1;
          SensNumber=clus->channelID().sensor();
        }else if (names[mtype]=="VeloPhi"){
          const LHCb::VeloCluster* clus = 
            static_cast<LHCb::VeloPhiMeasurement&>((*inode)->measurement()).cluster() ;
          const DeVeloPhiType* sensor = m_veloDet->phiSensor( clus->channelID().sensor() );
          clSize=clus->size();
          zSensPos = sensor->z();
          sensLeft = sensor->isLeft();            
          SensRtype=0;
          SensNumber=clus->channelID().sensor();
        }
        tuple->column( "run",m_runodin);
        tuple->column( "evt",m_eventodin);
        tuple->column( "bunchid",m_bunchid);
        tuple->column( "ndof", ndof);
        tuple->column( "chi2", chi2);
        tuple->column( "theta", theta);
        tuple->column( "slx", slx);
        tuple->column( "sly", sly);
        tuple->column( "x0",      x0);
        tuple->column( "y0",      y0);
        tuple->column( "vx",      slx*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+x0);
        tuple->column( "vy",      sly*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+y0);
        tuple->column( "vz",      -(slx*x0+sly*y0)/(slx*slx+sly*sly));
        tuple->column( "nhits", nVeloHits);
        tuple->column( "adcpertrack",adcpertrack);
        tuple->column( "side",sensLeft);
        tuple->column( "reso",resolution);
        tuple->column( "res",residual);
        tuple->column( "eres",(*inode)->errResidual());
        tuple->column( "pitch",pitch);
        tuple->column( "Rtype",SensRtype);
        tuple->column( "clSize",clSize);
        tuple->column( "adcsum",adcsum);
        tuple->column( "station", zSensPos);
        tuple->column( "sensnum", SensNumber );
        tuple->column( "pntx",pntx);
        tuple->column( "pnty",pnty);
        tuple->column( "pntz",pntz);
        tuple->column( "pntr",pntr);
        tuple->column( "pntphi",pntphi);
        tuple->column( "projectedAngle", projAngle);
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
        
      }
      //low tolerance
    }//end of loop on the node
  } //track.nodes.size>0
  if( msgLevel(MSG::DEBUG) ) debug() << "Filled cluster information ntuple " << endmsg;
  return ( StatusCode::SUCCESS );
}

//=============================================================================
void Velo::VeloTrackMonitorNT::FillVeloTrNtuple(const LHCb::Track& track)
{
  Tuple tuple=nTuple("VeloTrNtuple", "Track ntuple",CLID_ColumnWiseTuple );

  if( msgLevel(MSG::DEBUG) ) debug() << "Track information ntuple " << endmsg;
 

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nVeloHits = std::count_if(ids.begin(), ids.end(),
                                               bind(&LHCbID::isVelo,_1));
  unsigned int nExpectedHits = m_expectTool->nExpected(track);
  double pseudoEff=((double) nVeloHits)/((double) nExpectedHits);
  
  float m_adcpertrack=0.;
  unsigned int m_sideLeft=0, m_sideRight=0;
  double zmin= +800.;
  double zmax= -350.;
  unsigned int sensnrmin=105;
  unsigned int sensnrmax=0;
  unsigned int sensnpmin=105;
  unsigned int sensnpmax=0;
  std::vector<double> N_exp;    
  std::vector<double> N_rec;
  for (int j=0; j<106; j++){
    N_rec.push_back(0.);
    N_exp.push_back(0.);
  }

  //const std::vector<LHCbID>& vids = track.lhcbIDs();
  LHCb::Track::ConstNodeRange nodes = track.nodes();
  for(LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
       inode != nodes.end(); ++inode) {
    if(( (*inode)->type() == LHCb::Node::HitOnTrack )
       && (  ((*inode)->measurement().type() == LHCb::Measurement::VeloR)
             ||((*inode)->measurement().type() == LHCb::Measurement::VeloPhi))) {
      LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
      LHCb::VeloCluster *cluster;
      cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject((nodeID.channelID()));
      //  for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
      //da fissare
      LHCb::VeloChannelID chan  = nodeID.veloID();
      int sensorID = chan.sensor();
      const DeVeloSensor* sensor = m_veloDet->sensor( cluster->firstChannel().sensor() );
      m_adcpertrack+=(float)cluster->totalCharge();
      if (sensor->isLeft()) 
        m_sideLeft=1;
      else
        m_sideRight=1;
      N_rec[sensorID]+=1;
      if( msgLevel(MSG::DEBUG) ) debug()<<"z pos=" << sensor->z() <<" sensor" << chan.sensor()<<endmsg;
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
  if( msgLevel(MSG::DEBUG) ) debug()<<" zmin=" <<zmin <<" zmax="<<zmax<<" sensnmin="<<sensnmin<<" sensnmax="<<sensnmax<<endmsg;
  if( msgLevel(MSG::DEBUG) ) debug()<<" sensnrmin="<<sensnrmin<<" sensnrmax="<<sensnrmax<<endmsg;
  if( msgLevel(MSG::DEBUG) ) debug()<<" sensnpmin="<<sensnpmin<<" sensnpmax="<<sensnpmax<<endmsg;

  for(unsigned int j=(sensnmin); j<=sensnmax; j++){
    if ((j<=41) || ((j>=64)&&(j<=105))){
      if( msgLevel(MSG::DEBUG) ) debug()<<"sensor number "<<j<<endmsg;
      bool nExpectedHits_sensorR = m_expectTool -> isInside(track, j);
      if(nExpectedHits_sensorR == true) 
        N_exp[j]+=1;
      if( msgLevel(MSG::DEBUG) ) debug()<<j <<" ==> N_exp "<<N_exp[j]<<endmsg;
      bool nExpectedHits_sensorP = m_expectTool -> isInside(track, j+64);
      if(nExpectedHits_sensorP == true) 
        N_exp[j+64]+=1;
      if( msgLevel(MSG::DEBUG) ) debug()<<j+64<<" ==> N_exp "<<N_exp[j]<<endmsg;
    }
  }//end of loop over sensors

  if( msgLevel(MSG::DEBUG) ) debug()<<"Pseudoefficiency per sensor " <<endmsg;
  //Pseudoefficiency for each sensor
  //--------------------------------
  double N_expTot=0,N_recTot=0;
    
  for(unsigned int i=(sensnmin); i<=sensnmax; i++){
    if( msgLevel(MSG::DEBUG) ) debug()<<"N_recTot " << N_recTot <<endmsg;

    unsigned int i64 = i+64;
    N_expTot+=N_exp[i64];
    N_recTot+=N_rec[i64];
    N_expTot+=N_exp[i];
    N_recTot+=N_rec[i];
    
    if( msgLevel(MSG::DEBUG) ) debug()<<i<<" N_rec "<<N_rec[i]<<" N_exp "<<N_exp[i]<<endmsg;
    if(N_exp[i]>=1){
      if( msgLevel(MSG::DEBUG)) {
        double pseudoEfficiency_sens = N_rec[i] / N_exp[i];
        debug()<<i<<" ==> pseudoefficiency "<<pseudoEfficiency_sens<<endmsg;
      }
      if(N_rec[i] < N_exp[i]){
        const DeVeloSensor* sensor_h = m_veloDet->sensor(i); 
        Gaudi::XYZPoint trackInterceptGlobal_h= extrapolateToZ(&track, sensor_h->z());
        //        Gaudi::XYZPoint trackInterceptHalf_h = sensor_h->globalToVeloHalfBox ( trackInterceptGlobal_h );
        Gaudi::XYZPoint trackInterceptLocal_h(0,0,0) ;      
        trackInterceptLocal_h    = sensor_h -> globalToLocal( trackInterceptGlobal_h );
        //double rcoor_h = trackInterceptLocal_h.Rho();
        //double phicoor_h  = trackInterceptHalf_h.Phi();  
      } 
    }
    if( msgLevel(MSG::DEBUG) ) {
      debug()<<i64<<" N_rec "<<N_rec[i64]<<" N_exp "<<N_exp[i64]<<endmsg;
      double pseudoEfficiency_sens = N_rec[i64] / N_exp[i64];
      debug()<<i64<<" ==> pseudoefficiency "<<pseudoEfficiency_sens<<endmsg;
    }
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
    double chi2=track.chi2();
    double theta = track.firstState().slopes().theta();
    double slx = track.firstState().tx();
    double sly = track.firstState().ty();
    double x0 = track.firstState().x();
    double y0 = track.firstState().y();
    double z0 = track.firstState().z();
    double ndof=track.nDoF();
 
    tuple->column( "run",m_runodin);
    tuple->column( "evt",m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "ndof", ndof);
    tuple->column( "chi2", chi2);
    tuple->column( "theta", theta);
    tuple->column( "slx", slx);
    tuple->column( "sly", sly);
    tuple->column( "x0",      x0);
    tuple->column( "y0",      y0);
    tuple->column( "z0",      z0);
    tuple->column( "vx",      slx*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+x0);
    tuple->column( "vy",      sly*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+y0);
    tuple->column( "vz",      -(slx*x0+sly*y0)/(slx*slx+sly*sly));
    tuple->column( "nhits", nVeloHits);
    tuple->column( "nexpecthits", nExpectedHits);
    tuple->column( "adcpertrack", m_adcpertrack);
    tuple->column( "side",    m_side);
    tuple->column( "Eff", pseudoEff);
    tuple->column( "EffInterp", pseudoEfficiencyInterp);
    tuple->write();
  }
  if( msgLevel(MSG::DEBUG) ) debug() << "Filled Track information ntuple " << endmsg;
}

//=============================================================================
void Velo::VeloTrackMonitorNT::FillVeloEvNtuple(LHCb::Tracks* tracks)
{
  Tuple tuple=nTuple("VeloTrEvNtuple", "Event ntuple",CLID_ColumnWiseTuple );

  if( msgLevel(MSG::DEBUG) ) debug() << "Event information ntuple " << endmsg;

  int lefttracknum=0,righttracknum=0,troverlapnum=0;
  int tottrcl=0, tottrrcl=0;
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
        if (sensor->isLeft()) 
          m_sideLeft=1;
        else
          m_sideRight=1;
      }
    }
    if (m_sideRight==1 && m_sideLeft==1)
      troverlapnum+=1;
    else if (m_sideRight==1)
      righttracknum+=1;
    else 
      lefttracknum+=1;
  }
  
  int totcl= m_rawClusters->size();
  int totrcl=0;
  for(LHCb::VeloClusters::const_iterator cluIt=m_rawClusters->begin(); 
      cluIt!=m_rawClusters->end(); cluIt++){
    if ((*cluIt)->isRType())
      totrcl+=1;
  }

  tuple->column( "run",m_runodin);
  tuple->column( "evt",m_eventodin);
  tuple->column( "bunchid",m_bunchid);
  tuple->column( "trnum", tracks->size());
  tuple->column( "lefttrnum",  lefttracknum);
  tuple->column( "righttrnum",  righttracknum);
  tuple->column( "overlaptrnum",  troverlapnum );   
  tuple->column( "totcl", totcl);
  tuple->column( "totrcl", totrcl);
  tuple->column( "tottrcl", tottrcl);
  tuple->column( "tottrrcl", tottrrcl);
  tuple->write();

  if( msgLevel(MSG::DEBUG) ) debug() << "Filled Event information ntuple " << endmsg;

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

double VeloTrackMonitorNT::projAngleR(const Direction& locDirection,
                                      const Gaudi::XYZPoint& aLocPoint)
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " --> projAngleR() " <<endmsg;
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

double VeloTrackMonitorNT::projAnglePhi(const Direction& locDirection,
                                        const DeVeloPhiType* phiSensor,
                                        unsigned int centreStrip)
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " --> projAnglePhi() " <<endmsg;
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

Direction VeloTrackMonitorNT::localTrackDirection(const Gaudi::XYZVector& gloTrackDir,
                                                  const DeVeloSensor* sensor) const
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " --> localTrackDirection() " <<endmsg;
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

double VeloTrackMonitorNT::angleOfInsertion(const Direction& localSlopes,
                                            Gaudi::XYZVector& parallel2Track) const
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " --> angleOfInsertion() " <<endmsg;
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

const LHCb::MCHit* VeloTrackMonitorNT::getAssocMCHit(const LHCb::VeloCluster* clus) const
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " --> getAssocMCHit() " <<endmsg;
  //
  Range range=m_asctTable->relations(clus);
  if(range.begin()==range.end()){
    if( msgLevel(MSG::DEBUG) ) debug()<< " No MCHits associated " <<endmsg;
    return ( 0 );
  }
  const MCHit* aHit=(range.begin())->to();

  return ( aHit );
}

//============================================================================
VeloChannelID VeloTrackMonitorNT::weightedMean(const VeloCluster* cluster, double& isp)
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
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
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<< " int distance: " << intDistance <<endmsg;
    centre+=(static_cast<float>(intDistance+100))*
            cluster->stripValues()[strip].second;
    totalCharge+=cluster->stripValues()[strip].second;
  }
  centre/=totalCharge;
  centre-=100;
  intDistance=static_cast<int>(LHCb::Math::round(centre));
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
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

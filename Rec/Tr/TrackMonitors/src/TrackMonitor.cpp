// Include files 
#include "TrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/VeloCluster.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VPMeasurement.h"
#include "Event/KalmanFitResult.h"
#include "Kernel/HitPattern.h"
#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

// Det
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "OTDet/DeOTDetector.h"

#include "VeloDet/DeVelo.h"
#include "Event/FitNode.h"
#include "Event/RecVertex.h"

// gsl
#include "gsl/gsl_math.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"

#include <map>

#include "Map.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;


DECLARE_ALGORITHM_FACTORY( TrackMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitor::TrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator )
{
  // FIXME: remove this unused flag
  declareProperty( "UseUT"       , m_useUT =  false );
  declareProperty( "MaxMomentum", m_maxMomentum = 100. ) ;
  declareProperty( "MaxChi2Dof", m_maxChi2Dof = 5. ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitor::~TrackMonitor() {}

StatusCode TrackMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackMonitorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_veloDet = getDetIfExists<DeVelo>(  DeVeloLocation::Default ) ;
  m_veloExpectation = m_veloDet ? tool<IVeloExpectation>("VeloExpectation") : 0 ;

  m_ttExpectation = 
    getDetIfExists<DeSTDetector>(DeSTDetLocation::location("TT")) ? tool<IHitExpectation>("TTHitExpectation") : 
    (getDetIfExists<DeSTDetector>(DeSTDetLocation::location("UT")) ? tool<IHitExpectation>("UTHitExpectation") : 0);
  m_itExpectation = getDetIfExists<DeSTDetector>(DeSTDetLocation::location("IT")) ? tool<IHitExpectation>("ITHitExpectation") : 0;
  m_otExpectation = getDetIfExists<DeOTDetector>(DeOTDetectorLocation::Default) ? tool<IHitExpectation>("OTHitExpectation") : 0;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackMonitor::execute()
{
  
  // get the input data
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(inputContainer());

  for(MultiplicityMap::iterator it = m_multiplicityMap.begin() ;
      it != m_multiplicityMap.end(); ++it) it->second = 0 ;
  
  // # number of tracks
  plot(tracks.size(),1, "# tracks", 0., 500., 50);
  plot(tracks.size(),"TrackMultiplicityFine", "# tracks", 0.0, 2000., 200);
  
  // histograms per track
  BOOST_FOREACH( const LHCb::Track* track, tracks) {
    std::string type = histoDirName(*track) ;
    m_multiplicityMap[type] += 1;
    fillHistograms(*track,type);
    plot(track->type(),2, "track type" ,-0.5, 10.5, 11);
    plot(track->history(),"history" ,"track history",-0.5, 23.5, 24);
  }

  // fill counters....
  counter("#Tracks") += tracks.size();
  for(MultiplicityMap::const_iterator it = m_multiplicityMap.begin() ;
      it != m_multiplicityMap.end(); ++it) {
    counter("#"+it->first) += it->second;
    plot( it->second, it->first + "/multiplicity", 
          it->first + " multiplicity", -1.,201.,101) ;
  } // iterS
  
  return StatusCode::SUCCESS;
}

void TrackMonitor::findRefStates(const LHCb::Track& track,
                                 const LHCb::State*& firstMeasurementState,
                                 const LHCb::State*& lastMeasurementState) const
{
  firstMeasurementState = lastMeasurementState = 0  ;
  LHCb::Track::ConstNodeRange nodes = track.nodes() ;
  for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
       inode != nodes.end() ; ++inode) 
    if( (*inode)->type()==LHCb::Node::HitOnTrack ) {
      if( firstMeasurementState==0 ||
          (*inode)->z() < firstMeasurementState->z() )
        firstMeasurementState = &((*inode)->state()) ;
      if( lastMeasurementState==0 ||
          (*inode)->z() > lastMeasurementState->z() )
        lastMeasurementState = &((*inode)->state()) ;
    }
}

namespace {
  enum HitType {VeloR=0, VeloPhi, VPX, VPY, TT, IT, OT, Muon, HitTypeUnknown} ;
  const std::string HitTypeName[] = {"VeloR","VeloPhi","VPX","VPY","TT","IT","OT","Muon"} ;
  double      HitTypeMaxRes[] = {0.1,0.1,0.1,0.1,0.5,0.5,2.0,10} ;
  
  inline HitType hittypemap( const LHCb::Measurement& meas ) {
    LHCb::Measurement::Type type = meas.type() ;

    HitType rc = HitTypeUnknown ;
    switch( type ) {
    case LHCb::Measurement::Calo:
    case LHCb::Measurement::Origin :
    case LHCb::Measurement::Unknown: rc = HitTypeUnknown ; break ;
      
    case LHCb::Measurement::VP:
      {
        const LHCb::VPMeasurement* vpmeas = dynamic_cast<const LHCb::VPMeasurement*>(&meas);
        rc = vpmeas && vpmeas->projection() == LHCb::VPMeasurement::X ? VPX : VPY;
      }
      break;
    case LHCb::Measurement::VeloLiteR:
    case LHCb::Measurement::VeloR:   rc = VeloR ; break ;

    case LHCb::Measurement::VeloLitePhi:
    case LHCb::Measurement::VeloPhi: rc = VeloPhi ; break ;

    case LHCb::Measurement::UT :
    case LHCb::Measurement::UTLite :
    case LHCb::Measurement::TTLite:
    case LHCb::Measurement::TT: rc = TT; break;

    case LHCb::Measurement::FT :
    case LHCb::Measurement::ITLite:
    case LHCb::Measurement::IT: rc = IT; break;

    case LHCb::Measurement::OT: rc = OT; break;

    case LHCb::Measurement::Muon: rc = Muon; break;
    }
    return rc;
  }
}


void TrackMonitor::fillHistograms(const LHCb::Track& track, 
                                  const std::string& type) const
{
  // plots we should always make...
  plot(track.probChi2(),type+"/2","probChi2",0, 1, 50);
  plot(track.chi2PerDoF(),type+"/3","chi2/ndof",0,m_maxChi2Dof);
  plot(track.nLHCbIDs(),type+"/4","#nLHCbIDs", -0.5, 60.5, 61);
  plot(track.pseudoRapidity(),type+"/7", "eta", 0.95 , 6.05, 50);
  plot(track.phi(),type+"/8", "phi",-M_PI,M_PI,50) ;
  plot(track.nDoF(),type+"/ndof","ndof",-0.5,50.5,51);
  plot(track.flag(),type+"/flag","flag",-0.5,255.5,256) ;
  plot(track.history(),type+"/history","history",-0.5,23.5,24) ;
  plot(track.fitStatus(),type+"/fitstatus","fit status",-0.5,5.5,6) ;
  plot(track.nMeasurements(),type+"/100","#nMeas",  -0.5, 60., 61);

  const LHCb::State& firststate = track.firstState() ;
  plot(firststate.x(),type + "/120","x of first state",-100,100) ;
  plot(firststate.y(),type + "/121","y of first state",-100,100) ;
  plot(firststate.z(),type + "/122","z of first state",-500,500) ;
  plot(firststate.tx(),type + "/123","tx of first state",-1.0,1.0) ;
  plot(firststate.ty(),type + "/124","ty of first state",-1.0,1.0) ;
  plot(firststate.qOverP(),type + "/125","q/p of first state",-0.001,0.001) ; 

  if ( firststate.tx() != 0 || firststate.ty() != 0 ) {
    const TrackVector& vec = firststate.stateVector();
    double z = firststate.z();
    z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
    plot( z, type + "/126","z closest to z-axis",-2000,2000) ;
  }

  if( firststate.qOverP()!=0 ) {
    plot(track.p()/Gaudi::Units::GeV, type+"/5" ,"momentum", 0., m_maxMomentum, 100);
    plot(track.pt()/Gaudi::Units::GeV,type+"/6", "pt", 0, 10, 100);
  }

  if(track.fitResult()) {
    plot( track.fitResult()->nIter(), type + "/numiter","number of fit iterations",-0.5,10.5,11) ;
    plot( track.fitResult()->pScatter(), type + "/pscatter","momentum used for material corrections",0,100*Gaudi::Units::GeV) ;
  }

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nTTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isTT,_1));
  const unsigned int nUTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isUT,_1));
  const unsigned int nITHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isIT,_1));
  const unsigned int nOTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isOT,_1));
  const unsigned int nVeloHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVelo,_1));
  const unsigned int nVeloRHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVeloR,_1));
  const unsigned int nVeloPhiHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVeloPhi,_1));
  const unsigned int nMuonHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isMuon,_1));
  
  plot(nTTHits, type+"/110", "# TT hits",  -0.5, 10.5 ,11);
  plot(nITHits, type+"/111", "# IT hits",  -0.5, 50.5 ,51);
  plot(nOTHits, type+"/112", "# OT hits", -0.5, 50.5 ,51);
  plot(nVeloHits, type+"/113","# Velo hits" ,-0.5, 50.5 ,51);
  plot(nVeloRHits, type+"/114","# Velo R hits" ,-0.5, 20.5 ,21);
  plot(nVeloPhiHits, type+"/115","# Velo phi hits" ,-0.5, 20.5 ,21);
  plot(nMuonHits, type+"/116","# Muon hits" ,-0.5, 20.5 ,21);
  plot(nUTHits, type+"/117", "# UT hits",  -0.5, 10.5 ,11);
  
  size_t numoutliers(0) ;
  HitType mtype = VeloR; // initialize to avoid compiler warning
  if( track.nodes().size()>0 ) {
    LHCb::Track::ConstNodeRange nodes = track.nodes() ;
    for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin() ;
         inode != nodes.end(); ++inode) 
      if( (*inode)->type() == LHCb::Node::HitOnTrack 
          // discard extremely small fraction of hits with zero error
          // on residual. (e.g. a downstream track with only one
          // active TT hit)
          && (*inode)->errResidual2() > TrackParameters::lowTolerance 
          && (mtype = hittypemap( (*inode)->measurement() ) )!=HitTypeUnknown )  {
	
        const std::string& name = HitTypeName[ mtype ] ;
        const double resmax    = HitTypeMaxRes[ mtype ] ;
	
        // factor for unbiasing the rms (not the mean!)
        double f = std::sqrt( (*inode)->errMeasure2()/(*inode)->errResidual2()) ;
        plot(f*(*inode)->residual(),
             type+"/"+name+"Residual",name+" residual (rms-unbiased)",
             -resmax,resmax,50);
        plot((*inode)->residual()/(*inode)->errResidual(),
             type+"/"+name+"residualPull",name+" residual pull",-5,5,50);
        // these should be expert plots because 2D
        if((mtype==VeloR || mtype==VeloPhi) && fullDetail()) {
          // calculate R in the local frame
          Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
          Gaudi::XYZPoint localPoint = 
            (*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint ) ;
          double r = localPoint.Rho() ;
          // factor to calculate residual in detector plane
          const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(*inode) ;
          double cosalpha(1) ;
          if(fitnode) {
            // this vector is along residual, perp to strips
            Gaudi::XYZVector localUnitPocaVector = 
              (*inode)->measurement().detectorElement()->geometry()->toLocal( fitnode->pocaVector() ) ;
            cosalpha = localUnitPocaVector.Rho()/localUnitPocaVector.R() ;
          }
          plot2D(r,(*inode)->residual()*f/cosalpha,
                 type+"/"+name+"residualVsR",name+" residual vs R",
                 10,42,-resmax,resmax, 16, 50);

          if( m_veloDet) {
            // now get the pitch ...
            double pitch(1) ;
            if( (*inode)->measurement().type() == LHCb::Measurement::VeloPhi ||
                (*inode)->measurement().type() == LHCb::Measurement::VeloLitePhi) {
              const LHCb::VeloCluster* clus = 
                static_cast<LHCb::VeloPhiMeasurement&>((*inode)->measurement()).cluster() ;
              const DeVeloPhiType* phiDet = m_veloDet->phiSensor( clus->channelID().sensor() );
              pitch = r * phiDet->phiPitch( unsigned(clus->strip(0)) ) ;
            } else {
              const LHCb::VeloCluster* clus = 
                static_cast<LHCb::VeloRMeasurement&>((*inode)->measurement()).cluster() ;
              const DeVeloRType* rDet = m_veloDet->rSensor( clus->channelID().sensor() );
              pitch = rDet->rPitch( unsigned(clus->strip(0)) ) ;
            }
            plot2D(pitch,(*inode)->residual()*f/cosalpha,
                   type+"/"+name+"residualVsPitch",name+" residual vs pitch",
                   0.04,0.100,-resmax,resmax, 12, 50);
          }
        }
      } else if( (*inode)->type() == LHCb::Node::Outlier ) {
        ++numoutliers ;
      }
  }
  plot(numoutliers,type+"/101","#outliers", -0.5, 10.5, 11);

  const LHCb::KalmanFitResult* kalfit = dynamic_cast<const LHCb::KalmanFitResult*>(track.fitResult()) ;
  if(kalfit) {
    LHCb::ChiSquare tmp;
    if( (tmp=kalfit->chi2Velo()).nDoF() > 0 ) 
      plot(tmp.chi2PerDoF(),type+"/chi2PerDofVelo","chi/dof for velo segment",0,m_maxChi2Dof) ;
    if( (tmp=kalfit->chi2Downstream()).nDoF() > 0 ) 
      plot(tmp.chi2PerDoF(),type+"/chi2PerDofDownstream","chi/dof for T(Muon) segment",0,m_maxChi2Dof) ;
    if( (tmp=kalfit->chi2Match()).nDoF() > 0 )
      plot(tmp.chi2PerDoF(),type+"/chi2PerDofMatch","chi/dof upstream-downstream match",0,m_maxChi2Dof) ;
    if( (tmp=kalfit->chi2Muon()).nDoF() > 0 ) 
      plot(tmp.chi2PerDoF(),type+"/chi2PerDofMuon","chi/dof for muon segment",0,m_maxChi2Dof) ; 
    const double mom = track.p()/Gaudi::Units::GeV ;
    const double phi = track.phi() ;
    if( kalfit->chi2Velo().nDoF() > 0 ) {
      profile1D( mom, kalfit->chi2Velo().prob(), type+"/chi2ProbVeloVsMom",
                 "chi2 prob for velo segment versus momentum",0,m_maxMomentum,50) ;
      profile1D( phi, kalfit->chi2Velo().prob(), type+"/chi2ProbVeloVsPhi",
                 "chi2 prob for velo segment versus phi",-M_PI,M_PI,50) ;
    }
    if( kalfit->chi2Downstream().nDoF() > 0 ) {
      const LHCb::State* Tstate = track.stateAt( LHCb::State::AtT ) ;
      const double phiT = Tstate ? std::atan2(Tstate->y(),Tstate->x()) : phi ;
      profile1D( mom, kalfit->chi2Downstream().prob(), type+"/chi2ProbDownstreamVsMom",
                 "chi2 prob for T(muon) segment versus momentum",0,m_maxMomentum,50) ;
      profile1D( phiT, kalfit->chi2Downstream().prob(), type+"/chi2ProbDownstreamVsPhi",
                 "chi2 prob for T(muon) segment versus phi",-M_PI,M_PI,50) ;
    }
    if( kalfit->chi2Match().nDoF() > 0 ) {
      profile1D(mom, kalfit->chi2Match().prob(),type+"/chi2ProbMatchVsMom",
                "chi2 prob upstream-downstream match versus momentum",0,m_maxMomentum,50) ;
      profile1D(phi, kalfit->chi2Match().prob(),type+"/chi2ProbMatchVsPhi",
                "chi2 prob upstream-downstream match versus phi",-M_PI,M_PI,50) ;
    }
    if( kalfit->chi2().nDoF() > 0 ) {
      profile1D(mom, kalfit->chi2().prob(),type+"/chi2ProbVsMom","chi2 prob versus momentum",0,m_maxMomentum,50) ;
      profile1D(track.pseudoRapidity(), kalfit->chi2().prob(),type+"/chi2ProbVsEta","chi2 prob versus eta",2,5,30) ;
      profile1D(phi, kalfit->chi2().prob(),type+"/chi2ProbVsPhi","chi2 prob versus phi",-M_PI,M_PI,50) ;
    }
    
  }
  
  // expert checks  
  if (fullDetail() == true){

    static const double halfOverLog10 = 0.5/std::log(10.0) ;
    // find first and last node with measurement
    // First locate the first and last node that actually have information
    const LHCb::State *firstMState(0), *lastMState(0) ;
    findRefStates(track,firstMState,lastMState) ;
    if( firstMState ) {
      plot(log(firstMState->covariance()(0,0))*halfOverLog10,
           type+"/xerrorAtFirst", "10log(x error) at first measurement",-3,2);
      plot(log(firstMState->covariance()(1,1))*halfOverLog10,
           type+"/yerrorAtFirst", "10log(y error) at first measurement",-3,2);
      plot(log(firstMState->covariance()(2,2))*halfOverLog10,
           type+"/txerrorAtFirst", "10log(tx error) at first measurement",-7,0);
      plot(log(firstMState->covariance()(3,3))*halfOverLog10,
           type+"/tyerrorAtFirst", "10log(ty error) at first measurement",-7,0);
      plot(log(firstMState->covariance()(4,4))*halfOverLog10,
           type+"/qoperrorAtFirst", "10log(qop error) at first measurement",-8,0);
    }
    if( lastMState ) {
      plot(log(lastMState->covariance()(0,0))*halfOverLog10,
           type+"/xerrorAtLast", "10log(x error) at last measurement",-3,2);
      plot(log(lastMState->covariance()(1,1))*halfOverLog10,
           type+"/yerrorAtLast", "10log(y error) at last measurement",-3,2);
      plot(log(lastMState->covariance()(2,2))*halfOverLog10,
           type+"/txerrorAtLast", "10log(tx error) at last measurement",-7,0);
      plot(log(lastMState->covariance()(3,3))*halfOverLog10,
           type+"/tyerrorAtLast", "10log(ty error) at last measurement",-7,0);
      plot(log(lastMState->covariance()(4,4))*halfOverLog10,
           type+"/qoperrorAtLast", "10log(qop error) at last measurement",-8,0);
    }
    
 
    if( track.type() == LHCb::Track::Long ) {
      std::vector<LHCb::LHCbID > ids;
      IVeloExpectation::VeloPattern velo[4];
      if(m_veloExpectation) m_veloExpectation->expectedInfo(track, velo);
      if(m_ttExpectation) m_ttExpectation->collect(track, ids);
      if(m_itExpectation) m_itExpectation->collect(track, ids);
      if(m_otExpectation) m_otExpectation->collect(track, ids);
      
      LHCb::HitPattern expHitPattern = LHCb::HitPattern(ids);
      expHitPattern.setVeloRA(velo[0]);
      expHitPattern.setVeloRC(velo[1]); 
      expHitPattern.setVeloPhiA(velo[2]);
      expHitPattern.setVeloPhiC(velo[3]);


      // compare to what we expected
      if (expHitPattern.numOTHits() > 0 ){
        plot(nOTHits  - expHitPattern.numOTHits(), type+"/OTmissed", "# OT missed",  -10.5, 10.5 ,21);
      }
      
      // compare to what we expected
      if (expHitPattern.numITHits() > 0){
        plot(nITHits  - expHitPattern.numITHits(), type+"/ITmissed", "# IT missed",  -10.5, 10.5 ,21);
      }
      
      // compare to what we expected
      if (expHitPattern.numTTHits() > 0){
        plot(nTTHits - expHitPattern.numTTHits(), type+"/TTmissed","# TT missed" , -10.5, 10.5 ,21);
      }
      
      // compare to what we expected
      if (expHitPattern.numVeloR() + expHitPattern.numVeloPhi() > 0){
        plot(nVeloHits - expHitPattern.numVeloR() - expHitPattern.numVeloPhi(), 
             type+"/Velomissed","# Velo missed" ,-10.5, 10.5 ,21);
      }
    }

    const LHCb::Track::ExtraInfo& info = track.extraInfo();
    LHCb::Track::ExtraInfo::const_iterator iterInfo = info.begin();
    for (;iterInfo != info.end(); ++iterInfo ){
      LHCb::Track::AdditionalInfo infoName = 
        LHCb::Track::AdditionalInfo(iterInfo->first);
      std::string title = Gaudi::Utils::toString(infoName);
      const TrackMonitorMaps::InfoHistMap& histMap = TrackMonitorMaps::infoHistDescription();
      TrackMonitorMaps::InfoHistMap::const_iterator iterM = histMap.find(infoName);
      if (iterM != histMap.end()) {
        const TrackMonitorMaps::HistoRange range = histMap.find(infoName)->second;
        plot(iterInfo->second,type+"/info/"+range.fid, title,range.fxMin ,range.fxMax , 100);
      }
    } // iterInfo


    const LHCb::RecVertices* pvcontainer = 
      getIfExists<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary) ;
    if ( NULL != pvcontainer ) {
      for( LHCb::RecVertices::const_iterator ipv = pvcontainer->begin() ;
           ipv != pvcontainer->end(); ++ipv ) {
        // Note: this is all already done in trackvertexmonitor!
        const LHCb::State* aState = track.stateAt( LHCb::State::ClosestToBeam ) ;
        if(!aState) aState = &(track.firstState()) ;
        double dz = (*ipv)->position().z() - aState->z() ;
        double dx  = aState->x() + dz * aState->tx() - (*ipv)->position().x();
        double dy  = aState->y() + dz * aState->ty() - (*ipv)->position().y();
        plot( dx,type+"/IPx","IPx", -1.0, 1.0, 100);
        plot( dy,type+"/IPy","IPy", -1.0, 1.0, 100);
      }
    }
  }
  
  LHCb::HitPattern hitpattern( track.lhcbIDs()) ;
  plot( hitpattern.numVeloStations(),
        type+"/NumVeloStations", "Number of traversed stations in Velo", -0.5,21.5, 22) ;
  plot( hitpattern.numVeloStationsOverlap(), 
        type+"/NumVeloOverlapStations", "Number of traversed overlaps in Velo", -0.5,21.5, 22) ;
  plot( hitpattern.numITStationsOverlap(), 
        type+"/NumITOverlapStations", "Number of traversed overlaps in IT", -0.5,3.5,4) ;
  plot( hitpattern.numITOTStationsOverlap(), 
        type+"/NumITOTOverlapStations", "Number of T stations with both OT and IT", -0.5,3.5,4) ;
  plot( hitpattern.numVeloHoles(), 
        type+"/NumVeloHoles", "Number of missing velo layers", -0.5,10.5,11) ;
  plot( hitpattern.numTHoles(), 
        type+"/NumTHoles", "Number of missing T layers", -0.5,10.5,11) ;

  std::bitset<LHCb::HitPattern::NumT> it = hitpattern.it() ;
  for(size_t ilay=0; ilay<LHCb::HitPattern::NumT; ++ilay)
    profile1D( double(ilay), it.test(ilay),type+"/HitITLayers", "Hits per IT layer", -0.5,11.5,12) ;
  
  std::bitset<LHCb::HitPattern::NumT> ot = hitpattern.ot() ;
  for(size_t ilay=0; ilay<LHCb::HitPattern::NumT; ++ilay)
    profile1D( double(ilay), ot.test(ilay),type+"/HitOTLayers", "Hits per OT layer", -0.5,11.5,12) ;
  
  std::bitset<LHCb::HitPattern::NumTT> tt = hitpattern.tt() ;
  for(size_t ilay=0; ilay<LHCb::HitPattern::NumTT; ++ilay)
    profile1D( double(ilay),tt.test(ilay),type+"/HitTTLayers", "Hits per TT layer", -0.5,3.5,4) ;
  
  std::bitset<LHCb::HitPattern::NumVelo> velo = hitpattern.velo() ;
  for(size_t ilay=0; ilay<LHCb::HitPattern::NumVelo; ++ilay)
    profile1D( double(ilay),velo.test(ilay),type+"/HitVeloLayers", "Hits per Velo layer", -0.5,22.5,23) ;
  
}

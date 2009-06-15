// $Id: TrackMonitor.cpp,v 1.14 2009-06-15 12:55:21 wouter Exp $
// Include files 
#include "TrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/VeloCluster.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Kernel/HitPattern.h"

// Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "VeloDet/DeVelo.h"
#include "Event/FitNode.h"

// gsl
#include "gsl/gsl_math.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"

#include <map>

#include "Map.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;


DECLARE_ALGORITHM_FACTORY( TrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitor::TrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator )
{
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitor::~TrackMonitor() {}; 

StatusCode TrackMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackMonitorBase::initialize();
  if ( sc.isFailure() ) { return sc; }
  m_veloDet = getDet<DeVelo>(  DeVeloLocation::Default ) ;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackMonitor::execute()
{
  
  // get the input data
  if (!exist<LHCb::Tracks>(inputContainer())) 
    return Warning( inputContainer()+" not found", StatusCode::SUCCESS, 0);
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0., 500., 50);
  plot(tracks->size(),"TrackMultiplicityFine", "# tracks", -0.5, 50.5, 51);
  
  // histograms per track
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  
  for (; iterT != tracks->end(); ++iterT){
    if (/*selector((*iterT)->type())->accept(**iterT) ==*/ true){
      type = all() ;
      if( splitByType() ) {
	type = Gaudi::Utils::toString((*iterT)->type()) ;
	if( (*iterT)->checkFlag( LHCb::Track::Backward ) ) type += "Backward" ;
      } else if( splitByAlgorithm() ) {
	type = Gaudi::Utils::toString((*iterT)->history()) ;
      } 
      
      tMap[type]+= 1;
      fillHistograms(**iterT,type);
      plot((*iterT)->type(),2, "type" ,-0.5, 10.5, 11);
    }
  } // iterT

  // fill counters....
  counter("#Tracks") += tracks->size();
  for (std::map<std::string,unsigned int>::const_iterator iterS = tMap.begin();
       iterS != tMap.end(); ++iterS){
    counter("#"+iterS->first) += iterS->second;
  } // iterS

  return StatusCode::SUCCESS;
};

void TrackMonitor::findRefStates(const LHCb::Track& track,
				 const LHCb::State*& firstMeasurementState,
				 const LHCb::State*& lastMeasurementState) const
{
  firstMeasurementState = lastMeasurementState = 0  ;
  for( LHCb::Track::NodeContainer::const_iterator inode = track.nodes().begin() ;
       inode != track.nodes().end() ; ++inode) 
    if( (*inode)->type()==LHCb::Node::HitOnTrack ) {
      if( firstMeasurementState==0 ||
	  (*inode)->z() < firstMeasurementState->z() )
	firstMeasurementState = &((*inode)->state()) ;
      if( lastMeasurementState==0 ||
	  (*inode)->z() > lastMeasurementState->z() )
	lastMeasurementState = &((*inode)->state()) ;
    }
}

void TrackMonitor::fillHistograms(const LHCb::Track& track, 
                                  const std::string& type) const
{
  // plots we should always make...
  plot(track.probChi2(),type+"/2","probChi2", -0.01, 1.01, 51);
  plot(track.chi2PerDoF(),type+"/3","chi2/ndof",0,20);
  plot(track.nLHCbIDs(),type+"/4","#nLHCbIDs", -0.5, 60.5, 61);
  plot(track.pseudoRapidity(),type+"/7", "eta", 0.95 , 6.05, 50);
  plot(track.nDoF(),type+"/ndof","ndof",-0.5,10.5,11);
  plot(track.flag(),type+"/flag","flag",-0.5,255.5,256) ;
  plot(track.history(),type+"/history","history",-0.5,20.5,21) ;
  plot(track.fitStatus(),type+"/fitstatus","fit status",-0.5,5.5,6) ;
  plot(track.nMeasurements(),type+"/100","#nMeas",  -0.5, 60., 61);
  const LHCb::State& firststate = track.firstState() ;
  plot(firststate.x(),type + "/120","x of first state",-100,100) ;
  plot(firststate.y(),type + "/121","y of first state",-100,100) ;
  plot(firststate.z(),type + "/122","z of first state",-500,500) ;
  plot(firststate.tx(),type + "/123","tx of first state",-1.0,1.0) ;
  plot(firststate.ty(),type + "/124","ty of first state",-1.0,1.0) ;
  plot(firststate.qOverP(),type + "/125","q/p of first state",-0.001,0.001) ; 

  if( firststate.qOverP()!=0 ) {
    plot(track.p()/Gaudi::Units::GeV, type+"/5" ,"momentum", -5., 205., 21);
    plot(track.pt()/Gaudi::Units::GeV,type+"/6", "pt", -0.1, 10.1, 51);
  }

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nTTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isTT,_1));
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
  
  size_t numoutliers(0) ;
  if( track.nodes().size()>0 ) {
    std::string names[] = { "VeloR","VeloPhi","VeloLiteR",
			    "VeloLitePhi","TT","IT","OT","Muon",
			    "TTLite","ITLite" } ;
    double resmax[] = { 0.1,0.1,0.1,0.1, //VeloR,VeloPhi,VeloLiteR.VeloLitePhi
			0.5,0.5,2.0,10,  // TT,IT,OT,Muon
			0.5,0.5 } ;      // TTLite,ITLite
    for( LHCb::Track::NodeContainer::const_iterator inode = track.nodes().begin() ;
	 inode != track.nodes().end(); ++inode) 
      if( (*inode)->type() == LHCb::Node::HitOnTrack 
	  // discard extremely small fraction of hits with zero error
	  // on residual. (e.g. a downstream track with only one
	  // active TT hit)
	  && (*inode)->errResidual2() > TrackParameters::lowTolerance ) {
	size_t mtype = (*inode)->measurement().type() - 1  ;
	// factor for unbiasing the rms (not the mean!)
	double f = std::sqrt( (*inode)->errMeasure2()/(*inode)->errResidual2()) ;
	plot(f*(*inode)->residual(),
	     type+"/"+names[mtype]+"Residual",names[mtype]+" residual (rms-unbiased)",
	     -resmax[mtype],resmax[mtype],50);
	plot((*inode)->residual()/(*inode)->errResidual(),
	     type+"/"+names[mtype]+"residualPull",names[mtype]+" residual pull",-5,5,50);
	// these should be expert plots because 2D
	if(mtype<=1 && fullDetail()) {
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
		 type+"/"+names[mtype]+"residualVsR",names[mtype]+" residual vs R",
		 10,42,-resmax[mtype],resmax[mtype], 16, 50);

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
		 type+"/"+names[mtype]+"residualVsPitch",names[mtype]+" residual vs pitch",
		 0.04,0.100,-resmax[mtype],resmax[mtype], 12, 50);
 	}
      } else if( (*inode)->type() == LHCb::Node::Outlier ) {
	++numoutliers ;
      }
  }
  plot(numoutliers,type+"/101","#outliers", -0.5, 10.5, 11);

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
    
    // compare to what we expected
    if (track.hasInfo(LHCb::Track::nExpectedOT) == true){
      plot(nOTHits  - track.info(LHCb::Track::nExpectedOT,9999.), type+"/120", "# OT missed",  -10.5, 10.5 ,21);
    }

    // compare to what we expected
    if (track.hasInfo(LHCb::Track::nExpectedIT) == true){
      plot(nITHits  - track.info(LHCb::Track::nExpectedIT, 9999.), type+"/121", "# IT missed",  -10.5, 10.5 ,21);
    }

    // compare to what we expected
    if (track.hasInfo(LHCb::Track::nExpectedTT) == true){
      plot(nTTHits - track.info(LHCb::Track::nExpectedTT, 9999.), type+"/122","# TT missed" , -10.5, 10.5 ,21);
    }

    // compare to what we expected
    if (track.hasInfo(LHCb::Track::nExpectedVelo) == true){
      plot(nVeloHits - track.info(LHCb::Track::nExpectedVelo, 9999.), type+"/123","# Velo missed" ,-10.5, 10.5 ,21);
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
  }
   
  LHCb::HitPattern hitpattern( track.lhcbIDs() ) ;
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
}

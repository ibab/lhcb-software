// $Id: TrackMonitor.cpp,v 1.3 2008-07-25 08:01:48 wouter Exp $
// Include files 
#include "TrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"

// Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
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
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 0.0);
 
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

  // guess that the size of the histograms at ref is ~ 0.35 by 0.3
  m_xMax = GSL_MAX(5.0, 0.35*m_refZ/Gaudi::Units::cm);
  m_yMax = GSL_MAX(5.0, 0.3*m_refZ/Gaudi::Units::cm);

  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackMonitor::execute()
{
  
  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0., 500., 50);

  // histograms per track
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  
  for (; iterT != tracks->end(); ++iterT){
    if (selector((*iterT)->type())->accept(**iterT) == true){
      type = all() ;
      if( splitByType() ) {
	type = Gaudi::Utils::toString((*iterT)->type()) ;
	if( (*iterT)->checkFlag( LHCb::Track::Backward ) ) type += "Backward" ;
      } else if( splitByAlgorithm() ) {
	type = Gaudi::Utils::toString((*iterT)->history()) ;
      } 
      
      tMap[type]+= 1;
      fillHistograms(**iterT,type);
      plot((*iterT)->type(),2, "type" ,-0.5, 10.5, 10);
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
  plot(track.p()/Gaudi::Units::GeV, type+"/5" ,"momentum", -5., 205., 21);
  plot(track.pt()/Gaudi::Units::GeV,type+"/6", "pt", -0.1, 10.1, 51);
  plot(track.pseudoRapidity(),type+"/7", "eta", 0.95 , 6.05, 50);
  plot(track.flag(),type+"/flag","flag",-0.5,255.5,256) ;
  plot(track.history(),type+"/history","history",-0.5,20.5,21) ;
  plot(track.fitStatus(),type+"/fitstatus","fit status",-0.5,5.5,6) ;
  plot(track.nMeasurements(),type+"/100","#nMeas",  -0.5, 60., 61);
  plot(track.nMeasurementsRemoved(),type+"/101","#outliers", -0.5, 10.5, 11);

  static const double halfOverLog10 = 0.5/std::log(10.0) ;
  // find first and last node with measurement
   // First locate the first and last node that actually have information
  const LHCb::State *firstMState(0), *lastMState(0) ;
  findRefStates(track,firstMState,lastMState) ;
  if( firstMState ) {
    plot(log(firstMState->covariance()(0,0))*halfOverLog10,type+"/xerrorAtFirst", "10log(x error) at first measurement",-3,2);
    plot(log(firstMState->covariance()(1,1))*halfOverLog10,type+"/yerrorAtFirst", "10log(y error) at first measurement",-3,2);
    plot(log(firstMState->covariance()(2,2))*halfOverLog10,type+"/txerrorAtFirst", "10log(tx error) at first measurement",-7,0);
    plot(log(firstMState->covariance()(3,3))*halfOverLog10,type+"/tyerrorAtFirst", "10log(ty error) at first measurement",-7,0);
    plot(log(firstMState->covariance()(4,4))*halfOverLog10,type+"/qoperrorAtFirst", "10log(qop error) at first measurement",-8,0);
  }
  if( lastMState ) {
    plot(log(lastMState->covariance()(0,0))*halfOverLog10,type+"/xerrorAtLast", "10log(x error) at last measurement",-3,2);
    plot(log(lastMState->covariance()(1,1))*halfOverLog10,type+"/yerrorAtLast", "10log(y error) at last measurement",-3,2);
    plot(log(lastMState->covariance()(2,2))*halfOverLog10,type+"/txerrorAtLast", "10log(tx error) at last measurement",-7,0);
    plot(log(lastMState->covariance()(3,3))*halfOverLog10,type+"/tyerrorAtLast", "10log(ty error) at last measurement",-7,0);
    plot(log(lastMState->covariance()(4,4))*halfOverLog10,type+"/qoperrorAtLast", "10log(qop error) at last measurement",-8,0);
  }

  // found hits of each type
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  const unsigned int nTTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isTT,_1));
  const unsigned int nITHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isIT,_1));
  const unsigned int nOTHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isOT,_1));
  const unsigned int nVeloHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVelo,_1));
  const unsigned int nVeloRHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVeloR,_1));
  const unsigned int nVeloPhiHits = std::count_if(ids.begin(), ids.end(),bind(&LHCbID::isVeloPhi,_1));
  
  plot(nTTHits, type+"/110", "# TT hits",  -0.5, 10.5 ,11);
  plot(nITHits, type+"/111", "# IT hits",  -0.5, 50.5 ,51);
  plot(nOTHits, type+"/112", "# OT hits", -0.5, 50.5 ,51);
  plot(nVeloHits, type+"/113","# Velo hits" ,-0.5, 50.5 ,51);
  plot(nVeloRHits, type+"/114","# Velo R hits" ,-0.5, 20.5 ,21);
  plot(nVeloPhiHits, type+"/115","# Velo phi hits" ,-0.5, 20.5 ,21);
  
  // expert checks  
  if (fullDetail() == true){

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


    // track parameters at some reference z
    LHCb::State aState;
    StatusCode sc = extrapolator()->propagate(track, m_refZ,aState );
    plot2D(aState.x()/Gaudi::Units::cm,
           aState.y()/Gaudi::Units::cm, type+"/200", "x vs y", 
           -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
    plot(aState.tx(),type+"/103","tx", -1.,1., 50);
    plot(aState.ty(),type+"/104", "ty", -1.,1., 50);

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
}


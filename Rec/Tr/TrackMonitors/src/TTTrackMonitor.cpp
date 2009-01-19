// $Id: TTTrackMonitor.cpp,v 1.1 2009-01-19 11:25:10 mneedham Exp $
// Include files 
#include "TTTrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Kernel/SiChargeFun.h"
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "Event/STMeasurement.h"
#include "Event/Measurement.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <map>
#include "Map.h"
#include "Kernel/ITNames.h"
#include "LoKi/select.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;


DECLARE_ALGORITHM_FACTORY( TTTrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TTTrackMonitor::TTTrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 2500.0); // midpoint of IT 
  declareProperty("minNumITHits", m_minNumTTHits = 2u); 
}

//=============================================================================
// Destructor
//=============================================================================
TTTrackMonitor::~TTTrackMonitor() {}; 

StatusCode TTTrackMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackMonitorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // guess that the size of the histograms at ref is ~ 0.35 by 0.3
  m_xMax = 0.35*m_refZ/Gaudi::Units::cm;
  m_yMax = 0.3*m_refZ/Gaudi::Units::cm;

  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TTTrackMonitor::execute()
{
  
  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0, 500, 100 );

  // histograms per track
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  
  for (; iterT != tracks->end(); ++iterT){
    if (selector((*iterT)->type())->accept(**iterT) == true){

      // find the IT hits on the track 
      const std::vector<LHCb::LHCbID>& ids = (*iterT)->lhcbIDs();
      std::vector<LHCb::LHCbID> ttHits; ttHits.reserve(ids.size());
      LoKi::select(ids.begin(), ids.end(), std::back_inserter(ttHits), bind(&LHCbID::isTT,_1));

      if (ids.size() < m_minNumTTHits) continue;

      type = all() ;
      if( splitByType() ) {
	type = LHCb::Track::TypesToString((*iterT)->type());
	if( (*iterT)->checkFlag( LHCb::Track::Backward ) ) type += "Backward" ;
      } else if( splitByAlgorithm() ) {
	type = LHCb::Track::HistoryToString((*iterT)->history()) ;
      } 
      
      fillHistograms(**iterT,ids);
    }
  } // iterT


  return StatusCode::SUCCESS;
};


void TTTrackMonitor::fillHistograms(const LHCb::Track& track, 
                                   const std::vector<LHCb::LHCbID>& itIds) const
{

  // track parameters at some reference z
  LHCb::State aState;
  StatusCode sc = extrapolator()->propagate(track, m_refZ,aState );
  plot2D(aState.x()/Gaudi::Units::cm,
         aState.y()/Gaudi::Units::cm, "xy", "x vs y", 
           -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
  plot(aState.tx(),"tx","tx", -0.2,0.2, 200);
  plot(aState.ty(),"ty", "ty", -0.2,0.2, 200);
  plot(track.p()/Gaudi::Units::GeV, "momentum" ,"momentum", -5., 205., 21); 
  plot(aState.x()/Gaudi::Units::cm, "xdist","x dist", -m_xMax, m_xMax);
  plot(aState.y()/Gaudi::Units::cm, "ydist", "y dist", -m_yMax, m_yMax);


  // Loop over the nodes to get the hits variables
  std::vector<LHCb::Node*>::const_iterator iNodes = track.nodes().begin();
  std::vector<const LHCb::STMeasurement*> measVector; measVector.reserve(24);

  unsigned int nHigh = 0u;
  for ( ; iNodes != track.nodes().end(); ++iNodes ) {

      // Only loop on hits with measurement that is IT type
      LHCb::FitNode* fNode = dynamic_cast<LHCb::FitNode*>(*iNodes);

      if ( fNode->hasMeasurement() == false ||  fNode->measurement().type() != LHCb::Measurement::TT) continue;
    
      // unbiased residuals and biased residuals
      LHCb::LHCbID nodeID = fNode->measurement().lhcbID();
      STChannelID chan = fNode->measurement().lhcbID().stID();
      plot(fNode->unbiasedResidual(),"unbiasedResidual","unbiasedResidual",  -2., 2., 200 );
      plot(fNode->residual(),"biasedResidual","biasedResidual",  -2., 2., 200 );
      
      // get the measurement and plot ST related quantities
      STMeasurement* hit = dynamic_cast<STMeasurement*>(&fNode->measurement());
      plot(hit->totalCharge(),"charge", "clusters charge", 0., 200., 100);
      plot(hit->size(), "size",  "cluster size", -0.5, 10.5, 11);
      if (hit->highThreshold() ) ++nHigh;
      measVector.push_back(hit);

  } // nodes

  // plot high fraction and also shorth
  plot(nHigh/double(itIds.size()),"highFrac", "high fraction", 0., 1., 100);
  const double shorth = SiChargeFun::shorth(measVector.begin(),measVector.end());
  plot(shorth,"shorth" ,"shorth charge", 0., 100., 200);
  const double gm = SiChargeFun::generalizedMean(measVector.begin(), measVector.end()); 
  plot(gm, "generalized mean","generalized mean charge",  0., 100, 200);


}


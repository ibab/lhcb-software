// $Id: ITTrackMonitor.cpp,v 1.4 2009-02-17 09:59:10 pkoppenb Exp $
// Include files 
#include "ITTrackMonitor.h"

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


DECLARE_ALGORITHM_FACTORY( ITTrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ITTrackMonitor::ITTrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 7500.0); // midpoint of IT
  declareProperty("splitByITType", m_splitByITType = true); 
  declareProperty("minNumITHits", m_minNumITHits = 6u); 
}

//=============================================================================
// Destructor
//=============================================================================
ITTrackMonitor::~ITTrackMonitor() {}; 

StatusCode ITTrackMonitor::initialize()
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
StatusCode ITTrackMonitor::execute()
{
  
  // get the input data
  if (!exist<LHCb::Tracks>(inputContainer())) 
    return Warning( inputContainer()+" not found", StatusCode::SUCCESS, 0);
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
      std::vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
      LoKi::select(ids.begin(), ids.end(), std::back_inserter(itHits), bind(&LHCbID::isIT,_1));

      if (ids.size() < m_minNumITHits) continue;

      type = all() ;
      if( splitByType() ) {
	type = LHCb::Track::TypesToString((*iterT)->type());
	if( (*iterT)->checkFlag( LHCb::Track::Backward ) ) type += "Backward" ;
      } else if( splitByAlgorithm() ) {
	type = LHCb::Track::HistoryToString((*iterT)->history()) ;
      } 
      
      fillHistograms(**iterT,type,ids);
    }
  } // iterT


  return StatusCode::SUCCESS;
};


void ITTrackMonitor::fillHistograms(const LHCb::Track& track, 
                                    const std::string& type, 
                                    const std::vector<LHCb::LHCbID>& itIds) const
{

  std::string ittype = type;
  if (splitByITType() == true){
    ittype += "/"+ITCategory(itIds);
  }

  // plots we should always make...
  plot(track.probChi2(),ittype+"/probChi2","probChi2", -0.0025, 1.0025, 201);
  plot(track.chi2PerDoF(),ittype+"/chi2PerDoF","chi2PerDoF",0,1000,500);
  plot(track.nLHCbIDs(),ittype+"/nLHCbIDs","#nLHCbIDs", -0.5, 60.5, 61);
  plot(track.nMeasurements(),ittype+"/nMeas","#nMeas",  -0.5, 60., 61);
  plot(track.nMeasurementsRemoved(),ittype+"/outliers","#outliers", -0.5, 10.5, 11);

  // track parameters at some reference z
  LHCb::State aState;
  StatusCode sc = extrapolator()->propagate(track, m_refZ,aState );
  plot2D(aState.x()/Gaudi::Units::cm,
         aState.y()/Gaudi::Units::cm, type+"/xy", "x vs y", 
           -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
  plot(aState.tx(),type+"/tx","tx", -0.2,0.2, 200);
  plot(aState.ty(),type+"/ty", "ty", -0.2,0.2, 200);
  plot(track.p()/Gaudi::Units::GeV, type+"/momentum" ,"momentum", -5., 205., 21); 
  plot(aState.x()/Gaudi::Units::cm, type+"/xdist","x dist", -m_xMax, m_xMax);
  plot(aState.y()/Gaudi::Units::cm, type+"/ydist", "y dist", -m_yMax, m_yMax);


  // Loop over the nodes to get the hits variables
  std::vector<LHCb::Node*>::const_iterator iNodes = track.nodes().begin();
  std::vector<const LHCb::STMeasurement*> measVector; measVector.reserve(24);

  unsigned int nHigh = 0u;
  for ( ; iNodes != track.nodes().end(); ++iNodes ) {

      // Only loop on hits with measurement that is IT type
      LHCb::FitNode* fNode = dynamic_cast<LHCb::FitNode*>(*iNodes);

      if ( fNode->hasMeasurement() == false ||  fNode->measurement().type() != LHCb::Measurement::IT) continue;
    
      // unbiased residuals and biased residuals
      LHCb::LHCbID nodeID = fNode->measurement().lhcbID();
      STChannelID chan = fNode->measurement().lhcbID().stID();
      plot(fNode->unbiasedResidual(),ittype+"/unbiasedResidual","unbiasedResidual",  -2., 2., 200 );
      plot(fNode->residual(),ittype+"/biasedResidual","biasedResidual",  -2., 2., 200 );
      
      // get the measurement and plot ST related quantities
      STMeasurement* hit = dynamic_cast<STMeasurement*>(&fNode->measurement());
      plot(hit->totalCharge(),ittype+"/charge", "clusters charge", 0., 200., 100);
      plot(hit->size(), ittype+"/size",  "cluster size", -0.5, 10.5, 11);
      if (hit->highThreshold() ) ++nHigh;
      measVector.push_back(hit);

  } // nodes

  // plot high fraction and also shorth
  plot(nHigh/double(itIds.size()), ittype+"/highFrac", "high fraction", 0., 1., 100);
  const double shorth = SiChargeFun::shorth(measVector.begin(),measVector.end());
  plot(shorth,ittype+"/shorth" ,"shorth charge", 0., 100., 200);
  const double gm = SiChargeFun::generalizedMean(measVector.begin(), measVector.end()); 
  plot(gm,ittype+"/generalized mean","generalized mean charge",  0., 100, 200);


}

std::string ITTrackMonitor::ITCategory(const std::vector<LHCb::LHCbID>& ids) const {
  
  typedef std::map<unsigned int, unsigned int> BoxMap;
  BoxMap nBox;
  BOOST_FOREACH(LHCb::LHCbID id, ids) {
    nBox[id.stID().detRegion()]  += 1;
  } // for each
  
  unsigned int bestBox = 0; unsigned int nBest =0;
  BOOST_FOREACH( BoxMap::value_type  box, nBox ) {
    if (box.second > nBest ){ 
      bestBox = box.first;
      nBest = box.second;
    }
  }   // for eachç

  return ITNames().BoxToString(bestBox);
}

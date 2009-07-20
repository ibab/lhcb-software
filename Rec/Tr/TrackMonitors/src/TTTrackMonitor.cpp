// $Id: TTTrackMonitor.cpp,v 1.4 2009-07-20 11:17:31 mneedham Exp $
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
  declareProperty("InputData" , m_clusterLocation = STClusterLocation::TTClusters);
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
  if (!exist<LHCb::Tracks>(inputContainer())) 
    return Warning(inputContainer()+" not found", StatusCode::SUCCESS, 0);    
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());
  
  // locate the cluster container
  const LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);

  std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0, 500, 100 );

  // tmp container for ids
  std::vector<unsigned int> usedIDs; usedIDs.reserve(clusters->size());

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

      // insert into tmp container
      BOOST_FOREACH( LHCb::LHCbID id, ids ){
        usedIDs.push_back(id.stID());
      } // for each
      
      fillHistograms(**iterT,ids);
    }
  } // iterT

  // see how many hits in the IT were actually used
  if (clusters->size() > 0u) {
    std::sort(usedIDs.begin(), usedIDs.end());
    std::unique(usedIDs.begin(), usedIDs.end());
    plot(usedIDs.size()/(double)clusters->size(), "fraction used", 0., 1., 200);
  }

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
      STMeasurement* hit = dynamic_cast<STMeasurement*>(&fNode->measurement());
      plot(fNode->unbiasedResidual(),"unbiasedResidual","unbiasedResidual",  -2., 2., 200 );
      plot(fNode->residual(),"biasedResidual","biasedResidual",  -2., 2., 200 );

         // 2D plots in full detail mode
      if (fullDetail() == true){
	const unsigned int bin = histoBin(chan);
        const std::string layerName = TTNames().UniqueLayerToString(chan);
        plot2D(bin, fNode->unbiasedResidual() , "unbiasedResSector"+layerName ,
               "unbiasedResSector"+layerName  , 99.5, 500.5, -2., 2.,401 , 200  );
        plot2D(bin, fNode->residual() , "biasedResSector"+layerName , 
               "/biasedResSector"+layerName  , 99.5, 500.5, -2., 2.,401 , 200  );

        const double signalToNoise = hit->totalCharge()/hit->sector().noise(chan);
        plot2D(bin, signalToNoise,"SNSector"+layerName ,"SNSector"+layerName  , 99.5, 500.5, -0.25, 100.25, 401, 201);
        plot2D(bin, hit->totalCharge(),"CSector"+layerName ,"CSector"+layerName  , 99.5, 500.5, -0.5, 200.5,401,201 );
      }
      
      // get the measurement and plot ST related quantities
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

unsigned int TTTrackMonitor::histoBin(const LHCb::STChannelID& chan) const {

  // convert layer and station to a flat number
  unsigned int layer;
  chan.station() == 1u ?layer = chan.layer() : layer = chan.layer() + 2;
  return layer * 100 + chan.sector();
}



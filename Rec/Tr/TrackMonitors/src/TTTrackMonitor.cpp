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

#include "GaudiKernel/PhysicalConstants.h"

#include <map>
#include "Map.h"
#include "Kernel/ITNames.h"
#include "LoKi/select.h"
#include "AIDA/IHistogram1D.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;


DECLARE_ALGORITHM_FACTORY( TTTrackMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TTTrackMonitor::TTTrackMonitor(const std::string& name,
                               ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 2500.0); // midpoint of IT 
  declareProperty("minNumTTHits", m_minNumTTHits = 2u); 
  declareProperty("InputData" , m_clusterLocation = STClusterLocation::TTClusters);
  declareProperty("plotsBySector", m_plotsBySector = false);// residual and signal to noise plots for each sector
  declareProperty("HitsOnTrack", m_hitsOnTrack = false);// use only hits on tracks (a la track monitor)
  setSplitByType(false) ;
  // dummy intialization, redone in initialize
  m_xMax = 0.;
  m_yMax = 0.;
}

//=============================================================================
// Destructor
//=============================================================================
TTTrackMonitor::~TTTrackMonitor() {}

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
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(inputContainer());
  
  // locate the cluster container
  const LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);

  std::map<std::string, unsigned int> tMap;

  // tmp container for ids
  std::vector<unsigned int> usedIDs; usedIDs.reserve(clusters->size());

  // histograms per track
  BOOST_FOREACH( const LHCb::Track* track, tracks) 
    if( track->hasTT()) {
    
      // find the IT hits on the track 
      const std::vector<LHCb::LHCbID>& ids = track->lhcbIDs();
      std::vector<LHCb::LHCbID> ttHits; ttHits.reserve(ids.size());
      LoKi::select(ids.begin(), ids.end(), std::back_inserter(ttHits), bind(&LHCbID::isTT,_1));
      
      if (ids.size() < m_minNumTTHits) continue;
      
      std::string type = histoDirName(*track) ;
      
      // insert into tmp container
      BOOST_FOREACH( LHCb::LHCbID id, ids ){
        usedIDs.push_back(id.stID());
      } // for each
      
      fillHistograms(*track,ids,type);
    }
  
  // see how many hits in the IT were actually used
  if (clusters->size() > 0u) {
    std::sort(usedIDs.begin(), usedIDs.end());
    std::unique(usedIDs.begin(), usedIDs.end());
    plot(usedIDs.size()/(double)clusters->size(), "fraction of used TT hits", 0., 1., 50);
  }

  return StatusCode::SUCCESS;
}

namespace
{
  size_t ttUniqueSectorID(const LHCb::STChannelID& id) {
    //std::cout << "tt: " << id.station() << " " << id.layer() << " " << id.detRegion() << " " << id.sector() << std::endl ;
    return (id.detRegion()-1)*24 + id.sector()-1 ;
  }
}

void TTTrackMonitor::fillHistograms(const LHCb::Track& track, 
                                    const std::vector<LHCb::LHCbID>& itIds, 
                                    const std::string& type) const
{

  // track parameters at some reference z
  LHCb::StateVector aState;
  StatusCode sc = extrapolator()->propagate(track, m_refZ,aState );
  if( fullDetail() ) {
    plot2D(aState.x()/Gaudi::Units::cm,
           aState.y()/Gaudi::Units::cm, "xy", "x vs y", 
           -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
  }
  plot(aState.tx(),"tx","tx", -0.2,0.2, 200);
  plot(aState.ty(),"ty", "ty", -0.2,0.2, 200);
  plot(track.p()/Gaudi::Units::GeV, "momentum" ,"momentum", -5., 205., 21); 
  plot(aState.x()/Gaudi::Units::cm, "xdist","x dist", -m_xMax, m_xMax);
  plot(aState.y()/Gaudi::Units::cm, "ydist", "y dist", -m_yMax, m_yMax);
  
  // Loop over the nodes to get the hits variables
  LHCb::Track::ConstNodeRange nodes = track.nodes() ;
  LHCb::Track::ConstNodeRange::const_iterator iNodes = nodes.begin();
  std::vector<const LHCb::STMeasurement*> measVector; measVector.reserve(24);
  std::vector< const LHCb::FitNode* > nodesByTTLayer[4] ;

  unsigned int nHigh = 0u;
  for ( ; iNodes != nodes.end(); ++iNodes ) {
    // Only loop on hits with measurement that is IT type
    const LHCb::FitNode* fNode = dynamic_cast<const LHCb::FitNode*>(*iNodes);
    
    if ( fNode->hasMeasurement() == false || 
	 !(fNode->measurement().type() == LHCb::Measurement::TT ||
	   fNode->measurement().type() == LHCb::Measurement::TTLite ) ) continue;
    if( m_hitsOnTrack && ( (*iNodes)->type() != LHCb::Node::HitOnTrack ) ) continue;
    if( (*iNodes)->errResidual2() <= TrackParameters::lowTolerance ) continue;
    
    // monitor overlapping tracks
    const LHCb::Measurement& measurement = (**iNodes).measurement();
    LHCb::LHCbID lhcbID = measurement.lhcbID();

    if(lhcbID.isTT()){
      // unbiased residuals and biased residuals
      STChannelID chan = lhcbID.stID();
      unsigned int uniquelayer = (chan.station()-1)*2 + chan.layer()-1 ;
      nodesByTTLayer[uniquelayer].push_back( fNode ) ;

      const std::string layerName = TTNames().UniqueLayerToString(chan);
      const STMeasurement* hit = dynamic_cast<const STMeasurement*>(&fNode->measurement());
      plot(fNode->unbiasedResidual(),"unbiasedResidual","unbiasedResidual",  -2., 2., 200 );
      plot(fNode->residual(),"biasedResidual","biasedResidual",  -2., 2., 200 );
 
      // rms unbiased residual. that's the one you want to look at.
      double residual = fNode->residual() * std::sqrt(fNode->errMeasure2()/fNode->errResidual2()) ;
      plot(residual, "Residual","Residual (rms-unbiased)",-0.5,0.5,100) ;
      plot(residual, layerName + "/Residual","Residual (rms-unbiased)",-0.5,0.5,100) ;
      if ( hit && m_plotsBySector ) {
        std::string sectorName = hit->cluster()->sectorName();
        plot(residual, "BySector/Residual_"+sectorName, "Residual (rms-unbiased)", -0.5, 0.5, 100);
        plot(fNode->unbiasedResidual(), "BySector/UnbiasedResidual_"+sectorName, "Residual (unbiased)", -0.5, 0.5, 100);
      }
      
      // 2D plots in full detail mode
      if( fullDetail() ) {
        const unsigned int bin = histoBin(chan);
        plot2D(bin, residual, "unbiasedResRMSCorrSector"+layerName ,
               "unbiasedResRMSCorrSector (rms-corrected)", 99.5, 500.5, -2., 2.,401 , 200  );
        plot2D(bin, fNode->unbiasedResidual() , "unbiasedResSector"+layerName ,
               "unbiasedResSector"+layerName  , 99.5, 500.5, -2., 2.,401 , 200  );
        plot2D(bin, fNode->residual() , "biasedResSector"+layerName , 
               "biasedResSector"+layerName  , 99.5, 500.5, -2., 2.,401 , 200  );
	
	// plot residual versus Y position for every module
	const DeSTSector* sector = static_cast<const DeSTSector*>(measurement.detectorElement()) ;
	const std::string& name = sector->name();
	const std::string hisname = name.substr(45,4) + "/" + name.substr(55,9) ;
	profile1D(fNode->state().y(),fNode->unbiasedResidual(),hisname,hisname,-700,700,70) ;

	if( hit ) {
	  double noise = hit->sector().noise(chan) ;
	  if(noise>0) {
	    const double signalToNoise = hit->totalCharge()/noise;
	    plot2D(bin, signalToNoise,"SNSector"+layerName ,"SNSector"+layerName  , 99.5, 500.5, -0.25, 100.25, 401, 201);
	    plot2D(bin, hit->totalCharge(),"CSector"+layerName ,"CSector"+layerName  , 99.5, 500.5, -0.5, 200.5,401,201 );
	    if(m_plotsBySector) {
	      std::string sectorName = hit->cluster()->sectorName();
	      plot(signalToNoise, "BySector/SignalToNoise_"+sectorName, "Signal-to-noise", -0.5, 200.5, 200);
	    }
	  }
	}
      }

      if( hit ) {
	// get the measurement and plot ST related quantities
	plot(hit->totalCharge(),"charge", "clusters charge", 0., 200., 100);
	plot(hit->size(), "size",  "cluster size", -0.5, 10.5, 11);
      

	// for this one we actually need the component perpendicular to B field.
	Gaudi::XYZVector dir = fNode->state().slopes() ;
	profile1D(dir.x() , hit->size(), "cluster size vs tx", "cluster size vs tx", -0.3, 0.3 ) ;
	profile1D(std::sqrt( dir.x() * dir.x() + dir.y() * dir.y() )/ dir.z(),
		  hit->totalCharge(), "cluster charge vs slope", "cluster charge vs local slope",0,0.4) ;
      
	if (hit->highThreshold() ) ++nHigh;
	measVector.push_back(hit);
      }
    }
  } // nodes

  if( measVector.size() > 0 ) {
    // plot high fraction and also shorth
    plot(nHigh/double(itIds.size()),"highFrac", "high fraction", 0., 1., 100);
    const double shorth = SiChargeFun::shorth(measVector.begin(),measVector.end());
    plot(shorth,"shorth" ,"shorth charge", 0., 100., 200);
    const double gm = SiChargeFun::generalizedMean(measVector.begin(), measVector.end()); 
    plot(gm, "generalized mean","generalized mean charge",  0., 100, 200);
  }

  // make overlap histograms
  for(size_t ilay=0; ilay<4; ++ilay) {
    const char layname[4][128] = { "TTaX","TTaU","TTbV","TTbX" } ;
    std::string prefix = std::string(layname[ilay]) + "/" ;
    if( !type.empty() ) prefix.insert(0,type + "/") ;
    plot( nodesByTTLayer[ilay].size(), prefix + "Number of hits", "Number of hits",
          -0.5,5.5) ;

    if( nodesByTTLayer[ilay].size() == 2 ) {
      const LHCb::FitNode* firstnode = nodesByTTLayer[ilay].front() ;
      const LHCb::FitNode* secondnode = nodesByTTLayer[ilay].back() ;
      
      if( firstnode->measurement().detectorElement() != 
          secondnode->measurement().detectorElement() ) {

        if( firstnode->measurement().detectorElement()->geometry()->toGlobal(Gaudi::XYZPoint()).x() >
            secondnode->measurement().detectorElement()->geometry()->toGlobal(Gaudi::XYZPoint()).x() )
          std::swap( firstnode, secondnode ) ;
	
        int firstsign = firstnode->measurement().trajectory().direction(0).y() >0 ? 1 : -1 ;
        int secondsign = secondnode->measurement().trajectory().direction(0).y() >0 ? 1 : -1 ;
        double firstresidual  = firstsign * firstnode->residual() ;
        double secondresidual =  secondsign * secondnode->residual() ;

        double diff = firstresidual - secondresidual ;
	
        // let's still make the correction for the track angle, such that we really look in the wafer plane.
        Gaudi::XYZVector localdir = 
          firstnode->measurement().detectorElement()->geometry()->toLocal( firstnode->state().slopes() ) ;
        double localTx  = localdir.x()/localdir.z() ;
        diff *= std::sqrt( 1+localTx*localTx) ;

        const std::string layerName = TTNames().UniqueLayerToString(firstnode->measurement().lhcbID().stID());
        plot( diff, prefix + "Overlap residual",std::string("Overlap residuals in ") + layerName, -1.0, 1.0 ,100);
        plot( firstresidual * std::sqrt(firstnode->errMeasure2()/firstnode->errResidual2()),
              prefix + "Residuals in overlaps (left)", std::string("Residuals in overlaps (left) in ")+ layerName , 
              -0.5, 0.5, 100) ;
        plot( secondresidual * std::sqrt(secondnode->errMeasure2()/secondnode->errResidual2()),
              prefix + "Residuals in overlaps (right)", std::string("Residuals in overlaps (right) in ") + layerName, 
              -0.5, 0.5, 100) ;

        // this needs to be fixed: can we somehow can a consecutive ladder ID?
        size_t sectorID = ttUniqueSectorID( firstnode->measurement().lhcbID().stID()) ;
        profile1D( double(sectorID), diff,
                   prefix + "Overlap residual versus sector ID",
                   "Average overlap residual versus sector ID",-0.5,47.5,48) ;
	
        if(fullDetail()) {
          std::string firstname = firstnode->measurement().detectorElement()->name().substr(55,10) ;
          std::string secondname = secondnode->measurement().detectorElement()->name().substr(55,10) ;
          plot( diff, prefix + "Overlap residual for " + firstname + " and " + secondname,
                "Overlap residual for " + firstname + " and " + secondname,-1.0, 1.0 ,50);
        }
      }
    }
  }
}

unsigned int TTTrackMonitor::histoBin(const LHCb::STChannelID& chan) const {

  // convert layer and station to a flat number
  unsigned int layer;
  chan.station() == 1u ?layer = chan.layer() : layer = chan.layer() + 2;
  return layer * 100 + (chan.detRegion()-1) * 30 + chan.sector();
}


// Projected angle
double TTTrackMonitor::ProjectedAngle() const {
  // placeholder for new method
  return 0.;
}

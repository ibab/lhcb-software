// Include files 
#include "ITTrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Kernel/SiChargeFun.h"
#include "Event/FitNode.h"
#include "Event/STMeasurement.h"
#include "Event/Measurement.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <map>
#include "Map.h"
#include "Kernel/ITNames.h"

using namespace LHCb;
using namespace Gaudi;


DECLARE_ALGORITHM_FACTORY( ITTrackMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ITTrackMonitor::ITTrackMonitor(const std::string& name,
                               ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 7500.0); // midpoint of IT
  declareProperty("splitByITType", m_splitByITType = true); 
  declareProperty("plotsByLayer", m_plotsByLayer = true);
  declareProperty("plotsBySector", m_plotsBySector = false);// residual and signal to noise plots for each sector
  declareProperty("TH2DSummaryHist", m_2DSummaryHist = false);
  declareProperty("ProfileSummaryHist", m_ProfileSummaryHist = false);
  declareProperty("HitsOnTrack", m_hitsOnTrack = false);// use only hits on tracks (a la track monitor)
  declareProperty("minNumITHits", m_minNumITHits = 6u); 
  declareProperty("InputData" , m_clusterLocation = STClusterLocation::ITClusters);
  setSplitByType(false) ;

  // dummy intialization, redone in initialize
  m_xMax = 0.;
  m_yMax = 0.;
}

//=============================================================================
// Destructor
//=============================================================================
ITTrackMonitor::~ITTrackMonitor() {}

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
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(inputContainer());

  // locate the cluster container
  const LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);

  std::map<std::string, unsigned int> tMap;
  
  // tmp container for ids
  std::vector<unsigned int> usedIDs; usedIDs.reserve(clusters->size());

  // histograms per track
  for( const LHCb::Track* track: tracks) {
    if (track->hasT()){
      
      // find the IT hits on the track 
      const std::vector<LHCb::LHCbID>& ids = track->lhcbIDs();
      std::vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
      std::copy_if(ids.begin(), ids.end(), std::back_inserter(itHits),
                   [](const LHCb::LHCbID& id) { return id.isIT(); } );

      if (ids.size() < m_minNumITHits) continue;

      std::string type = histoDirName( *track ) ;
      fillHistograms(*track,type,ids);

      // insert into tmp container
      std::transform( ids.begin(), ids.end(), std::back_inserter(usedIDs),
                      [](const LHCb::LHCbID& id) {
          return id.stID();
      } );
    }

  } // iterT


  // see how many hits in the IT were actually used
  if (clusters->size() > 0u) {
    std::sort(usedIDs.begin(), usedIDs.end());
    std::unique(usedIDs.begin(), usedIDs.end());
    plot(usedIDs.size()/(double)clusters->size(), "fraction of used hits", 0., 1., 50);
  }

  return StatusCode::SUCCESS;
}


void ITTrackMonitor::fillHistograms(const LHCb::Track& track, 
                                    const std::string& type, 
                                    const std::vector<LHCb::LHCbID>& itIds) const
{

  std::string ittype = type;
  if (splitByITType()){
    if( !ittype.empty() ) ittype += "/" ;
    ittype += ITCategory(itIds);
  }

  // plots we should always make...
  plot(track.probChi2(),ittype+"/probChi2","probChi2", -0.0025, 1.0025, 201);
  plot(track.chi2PerDoF(),ittype+"/chi2PerDoF","chi2PerDoF",0,1000,500);
  plot(track.nLHCbIDs(),ittype+"/nLHCbIDs","#nLHCbIDs", -0.5, 60.5, 61);
  plot(track.nMeasurements(),ittype+"/nMeas","#nMeas",  -0.5, 60., 61);
  plot(track.nMeasurementsRemoved(),ittype+"/outliers","#outliers", -0.5, 10.5, 11);

  // track parameters at some reference z
  LHCb::StateVector aState;
  StatusCode sc = extrapolator()->propagate(track, m_refZ,aState );
  if( sc.isFailure() ) return ;

  plot2D(aState.x()/Gaudi::Units::cm,
         aState.y()/Gaudi::Units::cm, type+"/xy", "x vs y", 
         -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
  plot(aState.tx(),type+"/tx","tx", -0.5 , 0.5,  200);
  plot(aState.ty(),type+"/ty", "ty", -0.5, 0.5, 200);
  plot(track.p()/Gaudi::Units::GeV, type+"/momentum" ,"momentum", -5., 205., 21); 
  plot(aState.x()/Gaudi::Units::cm, type+"/xdist","x dist", -m_xMax, m_xMax);
  plot(aState.y()/Gaudi::Units::cm, type+"/ydist", "y dist", -m_yMax, m_yMax);


  // Loop over the nodes to get the hits variables
  LHCb::Track::ConstNodeRange nodes = track.nodes() ;
  LHCb::Track::ConstNodeRange::const_iterator iNodes = nodes.begin();
  std::vector<const LHCb::STMeasurement*> measVector; measVector.reserve(24);
  
  enum { NumUniqueLayers = 3*4*4 } ;
  std::vector< const LHCb::FitNode* > nodesByUniqueLayer[NumUniqueLayers] ;
  
  unsigned int nHigh = 0u;
  for ( ; iNodes != nodes.end(); ++iNodes ) {

    // Only loop on hits with measurement that is IT type
    const LHCb::FitNode* fNode = dynamic_cast<const LHCb::FitNode*>(*iNodes);

    if ( fNode->hasMeasurement() == false || 
	 !(fNode->measurement().type() == LHCb::Measurement::IT ||
	   fNode->measurement().type() == LHCb::Measurement::ITLite)) continue;
    if( m_hitsOnTrack && ( (*iNodes)->type() != LHCb::Node::HitOnTrack ) ) continue;
    if( (*iNodes)->errResidual2() <= TrackParameters::lowTolerance ) continue;

    const Measurement& meas = fNode->measurement() ;
    const STMeasurement* hit = dynamic_cast<const STMeasurement*>(&meas);    

    // unbiased residuals and biased residuals
    const STChannelID chan = meas.lhcbID().stID();
    unsigned int uniquelayer = chan.detRegion()-1 + 4*( chan.layer() -1 + 4* (chan.station()-1) ) ;
    nodesByUniqueLayer[ uniquelayer ].push_back( fNode ) ;

    plot(fNode->unbiasedResidual(),"/unbiasedResidual","unbiasedResidual",  -2., 2., 200 );
    plot(fNode->unbiasedResidual(),ittype+"/unbiasedResidual","unbiasedResidual",  -2., 2., 200 );

    plot(fNode->residual(),"/biasedResidual","biasedResidual",  -2., 2., 200 );
    plot(fNode->residual(),ittype+"/biasedResidual","biasedResidual",  -2., 2., 200 );

    // rms unbiased residual. that's the one you want to look at.
    double residual = fNode->residual() * std::sqrt(fNode->errMeasure2()/fNode->errResidual2()) ;
    plot(residual, "Residual","Residual (rms-unbiased)",-0.5,0.5,100) ;
    plot(residual, ittype+"/Residual","Residual (rms-unbiased)",-0.5,0.5,100) ;
    //if ( hit && m_plotsBySector ) {
      //std::string sectorName = hit->cluster()->sectorName();
      //plot(residual, "BySector/Residual_"+sectorName, "Residual (rms-unbiased)", -0.5, 0.5, 100);
      //plot(fNode->unbiasedResidual(), "BySector/UnbiasedResidual_"+sectorName, "Residual (unbiased)", -0.5, 0.5, 100);
    //}
    if ( m_plotsBySector ) {
        std::string sectorName = ITNames().UniqueSectorToString(chan);
        //std::cout << sectorName << ": " << chan.station() << " " << chan.layer() << " " << chan.detRegion() << " " << chan.sector() << " id: "<<histoBin(chan)<<std::endl ;
        plot(residual, "BySector/Residual_"+sectorName, "Residual (rms-unbiased)", -0.5, 0.5, 100);
        plot(fNode->unbiasedResidual(), "BySector/UnbiasedResidual_"+sectorName, "Residual (unbiased)", -0.5, 0.5, 100);
      }
    if ( m_2DSummaryHist ) {
      plot2D(histoBin(chan), fNode->unbiasedResidual(),  "BySector/AllSectorsUnbiasedResidualHisto", "Residual (unbiased)", -0.5, 336.5, -0.5, 0.5, 337  , 100);
      plot2D(histoBin(chan), fNode->residual(),  "BySector/AllSectorsBiasedResidualHisto", "Biased residual", -0.5, 336.5, -0.5, 0.5, 337  , 100);
      plot2D(histoBin(chan), residual,  "BySector/AllSectorsResidualHisto", "Residual (rms-unbiased)",  -0.5, 336.5, -0.5, 0.5,337 , 100);
    }
    if ( m_ProfileSummaryHist ) {
      profile1D(histoBin(chan), fNode->unbiasedResidual(),  "BySector/AllSectorsUnbiasedResidualProfile", "Residual (unbiased)",   -0.5, 336.5);
      profile1D(histoBin(chan), fNode->residual(),  "BySector/AllSectorsBiasedResidualProfile", "Biased residual",    -0.5, 336.5);
      profile1D(histoBin(chan), residual,  "BySector/AllSectorsResidualProfile", "Residual (rms-unbiased)",    -0.5, 336.5);
    } 

    // make plots per layer
    if (m_plotsByLayer) {
      const std::string layerName = ITNames().LayerToString(chan);
      const std::string stationName = ITNames().StationToString(chan);
      plot(fNode->unbiasedResidual(),ittype+"/unbiasedResidual"+layerName,"unbiasedResidual"+layerName,  -2., 2., 200 );
      plot(fNode->residual(),ittype+"/biasedResidual"+layerName,"biasedResidual"+layerName,  -2., 2., 200 );
      plot(fNode->unbiasedResidual(),ittype+"/unbiasedResidual"+stationName+layerName,
           "unbiasedResidual"+stationName+layerName,  -2., 2., 200 );
      plot(fNode->residual(),ittype+"/biasedResidual"+stationName+layerName,
           "biasedResidual"+stationName+layerName,  -2., 2., 200 );
    }
    
	
    // 2D plots in full detail mode
    if(fullDetail()) {
      const unsigned int bin = chan.station()*100 + chan.layer()*10 + chan.sector(); 
      const std::string boxName = ITNames().BoxToString(chan);
      plot2D(bin, fNode->unbiasedResidual() , ittype+"/unbiasedResSector"+boxName ,
             "unbiasedResSector"+boxName  , 99.5, 400.5, -2., 2.,301 , 200  );
      plot2D(bin, fNode->residual() , ittype+"/biasedResSector"+boxName , 
             "/biasedResSector"+boxName  , 99.5, 400.5, -2., 2.,301 , 200  );
      if( hit ) {
	double noise = hit->sector().noise(chan);
	if(noise>0) {
	  const double signalToNoise = hit->totalCharge()/noise;
	  plot2D(bin, signalToNoise,ittype+"/SNSector"+boxName ,"SNSector"+boxName  , 99.5, 400.5, -0.25, 100.25, 301, 201);
	  plot2D(bin, hit->totalCharge(),ittype+"/CSector"+boxName ,"CSector"+boxName  , 99.5, 400.5, -0.5, 200.5,301,201 );
	  if(m_plotsBySector) 
	    {
	      std::string sectorName = hit->cluster()->sectorName();
	      plot(signalToNoise, "BySector/SignalToNoise_"+sectorName, "Signal-to-noise", -0.5, 200.5, 200);
	    }
	}
      }
    }

    if ( hit ) {
      // get the measurement and plot ST related quantities
      plot(hit->totalCharge(),ittype+"/charge", "clusters charge", 0., 200., 100);
      plot(hit->size(), ittype+"/size",  "cluster size", -0.5, 10.5, 11);
      
      // get the measurement and plot ST related quantities
      const std::string stationName = ITNames().StationToString(chan);
      plot(hit->totalCharge(),stationName + "/charge", "clusters charge", 0., 200., 100);
      plot(hit->size(), stationName + "/size",  "cluster size", -0.5, 10.5, 11);
      
      // for this one we actually need the component perpendicular to B field.
      Gaudi::XYZVector dir = fNode->state().slopes() ;
      profile1D(dir.x() , hit->size(), stationName + "/cluster size vs tx", "cluster size vs tx", -0.3, 0.3 ) ;
      profile1D(std::sqrt( dir.x() * dir.x() + dir.y() * dir.y() )/ dir.z(),
		hit->totalCharge(), stationName + "/cluster charge vs slope", "cluster charge vs local slope",0,0.4) ;
      
      if (hit->highThreshold() ) ++nHigh;
      measVector.push_back(hit);
    }
  } // nodes

  if( measVector.size()>0 ) {
    // plot high fraction and also shorth
    plot(nHigh/double(itIds.size()), ittype+"/highFrac", "high fraction", 0., 1., 100);
    const double shorth = SiChargeFun::shorth(measVector.begin(),measVector.end());
    plot(shorth,ittype+"/shorth" ,"shorth charge", 0., 100., 200);
    const double gm = SiChargeFun::generalizedMean(measVector.begin(), measVector.end()); 
    plot(gm,ittype+"/generalized mean","generalized mean charge",  0., 100, 200);
  }

  // make overlap histograms.

  // TODO: this now creates one histogram per unique IT layer (all
  // combined in one 2D histogram). for display in monitoring, these
  // need to be combined in a small set of usefull histograms. I
  // haven't quite figured out what is the right granularity. (Box
  // seems fine, but 12 histos is too much.)
  for(size_t ilay=0; ilay<NumUniqueLayers; ++ilay) { 
    if( nodesByUniqueLayer[ilay].size() == 2 ) {
      const LHCb::FitNode* firstnode = nodesByUniqueLayer[ilay].front() ;
      const LHCb::FitNode* secondnode = nodesByUniqueLayer[ilay].back() ;
      
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
        plot2D( ilay, diff, "OverlapResidualByLayer","Overlap residuals by unique layer",
                -0.5, NumUniqueLayers-0.5, -0.5, 0.5, NumUniqueLayers, 50 ) ;
        // the problem with x-scale is most easily observed when plotting by stack
        plot1D( diff, ITNames().BoxToString((ilay%4)+1) + "/Overlap residual",
                "Overlap residuals in " + ITNames().BoxToString((ilay%4)+1), -0.5, 0.5, 50) ;
      }
    }
  }
}


unsigned int ITTrackMonitor::histoBin(const LHCb::STChannelID& chan) const {
  // convert sector to a flat number
  return (chan.station()-1)*7*4*4 + (chan.layer()-1)*7*4 + (chan.detRegion()-1)*7 + (chan.sector() -1);
}

std::string ITTrackMonitor::ITCategory(const std::vector<LHCb::LHCbID>& ids) const {
  
  typedef std::map<unsigned int, unsigned int> BoxMap;
  BoxMap nBox;
  for(const auto& id: ids) {
    nBox[id.stID().detRegion()]  += 1;
  }
  
  unsigned int bestBox = 0; unsigned int nBest =0;
  for( const auto&  box: nBox ) {
    if (box.second > nBest ){ 
      bestBox = box.first;
      nBest = box.second;
    }
  }   // for eachç

  return ITNames().BoxToString(bestBox);
}

// Projected angle
double ITTrackMonitor::ProjectedAngle() const {
  // placeholder for new method
  return 0.;
}

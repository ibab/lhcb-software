// local
#include "STTAEClusterMonitor.h"
#include "Event/STCluster.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STBoardMapping.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Kernel/Trajectory.h"

#include "Event/ODIN.h"

// from Boost
#include <boost/assign/list_of.hpp>

// AIDA histograms
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH2D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STTAEClusterMonitor
//
// 2009-03-10 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STTAEClusterMonitor )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STTAEClusterMonitor::STTAEClusterMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase ( name , pSvcLocator )
  , m_nSamples(0)
  , m_maxSample(0)
  , m_debug(false)
  , m_verbose(false)
  , m_prof_clustersVsSample(0)
  , m_2d_ADCsVsSample(0)
{
  // Ordered list of cluster locations
  std::vector<std::string> tmp =
    boost::assign::list_of("Event/Prev7/Raw/TT/Clusters")
    ("Event/Prev6/Raw/TT/Clusters")
    ("Event/Prev5/Raw/TT/Clusters") 
    ("Event/Prev4/Raw/TT/Clusters")
    ("Event/Prev3/Raw/TT/Clusters")
    ("Event/Prev2/Raw/TT/Clusters")
    ("Event/Prev1/Raw/TT/Clusters")
    ("Raw/TT/Clusters")
    ("Event/Next1/Raw/TT/Clusters")
    ("Event/Next2/Raw/TT/Clusters")
    ("Event/Next3/Raw/TT/Clusters")
    ("Event/Next4/Raw/TT/Clusters")
    ("Event/Next5/Raw/TT/Clusters")
    ("Event/Next6/Raw/TT/Clusters")
    ("Event/Next7/Raw/TT/Clusters");
  declareProperty( "ClusterLocations", m_clusterLocations=tmp);
  
  // Cuts
  declareProperty("BunchID",       m_bunchID               );// BunchID 
  declareProperty("ChargeCut", m_chargeCut=12);
  /// Maximum number of strips in clusters
  declareProperty( "MaxClusterSize", m_maxClusterSize=6 );

  /// Plot by service box
  declareProperty( "ByServiceBox", m_plotBySvcBox=false );

  /// Plot by detector region
  declareProperty( "ByDetectorRegion", m_plotByDetRegion=false );

  /// Use mean as MPV
  declareProperty( "UseMeanAsMPV", m_useMean=false );

  /// Select update rate for filling MPV histograms
  declareProperty( "MPVUpdateRate", m_updateRateMPV=10 );

}
//=============================================================================
// Destructor
//=============================================================================
ST::STTAEClusterMonitor::~STTAEClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STTAEClusterMonitor::initialize() {

  StatusCode sc = ST::HistoAlgBase::initialize(); 
  if ( sc.isFailure() ) return sc;

  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE);
  if(m_verbose) m_debug = true;
  if (m_debug) debug() << "==> Initialize" << endmsg;
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");
  
  m_nSamples = m_clusterLocations.size();
  // Time aligned events (Central +/- TAE)
  if(m_nSamples%2 == 0) {
    error() << "Expected odd number of Cluster Locations, got " << m_nSamples << endmsg;
    return StatusCode::FAILURE;
  }
  m_maxSample = (m_nSamples+1)/2 - 1 + 0.5;

  // Loop over input locations and fill label vector
  std::vector<std::string> tmp = boost::assign::list_of
    ("Prev7")("Prev6")("Prev5")("Prev4")("Prev3")("Prev2")("Prev1")
    ("Default")
    ("Next1")("Next2")("Next3")("Next4")("Next5")("Next6")("Next7");
  m_spills = tmp;
  
  unsigned int diff=m_spills.size()-m_clusterLocations.size();
  m_spills.erase(m_spills.begin(),m_spills.begin()+diff/2);
  m_spills.resize(m_clusterLocations.size());

  std::vector<std::string>::iterator itCL;
  /* Loop over input locations 
     Couldn't find a good way to do this in the base class yet */
  for(itCL = m_clusterLocations.begin(); itCL != m_clusterLocations.end(); ++itCL) {
    STDetSwitch::flip(detType(), (*itCL));
    if(m_debug) debug() << "Cluster Locations: " << (*itCL) << endmsg;
  }

  bookHistograms();

  return StatusCode::SUCCESS;
}
//==============================================================================
// Book histograms
//==============================================================================
void ST::STTAEClusterMonitor::bookHistograms() {
  m_prof_clustersVsSample = bookProfile1D("Number of clusters vs sampling points",-m_maxSample, m_maxSample, m_nSamples);
  m_2d_ADCsVsSample = book2D("Cluster ADC values vs sampling point",-m_maxSample, m_maxSample, m_nSamples, 0., 60., 60);
  if(detType() == "TT" || m_plotBySvcBox) {
    /// list of service boxes  
    std::vector<std::string>::const_iterator itSvcBoxes=(this->readoutTool())->serviceBoxes().begin();
    for(;itSvcBoxes != (this->readoutTool())->serviceBoxes().end(); ++itSvcBoxes) {
      std::string svcBox=(*itSvcBoxes);
      if(detType() == "TT") {
        std::string quadrant = svcBox.substr(0,2);
        if(m_histos_ADCsVsSampleByServiceBox.find(quadrant) == m_histos_ADCsVsSampleByServiceBox.end()) {
          m_histos_ADCsVsSampleByServiceBox[quadrant] = 
            HistoPair( Gaudi::Utils::Aida2ROOT::aida2root ( book2D("Cluster ADC values vs sampling point "+quadrant,
                                                                   -m_maxSample, m_maxSample, m_nSamples, 0., 60., 60) ),
                       bookProfile1D("ADC MPV vs sampling point"+quadrant,
                                     -m_maxSample, m_maxSample, m_nSamples, "", 0., 60.));
        }
      } // End of TT condition
      if(m_plotBySvcBox) {
        m_histos_ADCsVsSampleByServiceBox[svcBox] = 
          HistoPair( Gaudi::Utils::Aida2ROOT::aida2root ( book2D("Cluster ADC values vs sampling point "+svcBox,
                                                                 -m_maxSample, m_maxSample, m_nSamples, 0., 60., 60) ),
                     bookProfile1D("ADC MPV vs sampling point "+svcBox,
                                   -m_maxSample, m_maxSample, m_nSamples, "", 0., 60.));
      }
    } // End of service box loop
  } // End of service box condition
  if(m_plotByDetRegion) {
    std::vector<std::string> names;
    if(detType() == "TT"){
      names = LHCb::TTNames().allDetRegions();
    } else if(detType() == "IT"){
      names = LHCb::ITNames().allBoxes();
    }
    std::vector<std::string>::iterator itNames = names.begin();
    for( ; itNames != names.end(); ++itNames ){
      //      std::cout << (*itNames) << std::endl;
      std::string region = (*itNames);
      m_histos_ADCsVsSampleByDetRegion[region] = 
        HistoPair( Gaudi::Utils::Aida2ROOT::aida2root ( book2D("Cluster ADC values vs sampling point "+region,
                                                               -m_maxSample, m_maxSample, m_nSamples, 0., 60., 60) ),
                   bookProfile1D("ADC MPV vs sampling point "+region,
                                 -m_maxSample, m_maxSample, m_nSamples, "", 0., 60.));
    };
  }
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STTAEClusterMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  // Select the correct bunch id
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  if( NULL != odin ) {
    if( !m_bunchID.empty() && 
        std::find(m_bunchID.begin(), m_bunchID.end(), 
                  odin->bunchId()) == m_bunchID.end()) return StatusCode::SUCCESS;
  } else return Warning("No ODIN bank found", StatusCode::SUCCESS,1);

  counter("Number of events") += 1; 

  debug() << counter("Number of events") << endmsg;
  // code goes here  
  monitorClusters();
  if(!m_useMean && counter("Number of events").nEntries()%m_updateRateMPV) updateMPVHistograms();
  return StatusCode::SUCCESS;
}

//=============================================================================
// Look at the clusters histogram
//=============================================================================
void ST::STTAEClusterMonitor::monitorClusters() {
  std::vector<std::string>::const_iterator itCL;
  // Loop over input locations
  unsigned int iSample=0;
  for(itCL = m_clusterLocations.begin(); itCL != m_clusterLocations.end(); ++itCL, ++iSample) {
    const LHCb::STClusters* clusters = getIfExists<LHCb::STClusters>(*itCL);
    // Check location exists
    if( NULL != clusters ){
      LHCb::STClusters::const_iterator itClus;
      // Loop over clusters
      if(m_debug) {
        debug() << "Number of clusters in " << (*itCL) << " is " << (clusters->size()) << endmsg;
      }
      int sample = static_cast<int>(iSample-(m_nSamples-1)/2);
      m_prof_clustersVsSample->fill(sample, clusters->size());
      for(itClus = clusters->begin(); itClus != clusters->end(); ++itClus) {
        const LHCb::STCluster* cluster = (*itClus);
        const unsigned int clusterSize = cluster->size();

        // Look at 1, 2, 3 strip cluster separately by changing job options
        if(clusterSize <= m_maxClusterSize) {
          // Get service box and set up histogram IDs
          std::string svcBox = (this->readoutTool())->serviceBox(cluster->firstChannel());

          // ADC values vs spill
          const double totalCharge = cluster->totalCharge();
          if(totalCharge < m_chargeCut) continue;
          m_2d_ADCsVsSample->fill(sample, totalCharge);
          // Always fill histograms per readout quadrant for TT
          if(detType() == "TT") {
            std::string quadrant = svcBox.substr(0,2);
            fillHistogram(m_histos_ADCsVsSampleByServiceBox[quadrant], sample, totalCharge);
          }
          if(m_plotBySvcBox) {
            fillHistogram(m_histos_ADCsVsSampleByServiceBox[svcBox], sample, totalCharge);
          }
          if(m_plotByDetRegion) {
            std::string region = cluster->detRegionName();
            fillHistogram(m_histos_ADCsVsSampleByServiceBox[region], sample, totalCharge);
          }
        } // End of cluster condition
      }// End of cluster iterator
    } else Warning("No clusters found at "+(*itCL), StatusCode::SUCCESS, 0).ignore(); // End of cluster getIfExist
  }// End loop over cluster locations
}

void ST::STTAEClusterMonitor::fillHistogram(HistoPair histos, int sample, double charge) {
  histos.first->Fill(sample, charge);
  if(m_useMean) histos.second->fill(sample, charge);
}

void ST::STTAEClusterMonitor::updateMPVHistograms() {
  if(m_plotBySvcBox) {
    std::map<std::string, HistoPair>::iterator iHP =  m_histos_ADCsVsSampleByServiceBox.begin();
    for(; iHP != m_histos_ADCsVsSampleByServiceBox.end(); ++iHP) {
      TH2D* h2d = (*iHP).second.first;
      AIDA::IProfile1D* h1p = (*iHP).second.second;
      h1p->reset();
      int nbinsX=h2d->GetNbinsX();
      for(int i=1; i<=nbinsX; ++i) {
        TH1D* hpy = h2d->ProjectionY("hpy",i,i);
        if ( !(0.0 < hpy->GetEntries()) ) {
          continue;
        }
        double sample = h2d->GetXaxis()->GetBinCenter(i);
        double mpv = hpy->GetBinCenter(hpy->GetMaximumBin());
        h1p->fill(sample, mpv);
      }
    }
  }
}

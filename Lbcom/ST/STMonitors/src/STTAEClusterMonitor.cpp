// $Id: STTAEClusterMonitor.cpp,v 1.4 2009-05-28 15:42:07 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "STTAEClusterMonitor.h"
#include "Event/STCluster.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STBoardMapping.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Kernel/Trajectory.h"

// from Boost
#include <boost/assign/list_of.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : STTAEClusterMonitor
//
// 2009-03-10 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STTAEClusterMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STTAEClusterMonitor::STTAEClusterMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase ( name , pSvcLocator )
{
  // Ordered list of cluster locations
  //  std::vector<std::string> m_clusterDefaultLocations =
  m_clusterLocations =
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
  declareProperty( "ClusterLocations", m_clusterLocations);
  
  /// Maximum number of strips in clusters
  declareProperty( "MaxClusterSize", m_maxClusterSize=6 );

  /// Plot by service box
  declareProperty( "ByServiceBox", m_plotBySvcBox=false );

  /// Plot by detector region
  declareProperty( "ByDetectorRegion", m_plotByDetRegion=false );

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
  m_spills = boost::assign::list_of("Prev7")("Prev6")("Prev5") ("Prev4")("Prev3")("Prev2")("Prev1")
    ("Default")("Next1")("Next2")("Next3")("Next4")("Next5")("Next6")("Next7");
  
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
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STTAEClusterMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  counter("Number of events") += 1; 

  debug() << counter("Number of events") << endreq;
  // code goes here  
  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STTAEClusterMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ST::HistoAlgBase::finalize(); 
}

//=============================================================================
// Look at the clusters histogram
//=============================================================================
void ST::STTAEClusterMonitor::monitorClusters() {
  std::vector<std::string>::const_iterator itCL;
  // Loop over input locations
  unsigned int iSample=0;
  for(itCL = m_clusterLocations.begin(); itCL != m_clusterLocations.end(); ++itCL, ++iSample) {
    // Check location exists
    if(exist<LHCb::STClusters>(*itCL)){
      LHCb::STClusters* clusters = get<LHCb::STClusters>(*itCL);
      LHCb::STClusters::const_iterator itClus;
      // Loop over clusters
      if(m_debug) {
        debug() << "Number of clusters in " << (*itCL) << " is " << (clusters->size()) << endreq;
      }
      int sample = static_cast<int>(iSample-(m_nSamples-1)/2);
      profile1D(sample, clusters->size(),"Number of clusters vs sampling points",
                "Number of clusters vs sampling points",-m_maxSample, m_maxSample, m_nSamples);

      for(itClus = clusters->begin(); itClus != clusters->end(); ++itClus) {
        const LHCb::STCluster* cluster = (*itClus);
        const unsigned int clusterSize = cluster->size();

        // Look at 1, 2, 3 strip cluster separately by changing job options
        if(clusterSize <= m_maxClusterSize) {
          // Get service box and set up histogram IDs
          std::string svcBox = (this->readoutTool())->serviceBox(cluster->firstChannel());

          // ADC values vs spill
          const double totalCharge = cluster->totalCharge();

          std::string id2DADCBase = "Cluster ADC values vs sampling point";
          std::string idprofADCBase =  "ADC MPV vs sampling point";// PREMPTIVE - CURRENTLY THIS IS THE MEAN VALUE
          plot2D(sample, totalCharge, id2DADCBase, id2DADCBase, -m_maxSample, m_maxSample, -2., 202., m_nSamples, 51);
          // Always fill histograms per readout quadrant for TT
          if(detType() == "TT") {
            std::string quadrant = svcBox.substr(0,2);
            std::string id2DADC = id2DADCBase+" "+quadrant;
            plot2D(sample, totalCharge, id2DADC, id2DADC, -m_maxSample, m_maxSample, -2., 202., m_nSamples, 51);
          }
          if(m_plotBySvcBox || m_plotByDetRegion) {
            if(m_plotBySvcBox) {
              std::string idSvcBox = " "+svcBox;
              id2DADCBase += idSvcBox;
              plot2D(sample, totalCharge, id2DADCBase, id2DADCBase, -m_maxSample, m_maxSample, -2., 202., m_nSamples, 51);
              idprofADCBase += idSvcBox;
              profile1D(sample, totalCharge, idprofADCBase, idprofADCBase, -m_maxSample, m_maxSample, m_nSamples);
            }
            if(m_plotByDetRegion) {
              std::string idDetRegion = " "+cluster->detRegionName();
              id2DADCBase += idDetRegion;
              plot2D(sample, totalCharge, id2DADCBase, id2DADCBase, -m_maxSample, m_maxSample, -2., 202., m_nSamples, 51);
              idprofADCBase += idDetRegion;
              profile1D(sample, totalCharge, idprofADCBase, idprofADCBase, -m_maxSample, m_maxSample, m_nSamples);
            }
          }
        } // End of cluster condition
      }// End of cluster iterator
    } else Warning("No clusters found at "+(*itCL), StatusCode::SUCCESS, 50).ignore(); // End of cluster exists
  }// End loop over cluster locations
}

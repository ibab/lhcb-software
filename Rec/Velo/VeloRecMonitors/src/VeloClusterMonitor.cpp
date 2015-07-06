// Include files 
// -------------
// from STL
#include <string.h>

// from Boost
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ProcStatus.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// local
#include "VeloClusterMonitor.h"
#include "RawClusterDecoding.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterMonitor
//
// VELO clusters monitoring algorithm.
// Produces a set of histograms from the clusters bank in the TES.
//
// 2008-08-18 : Eduardo Rodrigues
// 2008-06-28 : Mark Tobin, Kazu Akiba
// 2008-04-30 : Aras Papadelis, Thijs Versloot
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
  DECLARE_ALGORITHM_FACTORY( VeloClusterMonitor )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloClusterMonitor::VeloClusterMonitor( const std::string& name,
    ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
  , m_nClustersPerSensor(256,0)
{
  declareProperty( "VeloClusterLocation",
                   m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "PerSensorPlots", m_perSensorPlots = false );
  declareProperty( "HighMultiplicityPlot", m_highMultiplicityPlot  = false );
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloClusterMonitor::~VeloClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloClusterMonitor::initialize() {

  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  // Try to find TAE sample name in algo instance name (for histo titles)
  std::string tmpTae = name();
  size_t posPrev = tmpTae.find("Prev");
  size_t posNext = tmpTae.find("Next");
  if ( std::string::npos != posPrev ) {
    m_tae = tmpTae.substr(posPrev, 5);
  } else if ( std::string::npos != posNext ) {
    m_tae = tmpTae.substr(posNext, 5);
  } else {
    m_tae = "Default";
  }

  // Find the largest sensor number from detector element
  // (to avoid making any assumptions about the sensor nubmering scheme)
  unsigned int maxSensNum = 0;
  for ( std::vector<DeVeloSensor*>::const_reverse_iterator si =
          m_veloDet->sensorsReverseBegin();
        si != m_veloDet->sensorsReverseEnd(); ++si ) {
    if ( (*si)->sensorNumber() > maxSensNum ) {
      maxSensNum = (*si)->sensorNumber();
    }
  }
  
  char nCluTitle[100];
  sprintf( nCluTitle, "Number of VELO clusters per event (%s)", m_tae.c_str() );
  m_hNCluEvt = book1D( "# VELO clusters", nCluTitle, 0., 6000., 600 );
  m_hNCluEvtZoom = book1D( "# VELO clusters (zoom)", nCluTitle, 0., 50., 50 );
  m_hCluSize = book1D( "Cluster size", "Number of strips per cluster",
                       -0.5, 5.5, 6 );
  m_hCluADC = book1D( "Cluster ADC value", "ADC value per cluster",
                      -0.5, 128*4+0.5, 128*4+1 );
  m_hCluADCR = book1D( "Cluster ADC value (R)", "ADC value per cluster (R)",
                       -0.5, 128*4+0.5, 128*4+1 );
  m_hCluADCPhi = book1D( "Cluster ADC value (Phi)",
                         "ADC value per cluster (Phi)",
                         -0.5, 128*4+0.5, 128*4+1 );
  m_hSeedADC = book1D( "ADC value of cluster seed strips",
                       "ADC value of cluster seed strips",
                       -0.5, 128*1+0.5, 128*1+1 );
  m_hSeedADCR = book1D( "ADC value of cluster seed strips (R)",
                        "ADC value of cluster seed strips (R)",
                        -0.5, 128*1+0.5, 128*1+1 );
  m_hSeedADCPhi = book1D( "ADC value of cluster seed strips (Phi)",
                          "ADC value of cluster seed strips (Phi)",
                          -0.5, 128*1+0.5, 128*1+1 );
  m_hIncADC = book1D( "ADC value of cluster include strips",
                      "ADC value of cluster include strips",
                      -0.5, 128*0.5+0.5, 65 );
  m_hIncADCR = book1D( "ADC value of cluster include strips (R)",
                       "ADC value of cluster include strips (R)",
                       -0.5, 128*0.5+0.5, 65 );
  m_hIncADCPhi = book1D( "ADC value of cluster include strips (Phi)",
                         "ADC value of cluster include strips (Phi)",
                         -0.5, 128*0.5+0.5, 65 );
  m_hCluSizeSens = book2D( "Cluster size vs sensor",
                           "Number of strips per cluster versus sensor",
                           -0.5, 131.5, 132, -0.5, 5.5, 6 );
  m_hCluADCSens = book2D( "Cluster ADC values vs sensor",
                          "Cluster ADC values versus sensor",
                          -0.5, 131.5, 132, 0, 515,  103 );
  m_hActiveLinkSens = book2D( "Active chip links vs sensor",
                              "Active chip links versus sensor",
                              -0.5, 131.5, 132, -0.5, 63.5, 64 );

  if ( m_perSensorPlots ) {
    m_hNCluSens.resize(maxSensNum+1,0);
    
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
        unsigned int s = (*si)->sensorNumber();
        boost::format fmtEvt ( "# clusters sensor %d" ) ;
        fmtEvt % s ;
        const std::string hName = fmtEvt.str() ;
        m_hNCluSens[s] = book1D( hName, hName, -0.5, 200.5, 201 );
    }
  }

  if ( m_highMultiplicityPlot ) {
    m_hNClustersHM = book1D("n_clusters_highmult", "Number of VELO clusters (extended)",
                      0.0, 2.5e4, 1000);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloClusterMonitor::execute() {

  // Count the number of events monitored
  // ------------------------------------
  counter( "# events" ) += 1;

  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
StatusCode Velo::VeloClusterMonitor::veloClusters() {

  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;

  m_clusters = getIfExists<LHCb::VeloClusters>( m_clusterCont );
  if ( NULL == m_clusters ) {
    if ( m_debugLevel ) debug() << "No VeloClusters container found for this event !" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
      << m_clusters->size() <<endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloClusterMonitor::monitorClusters() {

  if ( m_perSensorPlots ) {
    memset(&m_nClustersPerSensor[0],0,m_nClustersPerSensor.size()*sizeof(unsigned int));
  }

  // Retrieve the VeloClusters
  // -------------------------
  StatusCode sc = veloClusters();
  if( sc.isFailure() ) return;

  // Number of clusters per event
  // ----------------------------
  unsigned int nclus = m_clusters -> size();
  counter( "# VeloClusters" ) += nclus;
  if ( nclus > 0 ) {
    m_hNCluEvt->fill(nclus);
    if ( nclus < 51 ) m_hNCluEvtZoom -> fill( nclus );
  }
  
  if ( m_highMultiplicityPlot )
    fillHighMultiplicity(nclus);

  // Loop over the VeloClusters
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {
    
    LHCb::VeloCluster* cluster = (*itVC);
    
    // Number of strips and total charge
    // -----------------------------------
    unsigned int nstrips = cluster -> size();    
    double adc           = cluster -> totalCharge();
    
    m_hCluSize->fill(nstrips);
    m_hCluADC->fill(adc);
    
    //find the strip with the highest charge (seed strip)
    unsigned int iseedstrip=0;
    double adcseedstrip=0.;
    for (unsigned int j=0; j<nstrips; ++j) {
      if (cluster -> adcValue(j)>adcseedstrip){
        iseedstrip=j;
        adcseedstrip=cluster ->adcValue(j);
      }
    }
    m_hSeedADC->fill(adcseedstrip);
    if (cluster->isRType()) { 
      m_hSeedADCR->fill(adcseedstrip);
    }
    if (cluster->isPhiType()) {
      m_hSeedADCPhi->fill(adcseedstrip);
    }
    
    //plot the adc values of the include strips
    for (unsigned int j=0; j<nstrips; ++j) {
      double adcstrip=cluster -> adcValue(j);
      if (j!=iseedstrip) {
        m_hIncADC->fill(adcstrip);
        if (cluster->isRType()) {
          m_hIncADC->fill(adcstrip);
        }
        if (cluster->isPhiType()) { 
          m_hIncADC->fill(adcstrip);
        }
      }
    }
    
    if( cluster -> isRType() ) {
      m_hCluADCR->fill(adc);
    }
    
    if( cluster -> isPhiType() ) {
      m_hCluADCPhi->fill(adc);
    }
    
    // Number of strips and total charge versus the sensor number
    // ----------------------------------------------------------
    unsigned int sensorNumber = cluster -> channelID().sensor();
    
    ++m_nClustersPerSensor[sensorNumber];
    
    m_hCluSizeSens->fill(sensorNumber, nstrips);
    m_hCluADCSens->fill(sensorNumber, adc);    
    
    // Active chip links versus sensor number
    // --------------------------------------
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );
    
    unsigned int stripNumber   = cluster -> channelID().strip();
    unsigned int chipChannel   = veloSensor -> StripToChipChannel( stripNumber ); // 0 -> 2047
    unsigned int activeLink = chipChannel/32;
    m_hActiveLinkSens->fill(sensorNumber, activeLink);
    
    // plot number of clusters seperately for each sensor
    if ( m_perSensorPlots ) {
      for (unsigned int s=0; s<m_nClustersPerSensor.size(); ++s) {
        if ( 0 == m_nClustersPerSensor[s] ) continue;
        m_hNCluSens[s]->fill(m_nClustersPerSensor[s]);
      }
    }   
  }

} 

//=============================================================================
// Monitoring for large events with possibly >10k clusters
//  - nClustersDefault is the number of clusters found by the default decoding
//=============================================================================
void Velo::VeloClusterMonitor::fillHighMultiplicity(unsigned int nClustersDefault) {

  if ( nClustersDefault > 0 ) {
    // Use the default decoding without checking anything if non-zero
    m_hNClustersHM->fill(nClustersDefault);
  }
  else {
    // Check if there was a processing error
    LHCb::ProcStatus* procStat = getIfExists<LHCb::ProcStatus>( LHCb::ProcStatusLocation::Default );  
    if ( NULL != procStat ) { 
      if( procStat->aborted() ) { 
        // Processing was indeed aborted. Decode raw banks
        m_hNClustersHM->fill(getNClustersFromRaw());
      }   
      else {
        // Event was failed because of something else
        m_hNClustersHM->fill(0);
      }   
    }   
    else {
      // Just an ordinary event with 0 clusters
      m_hNClustersHM->fill(0);
    }   
  }
}


//=============================================================================
// Number of clusters from RAW banks
//=============================================================================
unsigned int Velo::VeloClusterMonitor::getNClustersFromRaw() {

  unsigned int nclusters = 0;

  LHCb::RawEvent* rawEvent = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Other);
  if( NULL == rawEvent) {
    rawEvent = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  }

  // Fetch raw VELO banks
  if ( NULL != rawEvent )
  {
    const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::Velo);
  
    std::vector<LHCb::RawBank*>::const_iterator bi;
    for (bi = banks.begin(); bi != banks.end(); ++bi) {
      const LHCb::RawBank* rb = *bi;
      if (rb->magic() == LHCb::RawBank::MagicPattern && rb->version() == BANK_VERSION_v3) {

        const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>(rb->data());
        VeloRawBankDecoder decoder(rawBank);
        if ( !decoder.hasError() ) {
          nclusters += decoder.nClusters();
        }
      }
    }
    // End of loop: if there was no error, nclusters will contain the number of
    // VELO (+PU) clusters
  }
  else
  {
    warning() << "Unable to access raw event to monitor events with high "
              << "cluster multiplicity. If raw event is at a non-standard " 
              << "location, please update the code." << endmsg;
  }
  return nclusters;
}


//=============================================================================


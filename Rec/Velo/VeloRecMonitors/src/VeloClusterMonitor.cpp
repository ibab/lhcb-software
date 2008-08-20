// $Id: VeloClusterMonitor.cpp,v 1.3 2008-08-20 11:23:18 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// local
#include "VeloClusterMonitor.h"

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
  DECLARE_ALGORITHM_FACTORY( VeloClusterMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloClusterMonitor::VeloClusterMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
{
  declareProperty( "VeloClusterLocation",
                   m_clusterCont = LHCb::VeloClusterLocation::Default );
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
//  Finalize
//=============================================================================
StatusCode Velo::VeloClusterMonitor::finalize() {

  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
StatusCode Velo::VeloClusterMonitor::veloClusters() {
  
  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;

  if ( !exist<LHCb::VeloClusters>( m_clusterCont ) ) {
    return Warning( "No VeloClusters container found for this event !",
                    StatusCode::FAILURE, 100 );
  }
  else {
    m_clusters = get<LHCb::VeloClusters>( m_clusterCont );
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << m_clusters->size() <<endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloClusterMonitor::monitorClusters() {

  // Retrieve the VeloClusters
  // -------------------------
  StatusCode sc = veloClusters();
  if( sc.isFailure() ) return;

  // Number of clusters per event
  // ----------------------------
  unsigned int nclus = m_clusters -> size();
  counter( "# VeloClusters" ) += nclus;
  plot1D( nclus, "# VELO clusters", "Number of VeloClusters per event",
          -0.5, 9000.5, 9001 );

  // Loop over the VeloClusters
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {
    
    LHCb::VeloCluster* cluster = (*itVC);

    // Number of strips and total charge
  // -----------------------------------
    unsigned int nstrips = cluster -> size();    
    double adc           = cluster -> totalCharge();

    plot1D( nstrips, "Cluster size", "Number of strips per cluster",
            -0.5, 5.5, 6 );
    plot1D( adc, "Cluster ADC value", "ADC value per cluster",
            -0.5, 128*4+0.5, 128*4+1 );

    // Number of strips and total charge versus the sensor number
    // ----------------------------------------------------------
    unsigned int sensorNumber = cluster -> channelID().sensor();

    plot2D( sensorNumber, nstrips, "Cluster size vs sensor",
            "Number of strips per cluster versus sensor",
            -0.5, 131.5, -0.5, 5.5, 132, 6 );
    plot2D( sensorNumber, adc, "Cluster ADC values vs sensor",
            "Cluster ADC values versus sensor",
            -0.5, 131.5, -0.5, 128*4+0.5, 132, 128*4+1 );

    // Active chip links versus sensor number
    // --------------------------------------
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );

    unsigned int stripNumber   = cluster -> channelID().strip();
    unsigned int chipChannel   = veloSensor -> StripToChipChannel( stripNumber ); // 0 -> 2047
    unsigned int chip          = veloSensor -> ChipFromChipChannel( chipChannel ); // 0 -> 15
    unsigned int channelInChip = chipChannel % 128; // 0 -> 127
    unsigned int linkInChip    = static_cast<int>( channelInChip / 32 ); // 0 -> 3
    // number 6 introduced to have 2 bins separating every 4 links per chip
    unsigned int activeLink = chip*6 + linkInChip;
    plot2D( sensorNumber, activeLink, "Active chip links vs sensor",
            "Active chip links versus sensor",
            -0.5, 131.5, -0.5, 95.5, 132, 96 );

  }

}

//=============================================================================

// $Id: VeloClusterMonitor.cpp,v 1.4 2008-08-21 11:39:38 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// local
#include "VeloClusterMonitor.h"

// from Boost
#include <boost/lexical_cast.hpp>

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
          -0.5, 20000.5, 20001 );

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

    // Produce the R correlation plots
    // -------------------------------
    if ( sensorNumber == 0 ||
         sensorNumber == 2 ||
         sensorNumber == 4 ||
         sensorNumber == 6 ||
         sensorNumber == 8 ||
         sensorNumber == 1 ||
         sensorNumber == 3 ||
         sensorNumber == 5 ||
         sensorNumber == 7 ||
         sensorNumber == 9
         ) {
      const DeVeloRType* rSensor1 = dynamic_cast<const DeVeloRType*>( veloSensor );
      double localR1 = rSensor1 -> rOfStrip( stripNumber );
      rCorrelations( sensorNumber, localR1, sensorNumber+2 );
    }

//     if ( sensorNumber==0 && veloSensor->isR() ) {
//       const DeVeloRType* rSensor1 = dynamic_cast<const DeVeloRType*>( veloSensor );
//       double localR1 = rSensor1 -> rOfStrip( stripNumber );

//       LHCb::VeloClusters::const_iterator itVC2;
//       for ( itVC2 = m_clusters -> begin(); itVC2 != m_clusters -> end(); ++itVC2 ) {
//         LHCb::VeloCluster* cluster2 = (*itVC2);
//         unsigned int sensorNumber2 = cluster2 -> channelID().sensor();
//         const DeVeloSensor* veloSensor2 = m_veloDet -> sensor( sensorNumber2 );        
//         if (sensorNumber2==2 && veloSensor2->isR() ) {
//           const DeVeloRType* rSensor2 = dynamic_cast<const DeVeloRType*>( veloSensor2 );
//           unsigned int stripNumber2 = cluster2 -> channelID().strip(); 
//           double localR2 = rSensor2 -> rOfStrip( stripNumber2 );
          
//           plot1D( localR1-localR2, "R difference plot (02)",
//                   "R difference plot (02)", 0., 20., 40 );
//         }
//       }
//     }
  }
}

//=============================================================================
//  R correlation distributions
//=============================================================================
void Velo::VeloClusterMonitor::rCorrelations( unsigned int sensorNumber1,
                                              double localR1,
                                              unsigned int sensorNumber2 ) {

  LHCb::VeloClusters::const_iterator itVC;
  
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {
    
    unsigned int sensorNumber = (*itVC) -> channelID().sensor();
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );

    if ( sensorNumber==sensorNumber2 ) {
      const DeVeloRType* rSensor = dynamic_cast<const DeVeloRType*>( veloSensor );
      unsigned int stripNumber = (*itVC) -> channelID().strip(); 
      double localR2           = rSensor -> rOfStrip( stripNumber );

      std::string sn  = boost::lexical_cast<std::string>( sensorNumber1 )
                      + "-"
                      + boost::lexical_cast<std::string>( sensorNumber2 );

      plot1D( localR1-localR2, "Sensors R diff. (" + sn + ")",
              "Sensors R difference (sensors " + sn + ")", -10.5, 10.5, 41 );
    }
  }
}

//=============================================================================

// $Id: VeloSamplingMonitor.cpp,v 1.4 2009-04-27 13:50:32 krinnert Exp $
// Include files
// -------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Tell1Kernel
#include "Tell1Kernel/VeloDecodeConf.h" 

// local
#include "VeloSamplingMonitor.h"

// from Boost
#include <boost/assign/list_of.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloSamplingMonitor
//
// 2008-08-20 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
  DECLARE_ALGORITHM_FACTORY( VeloSamplingMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloSamplingMonitor::VeloSamplingMonitor( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
{
  m_samplingLocations =
    boost::assign::list_of("Prev7")("Prev6")("Prev5")("Prev4")("Prev3")
                          ("Prev2")("Prev1")("")("Next1")("Next2")("Next3")
                          ("Next4")("Next5")("Next6")("Next7");
  
  declareProperty( "SamplingLocations", m_samplingLocations );
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloSamplingMonitor::~VeloSamplingMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloSamplingMonitor::initialize() {
  
  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloSamplingMonitor::execute() {
  
  // Count the number of events monitored
  // ------------------------------------
  counter( "# events" ) += 1;

  // Loop over the sampling locations and produce the monitoring plots
  // -----------------------------------------------------------------
  std::vector<std::string>::const_iterator itL;
  
  for ( itL = m_samplingLocations.begin();
        itL != m_samplingLocations.end(); ++itL ) {
    LHCb::VeloClusters* clusters = veloClusters( (*itL) );
    if( clusters == 0 ) continue;
    
    monitorClusters( (*itL), clusters );
  }
  
  for ( itL = m_samplingLocations.begin();
        itL != m_samplingLocations.end(); ++itL ) {
    LHCb::VeloTELL1Datas* tell1Datas = veloTell1Data( (*itL) );
    if( tell1Datas == 0 ) continue;
    
    monitorTell1Data( (*itL), tell1Datas );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloSamplingMonitor::finalize() {
  
  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;
  
  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
LHCb::VeloClusters*
Velo::VeloSamplingMonitor::veloClusters( std::string samplingLocation ) {
  
  std::string tesPath = "Raw/Velo/" + samplingLocation + "/Clusters";
  size_t pos = tesPath.find( "//" );
  if ( pos != std::string::npos )
    tesPath.replace( pos, 2, "/" );
  
  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << tesPath << endmsg;

  if ( !exist<LHCb::VeloClusters>( tesPath ) ) {
    debug() << "No VeloClusters container found for this event !" << endmsg;
    return 0;
  }
  else {
    LHCb::VeloClusters* clusters = get<LHCb::VeloClusters>( tesPath );
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << clusters->size() <<endmsg;
    return clusters;
  }
}

//=============================================================================
// Retrieve the VeloTell1Data
//=============================================================================
LHCb::VeloTELL1Datas*
Velo::VeloSamplingMonitor::veloTell1Data( std::string samplingLocation ) {
  
  std::string tesPath = "Raw/Velo/" + samplingLocation + "/ADCCMSuppressed";
  size_t pos = tesPath.find( "//" );
  if ( pos != std::string::npos )
    tesPath.replace( pos, 2, "/" );
  
  if ( m_debugLevel )
    debug() << "Retrieving VeloTell1Data from " << tesPath << endmsg;
  
  if ( !exist<LHCb::VeloTELL1Datas>( tesPath ) ) {
    debug() << "No VeloTell1Data container found for this event !" << endmsg; 
    return 0;
  }
  else {
    LHCb::VeloTELL1Datas* tell1Data = get<LHCb::VeloTELL1Datas>( tesPath );
    if ( m_debugLevel ) debug() << "  -> number of VeloTell1Data found in TES: "
                                << tell1Data->size() <<endmsg;
    return tell1Data;
  }
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloSamplingMonitor::monitorClusters( std::string samplingLocation,
                                                 LHCb::VeloClusters* clusters )
{
  
  unsigned int nxbins = m_samplingLocations.size();
  std::string histIDBase("ClusADCSamp");
  std::string histTitleBase("Cluster ADC values versus sampling index ");
  
  // Loop over the VeloClusters
  // ==========================
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = clusters -> begin(); itVC != clusters -> end(); ++itVC ) {
    
    LHCb::VeloCluster* cluster = (*itVC);

    // Total charge
    // ------------
    double adc = cluster -> totalCharge();

    unsigned int samplingIndex = 999;
    for( size_t i = 0; i < nxbins; ++i )
      if ( m_samplingLocations[i] == samplingLocation )
        samplingIndex = i;

    std::string histID;
    std::string histTitle;
    if ( cluster->isRType() ) {
      histID    = histIDBase + "R";
      histTitle = histTitleBase + "R";
    } else if ( cluster->isPhiType() ) {
      histID = histIDBase + "Phi";
      histTitle = histTitleBase + "Phi";
    } else {
      histID = histIDBase + "PU";
      histTitle = histTitleBase + "PU";
    } 
      
    plot2D( samplingIndex, adc, histID,
            histTitle,
            -0.5, nxbins - 0.5, -0.5, 50.5, nxbins, 51 );
   
    // C is right, A is left
    bool isCSide = m_velo->sensor(cluster->channelID().sensor())->isRight(); 
    if ( isCSide ) {
      histID = histID + "C";
      histTitle = histTitle + ", C-Side";
    } else { // A side
      histID = histID + "A";
      histTitle = histTitle + ", A-Side";
    }
    
    plot2D( samplingIndex, adc, histID,
            histTitle,
            -0.5, nxbins - 0.5, -0.5, 50.5, nxbins, 51 );
   
  }
  
}

//=============================================================================
// Monitor the VeloTell1Data
//=============================================================================
void Velo::VeloSamplingMonitor::monitorTell1Data( std::string samplingLocation,
                                                  LHCb::VeloTELL1Datas* tell1Datas ) 
{
  
  unsigned int nxbins = m_samplingLocations.size();

  // Loop over the VeloTell1Data
  // ==========================+
  LHCb::VeloTELL1Datas::const_iterator itD;
  for ( itD = tell1Datas -> begin(); itD != tell1Datas -> end(); ++itD ) {
    
    LHCb::VeloTELL1Data* data = (*itD);
    
    for( unsigned int channel = 0; channel < VeloTELL1::SENSOR_CHANNELS;
         ++channel ) {
      
      // Charge for each channel
      // -----------------------      
      signed int adc = data -> channelADC( channel );
      
      unsigned int samplingIndex = 999;
      for( size_t i = 0; i < nxbins; ++i )
        if ( m_samplingLocations[i] == samplingLocation )
          samplingIndex = i;

      plot2D( samplingIndex, adc, "ChanADCSamp",
              "Channel ADC values versus sampling index",
              -0.5, nxbins - 0.5, -0.5, 50.5, nxbins, 51 );
    }
  }
  
}

//=============================================================================

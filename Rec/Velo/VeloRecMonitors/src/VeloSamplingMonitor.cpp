// Include files
// -------------

// from Gaudi
#include "GaudiUtils/Aida2ROOT.h"

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
  DECLARE_ALGORITHM_FACTORY( VeloSamplingMonitor )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloSamplingMonitor::VeloSamplingMonitor( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
{
  std::vector<std::string> tmp =
    boost::assign::list_of("Prev7")("Prev6")("Prev5")("Prev4")("Prev3")
                          ("Prev2")("Prev1")("")("Next1")("Next2")("Next3")
                          ("Next4")("Next5")("Next6")("Next7");
  
  declareProperty( "SamplingLocations", m_samplingLocations = tmp);
  declareProperty( "UseNZS", m_useNZS=true );
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

  unsigned int nxbins = m_samplingLocations.size();
  std::string histIDBase("ClusADCSamp");
  std::string histTitleBase("Cluster ADC values versus sampling index ");
  m_hClusADCSampR = book2D( histIDBase + "R", histTitleBase + "R", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampRC = book2D( histIDBase + "RC", histTitleBase + "R, C-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampRA = book2D( histIDBase + "RA", histTitleBase + "R, A-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampPhi = book2D( histIDBase + "Phi", histTitleBase + "Phi", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampPhiC = book2D( histIDBase + "PhiC", histTitleBase + "Phi, C-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampPhiA = book2D( histIDBase + "PhiA", histTitleBase + "Phi, A-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampPU = book2D( histIDBase + "PU", histTitleBase + "PU", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampPUC = book2D( histIDBase + "PUC", histTitleBase + "PU, C-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampPUA = book2D( histIDBase + "PUA", histTitleBase + "PU, A-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampTop = book2D( histIDBase + "T", histTitleBase + "Top", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampTopC = book2D( histIDBase + "TC", histTitleBase + "Top, C-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampTopA = book2D( histIDBase + "TA", histTitleBase + "Top, A-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampBot = book2D( histIDBase + "B", histTitleBase + "Bottom", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampBotC = book2D( histIDBase + "BC", histTitleBase + "Bottom, C-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  m_hClusADCSampBotA = book2D( histIDBase + "BA", histTitleBase + "Bottom, A-Side", -0.5, nxbins-0.5, nxbins, -0.5, 50.5, 51);
  if ( m_useNZS ) {
    m_hChanADCSamp = book2D( "ChanADCSamp", "Channel ADC values versus sampling index",
                             -0.5, nxbins - 0.5, nxbins, -0.5, 50.5, 51 );
  }
  
  m_histClusADCSampAll = Gaudi::Utils::Aida2ROOT::aida2root(
      book2D("ClusADCSampAll", "Cluster ADC vs. Sampling Index",-0.5, nxbins - 0.5, nxbins, -0.5, 50.5, 51)); 
  m_histTaeADCDiffNext = Gaudi::Utils::Aida2ROOT::aida2root(
      book1D("TAEADCDiffNext", "ADC MPV Difference to next", -0.5, nxbins - 0.5, nxbins)); 
  m_histTaeADCDiffPrev = Gaudi::Utils::Aida2ROOT::aida2root(
      book1D("TAEADCDiffPrev", "ADC MPV Difference to prev.", -0.5, nxbins - 0.5, nxbins));
  m_histTaeADCDiffDiff = Gaudi::Utils::Aida2ROOT::aida2root(
      book1D("TAEADCDiffDiff", "ADC MPV Difference, next - prev.", -0.5, nxbins - 0.5, nxbins));

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

  monitorTAEDiff();
  
  if ( m_useNZS ) {
    for ( itL = m_samplingLocations.begin();
        itL != m_samplingLocations.end(); ++itL ) {
      LHCb::VeloTELL1Datas* tell1Datas = veloTell1Data( (*itL) );
      if( tell1Datas == 0 ) continue;

      monitorTell1Data( (*itL), tell1Datas );
    }
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

  LHCb::VeloClusters* clusters = getIfExists<LHCb::VeloClusters>( tesPath );
  if ( NULL == clusters ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "No VeloClusters container found for this event !" << endmsg;
  }
  else {
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << clusters->size() <<endmsg;
  }
  return clusters;
}

//=============================================================================
// Retrieve the VeloTell1Data
//=============================================================================
LHCb::VeloTELL1Datas*
Velo::VeloSamplingMonitor::veloTell1Data( std::string samplingLocation ) {
  
  std::string tesPath = "Raw/Velo/" + samplingLocation + "/DecodedADC";
  size_t pos = tesPath.find( "//" );
  if ( pos != std::string::npos )
    tesPath.replace( pos, 2, "/" );
  
  if ( m_debugLevel )
    debug() << "Retrieving VeloTell1Data from " << tesPath << endmsg;
  
  LHCb::VeloTELL1Datas* tell1Data = getIfExists<LHCb::VeloTELL1Datas>( tesPath );
  if ( NULL == tell1Data ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "No VeloTell1Data container found for this event !" << endmsg; 
  }
  else {
    if ( m_debugLevel ) debug() << "  -> number of VeloTell1Data found in TES: "
                                << tell1Data->size() <<endmsg;
  }
  return tell1Data;
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloSamplingMonitor::monitorClusters( std::string samplingLocation,
                                                 LHCb::VeloClusters* clusters )
{
  // Loop over the VeloClusters
  // ==========================
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = clusters -> begin(); itVC != clusters -> end(); ++itVC ) {
    
    LHCb::VeloCluster* cluster = (*itVC);
    double adc = cluster -> totalCharge();
    unsigned int sensorNumber = cluster->channelID().sensor();
    bool isCSide = m_velo->sensor(sensorNumber)->isRight(); 

    unsigned int samplingIndex = 999;
    for( size_t i = 0; i < m_samplingLocations.size(); ++i )
      if ( m_samplingLocations[i] == samplingLocation )
        samplingIndex = i;

    m_histClusADCSampAll->Fill(samplingIndex, adc);

    if ( cluster->isRType() ) {
      m_hClusADCSampR->fill(samplingIndex, adc);
      if ( isCSide ) {
        m_hClusADCSampRC->fill(samplingIndex, adc);
      } else {
        m_hClusADCSampRA->fill(samplingIndex, adc);
      }
    } else if ( cluster->isPhiType() ) {
      m_hClusADCSampPhi->fill(samplingIndex, adc);
      if ( isCSide ) {
        m_hClusADCSampPhiC->fill(samplingIndex, adc);
      } else {
        m_hClusADCSampPhiA->fill(samplingIndex, adc);
      }
    } else {
      m_hClusADCSampPU->fill(samplingIndex, adc);
      if ( isCSide ) {
        m_hClusADCSampPUC->fill(samplingIndex, adc);
      } else {
        m_hClusADCSampPUA->fill(samplingIndex, adc);
      }
    } 
      

    if (m_velo->sensor(sensorNumber)->isTop()) {
      m_hClusADCSampTop->fill(samplingIndex, adc);
      if ( isCSide ) {
        m_hClusADCSampTopC->fill(samplingIndex, adc);
      } else {
        m_hClusADCSampTopA->fill(samplingIndex, adc);
      }
    } else {
      m_hClusADCSampBot->fill(samplingIndex, adc);
      if ( isCSide ) {
        m_hClusADCSampBotC->fill(samplingIndex, adc);
      } else {
        m_hClusADCSampBotA->fill(samplingIndex, adc);
      }
    }
       
  }
  
}

//=============================================================================
// Monitor difference in MPV of ADC between TAEs
//=============================================================================
void Velo::VeloSamplingMonitor::monitorTAEDiff()
{
  unsigned int nxbins = m_histClusADCSampAll->GetNbinsX();

  m_histTaeADCDiffNext->Reset();
  m_histTaeADCDiffPrev->Reset();
  m_histTaeADCDiffDiff->Reset();
  for (unsigned int bin=1; bin<=nxbins; ++bin) {
    TH1D* hpy = m_histClusADCSampAll->ProjectionY("hpy",bin,bin);
    if ( !(0.0 < hpy->GetEntries()) ) {
      continue;
    }
    double mpv = hpy->GetBinCenter(hpy->GetMaximumBin());

    // difference to next TAE
    double mpvNext = 0.0;
    hpy = m_histClusADCSampAll->ProjectionY("hpy",bin+1,bin+1);
    if ( bin != nxbins && 0.0 < hpy->GetEntries() ) {
      mpvNext = hpy->GetBinCenter(hpy->GetMaximumBin());
      m_histTaeADCDiffNext->SetBinContent(bin,mpv - mpvNext);
    }

    // difference to previous TAE
    double mpvPrev = 0.0;
    hpy = m_histClusADCSampAll->ProjectionY("hpy",bin-1,bin-1);
    if ( 1 != bin && 0.0 < hpy->GetEntries() ) {
      mpvPrev = hpy->GetBinCenter(hpy->GetMaximumBin());
      m_histTaeADCDiffPrev->SetBinContent(bin,mpv - mpvPrev);
    }

    // difference of differences
    m_histTaeADCDiffDiff->SetBinContent(bin, mpvNext-mpvPrev);
  }
  
}

//=============================================================================
// Monitor the VeloTell1Data
//=============================================================================
void Velo::VeloSamplingMonitor::monitorTell1Data( std::string samplingLocation,
                                                  LHCb::VeloTELL1Datas* tell1Datas ) 
{
  // Loop over the VeloTell1Data
  // ==========================+
  LHCb::VeloTELL1Datas::const_iterator itD;
  for ( itD = tell1Datas -> begin(); itD != tell1Datas -> end(); ++itD ) {
    
    LHCb::VeloTELL1Data* data = (*itD);
    
    std::pair< std::vector<int>::const_iterator, std::vector<int>::const_iterator > linkADCs;
    for( unsigned int link = 0; link < 64; ++link) {
    
      //std::cout << "--->> A LINK # " << link << std::endl;

       linkADCs = (*data)[link];

       for ( std::vector<int>::const_iterator adc=linkADCs.first;
            adc != linkADCs.second;
            ++adc ) {
         unsigned int samplingIndex = 999;
         for( size_t i = 0; i < m_samplingLocations.size(); ++i )
           if ( m_samplingLocations[i] == samplingLocation )
             samplingIndex = i;

         m_hChanADCSamp->fill(samplingIndex, *adc);
       }
    }
  }
  
}

//=============================================================================

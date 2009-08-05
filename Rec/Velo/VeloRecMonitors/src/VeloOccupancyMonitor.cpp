// $Id: VeloOccupancyMonitor.cpp,v 1.1 2009-08-05 15:00:44 krinnert Exp $
// Include files 
// -------------

#include <string>
#include <fstream>

/// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiUtils/Aida2ROOT.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/VeloTell1Channel.h"

// local
#include "VeloOccupancyMonitor.h"

// from Boost
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloOccupancyMonitor
//
// VELO occupancy monitoring algorithm.
// Produces a set of histograms from the cluster banks in the TES.
// Can also be configured to produce strip masking conditions XML. 
//
// 2009-08-04 : Kurt Rinnert
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
  DECLARE_ALGORITHM_FACTORY( VeloOccupancyMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloOccupancyMonitor::VeloOccupancyMonitor( const std::string& name,
    ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
  , m_occupancyDenom(0)
{

  declareProperty( "VeloClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "OccupancyResetFrequency", m_occupancyResetFreq=10000 );
  declareProperty( "WriteXML", m_writeXML = false );
  declareProperty( "XMLDirectory", m_xmlDir = "." );
  declareProperty( "ParamName", m_paramName = "strip_mask" );
  declareProperty( "HighOccCut", m_highOccCut = 1.0 );

}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloOccupancyMonitor::~VeloOccupancyMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::initialize() {

  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  // try to find TAE sample name in algo instance name (for histo titles)
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

  // find the largest sensor number from detector element (to avoid making any assumptions
  // about the sensor nubmering scheme)
  unsigned int maxSensNum = 0;
  for ( std::vector<DeVeloSensor*>::const_reverse_iterator si = m_veloDet->sensorsReverseBegin();
      si != m_veloDet->sensorsReverseEnd(); ++si ) {
    if ( (*si)->sensorNumber() > maxSensNum ) {
      maxSensNum = (*si)->sensorNumber();
    }
  }
  

  m_stripOccupancyHistPerSensor.resize(maxSensNum+1,0);
  m_channelOccupancyHistPerSensor.resize(maxSensNum+1,0);

  for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
      si != m_veloDet->sensorsEnd();
      ++si ) {
    unsigned int s = (*si)->sensorNumber();

    boost::format fmtName ( "OccPerStripSens%d" ) ;
    fmtName % s;
    boost::format fmtTitle ( "Strip Occupancy, Sensor %d, " ) ;
    fmtTitle % s;

    m_stripOccupancyHistPerSensor[s] = Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtName.str(), fmtTitle.str()+m_tae, -0.5, 2047.5, 2048)); 

    boost::format fmtNameCh ( "OccPerChannelSens%d" ) ;
    fmtNameCh % s;
    boost::format fmtTitleCh ( "Channel Occupancy, Sensor %d, " ) ;
    fmtTitleCh % s;

    m_channelOccupancyHistPerSensor[s] = Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtNameCh.str(), fmtTitleCh.str()+m_tae, -0.5, 2047.5, 2048)); 
  }
  m_histOccSpectAll = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectAll", "Occupancy Spectrum", -0.5, 100.5, 202)); 
  m_histOccSpectLow = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectMaxLow", "Occupancy Spectrum", -0.5, 20.5, 210));
  m_histAvrgSensor  = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSens", "Avrg. Occupancy vs. Sensor", -0.5, 131.5, 132));

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::execute() {

  // Increment the occpancy denominator, reset at configurable
  // frequency
  if ( 0 == m_occupancyDenom % m_occupancyResetFreq ) {
    m_occupancyDenom = 1;
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
      unsigned int s = (*si)->sensorNumber();
      m_stripOccupancyHistPerSensor[s]->Reset();
      m_channelOccupancyHistPerSensor[s]->Reset();
    }
    m_histOccSpectAll->Reset();
    m_histOccSpectLow->Reset();
  } else {
    ++m_occupancyDenom;
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
      unsigned int s = (*si)->sensorNumber();
      m_stripOccupancyHistPerSensor[s]->Scale((m_occupancyDenom-1.0)/m_occupancyDenom); 
      m_channelOccupancyHistPerSensor[s]->Scale((m_occupancyDenom-1.0)/m_occupancyDenom); 
    }
  }

  monitorOccupancy();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::finalize() {

  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;


  // create conditions and write them to XML, if requested
  if ( m_writeXML ) {
    std::string fileNameBase = m_xmlDir + "/strip_mask_sensor_";
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
      unsigned int sensorNumber = (*si)->sensorNumber();

      std::vector<int> masks(2048,0);
      TH1D* occHist = m_channelOccupancyHistPerSensor[sensorNumber];
      for ( unsigned int chipChannel = 0; chipChannel < 2048; ++chipChannel ) {
        double occ = occHist->GetBinContent(chipChannel+1);
        if ( occ > m_highOccCut ) {
          masks[Velo::chipChannelToTell1Channel(chipChannel)] = 1;
        }
      }
      
      Condition sensorCond;
      sensorCond.addParam(m_paramName, masks);
      
      std::string sensorNumberStr = boost::lexical_cast<std::string>(sensorNumber);
      std::string fileName = fileNameBase + sensorNumberStr + ".xml";
      std::string condName = "VeloSensor" + sensorNumberStr;
      std::ofstream xmlFile(fileName.c_str());
      xmlFile << sensorCond.toXml(condName, true) << std::endl;
      xmlFile.close();
        
    }
  }
  
  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::veloClusters() {

  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;

  if ( !exist<LHCb::VeloClusters>( m_clusterCont ) ) {
    debug() << "No VeloClusters container found for this event !" << endmsg;
    return StatusCode::FAILURE;
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
void Velo::VeloOccupancyMonitor::monitorOccupancy() {

  // Retrieve the VeloClusters
  // -------------------------
  StatusCode sc = veloClusters();
  if( sc.isFailure() ) return;


  // Loop over the VeloClusters
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {

    LHCb::VeloCluster* cluster = (*itVC);

    unsigned int sensorNumber = cluster -> channelID().sensor();
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );
    unsigned int stripNumber   = cluster -> channelID().strip();
    unsigned int chipChannel   = veloSensor -> StripToChipChannel( stripNumber ); // 0 -> 2047

    // Produce occupancy plots
    // -----------------------
    TH1D* occHist = 0;
    TH1D* occHistCh = 0;
    occHist = m_stripOccupancyHistPerSensor[sensorNumber];
    occHistCh = m_channelOccupancyHistPerSensor[sensorNumber];
    double occ = occHist->GetBinContent(stripNumber+1)/100.0+(1.0/m_occupancyDenom);
    occHist->SetBinContent(stripNumber+1,occ*100.0);
    occHistCh->SetBinContent(chipChannel+1,occ*100.0);

  }
    
  // Produce occupancy spectra
  // -------------------------
  m_histOccSpectAll->Reset();
  m_histOccSpectLow->Reset();
  for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
      si != m_veloDet->sensorsEnd();
      ++si ) {
    unsigned int sens = (*si)->sensorNumber();
    for ( unsigned int strip=1; strip<2049; ++strip) {
      double occ = m_stripOccupancyHistPerSensor[sens]->GetBinContent(strip); 
      m_histOccSpectAll->Fill(occ);
      if ( occ <= 20.0 ) {
        m_histOccSpectLow->Fill(occ);
      }
    }
  }

  for ( unsigned int s=0;  s<m_stripOccupancyHistPerSensor.size(); ++s ) {
    TH1D* h = m_stripOccupancyHistPerSensor[s];
    if ( 0 != h ) {
      m_histAvrgSensor->SetBinContent(s + 1, h->Integral()/h->GetNbinsX());
    }
  }

}



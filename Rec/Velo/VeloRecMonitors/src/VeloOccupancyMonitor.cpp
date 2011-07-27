// Include files 
// -------------

#include <string>
#include <fstream>
#include <algorithm>

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif // !WIN32

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
// 2009-08-18 : James Michael Keaveney
// 2009-08-04 : Kurt Rinnert
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
  DECLARE_ALGORITHM_FACTORY( VeloOccupancyMonitor )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloOccupancyMonitor::VeloOccupancyMonitor( const std::string& name,
    ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
  , m_occupancyDenom(0)
  , m_nstrips(180224) // (84 + 4) * 2048
{
  declareProperty( "WriteXML", m_writeXML = false );
  declareProperty( "XMLDirectory", m_xmlDir = "." );
  declareProperty( "ParamName", m_paramName = "strip_mask" );
  declareProperty( "HighOccCut", m_highOccCut = 1.0 );
  declareProperty( "VeloClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "OccupancyResetFrequency", m_occupancyResetFreq=10000 );
  declareProperty( "UseOdin", m_useOdin = true );
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloOccupancyMonitor::~VeloOccupancyMonitor() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::initialize() {

  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;


  // get tools needed for writing XML files
  if ( m_writeXML ) {
#ifndef WIN32
    m_tell1Map   = tool<Velo::ITELL1SensorMap>("Velo::LivDBTELL1SensorMap","Tell1Map");
    m_timeStamps = tool<Velo::ITimeStampProvider>("Velo::TimeStamps","TimeStamps");
#else
    info() << "Writing strip mask XML files is not supported on Windows!" << endmsg;
#endif // !WIN32
  }

  // access to PVSS TELL1 names
  m_pvssTell1Names = tool<Velo::IPvssTell1Names>("Velo::PvssTell1Names","PvssTell1Names");
  
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

  m_occupancies.resize(maxSensNum+1,0);
  m_occupanciesCh.resize(maxSensNum+1,0);
  m_stripOccupancyHistPerSensor.resize(maxSensNum+1,0);
  m_channelOccupancyHistPerSensor.resize(maxSensNum+1,0);
  
  if ( m_useOdin ) {
    m_veloOccVsBunchId.push_back(bookProfile1D("h_veloOccVsBunchId_ASide"
          ,"Percentage Velo Occupancy vs LHC bunch-ID (ASide)"
          ,-0.5,3563.5,3564));
    m_veloOccVsBunchId.push_back(bookProfile1D("h_veloOccVsBunchId_CSide"
          ,"Percentage Velo Occupancy vs LHC bunch-ID (CSide)"
          ,-0.5,3563.5,3564));

    m_histBCIDSpec = Gaudi::Utils::Aida2ROOT::aida2root(book1D("BCID Spectrum", "BCID Spectrum",-0.5,3563.5,3564));  

  }

  for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
      si != m_veloDet->sensorsEnd();
      ++si ) {
    unsigned int s = (*si)->sensorNumber();

    boost::format fmtName ( "OccPerStripSens%d" ) ;
    fmtName % s;
    boost::format fmtTitle ( "Strip Occupancy, Sensor %d (" ) ;
    fmtTitle % s;
    std::string title = fmtTitle.str() + m_pvssTell1Names->pvssName(s) + ") " + m_tae;

    m_stripOccupancyHistPerSensor[s] = Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtName.str(), title, -0.5, 2047.5, 2048)); 
    m_occupancies[s] = new Velo::Hist1D(m_stripOccupancyHistPerSensor[s]);

    boost::format fmtNameCh ( "OccPerChannelSens%d" ) ;
    fmtNameCh % s;
    boost::format fmtTitleCh ( "Channel Occupancy, Sensor %d (" ) ;
    fmtTitleCh % s;
    std::string titleCh = fmtTitleCh.str() + m_pvssTell1Names->pvssName(s) + ") " + m_tae;

    m_channelOccupancyHistPerSensor[s] = Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtNameCh.str(), titleCh, -0.5, 2047.5, 2048)); 
    m_occupanciesCh[s] = new Velo::Hist1D(m_channelOccupancyHistPerSensor[s]);
  }
  m_histOccSpectAll = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectAll", "Occupancy Spectrum", -0.0025, 100.0025, 20001)); 
  m_histOccSpectLow = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectMaxLow", "Occupancy Spectrum", -0.0005, 20.0005, 20001));
  m_fastHistOccSpectAll = new Velo::Hist1D(m_histOccSpectAll);
  m_fastHistOccSpectLow = new Velo::Hist1D(m_histOccSpectLow);
  m_histAvrgSensor  = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSens", "Avrg. Occupancy vs. Sensor", -0.5, 131.5, 132));
  m_histAvrgSensorPO1   = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSensPO1", "Avrg. Occupancy vs. Sensor, 1 powered", -0.5, 131.5, 132));
  m_histAvrgSensorPO11  = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSensPO11", "Avrg. Occupancy vs. Sensor, 11 powered", -0.5, 131.5, 132));

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::execute() {

  // Increment the occpancy denominator, reset at configurable
  // frequency
  if ( 0 == (m_occupancyDenom % m_occupancyResetFreq) ) {
    m_occupancyDenom = 1;
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
      unsigned int s = (*si)->sensorNumber();
      m_occupancies[s]->reset();  
      m_occupanciesCh[s]->reset();  
    }
  } else {
    ++m_occupancyDenom;
    double scale = (m_occupancyDenom-1.0)/m_occupancyDenom;
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
      unsigned int s = (*si)->sensorNumber();
      m_occupancies[s]->scale(scale);
      m_occupanciesCh[s]->scale(scale);
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

  // create conditions and write them to XML, if requested.
  // not supported on Windows.
#ifndef WIN32
  if ( m_writeXML ) {
    struct stat statbuf;
    
    mode_t perm = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    umask(~perm);
          
    if ( -1 == stat(m_xmlDir.c_str(),&statbuf) ) {
      if ( -1 == mkdir(m_xmlDir.c_str(), perm) ) {
        error() << "Could not create directory '" << m_xmlDir << "'." << endmsg;
        return StatusCode::FAILURE;
      }
    }
   
    std::string tsDir = m_xmlDir + "/" + m_timeStamps->initTime();
    if ( -1 == stat(tsDir.c_str(),&statbuf) ) {
      if ( -1 == mkdir(tsDir.c_str(), perm) ) {
        error() << "Could not create directory '" << tsDir << "'." << endmsg;
        return StatusCode::FAILURE;
      }
    }

    std::ofstream txtFile("masked_channels.txt");
    
    std::string fileNameBase = tsDir + "/strip_mask_tell1_";
    for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si ) {
      unsigned int sensorNumber = (*si)->sensorNumber();
      unsigned int tell1Number = m_tell1Map->sensorToTell1(sensorNumber);

      std::vector<int> masks(2048,0);
      TH1D* occHist = m_channelOccupancyHistPerSensor[sensorNumber];
      for ( unsigned int chipChannel = 0; chipChannel < 2048; ++chipChannel ) {
        double occ = occHist->GetBinContent(chipChannel+1);
        if ( occ > m_highOccCut ) {
          txtFile << "sensor number, chip channel --> " << sensorNumber << "\t" << chipChannel << std::endl;
          masks[Velo::chipChannelToTell1Channel(chipChannel)] = 1;
        }
      }

      Condition sensorCond;
      sensorCond.addParam(m_paramName, masks);

      std::string tell1NumberStr = boost::lexical_cast<std::string>(tell1Number);
      std::string fileName = fileNameBase + tell1NumberStr + ".xml";
      std::string condName = "VeloTELL1Board" + tell1NumberStr;
      std::ofstream xmlFile(fileName.c_str());
      xmlFile << sensorCond.toXml(condName, true) << std::endl;
      xmlFile.close();

    }
    txtFile.close();
  }
#endif // !WIN32
  
  delete m_fastHistOccSpectAll;
  delete m_fastHistOccSpectLow;

  while ( ! m_occupancies.empty() ) {
    delete m_occupancies.back();
    m_occupancies.pop_back();
    delete m_occupanciesCh.back();
    m_occupanciesCh.pop_back();
  }

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
void Velo::VeloOccupancyMonitor::veloClusters() {

  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;

  m_clusters = 0;
  if ( !exist<LHCb::VeloClusters>( m_clusterCont ) ) {
    info() << "No VeloClusters found for this event!" << endmsg;
  } else {
    m_clusters = get<LHCb::VeloClusters>( m_clusterCont );

    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
      << m_clusters->size() <<endmsg;
  }

}

//=============================================================================
// Retrieve the ODIN bank
//=============================================================================
void Velo::VeloOccupancyMonitor::getOdinBank() {
  
  if ( m_debugLevel )
    debug() << "Retrieving ODIN bank from " << LHCb::ODINLocation::Default << endmsg;

  m_odin = 0;
  if (!exist<LHCb::ODIN> (LHCb::ODINLocation::Default)) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "No ODIN bank found. Histograms involving bunch IDs disabled."<< endmsg;
  } else {
    m_odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  }

}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloOccupancyMonitor::monitorOccupancy() {

  // Retrieve the VeloClusters
  // -------------------------
  veloClusters();
  if( 0 == m_clusters ) return; // no clusters in this event

  if ( m_useOdin ) {
    getOdinBank();
  }

  std::vector<unsigned int> nCLustersOnSide(2,0);

  // Loop over the VeloClusters
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {

    LHCb::VeloCluster* cluster = (*itVC);
    unsigned int sensorNumber = cluster -> channelID().sensor();
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );
    unsigned int stripNumber   = cluster -> channelID().strip();
    unsigned int chipChannel   = veloSensor -> StripToChipChannel( stripNumber ); // 0 -> 2047

    if ( m_useOdin && 0 != m_odin ) {
      // side is either 0 (left/A) or 1 (right/C)
      ++nCLustersOnSide[(veloSensor->isRight() ? 1 : 0)];
    }

    // Produce occupancy plots
    // -----------------------
    double occ = m_occupancies[sensorNumber]->getBinContent(stripNumber+1)/100.0+(1.0/m_occupancyDenom); 
    Velo::Hist1D* occHist = m_occupancies[sensorNumber];
    Velo::Hist1D* occHistCh = m_occupanciesCh[sensorNumber];
    occHist->setBinContent(stripNumber+1,occ*100.0);
    occHistCh->setBinContent(chipChannel+1,occ*100.0);
  }//end clusters loop


  // these are only available when the ODIN bank is present
  if ( m_useOdin && 0 != m_odin ) {

    m_histBCIDSpec->Fill(m_odin->bunchId());    

    // side is either 0 (left/A) or 1 (right/C)
    for(unsigned int side=0; side<2; ++side){
      // the number of strips is the same in both halves and half
      // the total number of strips, occupancy is in %
      m_veloOccVsBunchId[side]->fill(m_odin->bunchId(),nCLustersOnSide[side]/(m_nstrips*0.5)*100.0);
    }
  }

  // Produce occupancy spectra
  // -------------------------
  m_fastHistOccSpectAll->reset();
  m_fastHistOccSpectLow->reset();
  for ( std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
      si != m_veloDet->sensorsEnd();
      ++si ) {
    unsigned int sens = (*si)->sensorNumber();
    for ( unsigned int strip=0; strip<2048; ++strip) {
      double occ = m_occupancies[sens]->getBinContent(strip+1);
      m_fastHistOccSpectAll->fillFast(occ);
      if ( occ <= 20.0 ) {
        m_fastHistOccSpectLow->fillFast(occ);
      }
    }
    // set number of entries here to avoid separate loop
    m_occupancies[sens]->setEntries(m_occupancyDenom-1); 
    m_occupanciesCh[sens]->setEntries(m_occupancyDenom-1); 
  }
  m_fastHistOccSpectAll->updateEntries();
  m_fastHistOccSpectLow->updateEntries();

  for ( unsigned int s=0;  s<m_occupancies.size(); ++s ) {
    Velo::Hist1D* occs = m_occupancies[s];
    if ( 0 != occs ) {
      double avrgOcc =  occs->integral()/2048.0;
      m_histAvrgSensor->SetBinContent(s + 1, avrgOcc);

      // power on step 1, 1 station ( 2 modules ) powered. also included in step 2.
      if ( 0 == s || 1 == s || 64 == s || 65 == s ) {
        m_histAvrgSensorPO1->SetBinContent(s + 1, avrgOcc);
        m_histAvrgSensorPO11->SetBinContent(s + 1, avrgOcc);
      }
      // power on step 2, 11 station ( 22 modules ) powered
      if (   ( 2  <= s && 9  >= s )
          || ( 66 <= s && 73 >= s )
          || ( 20 <= s && 31 >= s )
          || ( 84 <= s && 95 >= s ) ) {
        m_histAvrgSensorPO11->SetBinContent(s + 1, avrgOcc);
      }
    }
  }
  // set the number of entries to the number of events 
  m_histAvrgSensor->SetEntries(m_occupancyDenom-1);
  m_histAvrgSensorPO1->SetEntries(m_occupancyDenom-1);
  m_histAvrgSensorPO11->SetEntries(m_occupancyDenom-1);

}



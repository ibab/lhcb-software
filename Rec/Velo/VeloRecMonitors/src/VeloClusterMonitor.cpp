// $Id: VeloClusterMonitor.cpp,v 1.21 2009-06-23 12:49:26 krinnert Exp $
// Include files 
// -------------

#include <string.h>

/// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiUtils/Aida2ROOT.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// local
#include "VeloClusterMonitor.h"

// from Boost
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

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
  , m_nClustersPerSensor(256,0)
, m_occupancyDenom(0)
{
  m_rSensorNumbers = boost::assign::list_of(1)(3)(5)(7)(9)(20)(24)(26)(28)(30);
  m_phiSensorNumbers = boost::assign::list_of(65)(67)(69)(71)(73)(84)(88)(90)(92)(94);

  declareProperty( "VeloClusterLocation",
      m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "RSensorNumbersForPlots",   m_rSensorNumbers );
  declareProperty( "PhiSensorNumbersForPlots", m_phiSensorNumbers );
  declareProperty( "RCorrelationPlots", m_rCorrelationPlots=false );
  declareProperty( "PerSensorPlots", m_perSensorPlots=false );
  declareProperty( "OccupancyPlots", m_occupancyPlots=false );
  declareProperty( "OccupancyResetFrequency", m_occupancyResetFreq=10000 );
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
  
  std::string nCluTitle = "Number of VeloClusters per event, " + m_tae;
  m_hNCluEvt = book1D( "# VELO clusters", nCluTitle, -0.5, 20000.5, 2000 );
  m_hCluSize = book1D( "Cluster size", "Number of strips per cluster", -0.5, 5.5, 6 );
  m_hCluADC = book1D( "Cluster ADC value", "ADC value per cluster", -0.5, 128*4+0.5, 128*4+1 );
  m_hCluADCR = book1D( "Cluster ADC value (R)", "ADC value per cluster (R)", -0.5, 128*4+0.5, 128*4+1 );
  m_hCluADCPhi = book1D( "Cluster ADC value (Phi)", "ADC value per cluster (Phi)", -0.5, 128*4+0.5, 128*4+1 );
  m_hSeedADC = book1D( "ADC value of cluster seed strips", "ADC value of cluster seed strips", -0.5, 128*1+0.5, 128*1+1 );
  m_hSeedADCR = book1D( "ADC value of cluster seed strips (R)", "ADC value of cluster seed strips (R)", -0.5, 128*1+0.5, 128*1+1 );
  m_hSeedADCPhi = book1D( "ADC value of cluster seed strips (Phi)", "ADC value of cluster seed strips (Phi)", -0.5, 128*1+0.5, 128*1+1 );
  m_hIncADC = book1D( "ADC value of cluster include strips", "ADC value of cluster include strips", -0.5, 128*0.5+0.5, 65 );
  m_hIncADCR = book1D( "ADC value of cluster include strips (R)", "ADC value of cluster include strips (R)", -0.5, 128*0.5+0.5, 65 );
  m_hIncADCPhi = book1D( "ADC value of cluster include strips (Phi)", "ADC value of cluster include strips (Phi)", -0.5, 128*0.5+0.5, 65 );
  m_hCluSizeSens = book2D( "Cluster size vs sensor", "Number of strips per cluster versus sensor", -0.5, 131.5, 132, -0.5, 5.5, 6 );
  m_hCluADCSens = book2D( "Cluster ADC values vs sensor", "Cluster ADC values versus sensor", -0.5, 131.5, 132, 0, 515,  103 );
  m_hActiveLinkSens = book2D( "Active chip links vs sensor", "Active chip links versus sensor", -0.5, 131.5, 132, -0.5, 63.5, 64 );

  if ( m_rCorrelationPlots ) {
    for ( unsigned int s=0; s<m_rSensorNumbers.size(); ++s ) {
      std::string sn = boost::lexical_cast<std::string>( s );
      std::string hCorrName = "R-Sensors R_s(i+2)-R_s(i+4) vs R_s(i)-R_s(i+2), i=" + sn;
      m_hRCorr[s] = book2D( hCorrName, hCorrName, -10.5, 10.5, 41, -10.5, 10.5, 41 );
      sn  = sn + "-" + boost::lexical_cast<std::string>( s+2 );
      m_hRDiff[s] = book1D( "Sensors R diff. (" + sn + ")", "Sensors R difference (sensors " + sn + ")", -10.5, 10.5, 41 ); 
    }
  }
  
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
  
  if ( m_occupancyPlots ) {
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

  // Increment the occpancy denominator, reset at configurable
  // frequency
  if ( m_occupancyPlots ) {
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
  }

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
  if ( nclus > 0 && nclus < 20000 )
    m_hNCluEvt->fill(nclus);
  else if ( nclus > 0 )
    m_hNCluEvt->fill(20000);

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

    // Produce occupancy plots
    // -----------------------
    if ( m_occupancyPlots ) {
      TH1D* occHist = 0;
      TH1D* occHistCh = 0;
      occHist = m_stripOccupancyHistPerSensor[sensorNumber];
      occHistCh = m_channelOccupancyHistPerSensor[sensorNumber];
      double occ = occHist->GetBinContent(stripNumber+1)/100.0+(1.0/m_occupancyDenom);
      occHist->SetBinContent(stripNumber+1,occ*100.0);
      occHistCh->SetBinContent(chipChannel+1,occ*100.0);
    }    

    // Produce the R correlation plots
    // -------------------------------
    if ( m_rCorrelationPlots && m_rSensorNumbers.end()
        != std::find( m_rSensorNumbers.begin(), m_rSensorNumbers.end(), sensorNumber )
        && veloSensor -> isR() ) {

      const DeVeloRType* rSensor1 = dynamic_cast<const DeVeloRType*>( veloSensor );
      double localR1 = rSensor1 -> rOfStrip( stripNumber );
      rDifferences( sensorNumber, localR1, sensorNumber+2 );

      if ( sensorNumber == 24 ) rCorrelations( sensorNumber, localR1 );
    }
  }
    
  // Produce occupancy spectra
  // -------------------------
  if ( m_occupancyPlots ) {
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
  }    

  // plot number of clusters seperately for each sensor
  if ( m_perSensorPlots ) {
    for (unsigned int s=0; s<m_nClustersPerSensor.size(); ++s) {
      if ( 0 == m_nClustersPerSensor[s] ) continue;
      m_hNCluSens[s]->fill(m_nClustersPerSensor[s]);
    }
  }

  if ( m_occupancyPlots ) {
    for ( unsigned int s=0;  s<m_stripOccupancyHistPerSensor.size(); ++s ) {
      TH1D* h = m_stripOccupancyHistPerSensor[s];
      if ( 0 != h ) {
        m_histAvrgSensor->SetBinContent(s + 1, h->Integral()/h->GetNbinsX());
      }
    }
  }

}

//=============================================================================
//  R_i -R_j difference distributions
//=============================================================================
void Velo::VeloClusterMonitor::rDifferences( unsigned int sensorNumber1,
    double localR1,
    unsigned int sensorNumber2 ) {

  LHCb::VeloClusters::const_iterator itVC;

  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {

    unsigned int sensorNumber = (*itVC) -> channelID().sensor();
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );
    if ( ! veloSensor -> isR() ) continue;  // only consider R sensors!

    if ( sensorNumber==sensorNumber2 ) {
      const DeVeloRType* rSensor = dynamic_cast<const DeVeloRType*>( veloSensor );
      unsigned int stripNumber = (*itVC) -> channelID().strip(); 
      double localR2           = rSensor -> rOfStrip( stripNumber );      

      m_hRDiff[sensorNumber1]->fill( localR1-localR2 );
    }
  }
}

//=============================================================================
//  R_i -R_j difference distributions
//=============================================================================
void Velo::VeloClusterMonitor::rCorrelations( unsigned int sensorNumber1,
    double localR1 ) {

  // check whether this all makes sense ;-)!
  const DeVeloSensor* veloSensor2 = m_veloDet -> sensor( sensorNumber1+2 );
  const DeVeloSensor* veloSensor4 = m_veloDet -> sensor( sensorNumber1+4 );
  if ( ! veloSensor2 -> isR() ) return;  // only consider R sensors!
  if ( ! veloSensor4 -> isR() ) return;  // only consider R sensors!

  LHCb::VeloClusters::const_iterator itVC;

  std::vector<LHCb::VeloCluster*> clusters2;
  std::vector<LHCb::VeloCluster*> clusters4;

  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {

    unsigned int sensorNumber = (*itVC) -> channelID().sensor();

    if      ( sensorNumber == sensorNumber1+2 ) clusters2.push_back( (*itVC) );
    else if ( sensorNumber == sensorNumber1+4 ) clusters4.push_back( (*itVC) );

  }

  std::vector<LHCb::VeloCluster*>::const_iterator it2, it4;

  for ( it2 = clusters2.begin(); it2 != clusters2.end(); ++it2 ) {

    for ( it4 = clusters4.begin(); it4 != clusters4.end(); ++it4 ) {

      const DeVeloSensor* veloSensor2 = m_veloDet -> sensor( sensorNumber1 + 2 );
      const DeVeloSensor* veloSensor4 = m_veloDet -> sensor( sensorNumber1 + 4 );
      const DeVeloRType* rSensor2 = dynamic_cast<const DeVeloRType*>( veloSensor2 );
      const DeVeloRType* rSensor4 = dynamic_cast<const DeVeloRType*>( veloSensor4 );
      unsigned int stripNumber2 = (*it2) -> channelID().strip();
      unsigned int stripNumber4 = (*it4) -> channelID().strip();
      double localR2 = rSensor2 -> rOfStrip( stripNumber2 );
      double localR4 = rSensor4 -> rOfStrip( stripNumber4 );

      m_hRCorr[sensorNumber1]->fill(localR1-localR2, localR2-localR4);

    }

  }

}

//=============================================================================

// $Id: STPulseMonitor.cpp,v 1.1 2009-10-26 14:46:26 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STFun.h"

// STTELL1Event
#include "Event/STTELL1Data.h"

// DigiEvent
#include "Event/ODIN.h"
#include "Event/STCluster.h"

// STDet
#include "STDet/DeSTSector.h"

// Boost
#include "boost/lexical_cast.hpp"
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

// local
#include "STPulseMonitor.h"

using namespace LHCb;
using namespace LHCbConstants;
using namespace boost::assign;
using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY( STPulseMonitor);

//--------------------------------------------------------------------
//
//  STPulseMonitor: Extract the pulse shape from the NZS data.
//
//      More details can be found in STPulseMonitor.h
//
//--------------------------------------------------------------------

STPulseMonitor::STPulseMonitor( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  declareSTConfigProperty("ClusterLocation", m_clusterLocation, 
                          LHCb::STClusterLocation::TTClusters);
  declareSTConfigProperty("InputData" , m_dataLocation, "Raw/TT/LCMSADCs");
  declareProperty("Spills",        m_spills         = 
                  list_of("Prev2")("Prev1")("Central")("Next1")("Next2")) ;
  declareProperty("ChargeCut",     m_chargeCut      = -1.0 );// ADC counts
  declareProperty("BunchID",       m_bunchID        = 0u   );// BunchID 
  declareProperty("SkipShortThick",m_skipShortThick = true );
  declareProperty("DirNames",      m_dirNames       = list_of("all") );
  
}

StatusCode STPulseMonitor::initialize()
{
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  // initialize event counter
  m_evtNumber = 0;

  return StatusCode::SUCCESS;
}

StatusCode STPulseMonitor::execute()
{
  m_evtNumber++;

  // Select the correct bunch id
  const ODIN* odin = get<ODIN> ( ODINLocation::Default );
  if( m_bunchID != 0 &&
      odin->bunchId() != m_bunchID ) return StatusCode::SUCCESS;
  counter("Number of TED events") += 1;

  // Get the NZS data for the 5 different spills
  std::vector<STTELL1Datas*> data;
  for( int iSpill = 0; iSpill < 5; ++iSpill ) {
    std::string spillName = m_spills[iSpill];
    if( spillName == "Central" ) spillName = "";
    else                         spillName = "/" + spillName;
    std::string dataLocation = "/Event" + spillName + "/" + m_dataLocation;
    data += get<STTELL1Datas>(dataLocation) ;
  }
  
  // Check cluster location exists
  if(!exist<LHCb::STClusters>(m_clusterLocation)) return StatusCode::SUCCESS;

  // Loop over the clusters
  LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);
  LHCb::STClusters::const_iterator itClus;
  for(itClus = clusters->begin(); itClus != clusters->end(); ++itClus) {

    // Select clusters above a certain threshold
    const LHCb::STCluster* cluster = (*itClus);
    if( cluster->totalCharge() < m_chargeCut ) continue;

    // Get the DetectorElement
    DeSTSector* sector = findSector( cluster->channelID());

    // find which ladders are short and thick.. and skip those...
    if ( m_skipShortThick &&
        sector->thickness() > 350*um && sector->type() == "Short" ) continue;

    // Get the sourceID of the Tell1 for this cluster
    STChannelID chanID = cluster->channelID();
    int sourceID = cluster->sourceID();

    // Get the noise corresponding to this cluster
    double noise = sector->noise( chanID );

    // Loop over the strips in this cluster and find the highest ADC
    unsigned int highestADC = 0;
    int highestStrip = 0;
    bool foundBadChannel = false;
    STCluster::ADCVector adcVec = cluster->stripValues();
    STCluster::ADCVector::const_iterator iVec = adcVec.begin();
    for( ; iVec < adcVec.end(); ++iVec ) {
      if( iVec->second > highestADC ) {
        highestStrip = iVec->first;
        highestADC = iVec->second;
        // Check for bad channels
        STChannelID iChan = STChannelID(int(chanID) + iVec->first);
        if( !findSector( iChan )->isOKStrip( iChan ) ) foundBadChannel = true;
      } 
    }
    // If there is any strip in this cluster that is bad: ignore cluster
    if( foundBadChannel ) continue;

    // Loop over the spills
    for( int iSpill = 0; iSpill < 5; ++iSpill ) {
      std::string spillName = m_spills[iSpill];
      STTELL1Data* boardData = data[iSpill]->object(sourceID);
      const STTELL1Data::Data& dataValues = boardData->data();

      // Loop over the strips in this cluster and calculate the charge in ADC
      double value = 0.0;
      double highValue = 0.0;
      STCluster::ADCVector::const_iterator iVec = adcVec.begin();
      for( ; iVec < adcVec.end(); ++iVec ) {       

        // Determine Tell1 channel of this strip
        STChannelID iChan = STChannelID(int(chanID) + iVec->first);
        STDAQ::chanPair cPair = readoutTool()->offlineChanToDAQ(iChan, 0.0);
        int iStrip = cPair.second;

        // Add the charges and find the highest charge
        value += dataValues[iStrip/nStripsInBeetle][iStrip%nStripsInBeetle];
        if( iVec->first == highestStrip ) 
          highValue =dataValues[iStrip/nStripsInBeetle][iStrip%nStripsInBeetle];
      }
      
      std::string sb = readoutTool()->serviceBox(chanID);
      std::string clusSize = boost::lexical_cast<std::string>(cluster->size());

      // Loop over the directories (can be used to subdivide the service box)
      std::vector<std::string>::const_iterator iDir = m_dirNames.begin();
      for( ; iDir != m_dirNames.end(); ++iDir ) {

        // Create the directory name. 
        std::string dir = "/" + *iDir;
        if( *iDir == "sector" ) // if it contains "sector", add sector name 
          dir += sector->nickname();
        if( *iDir == "type"   ) // if it contains "type", add sector type
          dir += sector->type();

        // Make histograms per service box
        plot1D(value, sb + dir + "/Charge" + spillName,
               "Charge " + spillName, -50.5, 150.5, 201);
        plot1D(value, sb + dir + "/Charge" + clusSize + spillName,
               "Charge " + clusSize + "-strip "+ spillName,-50.5,150.5,201);
        plot1D(highValue, sb + dir + "/HighestCharge" + spillName,
               "Highest charge " + spillName, -50.5, 150.5, 201);
        if( noise > 0.0 ) {
          plot1D(value/noise, sb + dir + "/SN" + spillName,
                 "Signal over noise " + spillName, -50.5, 150.5, 201);
          plot1D(value/noise, sb + dir + "/SN" + clusSize + spillName,
                 "Signal over noise " + clusSize + "-strip " + spillName,
                 -50.5, 150.5, 201);
          plot1D(highValue/noise, sb + dir + "/HighestSN" + spillName,
                 "Highest signal over noise " + spillName, -50.5,150.5,201);
        } 
      }
    }

    plot1D(double(cluster->totalCharge()), "ChargeOfCluster",
           "Charge of cluster", -50.5, 150.5, 201);
    if(  noise > 0.0 ) {
      plot1D(double(cluster->totalCharge())/noise, "SNofCluster",
             "Signal over noise of cluster", -50.5, 150.5, 201);
    }
    plot1D(double(cluster->size()),"SizeOfCluster", 
           "Size of cluster", 0.5, 4.5, 4 );

  } // End of cluster iterator

  plot1D(double(m_evtNumber), "NumClustersVsEvt", 
         "Number of clusters vs event number",
         -0.5, 100.5, 101, double(clusters->size()) );

  return StatusCode::SUCCESS;
}


StatusCode STPulseMonitor::finalize()
{
  return ST::HistoAlgBase::finalize();// must be called after all other actions
}

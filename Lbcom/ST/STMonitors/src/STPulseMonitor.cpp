// Gaudi
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
#include <boost/assign/std/vector.hpp>

// local
#include "STPulseMonitor.h"



using namespace LHCb;
using namespace LHCbConstants;
using namespace boost::assign;
using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY( STPulseMonitor)

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
  declareProperty("Spills",        m_spills         = { {"Prev2"},{"Prev1"},{"Central"},{"Next1"},{"Next2"} } );
  declareProperty("ChargeCut",     m_chargeCut      = -1.0 );// ADC counts
  declareProperty("BunchID",       m_bunchID               );// BunchID 
  declareProperty("SkipShortThick",m_skipShortThick = true );
  declareProperty("DirNames",      m_dirNames       = { { "all" } } );
  declareProperty("UseNZSdata",    m_useNZSdata     = true );
  
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
  const ODIN* odin = getIfExists<ODIN> ( ODINLocation::Default );
  if( odin ) {
    if( !m_bunchID.empty() && 
        std::find(m_bunchID.begin(), m_bunchID.end(), 
                  odin->bunchId()) == m_bunchID.end()) return StatusCode::SUCCESS;
  } else return Warning("No ODIN bank found", StatusCode::SUCCESS,1);

  counter("Number of selected events") += 1;

  // Get the NZS or ZS data for the different spills
  std::vector<STTELL1Datas*> dataNZS;
  std::vector<STClusters*>   dataZS;
  for( unsigned int iSpill = 0; iSpill < m_spills.size(); ++iSpill ) {
    std::string spillName = m_spills[iSpill];
    if( spillName == "Central" ) spillName.clear();
    else                         spillName.insert('/',0);
    std::string dataLocation = "/Event" + spillName + "/" + m_dataLocation;
    if( m_useNZSdata ) {
      if(!exist<STTELL1Datas>(dataLocation) ) dataNZS.push_back(0)  ;
      else dataNZS += get<STTELL1Datas>(dataLocation) ;
    } else { 
      if(!exist<STClusters>(dataLocation) ) dataZS.push_back(0)  ;
      else dataZS += get<STClusters>(dataLocation) ;
    }
    
  }
  
  const LHCb::STClusters* clusters = getIfExists<LHCb::STClusters>(m_clusterLocation);

  // Check cluster location exists
  if( NULL == clusters ) return StatusCode::SUCCESS;
  
  // Loop over the clusters
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
    for( unsigned int iSpill = 0; iSpill < m_spills.size(); ++iSpill ) {
      std::string spillName = m_spills[iSpill];

      double charge = 0.0;
      double highCharge = 0.0;

      if( m_useNZSdata ) {
        if( dataNZS[iSpill] == 0 ) continue;
        STTELL1Data* boardData = dataNZS[iSpill]->object(sourceID);
        if(boardData == 0) {
          std::string sWarning = "No valid data from source "+std::to_string(sourceID)+" in "+spillName;
          Warning(sWarning, StatusCode::SUCCESS,1).ignore();
          continue;
        }
        const STTELL1Data::Data& dataValues = boardData->data();
        // Loop over the strips in this cluster and calculate charge in ADC
        STCluster::ADCVector::const_iterator iVec = adcVec.begin();

        for( ; iVec < adcVec.end(); ++iVec ) {       
          // Determine Tell1 channel of this strip
          STChannelID iChan = STChannelID(int(chanID) + iVec->first);
          STDAQ::chanPair cPair = readoutTool()->offlineChanToDAQ(iChan, 0.0);
          int iStrip = cPair.second;
          
          // Add the charges and find the highest charge
          charge += dataValues[iStrip/nStripsInBeetle][iStrip%nStripsInBeetle];
          if( iVec->first == highestStrip ) 
            highCharge = 
              dataValues[iStrip/nStripsInBeetle][iStrip%nStripsInBeetle];
        }
      } else { // use the ZS data (i.e. use the clusters)
        if( dataZS[iSpill] == 0 ) continue;
        STClusters::iterator iclus = dataZS[iSpill]->begin();
        while( iclus != dataZS[iSpill]->end() && !(*iclus)->overlaps(cluster))
          ++iclus;
        if( iclus != dataZS[iSpill]->end() ) {
          charge = (*iclus)->totalCharge();
          highCharge = (*iclus)->maxADCValue();
        } else {
          continue; // To avoid peak at zero in Landau plot
        }
      }
      

      std::string sb = readoutTool()->serviceBox( chanID );
      std::string clusSize = std::to_string(cluster->size());

      // Loop over the directories (can be used to subdivide the service box)
      std::vector<std::string>::const_iterator iDir = m_dirNames.begin();
      for( ; iDir != m_dirNames.end(); ++iDir ) {

        // Create the directory name. 
        std::string dir = *iDir;
        if( *iDir == "sector" ) // if it contains "sector", add sector name 
          dir += sector->nickname();
        if( *iDir == "type"   ) // if it contains "type", add sector type
          dir += sector->type();

        // Make histograms for the whole detector
        plot1D(charge, dir + "/Charge" + spillName,
               "Charge " + spillName, -50.5, 150.5, 201);
        plot1D(charge, dir + "/Charge" + clusSize + spillName,
               "Charge " + clusSize + "-strip "+ spillName,-50.5,150.5,201);
        plot1D(highCharge, dir + "/HighestCharge" + spillName,
               "Highest charge " + spillName, -50.5, 150.5, 201);
        if( noise > 0.0 ) {
          plot1D(charge/noise, dir + "/SN" + spillName,
                 "Signal over noise " + spillName, -50.5, 150.5, 201);
          plot1D(charge/noise, dir + "/SN" + clusSize + spillName,
                 "Signal over noise " + clusSize + "-strip " + spillName,
                 -50.5, 150.5, 201);
          plot1D(highCharge/noise, dir + "/HighestSN" + spillName,
                 "Highest signal over noise " + spillName, -50.5,150.5,201);
        } 

        // Make histograms per service box
        plot1D(charge, sb + "/" + dir + "/Charge" + spillName,
               "Charge " + spillName, -50.5, 150.5, 201);
        plot1D(charge, sb + "/" + dir + "/Charge" + clusSize + spillName,
               "Charge " + clusSize + "-strip "+ spillName,-50.5,150.5,201);
        plot1D(highCharge, sb + "/" + dir + "/HighestCharge" + spillName,
               "Highest charge " + spillName, -50.5, 150.5, 201);
        if( noise > 0.0 ) {
          plot1D(charge/noise, sb + "/" + dir + "/SN" + spillName,
                 "Signal over noise " + spillName, -50.5, 150.5, 201);
          plot1D(charge/noise, sb + "/" + dir + "/SN" + clusSize + spillName,
                 "Signal over noise " + clusSize + "-strip " + spillName,
                 -50.5, 150.5, 201);
          plot1D(highCharge/noise, sb + "/" + dir + "/HighestSN" + spillName,
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

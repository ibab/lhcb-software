// Include files:
// STL
#include <string>
#include <vector>
// GSL
#include "gsl/gsl_math.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// LHCbKernel
#include "Kernel/VPChannelID.h"
#include "Kernel/VPDataFunctor.h"
#include "LHCbMath/LHCbMath.h"
#include "Kernel/FastClusterContainer.h"
// VPelDet
#include "VPDet/DeVP.h"
// Event
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
// Boost
#include "boost/numeric/conversion/bounds.hpp"
// Local
#include "VPClusterCreator.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VPClusterCreator
//
// 12/11/2009 : Marcin Kucharczyk
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPClusterCreator)

bool sortByChannel(LHCb::VPChannelID first,
                   LHCb::VPChannelID second) {
     return first < second;
}

//=============================================================================
// Constructor
//=============================================================================
VPClusterCreator::VPClusterCreator(const std::string& name, 
                                             ISvcLocator* pSvcLocator)
#ifdef DEBUG_HISTO
  : GaudiTupleAlg(name, pSvcLocator)
#else
  : GaudiAlgorithm(name, pSvcLocator)
#endif
  , m_vPelDet(NULL)

{
  declareProperty("InputLocation", m_inputLocation = 
                  LHCb::VPDigitLocation::VPDigitLocation);
  declareProperty("OutputLocation", m_outputLocation = 
                  LHCb::VPClusterLocation::VPClusterLocation);
  declareProperty("OutputLocationLite", m_outputLocationLite = 
                  "VP/LiteClusters");
  // ToT sum in cluster divided by this number before saving in Lite cluster
  declareProperty("scaleFactorForToTSum", m_scaleFactor = 2.0);
  // number of bits to be used for saving Fractional positions and totSum in Lite cluster
  declareProperty("NumberOfBitsInLiteCluster",m_nBits = 3); // 3 bits

  // if false then only clusters in which seed was a local maximum are saved
  declareProperty("SaveAllClusters",m_saveAllClusters = true);  
}

//=============================================================================
// Destructor
//=============================================================================
VPClusterCreator::~VPClusterCreator(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPClusterCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_maxValue = double(2 << (m_nBits-1) ) - 1;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  m_vPelDet = getDet<DeVP>(DeVPLocation::Default);
#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPClusterCreator::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Get VPDigits
  VPDigits* digitCont = get<VPDigits>(m_inputLocation);
  // Clusterization
  VPClusters* clusterCont = new VPClusters();
  VPLiteCluster::VPLiteClusters* clusterLiteCont =
                      new VPLiteCluster::VPLiteClusters();
  put(clusterCont,m_outputLocation);
  put(clusterLiteCont,m_outputLocationLite);
  StatusCode sc = createClusters(digitCont,clusterCont,clusterLiteCont);
  if(sc.isFailure()) {
    return Warning("Failed to create clusters, empty container registered", 
                   StatusCode::FAILURE);
  }
  return sc;
}


//============================================================================
// Create VPClusters
//============================================================================
StatusCode VPClusterCreator::createClusters(VPDigits* digitCont,
           VPClusters* clusterCont,
           VPLiteCluster::VPLiteClusters* clusterLiteCont)
{ // printf("VPClusterCreator::createClusters() =>\n");
  // Sort VPDigits by totValue
  std::stable_sort(digitCont->begin(),digitCont->end(),                               // sort all digits: strongest signal first
              VPDataFunctor::Greater_by_totValue<const VPDigit*>());
  // Prepare temporary digits
  std::vector<PixDigit> pixDigits;                                                    // copy only essential data into a temporary vector
  for(VPDigits::const_iterator ipd = digitCont->begin();
      ipd != digitCont->end(); ipd++) {
    PixDigit tmpDigit;
    tmpDigit.key = (*ipd)->channelID();                                                // pixel number
    tmpDigit.tot = (*ipd)->ToTValue();                                                 // amplitude
    tmpDigit.isUsed = 0;                                                               // mark all as not used by a cluster
    pixDigits.push_back(tmpDigit);
  }
  // Find clusters
  for(std::vector<PixDigit>::iterator id = pixDigits.begin();                          // search for clusters, start with strongest signals
      id != pixDigits.end(); id++) {
    PixDigit & dgt = *id;
    if(dgt.isUsed == 0) {                                                              // only consider if not already used for a cluster
      // Get 8 neighbour pixels
      const DeVPSensor* sensor = m_vPelDet->sensorOfChannel(dgt.key);
      std::vector<LHCb::VPChannelID> neighbsVec; neighbsVec.clear();
      StatusCode channelsValid;
      channelsValid = sensor->channelToNeighbours(dgt.key,neighbsVec);                 // list of adjacent channels 
      if(channelsValid) {
	      // Add central channelID
	      neighbsVec.push_back(dgt.key);                                           // add the center channel to this list
	      // Sort by channelID
        std::sort(neighbsVec.begin(),neighbsVec.end(),sortByChannel);                  // sort the list by channel number
        // Find active pixels
        std::vector<PixDigit> activePixels; activePixels.clear();
        int totSum = 0;
	      std::vector< std::pair<LHCb::VPChannelID,int> > totVec;
        totVec.clear();
        bool isMax=true;
        for(std::vector<PixDigit>::iterator idi = pixDigits.begin();
            idi != pixDigits.end(); idi++) {
          PixDigit & digit = *idi;
          for(std::vector<LHCb::VPChannelID>::iterator 
              inc = neighbsVec.begin(); inc != neighbsVec.end(); ++inc) {
            LHCb::VPChannelID nChannel = *inc;
            if( digit.key == nChannel ){
              if( digit.tot > dgt.tot ) isMax=false;
              if( digit.isUsed == 0 ){
                activePixels.push_back(digit);
                totSum += digit.tot;
                std::pair<LHCb::VPChannelID,int> totPair;
                totPair.first = digit.key;
                totPair.second = digit.tot;
                totVec.push_back(totPair);
                digit.isUsed = 1;
              }
            }
	        }
        }
        // Check if central pixel contains max ToT
        //        bool isMax = maxCentral(dgt,activePixels);
        if( m_saveAllClusters || isMax) {
          // Make VPLiteCluster & VPCluster
          LHCb::VPChannelID baryCenterChID;
          std::pair<double,double> xyFraction;
	        bool isLong;
          // Determine barycenter
	        baryCenter(activePixels,baryCenterChID,xyFraction,isLong);
	        // Scale 3 bit xyFraction & ToT
          std::pair<unsigned int,unsigned int> xyFrac = scaleFrac(xyFraction);
	        unsigned int scaledToT = scaleToT(totSum);
          // Fill new lite cluster
          const VPLiteCluster newLiteCluster(baryCenterChID,
                                                  scaledToT,
                                                  xyFrac,
                                                  isLong);
#ifdef DEBUG_HISTO
          plot2D( xyFraction.first, xyFraction.second,
                  "ClusterLinBaryCenter", "VPClusterCreator: lin. fraction of cluster barycenter",
                  0.0, 1.0, 0.0, 1.0, 15, 15);
          plot2D( xyFrac.first, xyFrac.second,
                  "ClusterDigBaryCenter", "VPClusterCreator: 3-bit fraction of cluster barycenter",
                  -0.5, 7.5, -0.5, 7.5, 8, 8);
          plot( totSum, "ChargePerCluster", "VPClusterCreator: (dig.) Charge per cluster [ADC]",
                0.5, 50.5, 50);
          plot( totVec.size(), "PixelsPerCluster", "VPClusterCreator: Number of pixels in a cluster",
                0.5, 9.5, 9);
#endif
          // printf(" totSum=%2d => scaledToT=%2d, xyFraction=[%4.2f,%4.2f] => xyFrac=[%d,%d]",
          //               totSum, scaledToT, xyFraction.first, xyFraction.second, xyFrac.first, xyFrac.second);
          // printf(" @ [%02d:%c, %02d:%03dx%03d]\n",
          //   baryCenterChID.station(), baryCenterChID.sidepos()?'R':'L',
          //   baryCenterChID.chip(), baryCenterChID.pixel_lp(), baryCenterChID.pixel_hp() );
 
          clusterLiteCont->push_back(newLiteCluster);
          // printf(" totSum=%2d => scaledToT=%2d, xyFraction=[%4.2f,%4.2f] => xyFrac=[%d,%d]",
          //               totSum, scaledToT, xyFraction.first, xyFraction.second, xyFrac.first, xyFrac.second);
          // printf(" @ [%02d:%c, %02d:%03dx%03d]\n",
          //   baryCenterChID.station(), baryCenterChID.sidepos()?'R':'L',
          //   baryCenterChID.chip(), baryCenterChID.pixel_lp(), baryCenterChID.pixel_hp() );
 
          // Fill new cluster
          VPCluster* newCluster = 
                          new VPCluster(newLiteCluster,totVec);
          // here comes a failure sometimes, because the ChannelID already exists in the KeyedContainer - need to be fixed.
          VPCluster* existCluster = clusterCont->object(baryCenterChID);
          if(existCluster==0) clusterCont->insert(newCluster, baryCenterChID);
                         else Warning("Duplicated baryChannelID in cluster container");
          // printf("\n");
      	} 
      } else {
        Warning("channelToNeighbours failure");
      }   
    }
  }
#ifdef DEBUG_HISTO
  plot(clusterCont->size(), "ClustersPerEvent", "VPClusterCreator: Clusters/event", 0.5, 4000.5, 40);
#endif
  return StatusCode::SUCCESS;
}


//============================================================================
// Calculate barycenter of the cluster
//============================================================================
void VPClusterCreator::baryCenter(std::vector<PixDigit> activePixels,
                                       LHCb::VPChannelID& baryCenterChID,
                                       std::pair<double,double>& xyFraction,
                                       bool& isLong)
{
  double sumXW = 0.0;
  double sumYW = 0.0;
  double sumZW = 0.0;
  double sumWeight = 0.0;
  for(std::vector<PixDigit>::iterator ipc = activePixels.begin();
      ipc != activePixels.end(); ipc++) {
    PixDigit dgt = *ipc;
    const DeVPSensor* sensor = m_vPelDet->sensorOfChannel(dgt.key);
    // removed pointValid check (hschindl)
    // Gaudi::XYZPoint midPoint(0.0,0.0,0.0);
    // StatusCode pointValid;
    // pointValid = sensor->channelToPoint(dgt.key,midPoint);
    Gaudi::XYZPoint midPoint = sensor->channelToPoint(dgt.key);
    // if(pointValid) {
      sumXW = sumXW + dgt.tot * midPoint.x();
      sumYW = sumYW + dgt.tot * midPoint.y();
      sumZW = sumZW + dgt.tot * midPoint.z();
      sumWeight = sumWeight + dgt.tot;
    // } else {
    //   Warning("channelToPoint failure");
    // }
    
  }
  double avX = sumXW / sumWeight;
  double avY = sumYW / sumWeight;
  double avZ = sumZW / sumWeight;
  Gaudi::XYZPoint baryCenter(avX,avY,avZ);
  StatusCode EntryValid;
  const DeVPSensor* sensor = m_vPelDet->sensorOfChannel(activePixels[0].key);
  EntryValid = sensor->pointToChannel(baryCenter,baryCenterChID,xyFraction);
  if(!EntryValid) Warning("pointToChannel failure");
  isLong = sensor->isLong(baryCenterChID);
}


//============================================================================
// Scale 3 bit xyFraction
//============================================================================
std::pair<unsigned int, unsigned int> 
     VPClusterCreator::scaleFrac(std::pair<double,double> xyFraction)
{
  std::pair<unsigned int, unsigned int> xyFrac;
  xyFrac.first = int(ceil(xyFraction.first * m_maxValue));
  xyFrac.second = int(ceil(xyFraction.second * m_maxValue));
  if(xyFrac.first > m_maxValue) xyFrac.first = int(m_maxValue);
  if(xyFrac.second > m_maxValue) xyFrac.second = int(m_maxValue);
  return xyFrac;
}


//============================================================================
// Scale 3 bit total ToT
//============================================================================
unsigned int VPClusterCreator::scaleToT(int totSum)
{
  unsigned int scaledToT = int(ceil(totSum / m_scaleFactor));
  if(scaledToT > m_maxValue) scaledToT = int(m_maxValue);
  return scaledToT;
}

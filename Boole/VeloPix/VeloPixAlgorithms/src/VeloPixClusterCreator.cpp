// $Id: VeloPixClusterCreator.cpp,v 1.3 2010-01-06 14:08:46 marcin Exp $
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
#include "Kernel/VeloPixChannelID.h"
#include "Kernel/VeloPixDataFunctor.h"
#include "LHCbMath/LHCbMath.h"
#include "Kernel/FastClusterContainer.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Event
#include "Event/VeloPixDigit.h"
#include "Event/VeloPixCluster.h"
#include "Event/VeloPixLiteCluster.h"
// Boost
#include "boost/numeric/conversion/bounds.hpp"
// Local
#include "VeloPixClusterCreator.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VeloPixClusterCreator
//
// 12/11/2009 : Marcin Kucharczyk
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixClusterCreator);

bool sortByChannel(LHCb::VeloPixChannelID first,
                   LHCb::VeloPixChannelID second) {
     return first < second;
}

//=============================================================================
// Constructor
//=============================================================================
VeloPixClusterCreator::VeloPixClusterCreator(const std::string& name, 
                                             ISvcLocator* pSvcLocator)
#ifdef DEBUG_HISTO
  : GaudiTupleAlg(name, pSvcLocator)
#else
  : GaudiAlgorithm(name, pSvcLocator)
#endif

{
  declareProperty("InputLocation", m_inputLocation = 
                  LHCb::VeloPixDigitLocation::VeloPixDigitLocation);
  declareProperty("OutputLocation", m_outputLocation = 
                  LHCb::VeloPixClusterLocation::VeloPixClusterLocation);
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
VeloPixClusterCreator::~VeloPixClusterCreator(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixClusterCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_maxValue = double(2 << (m_nBits-1) ) - 1;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  m_veloPixelDet = getDet<DeVeloPix>(DeVeloPixLocation::Default);
#ifdef DEBUG_HISTO
  setHistoTopDir("VeloPix/");
#endif
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixClusterCreator::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Get VeloPixDigits
  VeloPixDigits* digitCont = get<VeloPixDigits>(m_inputLocation);
  // Clusterization
  VeloPixClusters* clusterCont = new VeloPixClusters();
  VeloPixLiteCluster::VeloPixLiteClusters* clusterLiteCont =
                      new VeloPixLiteCluster::VeloPixLiteClusters();
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
// Create VeloPixClusters
//============================================================================
StatusCode VeloPixClusterCreator::createClusters(VeloPixDigits* digitCont,
           VeloPixClusters* clusterCont,
           VeloPixLiteCluster::VeloPixLiteClusters* clusterLiteCont)
{ // printf("VeloPixClusterCreator::createClusters() =>\n");
  // Sort VeloPixDigits by totValue
  std::stable_sort(digitCont->begin(),digitCont->end(),                               // sort all digits: strongest signal first
              VeloPixDataFunctor::Greater_by_totValue<const VeloPixDigit*>());
  // Prepare temporary digits
  std::vector<PixDigit> pixDigits;                                                    // copy only essential data into a temporary vector
  for(VeloPixDigits::const_iterator ipd = digitCont->begin();
      ipd != digitCont->end(); ipd++) {
    PixDigit tmpDigit;
    tmpDigit.key = (*ipd)->channelID();                                                // pixel number
    tmpDigit.tot = (*ipd)->ToTValue();                                                 // amplitude
    tmpDigit.isUsed = 0;                                                               // mark all as not used by a cluster
    pixDigits.push_back(tmpDigit);
  }
  LHCb::VeloPixChannelID baryCenterChID_prev = -1; // for quick clustering bug fix
  // Find clusters
  for(std::vector<PixDigit>::iterator id = pixDigits.begin();                          // search for clusters, start with strongest signals
      id != pixDigits.end(); id++) {
    PixDigit & dgt = *id;
    if(dgt.isUsed == 0) {                                                              // only consider if not already used for a cluster
      // Get 8 neighbour pixels
      const DeVeloPixSensor* sensor = m_veloPixelDet->sensor(dgt.key);
      std::vector<LHCb::VeloPixChannelID> neighbsVec; neighbsVec.clear();
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
	      std::vector< std::pair<LHCb::VeloPixChannelID,int> > totVec;
        totVec.clear();
        bool isMax=true;
        for(std::vector<PixDigit>::iterator idi = pixDigits.begin();
            idi != pixDigits.end(); idi++) {
          PixDigit & digit = *idi;
          for(std::vector<LHCb::VeloPixChannelID>::iterator 
              inc = neighbsVec.begin(); inc != neighbsVec.end(); ++inc) {
            LHCb::VeloPixChannelID nChannel = *inc;
            if( digit.key == nChannel ){
              if( digit.tot > dgt.tot ) isMax=false;
              if( digit.isUsed == 0 ){
                activePixels.push_back(digit);
                totSum += digit.tot;
                std::pair<LHCb::VeloPixChannelID,int> totPair;
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
          // Make VeloPixLiteCluster & VeloPixCluster
          LHCb::VeloPixChannelID baryCenterChID;
          std::pair<double,double> xyFraction;
	        bool isLong;
          // Determine barycenter
	        baryCenter(activePixels,baryCenterChID,xyFraction,isLong);
	        // Scale 3 bit xyFraction & ToT
          std::pair<unsigned int,unsigned int> xyFrac = scaleFrac(xyFraction);
	        unsigned int scaledToT = scaleToT(totSum);
          // Fill new lite cluster
          const VeloPixLiteCluster newLiteCluster(baryCenterChID,
                                                  scaledToT,
                                                  xyFrac,
                                                  isLong);
#ifdef DEBUG_HISTO
          plot2D( xyFraction.first, xyFraction.second,
                  "ClusterLinBaryCenter", "VeloPixClusterCreator: lin. fraction of cluster barycenter",
                  0.0, 1.0, 0.0, 1.0, 15, 15);
          plot2D( xyFrac.first, xyFrac.second,
                  "ClusterDigBaryCenter", "VeloPixClusterCreator: 3-bit fraction of cluster barycenter",
                  -0.5, 7.5, -0.5, 7.5, 8, 8);
          plot( totSum, "ChargePerCluster", "VeloPixClusterCreator: (dig.) Charge per cluster [ADC]",
                0.5, 50.5, 50);
          plot( totVec.size(), "PixelsPerCluster", "VeloPixClusterCreator: Number of pixels in a cluster",
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
          VeloPixCluster* newCluster = 
                          new VeloPixCluster(newLiteCluster,totVec);
          // here comes a failure sometimes, because the ChannelID already exists in the KeyedContainer - need to be fixed.
          if(baryCenterChID!=baryCenterChID_prev)
          { clusterCont->insert(newCluster, baryCenterChID);
            baryCenterChID_prev=baryCenterChID; }
          // printf("\n");
      	} 
      } else {
        Warning("channelToNeighbours failure");
      }   
    }
  }
#ifdef DEBUG_HISTO
  plot(clusterCont->size(), "ClustersPerEvent", "VeloPixClusterCreator: Clusters/event", 0.5, 4000.5, 40);
#endif
  return StatusCode::SUCCESS;
}


//============================================================================
// Calculate barycenter of the cluster
//============================================================================
void VeloPixClusterCreator::baryCenter(std::vector<PixDigit> activePixels,
                                       LHCb::VeloPixChannelID& baryCenterChID,
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
    const DeVeloPixSensor* sensor = 
                           m_veloPixelDet->sensor(dgt.key);
    Gaudi::XYZPoint midPoint(0.0,0.0,0.0);
    StatusCode pointValid;
    pointValid = sensor->channelToPoint(dgt.key,midPoint);
    if(pointValid) {
      sumXW = sumXW + dgt.tot * midPoint.x();
      sumYW = sumYW + dgt.tot * midPoint.y();
      sumZW = sumZW + dgt.tot * midPoint.z();
      sumWeight = sumWeight + dgt.tot;
    } else {
      Warning("channelToPoint failure");
    }
    
  }
  double avX = sumXW / sumWeight;
  double avY = sumYW / sumWeight;
  double avZ = sumZW / sumWeight;
  Gaudi::XYZPoint baryCenter(avX,avY,avZ);
  StatusCode EntryValid;
  const DeVeloPixSensor* sensor = m_veloPixelDet->sensor(activePixels[0].key);
  EntryValid = sensor->pointToChannel(baryCenter,baryCenterChID,xyFraction);
  if(!EntryValid) Warning("pointToChannel failure");
  isLong = sensor->isLong(baryCenterChID);
}


//============================================================================
// Scale 3 bit xyFraction
//============================================================================
std::pair<unsigned int, unsigned int> 
     VeloPixClusterCreator::scaleFrac(std::pair<double,double> xyFraction)
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
unsigned int VeloPixClusterCreator::scaleToT(int totSum)
{
  unsigned int scaledToT = int(ceil(totSum / m_scaleFactor));
  if(scaledToT > m_maxValue) scaledToT = int(m_maxValue);
  return scaledToT;
}



//============================================================================
StatusCode VeloPixClusterCreator::finalize() {

  return GaudiAlgorithm::finalize();

}

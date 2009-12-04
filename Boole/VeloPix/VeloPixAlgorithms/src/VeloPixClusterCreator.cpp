// $Id: VeloPixClusterCreator.cpp,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
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
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("InputLocation", m_inputLocation = 
                  "VeloPix/Digits");
  declareProperty("OutputLocation", m_outputLocation = 
                  "VeloPix/Clusters");
  declareProperty("OutputLocationLite", m_outputLocationLite = 
                  "VeloPix/LiteClusters");
  declareProperty("scaleFactor", m_scaleFactor = 2.0);
  declareProperty("nBits",m_nBits = 2u); // 3 bits
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
  m_maxValue = double(2 << m_nBits) - 1;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  m_veloPixelDet = getDet<DeVeloPix>(DeVeloPixLocation::Default);
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
  VeloPixLiteCluster::FastContainer* clusterLiteCont =
                      new VeloPixLiteCluster::FastContainer();
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
                         VeloPixLiteCluster::FastContainer* clusterLiteCont)
{
  // Sort VeloPixDigits by totValue
  std::stable_sort(digitCont->begin(),digitCont->end(),
              VeloPixDataFunctor::Greater_by_totValue<const VeloPixDigit*>());
  // Prepare temporary digits
  std::vector<pixDigit> pixDigits;
  for(VeloPixDigits::const_iterator ipd = digitCont->begin(); 
      ipd != digitCont->end(); ipd++) {
    pixDigit tmpDigit;
    tmpDigit.key = (*ipd)->channelID();
    tmpDigit.tot = (*ipd)->ToTValue();
    tmpDigit.isUsed = 0;
    pixDigits.push_back(tmpDigit);
  }
  // Find clusters
  for(std::vector<pixDigit>::iterator id = pixDigits.begin();
      id != pixDigits.end(); id++) {
    pixDigit dgt = *id;
    if(dgt.isUsed == 0) {
      // Get 8 neighbour pixels
      const DeVeloPixSensor* sensor = m_veloPixelDet->sensor(dgt.key);
      std::vector<LHCb::VeloPixChannelID> neighbsVec; neighbsVec.clear();
      StatusCode channelsValid;
      channelsValid = sensor->channelToNeighbours(dgt.key,neighbsVec);
      if(channelsValid) {
	      // Add central channelID
	      neighbsVec.push_back(dgt.key);
	      // Sort by channelID
        std::sort(neighbsVec.begin(),neighbsVec.end(),sortByChannel);
        // Find active pixels
        std::vector<pixDigit> activePixels; activePixels.clear();
        int totSum = 0;
	      std::vector< std::pair<LHCb::VeloPixChannelID,int> > totVec;
        totVec.clear();
        for(std::vector<pixDigit>::iterator idi = pixDigits.begin();
            idi != pixDigits.end(); idi++) {
          pixDigit digit = *idi;
          for(std::vector<LHCb::VeloPixChannelID>::iterator 
              inc = neighbsVec.begin(); inc != neighbsVec.end(); ++inc) {
            LHCb::VeloPixChannelID nChannel = *inc;
            if((digit.key == nChannel) && (digit.isUsed == 0)) {
              activePixels.push_back(digit);
              totSum = totSum + digit.tot;
              std::pair<LHCb::VeloPixChannelID,int> totPair;
              totPair.first = digit.key;
	            totPair.second = digit.tot;
              totVec.push_back(totPair);
              digit.isUsed = 1;
	          }
	        }
        }
        // Check if central pixel contains max ToT
        bool isMax = maxCentral(dgt,activePixels);
        if(isMax) {
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
          const VeloPixLiteCluster newLiteCluster(dgt.key,scaledToT,xyFrac,
                                                  isLong);
          clusterLiteCont->push_back(newLiteCluster);
          // Fill new cluster
          VeloPixCluster* newCluster = 
                          new VeloPixCluster(newLiteCluster,totVec);
          clusterCont->insert(newCluster,dgt.key);
      	}
      } else {
        Warning("channelToNeighbours failure");
      }   
    }
  }
  return StatusCode::SUCCESS;
}


//============================================================================
// Calculate barycenter of the cluster
//============================================================================
void VeloPixClusterCreator::baryCenter(std::vector<pixDigit> activePixels,
                                       LHCb::VeloPixChannelID& baryCenterChID,
                                       std::pair<double,double>& xyFraction,
                                       bool& isLong)
{
  double sumXW = 0.0;
  double sumYW = 0.0;
  double sumZW = 0.0;
  double sumWeight = 0.0;
  for(std::vector<pixDigit>::iterator ipc = activePixels.begin();
      ipc != activePixels.end(); ipc++) {
    pixDigit dgt = *ipc;
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
// Check if central pixel has maximum ToT
//============================================================================
bool VeloPixClusterCreator::maxCentral(pixDigit dgt,
                                       std::vector<pixDigit> activePixels)
{
  bool isMax = true;
  for(std::vector<pixDigit>::iterator ipd = activePixels.begin();
      ipd != activePixels.end(); ipd++) {
    pixDigit pixDgt = *ipd;
    if(dgt.key != pixDgt.key) {
      if(dgt.tot <= pixDgt.tot) isMax = false;
    }
  }
  return isMax;
}


//============================================================================
StatusCode VeloPixClusterCreator::finalize() {

  return GaudiAlgorithm::finalize();

}

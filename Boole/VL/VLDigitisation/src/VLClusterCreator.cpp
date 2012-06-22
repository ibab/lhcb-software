// Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel/LHCbKernel
#include "Kernel/VLChannelID.h"
#include "Kernel/VLDataFunctor.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "IStripNoiseTool.h"
#include "VLClusterCreator.h"

using namespace LHCb;

/** @file VLClusterCreator.cpp
 *
 *  Implementation of class : VLClusterCreator
 *
 */

DECLARE_ALGORITHM_FACTORY(VLClusterCreator)

//=============================================================================
/// Constructor
//=============================================================================
VLClusterCreator::VLClusterCreator(const std::string& name,
                                   ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_digits(0),
    m_clusters(0),
    m_det(0) {

  declareProperty("DigitLocation", 
                  m_digitLocation = MCVLDigitLocation::Default);
  declareProperty("ClusterLocation",
                  m_clusterLocation = VLClusterLocation::Default);

  declareProperty("SeedSignalToNoiseCut", m_seedSignalToNoiseCut = 4.5);
  declareProperty("LowSignalToNoiseCut", m_lowSignalToNoiseCut = 1.5);

  declareProperty("MaxClusters", m_maxClusters = 10000);
  declareProperty("HighThreshold", m_highThreshold = 30.0);
  declareProperty("ElectronsPerADC", m_electronsPerADC = 1200.);

  declareProperty("MCTruth", m_truth = true);

}

//=============================================================================
/// Initialisation
//=============================================================================
StatusCode VLClusterCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_debug   = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);
  if (m_debug) debug() << " ==> initialize()" << endmsg;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  m_StripNoiseTool = tool<IStripNoiseTool>("VLStripNoiseTool",
                                           "StripNoise", this);
  return StatusCode::SUCCESS;

}

//=========================================================================
///  Main execution
//=========================================================================
StatusCode VLClusterCreator::execute() {

  if (m_debug) debug() << " ==> execute()" << endmsg;
  m_clusters = new VLClusters;
  // Retrieve digits. 
  m_digits = get<MCVLDigits>(m_digitLocation);
  if (m_debug) {
    debug() << m_digits->size() << " digits in " 
            << m_digitLocation << endmsg;
  }
  // Do the clustering.
  makeClusters();
  // Sort the clusters in order of ascending sensor + strip.
  std::stable_sort(m_clusters->begin(), m_clusters->end(),
                   VLDataFunctor::LessByKey<const VLCluster*>());
  // Store the clusters on the TES.
  put(m_clusters, m_clusterLocation);
  // Do the MC truth association.
  linkClustersToHits();
  linkClustersToParticles();
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Finalisation 
//=============================================================================
StatusCode VLClusterCreator::finalize() {

  return GaudiAlgorithm::finalize();

}


//==========================================================================
/// Clustering procedure
//==========================================================================
void VLClusterCreator::makeClusters() {
  
  if (m_debug) debug() << " ==> makeClusters()" << endmsg;
  m_hitTruthTable.clear();
  m_particleTruthTable.clear();
  // Sort the digits by sensor number.
  std::stable_sort(m_digits->begin(), m_digits->end(),
                   VLDataFunctor::LessBySensor<const MCVLDigit*>());
  std::vector<DeVLSensor*>::const_iterator its; 
  for (its = m_det->sensorsBegin(); its != m_det->sensorsEnd(); ++its) {
    const unsigned int sensor = (*its)->sensorNumber();
    // Tag all channels as unused.
    m_used.clear();
    m_used.resize((*its)->numberOfStrips(), false); 
    // Retrieve the range of digits coming from this sensor.
    std::pair<MCVLDigits::iterator, MCVLDigits::iterator> range;
    getRange(sensor, range);
    if (m_debug) {
      debug() << "Retrieved " << range.second - range.first 
              << " digits from sensor " << sensor << endmsg;
    }
    // Sort the digits by increasing ADC value. 
    std::stable_sort(range.first, range.second,
                     VLDataFunctor::LessByAdc<const MCVLDigit*>());
    // Swap to decreasing ADC.
    std::reverse(range.first, range.second);
    // Loop over the digits.
    MCVLDigits::iterator itd;
    for (itd = range.first; itd != range.second; ++itd) {
      MCVLDigit* digit = *itd;
      m_candidate.clear();
      // Try to make a cluster from this digit.
      if (!makeCluster(digit)) continue;
      // Calculate the cluster centre and interstrip fraction. 
      double sum = 0.;
      double mean = 0.;
      std::vector<std::pair<int, unsigned int> > strips;
      for (unsigned int i = 0; i < m_candidate.size(); ++i) {
        const unsigned int adc = m_candidate[i]->adc();
        const int strip = m_candidate[i]->strip();
        strips.push_back(std::make_pair(strip, adc));
        sum += adc;
        mean += strip * adc;
      }
      mean /= sum;
      std::sort(strips.begin(), strips.begin());
      unsigned int centre = static_cast<unsigned int>(floor(mean));
      const double isp = mean - centre;
      // Set the key.
      VLChannelID channelID(sensor, centre, VLChannelID::Null);
      if ((*its)->isPhi()) {
        channelID.setType(VLChannelID::PhiType);
      } else {
        channelID.setType(VLChannelID::RType);
      }
      // Set the high threshold flag.
      bool highThreshold = false;
      if (sum > m_highThreshold) highThreshold = true; 
      // Make a lite cluster.
      const VLLiteCluster liteCluster(channelID, isp, highThreshold);
      // Make a cluster and add it to the container.
      VLCluster* cluster = new VLCluster(liteCluster, strips);
      m_clusters->insert(cluster, channelID);
      if (m_truth) {
        // Get the underlying MCHits.
        std::map<MCHit*, double> hitMap;
        std::map<const MCParticle*, double> particleMap;
        for (unsigned int i = 0; i < m_candidate.size(); ++i) {
          SmartRefVector<MCHit> hits = m_candidate[i]->mcHits();
          std::vector<double> weights = m_candidate[i]->mcHitsCharge();
          for (unsigned int j = 0; j < hits.size(); ++j) {
            hitMap[hits[j]] += weights[j];
            particleMap[hits[j]->mcParticle()] += weights[j];
          }
        }
        m_hitTruthTable.push_back(std::make_pair(cluster, hitMap));
        m_particleTruthTable.push_back(std::make_pair(cluster, particleMap));
      }
      if (m_clusters->size() >= m_maxClusters) {
        // Too many clusters. Stop clustering after this one.
        Warning("Too many clusters in this event. Processing incomplete.").ignore();
        info() << "Processing limit is " << m_maxClusters << " clusters." << endmsg;
        info() << "Stopping clusterization while processing sensor " 
               << sensor << endmsg;
        return;
      }
    }
    if (m_debug) {
      // Count the unused digits.
      unsigned int nUnused = 0;
      for (itd = range.first; itd != range.second; ++itd) {
        if (!m_used[(*itd)->strip()]) ++nUnused;
      }
      debug() << nUnused << " unused digits" << endmsg; 
    }
  } 

}  

//==========================================================================
/// Create cluster from digit
//==========================================================================
bool VLClusterCreator::makeCluster(MCVLDigit* digit) { 
  
  if (m_debug) debug() << " ==> makeCluster()" << endmsg;
  // Check if the channel is already used.
  if (m_used[digit->strip()]) return false; 
  // Check if the S/N ratio exceeds the seeding threshold.
  if (signalToNoise(digit) < m_seedSignalToNoiseCut) return false;
  // This seems to be a suitable seed.
  m_candidate.push_back(digit);
  // Tag the channel as used.
  m_used[digit->strip()] = true;
  // Try to add neighbouring channels in both directions.
  if (addChannel(digit, -1)) {
    if (m_debug) debug() << "Added -1 channel" << endmsg;
    // Try to also add the channel two strips away.
    if (addChannel(digit, -2)) {
      if (m_debug) debug() << "Added -2 channel" << endmsg;
    }
  }
  if (addChannel(digit, +1)) {
    if (m_debug) debug() << "Added +1 channel" << endmsg;
    // Try to also add the channel two strips away.
    if (addChannel(digit, +2)) { 
      if (m_debug) debug() << "Added +2 channel" << endmsg; 
    }
  }
  return true;

}  

//==========================================================================
/// Try to add a neighbouring channel with given offset to a cluster
//==========================================================================
bool VLClusterCreator::addChannel(MCVLDigit* digit, int offset) {
  
  if (m_debug) debug() << " ==> addChannel()" << endmsg;
  const DeVLSensor* sens = m_det->sensor(digit->sensor());
  // Check if a strip with the given offset exists and get its ID.
  VLChannelID nearbyStripId;
  StatusCode sc = sens->neighbour(digit->key(), offset, nearbyStripId);
  if (!sc) return false; 
  // Check if the nearby strip is already used.
  if (m_used[nearbyStripId.strip()]) return false;  
  // Check if the nearby strip has a hit.
  MCVLDigit* nearbyDigit = m_digits->object(nearbyStripId);
  if (!nearbyDigit) return false;

  if (abs(offset) > 2) {
    // Reject digits more than two strips away from the central one.
    return false;
  } else if (abs(offset) == 2) {
    // Get the ID of the intermediate strip.
    VLChannelID innerStripId;
    if (offset > 0) {
      sc = sens->neighbour(digit->key(),  1, innerStripId);
    } else {
      sc = sens->neighbour(digit->key(), -1, innerStripId);
    }
    if (sc.isFailure()) return false; 
    // Get the digit from the intermediate strip.
    MCVLDigit* innerDigit = m_digits->object(innerStripId); 
    if (!innerDigit) {
      Warning("Asking for a +2 digit where +1 digit does not exist!").ignore();
      return false;
    }
    // Check if the digits are monotonically decreasing in S/N. 
    if (signalToNoise(innerDigit) < signalToNoise(nearbyDigit)) return false;
  }

  // Check if the nearby digit passes the inclusing threshold.
  if (signalToNoise(nearbyDigit) < m_lowSignalToNoiseCut) {
    return false;
  }
  // Tag the channel as used.
  m_used[nearbyStripId.strip()] = true;
  // Add the nearby digit to the cluster.
  m_candidate.push_back(nearbyDigit);
  return true;

}

//=========================================================================
/// Get iterators to the first and last digit belonging to given sensor 
//=========================================================================
void VLClusterCreator::getRange(const unsigned int sensor,
                                std::pair<MCVLDigits::iterator,
                                          MCVLDigits::iterator>& r) {

  MCVLDigit cmp(VLChannelID(sensor, 0, VLChannelID::Null));
  r = std::equal_range(m_digits->begin(), m_digits->end(), &cmp,
                       VLDataFunctor::LessBySensor<const MCVLDigit*>());

}

//=========================================================================
/// Calculate the signal to noise ratio for a given digit
//=========================================================================
double VLClusterCreator::signalToNoise(MCVLDigit* digit) {

  double noise = m_StripNoiseTool->noise(digit->sensor(), digit->strip());
  noise /= m_electronsPerADC;
  if (fabs(noise) < 0.00001) noise = 0.00001;
  return static_cast<double>(digit->adc()) / noise;

}

//=========================================================================
/// Link the clusters to the underlying MC hits
//=========================================================================
void VLClusterCreator::linkClustersToHits() {

  if (m_debug) debug() << " ==> linkClustersToHits()" << endmsg;
  const std::string location = m_clusterLocation + "2MCHits";
  // Create a linker for associating clusters with MC hits.
  LinkerWithKey<MCHit, VLCluster> linker(evtSvc(), msgSvc(), location);
  std::vector<std::pair<VLCluster*, std::map<MCHit*, double> > >::iterator it;
  for (it = m_hitTruthTable.begin(); it != m_hitTruthTable.end(); ++it) {
    VLCluster* cluster = (*it).first;
    std::map<MCHit*, double>::iterator ith;
    for (ith = (*it).second.begin(); ith != (*it).second.end(); ++ith) {
      linker.link(cluster, (*ith).first, (*ith).second);
    }
  }

}

//=========================================================================
/// Link the clusters to the underlying MC hits
//=========================================================================
void VLClusterCreator::linkClustersToParticles() {

  if (m_debug) debug() << " ==> linkClustersToParticles()" << endmsg;
  const std::string location = m_clusterLocation + "2MCParticles";
  // Create a linker for associating clusters with MC particles.
  LinkerWithKey<MCParticle, VLCluster> linker(evtSvc(), msgSvc(), location);
  // TODO: merging of hits created by delta electrons with parent particle hit
  std::vector<std::pair<VLCluster*, std::map<const MCParticle*, 
                                             double> > >::iterator it;
  for (it = m_particleTruthTable.begin(); it != m_particleTruthTable.end(); ++it) {
    VLCluster* cluster = (*it).first;
    std::map<const MCParticle*, double>::iterator itp;
    for (itp = (*it).second.begin(); itp != (*it).second.end(); ++itp) {
      linker.link(cluster, (*itp).first, (*itp).second);
    }
  }

}


#ifndef __VeloClusterMaker_H__
#define __VeloClusterMaker_H__

/** @class VeloClusterMaker VeloClusterMaker.h
 *
 * Fill InternalVeloCluster, based on VeloDigit
 * Emulate Data Processor Board 
 * This is a ported version of the testbeam code version by Mat Charles 
 * (originally David Steele)
 * rewritten to use STL rather than root
 *  @author Chris Parkes, update Tomasz Szumlak
 *  @date   04/02/02
 */

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from Event
#include "VeloEvent/InternalVeloCluster.h"

// fwd declarations
class VeloDigit;

class VeloClusterMaker : public GaudiAlgorithm {

  // Constructors, destructors et al.
 public:
  /// Constructor
  VeloClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloClusterMaker( ); 
  /// Algorithm initialization
  virtual StatusCode initialize();
  /// Algorithm execution - Make Clusters
  virtual StatusCode execute   ();
  /// get S/N cut of individual hits in a detector
  float  signalToNoiseCut(int detID) const;
  /// set S/N cut of individual hits in a detector
/*    void   setSignalToNoiseCut(int detID, float newSN); */
  // get cluster S/N cut in a detector
  float  clusterSignalToNoiseCut(int detID) const;
  // get cluster S/N cut in a detector
/*    void   setClusterSignalToNoiseCut(int detID, float newSN); */

//////////////////////////////////////////////////////////////////////////////
// Private member functions
 private:

  /// make Clusters from VeloDigits
  void    makeClusters();
  /// after making all clusters for the event store them on TDS 
  StatusCode VeloClusterMaker::storeClusters();
  /// Try to make a cluster using currentDigit as the central hit
  LHCb::InternalVeloCluster* makeClusterFromDigit(
           LHCb::VeloDigit* currentDigit, float& currentClusterSTN);
  /// Try to add a neighbouring channel to the cluster
  bool TryToAddChannel(LHCb::InternalVeloCluster* currentCluster, 
                       float& currentClusterSTN,
                       LHCb::VeloDigit* currentDigit,
                       int offset);
  bool TryToAddCentralChannel(LHCb::InternalVeloCluster* currentCluster, 
                              float& currentClusterSTN,
                              LHCb::VeloDigit* currentDigit);
  /// increase the size of a cluster by adding an extra digit
  void addDigit(LHCb::InternalVeloCluster* currentCluster, 
                float& currentClusterSTN,
                LHCb::VeloDigit* nearbyDigit,
                signed int offset);
  /// perform final check that cluster is OK (S/N cut)
  bool checkCluster(LHCb::InternalVeloCluster* currentCluster,
                    float& currentClusterSTN);
  /// rejected a cluster allowing the hits in it to be used in other clusters
  void unmarkCluster(LHCb::InternalVeloCluster* currentCluster);
  ///
  std::pair<LHCb::VeloDigits::iterator, LHCb::VeloDigits::iterator> 
          getVeloDigitsOfSensor(int detId);

  // very temporary solution to calculate signal/noise ratio
  double signalToNoise(int adcValue);
  
private:

  // data members
  enum {maxVeloSensors=100};
  std::string m_inputContainer;       ///< Name of input container
  std::string m_outputContainer;      ///< Name of output container
  LHCb::VeloDigits* m_digits; ///< store digits for event considered
  std::vector<bool> m_channelUsed; ///< store channels used on current detector
  int m_sensor; // current sensor
  LHCb::InternalVeloClusters* m_clusters; ///< vector to store clusters
  /// S/N cut to apply to all detectors from job opts
  float  m_defaultSignalToNoiseCut;
  /// S/N cut for clusters to apply to all detectors from job opts
  float  m_defaultClusterSignalToNoiseCut;
  /// S/N cut for individual strips for each detector
  float  m_signalToNoiseCut[maxVeloSensors];
  /// S/N cut for clusters for each detector
  float  m_clusterSignalToNoiseCut[maxVeloSensors];
  int m_maxClusters; ///< maximum number of clusters to make per event
  float m_inclusionThreshold; ///< for adding strips to the cluster
  DeVelo* m_velo;
  
};
#endif // VeloClusterMaker_H

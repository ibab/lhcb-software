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

//////////////////////////////////////////////////////////////////////////////
// Private member functions
 private:

  /// make Clusters from VeloDigits
  void    makeClusters();
  /// after making all clusters for the event store them on TDS 
  StatusCode storeClusters();
  /// Try to make a cluster using currentDigit as the central hit
  LHCb::InternalVeloCluster* makeClusterFromDigit(
           LHCb::VeloDigit* currentDigit, double& currentClusterSTN);
  /// Try to add a neighbouring channel to the cluster
  bool TryToAddChannel(LHCb::InternalVeloCluster* currentCluster, 
                       double& currentClusterSTN,
                       LHCb::VeloDigit* currentDigit,
                       int offset);
  /// Try to start a cluster with this digit, set currentClusterSTN on return
  bool TryToAddCentralChannel(LHCb::InternalVeloCluster* currentCluster, 
                              double& currentClusterSTN,
                              LHCb::VeloDigit* currentDigit);
  /// increase the size of a cluster by adding an extra digit
  void addDigit(LHCb::InternalVeloCluster* currentCluster, 
                double& currentClusterSTN,
                LHCb::VeloDigit* nearbyDigit,
                signed int offset);
  /// rejected a cluster allowing the hits in it to be used in other clusters
  void unmarkCluster(LHCb::InternalVeloCluster* currentCluster);
  ///
  std::pair<LHCb::VeloDigits::iterator, LHCb::VeloDigits::iterator> 
          getVeloDigitsOfSensor(int detId);

  /// get the S/N for this digit
  double signalToNoise(LHCb::VeloDigit* digit);
  
private:

  // data members
  std::string m_inputContainer;       ///< Name of input container
  std::string m_outputContainer;      ///< Name of output container
  LHCb::VeloDigits* m_digits; ///< store digits for event considered
  std::vector<bool> m_channelUsed; ///< store channels used on current detector
  int m_sensor; // current sensor
  LHCb::InternalVeloClusters* m_clusters; ///< vector to store clusters
  /// S/N cut to apply to all detectors from job opts
  double  m_signalToNoiseCut;
  /// S/N cut for clusters to apply to all detectors from job opts
  double  m_clusterSignalToNoiseCut;
  int m_maxClusters; ///< maximum number of clusters to make per event
  double m_inclusionThreshold; ///< for adding strips to the cluster
  double m_highThreshold; ///< Spillover Threshold 
  DeVelo* m_velo;

  // output control
  bool m_isDebug;
  bool m_isVerbose;
  
};
#endif // VeloClusterMaker_H

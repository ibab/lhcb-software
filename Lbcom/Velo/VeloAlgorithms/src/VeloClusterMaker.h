#ifndef __VeloClusterMaker_H__
#define __VeloClusterMaker_H__

/** @class VeloClusterMaker VeloClusterMaker.h
 *
 * Fill VeloCluster, based on VeloFullDigit
 * Emulate Data Processor Board 
 * This is a ported version of the testbeam code version by Mat Charles 
 * (originally David Steele)
 * rewritten to use STL rather than root
 *  @author Chris Parkes
 *  @date   04/02/02
 */

// from Gaudi
#include "GaudiKernel/Algorithm.h"
// from Event
#include "Event/VeloCluster.h"
// fwd declarations
class VeloFullDigit;

class VeloClusterMaker : public Algorithm {

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
  /// Algorithm finalization
  virtual StatusCode finalize  ();   

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
  /// make Clusters from VeloFullDigits
  void    makeClusters();
  /// after making all clusters for the event store them on TDS 
  StatusCode VeloClusterMaker::storeClusters();

  /// Try to make a cluster using currentDigit as the central hit
  VeloCluster* makeClusterFromDigit(VeloFullDigit* currentDigit, 
                                    float& currentClusterSTN);
  /// Try to add a neighbouring channel to the cluster
  bool TryToAddChannel(VeloCluster* currentCluster, 
                       float& currentClusterSTN, 
                       VeloFullDigit* currentDigit, 
                       int offset);
  bool TryToAddCentralChannel(VeloCluster* currentCluster, 
                              float& currentClusterSTN, 
                              VeloFullDigit* currentDigit);


  /// increase the size of a cluster by adding an extra digit
  void addDigit(VeloCluster* currentCluster, 
                float& currentClusterSTN, 
                VeloFullDigit* nearbyDigit, 
                signed int offset);
  /// perform final check that cluster is OK (S/N cut)
  bool checkCluster(VeloCluster* currentCluster, float& currentClusterSTN);
  /// rejected a cluster allowing the hits in it to be used in other clusters
  void unmarkCluster(VeloCluster* currentCluster);
  /// 
  void getVeloFullDigits();
  std::pair<VeloFullDigits::iterator,VeloFullDigits::iterator> 
          getVeloFullDigitsOfSensor(int detId);

private:

  // data members
  enum {maxVeloSensors=100};

  std::string m_inputContainer;       ///< Name of input container
  std::string m_outputContainer;      ///< Name of output container
  VeloFullDigits* m_digits; ///< store digits for event considered
  std::vector<bool> m_channelUsed; ///< store channels used on current detector
  int m_sensor; // current sensor
  VeloClusters* m_clusters; ///< vector to store clusters
  /// S/N cut to apply to all detectors from job opts
  float  m_defaultSignalToNoiseCut; 

  /// S/N cut for clusters to apply to all detectors from job opts
  float  m_defaultClusterSignalToNoiseCut; 

  /// S/N cut for individual strips for each detector
  float  m_signalToNoiseCut[maxVeloSensors]; 

  /// S/N cut for clusters for each detector
  float  m_clusterSignalToNoiseCut[maxVeloSensors];

  DeVelo* m_velo; ///< Detector Element

  int m_maxClusters; ///< maximum number of clusters to make per event
  float m_inclusionThreshold; ///< for adding strips to the cluster
  
};



#endif // VeloClusterMaker_H









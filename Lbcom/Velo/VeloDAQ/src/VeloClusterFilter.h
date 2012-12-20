#ifndef VELOCLUSTERFILTER_H 
#define VELOCLUSTERFILTER_H 1

#include "Kernel/VeloChannelID.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

class DeVelo;

///< A class to filter Velo clusters according to some defined criterion
///< Accepted filters are:
///<    "Right": retains only clusters in the Velo Right
///<    "Left" : retains only clusters in the Velo Left
///<    "R"    : retains only clusters in R sensors
///<    "Phi"  : retains only clusters in Phi sensors
///<    "PU"  : retains only clusters in PU sensors

class VeloClusterFilter : public GaudiAlgorithm {
public: 
  VeloClusterFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~VeloClusterFilter( );

  virtual StatusCode initialize();
  virtual StatusCode execute   ();

  bool passesFilter(LHCb::VeloChannelID id);
  void incrementCounters(LHCb::VeloChannelID, int&, int&, int&);

protected:
  std::string m_inputLiteClusterLocation;
  std::string m_outputLiteClusterLocation;
  std::string m_inputClusterLocation;
  std::string m_outputClusterLocation;
  std::string m_filterCriterion;

  int m_minNRClustersCut;
  int m_minNPhiClustersCut;
  int m_minNClustersCut;
  int m_maxNRClustersCut;
  int m_maxNPhiClustersCut;
  int m_maxNClustersCut;
 private:
  DeVelo* m_velo;                  ///< Detector element
};
#endif // VELOCLUSTERFILTER_H

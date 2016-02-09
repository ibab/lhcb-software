// $Id: VeloSimTell1ClusterMaker.h,v 1.3 2009-12-18 08:12:33 szumlat Exp $
#ifndef VELOSIMTELL1CLUSTERMAKER_H 
#define VELOSIMTELL1CLUSTERMAKER_H 1

// Include files
#include "VeloSimTell1Algorithm.h"
#include "DetDesc/Condition.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// event
#include "VeloEvent/VeloProcessInfo.h"
#include "Event/RawEvent.h"

// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"

// kernel
#include "Tell1Kernel/BitReader.h"

/** @class VeloSimTell1ClusterMaker VeloSimTell1ClusterMaker.h
 *  
 *  The algorithm is an implementation of the tell1 cluster
 *  maker that is a part of the tell1lib library. The input 
 *  of the algorithm is the data after reordering and the output
 *  is the RawEvent structure with encoded clusters.
 *  Values of the thresholds are defined inside ClusterMakerThresholds.h
 *  the settings are most critical for the algorithm since
 *  the values are used to serch for seeding strips.
 *  At the end of the algorithm RawEvent structure with RawBanks
 *  containing zero suppressed data is created. The RawEvent is
 *  subsequently stored in TES and decoded to VeloLite and VeloClusters
 *  by VeloDAQ package.
 *
 *  @author Tomasz Szumlak
 *  @date   2009-08-06
 */

//using namespace LHCb;

class SimTell1ZSProcessEngine;

class VeloSimTell1ClusterMaker : public VeloSimTell1Algorithm{

public:

  //typedef VeloTELL1::u_int32_t buffer_word;

  enum version{
    V2=2,
    V3=3
  };

  enum positionConst{
    POS_SHIFT=3,
    IS_POS_MASK=0x7,     /// inter strip position mask
    POS_MASK=0x7FF,
    BIT12_MAX=0x1000,
    BIT13_MAX=8192,
    BIT16_MAX=65536
  };

  /// Standard constructor
  VeloSimTell1ClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~VeloSimTell1ClusterMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode runClusterMaker();
  StatusCode veloDataSuppression();
  void calculateClusterPosition(int chan, int cluSize, int firstStripInBlock);
  void addADCToList(int chan, int cluSize);
  StatusCode veloClusterization(int firstStripInBlock);
  void flushMemory();
  void fillAndWriteRawEvent();
  StatusCode storeBank();
  StatusCode createRawEvent();
  StatusCode createAndConfigureEngines();
  StatusCode i_cacheConditions();
  void addEngineInStaticMode(unsigned int tell1, const DeVeloSensor* sens);
  void addEngineInDynamicMode(unsigned int tell1);

private:

  LHCb::RawEvent* m_rawEvent;
  std::string m_rawEventLoc;
  int m_hitThresholdValue;
  int m_lowThresholdValue;
  double m_hitSigmas;
  double m_incSigmas;
  VeloTELL1::u_int32_t m_sumThresholdValue;
  VeloTELL1::ThresholdsVec m_hitThresholds;
  VeloTELL1::ThresholdsVec m_lowThresholds;
  VeloTELL1::SumThresholdsVec m_sumThresholds;
  VeloTELL1::TELL1ADCVec m_adcs;
  VeloTELL1::ADC_MEMORY m_adcsMem;   /// list of adcs contributed to the clusters
  VeloTELL1::BoundaryStripVec m_boundaryStripsR;
  VeloTELL1::BoundaryStripVec m_boundaryStripsPhi;
  unsigned int m_zsProcessEnable;
  VeloTELL1::IntCondMap m_zsEnableMap;
  VeloTELL1::ThresholdsMap m_hitThresholdMap;
  VeloTELL1::ThresholdsMap m_lowThresholdMap;
  VeloTELL1::SumThresholdsMap m_sumThresholdMap;
  VeloTELL1::BoundaryStripMap m_boundaryStripMap;
  unsigned int m_eventNumber;
  VeloTELL1::dataVec m_sensors;      /// container with source numbers
  std::vector<SiDAQ::buffer_word> m_bankBody;
  int m_bankVersion;
  unsigned int m_bankBodySize;
  bool m_printInfo;
  bool m_isDebug;
  std::map<unsigned int, std::unique_ptr<SimTell1ZSProcessEngine>> m_zsEngines;
  VeloTELL1::TELL1ClusterVec m_clusters;
  VeloTELL1::CLUSTER_MEMORY m_clustersMem;  /// list of created clusters

};
#endif // VELOSIMTELL1CLUSTERMAKER_H

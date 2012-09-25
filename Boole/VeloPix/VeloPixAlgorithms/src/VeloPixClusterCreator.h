// $Id: VeloPixClusterCreator.h,v 1.2 2010-01-06 14:08:46 marcin Exp $
#ifndef VELOPIXCLUSTERCREATOR_H
#define VELOPIXCLUSTERCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi

// #define	DEBUG_HISTO // fill some histograms while the algorithm	runs
#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif

// GSL
#include "gsl/gsl_sf_erf.h"
#include "gsl/gsl_math.h"
// Event
#include "Event/VeloPixDigit.h"
#include "Event/VeloPixCluster.h"
#include "Event/VeloPixLiteCluster.h"
#include "LHCbMath/LHCbMath.h"
// LHCbKernel
#include "Kernel/VeloPixChannelID.h"
#include "Kernel/FastClusterContainer.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"


/** @class VeloPixClusterCreator.h 
 *  VeloPixAlgorithms/VeloPixClusterCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/11/12
 */

#ifdef DEBUG_HISTO
class VeloPixClusterCreator : public GaudiTupleAlg {
#else
class VeloPixClusterCreator : public GaudiAlgorithm {
#endif

public:
  /// Standard constructor
  VeloPixClusterCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VeloPixClusterCreator();   ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalise

  class PixDigit {
  public:
    LHCb::VeloPixChannelID key;
    int tot;
    int isUsed;
  };

protected:

private:

  StatusCode createClusters(LHCb::VeloPixDigits* digitCont,
             LHCb::VeloPixClusters* clusterCont,
             LHCb::VeloPixLiteCluster::VeloPixLiteClusters* clusterLiteCont);
  void baryCenter(std::vector<PixDigit> activePixels,
                  LHCb::VeloPixChannelID& baryCenterChID,
                  std::pair<double,double>& xyFraction,
                  bool& isLong);
  std::pair<unsigned int,unsigned int> scaleFrac(
                                       std::pair<double,double> xyFraction);
  unsigned int scaleToT(int totSum);
  std::string m_inputLocation;
  std::string m_outputLocation;
  std::string m_outputLocationLite;
  bool m_saveAllClusters;  
  bool m_isDebug; 
  bool m_isVerbose;
  double m_scaleFactor;
  int m_nBits;
  double m_maxValue;
  DeVeloPix* m_veloPixelDet;
};
#endif // VeloPixClusterCreator_H

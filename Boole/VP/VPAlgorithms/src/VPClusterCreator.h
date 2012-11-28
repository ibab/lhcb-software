// $Id: VPClusterCreator.h,v 1.2 2010-01-06 14:08:46 marcin Exp $
#ifndef VPCLUSTERCREATOR_H
#define VPCLUSTERCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi

// #define DEBUG_HISTO // fill some histograms while the algorithm	runs
#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif

// GSL
#include "gsl/gsl_sf_erf.h"
#include "gsl/gsl_math.h"
// Event
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
#include "LHCbMath/LHCbMath.h"
// LHCbKernel
#include "Kernel/VPChannelID.h"
#include "Kernel/FastClusterContainer.h"
// VPelDet
#include "VPDet/DeVP.h"


/** @class VPClusterCreator.h 
 *  VPAlgorithms/VPClusterCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/11/12
 */

#ifdef DEBUG_HISTO
class VPClusterCreator : public GaudiTupleAlg {
#else
class VPClusterCreator : public GaudiAlgorithm {
#endif

public:
  /// Standard constructor
  VPClusterCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VPClusterCreator();   ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalise

  class PixDigit {
  public:
    LHCb::VPChannelID key;
    int tot;
    int isUsed;
  };

protected:

private:

  StatusCode createClusters(LHCb::VPDigits* digitCont,
             LHCb::VPClusters* clusterCont,
             LHCb::VPLiteCluster::VPLiteClusters* clusterLiteCont);
  void baryCenter(std::vector<PixDigit> activePixels,
                  LHCb::VPChannelID& baryCenterChID,
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
  DeVP* m_vPelDet;
};
#endif // VPClusterCreator_H

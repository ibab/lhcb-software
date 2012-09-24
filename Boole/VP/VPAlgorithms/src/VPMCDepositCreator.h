#// $Id: VPMCDepositCreator.h,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
#ifndef VPMCDEPOSITCREATOR_H
#define VPMCDEPOSITCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi

// #define DEBUG_HISTO // fill some histograms while the algorithm runs
#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Point3DTypes.h"
// GSL
#include "gsl/gsl_sf_erf.h"
#include "Kernel/FPEGuard.h"

class DeVP;
class MCHit;
class VPChannelID;
class ISiDepositedCharge;

/** @class VPMCDepositCreator.h 
 *  VPAlgorithms/VPMCDepositCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   20/09/09
 */

#ifdef DEBUG_HISTO
class VPMCDepositCreator : public GaudiTupleAlg {
#else
class VPMCDepositCreator : public GaudiAlgorithm {
#endif

public:
  /// Standard constructor
  VPMCDepositCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VPMCDepositCreator(); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalise

protected:

private:
  inline double safe_gsl_sf_erf_Q(double const &arg) const {
    if(fabs(arg) > 10.0) return (arg > 0.0 ? 0.0 : 1.0);
    FPE::Guard reducedFPE(FPE::Guard::mask("Inexact"),true);
    return gsl_sf_erf_Q(arg);
  }
  void createDeposits(const LHCb::MCHits* mcHitContainer, 
                      LHCb::MCVPDeposits*& depositVect);
  int simulatedPoints(LHCb::MCHit* hit);
  void chargeToPoint(LHCb::MCHit* hit, std::vector<double>& simPoints);
  void deltaRayCh(double charge, double frCh, std::vector<double>& simPoints);
  double ranomDRTail(double minDRC, double maxDRC);
  void diffuseCharge(LHCb::MCHit* hit, std::vector<double>& simPoints,
                     LHCb::MCVPDeposits*& depositCont);
  std::string m_inputLocation;
  std::string m_outputLocation;
  std::vector<std::string> m_sampleNames;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;
  std::vector<std::string> m_outPaths;
  DeVP* m_vPelDet;
  double m_siteSize;
  int m_maxNumSites;
  int m_simPointsPerPixel;
  int m_inhomogenCh;
  int m_simulationPointsPerPixel;
  double m_diffuseSigma; 
  double m_chargeUniform;
  double m_eVPerElectron;
  double m_deltaRayMinEnergy;
  double m_kT;
  double m_biasVoltage;
  std::vector<double> m_paramsInElectron;
  ISiDepositedCharge* m_depositedCharge;
  std::string m_depChTool;
  bool m_useDepTool;
  Rndm::Numbers m_gaussDist;
  Rndm::Numbers m_uniformDist;
  bool m_isDebug; 
  bool m_isVerbose;
  
};
#endif // VPMCDepositCreator_H

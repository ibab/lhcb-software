#// $Id: VeloSim.h,v 1.3 2006-02-10 14:03:31 cattanem Exp $
#ifndef VELOSIM_H
#define VELOSIM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "Kernel/Point3DTypes.h"

// from Velo
#include "Event/MCVeloFE.h"

// local
#include "pulseShapeFunctor.h"

class DeVelo;
class MCHit;
class VeloChannelID;
class pulseShapeFunctor;

/** @class VeloSim VeloSim.h VeloAlgs/MCVeloSim.h
 *
 * Fill MCVeloDigit, based on MCHit
 * In general would contain silicon and Front End Chip Simulation
 * and digitisation.
 * Currently just a fast test version.
 *  @author Chris Parkes, based on code of Oliver Callot; update Tomasz Szumlak
 *  @date   10/01/02
 */

class VeloSim : public GaudiAlgorithm {
public:
  //
  static const double k_spillOverTime=25.;
  static const double k_pulseShapePeakTime=30.7848;
  //
  /// Standard constructor
  VeloSim( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloSim( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  StatusCode getInputData();
  StatusCode simulation();
  StatusCode chargeSim(bool spillOver);
  long simPoints(LHCb::MCHit* hit);
  void chargePerPoint(LHCb::MCHit* hit, int Npoints,
                      std::vector<double>& Spoints, bool SpillOver);
  void deltaRayCharge(double charge, double tol,
                      int Npoints, std::vector<double>& Spoints);
  void diffusion(LHCb::MCHit* hit, int Npoints,
                 std::vector<double>& Spoints, bool SpillOver);
  void testSim(LHCb::MCHit* hit, bool SpillOver);
  void fillFE(LHCb::MCVeloFE* myFE, LHCb::MCHit* hit, double charge);
  void fillFE(LHCb::MCVeloFE* myFE, double charge);
  StatusCode coupling();
  LHCb::MCVeloFE* findOrInsertPrevStrip(LHCb::MCVeloFEs::iterator FEIt, 
                                        bool& valid, bool& create);
  LHCb::MCVeloFE* findOrInsertNextStrip(LHCb::MCVeloFEs::iterator FEIt, 
                                        bool& valid, bool& create);
  StatusCode pedestalSim();
  StatusCode noiseSim();
  double noiseSigma(double stripCapacitance);
  double noiseValue(double stripCapacitance);
  double noiseValueTail(double stripCapacitance);
  StatusCode CMSim();
  StatusCode VeloSim::deadStrips();
  StatusCode finalProcess();
  StatusCode storeOutputData();
  LHCb::MCVeloFE* findOrInsertFE(LHCb::VeloChannelID& stripKey);
  double spillOverReminder(double TOF);
  // data members
  std::string m_inputContainer;       ///< Name of input container  
  std::string m_spillOverInputContainer; ///< Name of spill Over event input container
  std::string m_pileUpInputContainer;  ///< Name of pileUp input container
  std::string m_pileUpSpillOverInputContainer;  ///< Name of spill Over event input cont
  std::string m_outputContainer;      ///< Name of output container
  std::string m_pileUpOutputContainer;      ///< Name of output container
  //
  DeVelo* m_veloDet; ///< Detector Element
  LHCb::MCHits* m_hits; ///< vector of input hits
  LHCb::MCHits* m_spillOverHits; ///< vector of input hits from spill over event
  LHCb::MCVeloFEs* m_FEs; ///< vector of FE output  signals
  LHCb::MCVeloFEs* m_FEs_coupling; ///< vector of coupled FE output signals
  // 
  LHCb::MCHits* m_veloHits; // vector of input hits
  LHCb::MCHits* m_pileUpHits; // vector of input hits from pileUp
  LHCb::MCHits* m_veloSpillOverHits; // vector of input hits
  LHCb::MCHits* m_pileUpSpillOverHits; // vector of input hits from pileUp
  LHCb::MCVeloFEs* m_veloFEs; ///< vector of FE output  signals
  LHCb::MCVeloFEs* m_pileUpFEs; ///< vector of FE output  signals
  //
  //  double m_baseDiffuseSigma; // diffusion sigma in microns/sqrt(thickness)
  // control simulation sections
  bool m_chargeSim;
  bool m_inhomogeneousCharge;
  bool m_coupling;
  bool m_noiseSim;
  bool m_pedestalSim;
  bool m_CMSim;
  double m_stripInefficiency; 
  // if strip Ineff. > 0 then that fraction of strips have charge removed.
  bool m_spillOver;
  bool m_pileUp;
  bool m_testSim;
  double m_smearPosition;
  // allows resolution to be degraded for robustness studies
  Gaudi::XYZPoint m_movePosition; // used when degrading resolution
  std::string m_simMode; // velo or pileup
  //gaussian random numbers
  Rndm::Numbers m_gaussDist;
  //uniform random numbers
  Rndm::Numbers m_uniformDist;
  /// random numbers from upper tail of gaussian > a
  double ran_gaussian_tail(const double a, const double sigma);
  /// random numbers from 1/E^2
  double ran_inv_E2(double Emin, double Emax);
  // previously in Velo/VeloKernel package
  double m_threshold;
  double m_noiseConstant;
  double m_kT;
  double m_biasVoltage;
  double m_eVPerElectron;
  long m_simulationPointsPerStrip;
  double m_chargeUniform;
  double m_deltaRayMinEnergy;
  double m_capacitiveCoupling;
  double m_averageStripCapacitance;
  double m_noiseCapacitance;
  double m_baseDiffuseSigma;
  std::vector<double> m_fitParams;
  double m_offPeakSamplingTime;
  
};
//
#endif // VELOSIM_H

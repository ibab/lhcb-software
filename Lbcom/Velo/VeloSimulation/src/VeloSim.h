#// $Id: VeloSim.h,v 1.9 2007-09-05 13:21:17 dhcroft Exp $
#ifndef VELOSIM_H
#define VELOSIM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Point3DTypes.h"

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
  /// Standard constructor
  VeloSim( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloSim( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

/// test if MCVeloFE charge is below m_threshold
  inline bool isLess(LHCb::MCVeloFE* fe) const  {
    return ( (fe->charge()<m_threshold)?true:false );
  }

  /// Load the event and spillover from the TES
  void getInputData(); 

  /// process requested simulation steps
  StatusCode simulation();
  
  /// Simulation of charge deposition between entry and exit of MCHit
  void chargeSim(bool spillOver);

  /// Number of points to deposite charge along the trajectory of the hit
  int simPoints(LHCb::MCHit* hit);

  /// spread the charge over the points to simulate
  void chargePerPoint(LHCb::MCHit* hit, int Npoints,
                      std::vector<double>& Spoints, bool SpillOver);

  /// add delta ray charges if required
  void deltaRayCharge(double charge, double tol,
                      int Npoints, std::vector<double>& Spoints);

  /// allow the deposited charge points to diffuse to the strips
  void diffusion(LHCb::MCHit* hit, int Npoints,
                 std::vector<double>& Spoints, bool SpillOver);

  /// add the charge to a front end channel with MChit link (if not 0)
  void fillFE(LHCb::MCVeloFE* myFE, LHCb::MCHit* hit, double charge);
  
  /// add the charge to a front end channel, no MC link
  inline void fillFE(LHCb::MCVeloFE* myFE, double charge){
    fillFE(myFE, 0, charge);
  }

  /// setup capacitive coupling
  void coupling();

  /// From an originally sorted list, find the strip with the previous key,
  /// or create a new one.
  LHCb::MCVeloFE* findOrInsertPrevStrip(LHCb::MCVeloFEs::iterator FEIt, 
                                        bool& valid, bool& create);

  /// From an originally sorted list, find the strip with the next key,
  /// or create a new one.
  LHCb::MCVeloFE* findOrInsertNextStrip(LHCb::MCVeloFEs::iterator FEIt, 
                                        bool& valid, bool& create);

  void pedestalSim(); ///< add a pedestal value to the FEs
  void noiseSim();    ///< Add noise to the strips 
  /// sigma of the noise from strip capacitance
  double noiseSigma(double stripCapacitance); 
  /// Added noise of a strip from strip capacitance
  double noiseValue(double stripCapacitance);
  /// Noise to add to an otherwise empty strip from strip capacitance
  double noiseValueTail(double stripCapacitance);
  void CMSim(); ///< Common mode simulation
  StatusCode VeloSim::deadStrips();
  StatusCode finalProcess();
  StatusCode storeOutputData();
  LHCb::MCVeloFE* findOrInsertFE(LHCb::VeloChannelID& stripKey);

  double chargeTimeFactor(double TOF, double bunchOffset);

  // check conditions Data Base
  bool checkConditions(LHCb::MCHit* aHit);
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
  double m_pedestalConst;
  double m_pedestalVariation;
  double m_adc2Electrons;
  double m_adcScale;
  double m_offPeakSamplingTime;
  bool m_makeNonZeroSuppressedData;

  bool m_isDebug; ///< debug level for output
  bool m_isVerbose; ///< debug level for output
  
};
//
#endif // VELOSIM_H

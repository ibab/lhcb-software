#// $Id: VeloSim.h,v 1.9 2003-04-14 07:34:26 cattanem Exp $
#ifndef VELOSIM_H
#define VELOSIM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// from Velo
#include "Event/MCVeloFE.h"
class DeVelo;
class MCVeloHit;
class VeloChannelID;

/** @class VeloSim VeloSim.h VeloAlgs/MCVeloSim.h
 *
 * Fill MCVeloDigit, based on MCVeloHit
 * In general would contain silicon and Front End Chip Simulation
 * and digitisation.
 * Currently just a fast test version.
 *  @author Chris Parkes, based on code of Oliver Callot
 *  @date   10/01/02
 */
class VeloSim : public Algorithm {
public:
  /// Standard constructor
  VeloSim( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloSim( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  StatusCode getInputData();
  StatusCode simulation();
  StatusCode chargeSim(bool spillOver);
  long simPoints(MCVeloHit* hit);
  void chargePerPoint(MCVeloHit* hit,
                      int Npoints,
                      std::vector<double>& Spoints,
                      bool SpillOver);
  void deltaRayCharge(double charge,
                      double tol,
                      int Npoints,
                      std::vector<double>& Spoints);
  void diffusion(MCVeloHit* hit, int Npoints, std::vector<double>& Spoints, bool SpillOver);
  void testSim(MCVeloHit* hit, bool SpillOver);
  void fillFE(MCVeloFE* myFE, MCVeloHit* hit, double charge);
  void fillFE(MCVeloFE* myFE, double charge);
  StatusCode coupling();
  MCVeloFE* findOrInsertPrevStrip(MCVeloFEs::iterator FEIt, 
                                  bool& valid, 
                                  bool& create);
  MCVeloFE* findOrInsertNextStrip(MCVeloFEs::iterator FEIt, 
                                  bool& valid, 
                                  bool& create);
  StatusCode pedestalSim();
  StatusCode noiseSim();
  double noiseSigma(double stripCapacitance);
  double noiseValue(double stripCapacitance);
  double noiseValueTail(double stripCapacitance);
  StatusCode CMSim();
  StatusCode VeloSim::deadStrips();
  StatusCode finalProcess();
  StatusCode storeOutputData();
  MCVeloFE* findOrInsertFE(VeloChannelID& stripKey);

  // data members
  std::string m_inputContainer;       ///< Name of input container  
  std::string m_spillOverInputContainer; ///< Name of spill Over event input container
  std::string m_pileUpInputContainer;  ///< Name of pileUp input container
  std::string m_pileUpSpillOverInputContainer;  ///< Name of spill Over event input container

  std::string m_outputContainer;      ///< Name of output container
  std::string m_pileUpOutputContainer;      ///< Name of output container

  DeVelo* m_velo; ///< Detector Element
  MCVeloHits* m_hits; ///< vector of input hits
  MCVeloHits* m_spillOverHits; ///< vector of input hits from spill over event
  MCVeloFEs* m_FEs; ///< vector of FE output  signals
  MCVeloFEs* m_FEs_coupling; ///< vector of coupled FE output signals
 
  MCVeloHits* m_veloHits; // vector of input hits
  MCVeloHits* m_pileUpHits; // vector of input hits from pileUp
  MCVeloHits* m_veloSpillOverHits; // vector of input hits
  MCVeloHits* m_pileUpSpillOverHits; // vector of input hits from pileUp
  MCVeloFEs* m_veloFEs; ///< vector of FE output  signals
  MCVeloFEs* m_pileUpFEs; ///< vector of FE output  signals


  double m_baseDiffuseSigma; // diffusion sigma in microns/sqrt(thickness)
  // control simulation sections
  bool m_chargeSim;
  bool m_inhomogeneousCharge;
  bool m_coupling;
  bool m_noiseSim;
  double m_noiseConstant;
  bool m_pedestalSim;
  bool m_CMSim;
  double m_stripInefficiency; 
  // if strip Ineff. > 0 then that fraction of strips have charge removed.
  bool m_spillOver;
  bool m_pileUp;
  bool m_testSim;
  double m_smearPosition;
  // allows resolution to be degraded for robustness studies

  HepPoint3D m_movePosition; // used when degrading resolution

  std::string m_simMode; // velo or pileup

  //gaussian random numbers
  Rndm::Numbers m_gaussDist;
  //uniform random numbers
  Rndm::Numbers m_uniformDist;
  /// random numbers from upper tail of gaussian > a
  double ran_gaussian_tail(const double a, const double sigma);
  /// random numbers from 1/E^2
  double ran_inv_E2(double Emin, double Emax);

};
#endif // VELOSIM_H

















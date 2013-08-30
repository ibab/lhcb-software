// $Id: MomentumSpectrum.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_MOMENTUMSPECTRUM_H
#define PARTICLEGUNS_MOMENTUMSPECTRUM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"
#include "GaudiKernel/RndmGenerators.h"

// from ROOT
#include "TH3D.h"
#include "TFile.h"

/** @class MomentumSpectrum MomentumSpectrum.h "MomentumSpectrum.h"
 *
 *  \brief Particle gun with momentum spectrum given by 3D histogram
 *  
 * Options:
 * - PdgCodes: PDG numbers of particles to be produced by particle gun
 * - InputFile: Name of the file containing the histogram
 * - HistogramPath: Path to the histogram in the file
 * 
 *  @author Michel De Cian
 *  @date   2013-08-29
 */
class MomentumSpectrum : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  MomentumSpectrum( const std::string & type , const std::string& name, 
                 const IInterface * parent ) ;
  
  /// Destructor
  virtual ~MomentumSpectrum();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector & momentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) ;

  /// Print counters
  virtual void printCounters( ) { ; } ;
                                 
 private:
  
  /// Pdg Codes of particles to generate (Set by options)
  std::vector<int>         m_pdgCodes;

  /// Masses of particles to generate
  std::vector<double>      m_masses;

  /// Names of particles to generate
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;

  /// Name of input file
  std::string m_inputFileName;

  /// Path of histogram in input file
  std::string m_histoPath;

  TH3D* m_hist;

};

#endif // PARTICLEGUNS_MOMENTUMSPECTRUM_H

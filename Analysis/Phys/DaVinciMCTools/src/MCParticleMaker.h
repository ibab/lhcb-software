// $Id: MCParticleMaker.h,v 1.4 2005-11-11 17:03:51 pkoppenb Exp $
#ifndef MCPARTICLEMAKER_H 
#define MCPARTICLEMAKER_H 1

// Include files
// from STL
#include <vector>
#include <string>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "CLHEP/Matrix/SymMatrix.h"

// from DaVinciTools
#include "Kernel/IParticleMaker.h"
#include "DaVinciMCTools/IDebugTool.h"
// Forward declarations
class IParticlePropertySvc;
class IRndmGenSvc;
class IMCDecayFinder;

class ProtoParticle;

/** @class MCParticleMaker MCParticleMaker.h
 *  This tool fills the particle class with information from
 *  MCParticles and places it in the Transient Event Store.
 *
 *  @author Gerhard Raven
 *  @date   2002-10-08
 */
class MCParticleMaker : public GaudiTool,
                        virtual public IParticleMaker {
public:
  /// Standard constructor
  MCParticleMaker( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~MCParticleMaker( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  /// Functional method to make particles.
  virtual StatusCode makeParticles( ParticleVector & parts );
  
  /// Finalize
///  StatusCode finalize();
 
protected:
  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
 
private:

  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector< int > m_ids; ///< PDGid of Particles to make
  std::string m_input;
  bool    m_onlyDecayProducts;
  bool    m_onlyStableDecayProducts;
  bool    m_onlyReconstructable;
  bool    m_onlyReconstructed;
  bool    m_useReconstructedCovariance;
  bool    m_smearParticle;
  double  m_ipErrorC0;
  double  m_ipErrorC1;
  double  m_slopeError;
  double  m_momError;
  
  bool    m_smearATPoT;
  double m_rhoxy;
  double m_rhoxz;
  double m_rhoyz;
  double m_rhoxtx;   
  double m_rhoxty;   
  double m_rhoxp;
  double m_rhoytx;   
  double m_rhoyty;   
  double m_rhoyp;
  double m_rhoztx;   
  double m_rhozty;   
  double m_rhozp;
  double m_rhotxty;   
  double m_rhotxp;   
  double m_rhotyp;

  std::vector< double >  m_covSFsC0, m_covSFsC1;//SV===========================
  std::vector< double >  m_BIASsC0,  m_BIASsC1;//SV===========================
  std::vector< double > m_dualGaussSF;     //SV===========================
  std::vector< double > m_dualGaussWeight; //SV===========================


  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDebugTool* m_debug;          //SV===========================================
  IMCDecayFinder* m_pMCDecFinder;
  Rndm::Numbers m_ranGauss;
  Rndm::Numbers m_ranFlat;


  /// internal method
  StatusCode fillParticle( const MCParticle& mc, 
                           Particle& particle,
                           const HepSymMatrix& cov);
  bool reconstructable(const MCParticle& icand) const;
  const Particle *reconstructed(const MCParticle& icand) const;
  HepSymMatrix * fetchCovariance(const Particle& p ) const;
  HepSymMatrix * generateCovariance(const HepLorentzVector& p) const;
  std::vector<const MCParticle*> getFinalState(const MCParticle& m);
  
};
#endif // MCPARTICLEMAKER_H

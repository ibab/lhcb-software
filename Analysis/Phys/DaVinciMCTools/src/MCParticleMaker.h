// $Id: MCParticleMaker.h,v 1.2 2005-01-11 12:36:08 pkoppenb Exp $
#ifndef MCPARTICLEMAKER_H 
#define MCPARTICLEMAKER_H 1

// Include files
// from STL
#include <vector>
#include <string>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// from DaVinciTools
#include "Kernel/IParticleMaker.h"

// Forward declarations
class IParticlePropertySvc;
class IRndmGenSvc;
class IMCDecayFinder;

class ProtoParticle;
class HepSymMatrix;

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
  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* eventSvc() const;

  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
 
private:

  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector< int > m_ids; ///< PDGid of Particles to make
  std::string m_input;
  bool    m_onlyDecayProducts;
  bool    m_onlyReconstructable;
  bool    m_onlyReconstructed;
  bool    m_useReconstructedCovariance;
  bool    m_smearParticle;
  double  m_ipErrorC0;
  double  m_ipErrorC1;
  double  m_slopeError;
  double  m_momError;

  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service
  IMCDecayFinder* m_pMCDecFinder;
  Rndm::Numbers m_ranGauss;


  /// internal method
  StatusCode fillParticle( const MCParticle& mc, 
                           Particle& particle,
                           const HepSymMatrix& cov);
  bool reconstructable(const MCParticle& icand) const;
  const Particle *reconstructed(const MCParticle& icand) const;
  HepSymMatrix * fetchCovariance(const Particle& p ) const;
  HepSymMatrix * generateCovariance(const HepLorentzVector& p) const;
};
#endif // MCPARTICLEMAKER_H

// $Id: RichParticleMaker.h,v 1.1 2002-10-15 18:12:38 gcorti Exp $
#ifndef RichParticleMaker_H 
#define RichParticleMaker_H 1

// Include files
// from STL
#include <string>
#include <utility>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleMaker.h"

// Forward declarations
class IParticlePropertySvc;

/** @class RichParticleMaker RichParticleMaker.h
 * Makes pions and kaons using RICH normalized info and setting all light
 * particles to be pion and all heavy ones to be kaons taking their best
 * particle ID.
 *  
 *
 *  @author Gloria Corti
 *  @date   2002-07-11
 */
class RichParticleMaker : public AlgTool,
                             virtual public IParticleMaker {
public:
  /// Standard constructor
  RichParticleMaker( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~RichParticleMaker( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* eventSvc() const;

  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
  
  /// Functional method to make particles.
  virtual StatusCode makeParticles( ParticleVector & parts );
  
  /// Finalize
  StatusCode finalize();

  /// internal method
  StatusCode fillParticle( const ProtoParticle* protoCand, 
                           int protoID, double protoCL,
                           Particle* particle );
 
protected:
 
private:

  std::string m_input;            ///< Location of ProtoP in TES
  int m_eID;                      ///< absolute PDG code for e+/-
  int m_muID;                     ///< absolute PDG code for mu+/- 
  int m_piID;                     ///< absolute PDG code for pi+/-
  int m_kID;                      ///< absolute PDG code for K+/-
  int m_pID;                      ///< absolute PDG code for p+/-
  
  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service

};
#endif // RichParticleMaker_H

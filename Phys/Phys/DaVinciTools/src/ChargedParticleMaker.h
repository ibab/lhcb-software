// $Id: ChargedParticleMaker.h,v 1.1 2002-07-18 18:16:21 gcorti Exp $
#ifndef CHARGEDPARTICLEMAKER_H 
#define CHARGEDPARTICLEMAKER_H 1

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

/** @class ChargedParticleMaker ChargedParticleMaker.h
 *  
 *
 *  @author Gloria Corti
 *  @date   2002-07-11
 */
class ChargedParticleMaker : public AlgTool,
                             virtual public IParticleMaker {
public:
  /// Standard constructor
  ChargedParticleMaker( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~ChargedParticleMaker( ); ///< Destructor

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

  std::string m_input;

  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector< std::pair<int,double> > m_ids; ///< PDGid of Particles to make
  std::vector<double> m_confLevels;  ///< Cut Value on the CL of pid Hypothesis

  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service

};
#endif // CHARGEDPARTICLEMAKER_H

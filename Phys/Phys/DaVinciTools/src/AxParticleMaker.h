// $Id: AxParticleMaker.h,v 1.1 2002-05-15 23:37:11 gcorti Exp $
#ifndef AXPARTICLEMAKER_H 
#define AXPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleMaker.h"

// Forward declarations
class IParticlePropertySvc;


/** @class AxParticleMaker AxParticleMaker.h
 *  Creates Particles from AxPartCandidates.
 *
 *  @author Jose Helder Lopes
 *  @date   22/04/2002
 */
class AxParticleMaker : public AlgTool,
                        virtual public IParticleMaker {
public:
  /// Standard constructor
  AxParticleMaker( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);
  
  virtual ~AxParticleMaker( ); ///< Destructor
 
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
protected:
  
private:

  std::string m_inputLocation;     ///< Location where to load AxPartCandidates
  
  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector<int> m_ids;    ///< PDGid of Particles to make
  std::vector<double> m_confLevels;  ///< Cut Value on the CL of pid Hypothesis
  
  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service
};

#endif // AXPARTICLEMAKER_H

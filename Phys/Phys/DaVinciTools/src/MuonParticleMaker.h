// $Id: MuonParticleMaker.h,v 1.1 2002-07-27 20:22:29 gcorti Exp $
#ifndef MUONPARTICLEMAKER_H 
#define MUONPARTICLEMAKER_H 1

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

/** @class MuonParticleMaker MuonParticleMaker.h
 *  Makes Particles as muons looking only at the Muon Detector
 *  particleID 
 *
 *  @author Jose' Helder Lopes
 *  @date   22/07/2002
 */
class MuonParticleMaker : public AlgTool,
                          virtual public IParticleMaker {
public:
  /// Standard constructor
  MuonParticleMaker( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~MuonParticleMaker( ); ///< Destructor
  
  /// Initialize
  StatusCode initialize();
  
  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* eventSvc() const;
  
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
  
  std::string m_input; /// Location for charged protoparticles in TES
  
  int m_muonPID; /// Pid code of muons
  
  double m_confLevel;  ///< Cut Value on the CL for muon hypothesis
  
  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service
  
};
#endif // MUONPARTICLEMAKER_H

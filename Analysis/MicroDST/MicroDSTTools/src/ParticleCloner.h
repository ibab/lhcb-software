// $Id: ParticleCloner.h,v 1.2 2008-08-13 16:56:18 jpalac Exp $
#ifndef MICRODST_PARTICLECLONER_H 
#define MICRODST_PARTICLECLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneParticle.h>            // Interface

#include <MicroDST/Functors.hpp>

class ICloneVertex;
class ICloneProtoParticle;

/** @class ParticleCloner ParticleCloner.h src/ParticleCloner.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class ParticleCloner : public MicroDSTTool, 
                       virtual public ICloneParticle {
public: 
  /// Standard constructor
  ParticleCloner( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~ParticleCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::Particle* operator() (const LHCb::Particle* particle);

private:

  LHCb::Particle* clone(const LHCb::Particle* particle);

  void storeDaughters(LHCb::Particle* particleClone,
                      const SmartRefVector<LHCb::Particle>& daughters);
  

private:

  typedef MicroDST::BasicItemCloner<LHCb::Particle> BasicParticleCloner;

  ICloneVertex* m_vertexCloner;

  std::string m_vertexClonerName;

  ICloneProtoParticle* m_ppCloner;

  std::string m_ppClonerName;

};
#endif // MICRODST_PARTICLECLONER_H

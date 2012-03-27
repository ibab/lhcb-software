// $Id: ParticleCloner.h,v 1.4 2009-07-30 10:03:01 jpalac Exp $
#ifndef MICRODST_PARTICLECLONER_H 
#define MICRODST_PARTICLECLONER_H 1

#include <algorithm>

// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneParticle.h>            // Interface

#include <MicroDST/Functors.hpp>

class ICloneVertex;
class ICloneProtoParticle;

/** @class ParticleCloner ParticleCloner.h src/ParticleCloner.h
 *  
 *  Clone an LHCb::Particle. Recursively clone it's daughter LHCb::Particles.
 *  Depending on the value of the ICloneVertex and ICloneProtoParticle
 *  properties, clone the end vertex and associated LHCb::ProtoParticle.
 *
 *  <b>Oprions</b>
 *  ICloneVertex: string, implementation of ICloneVertex interface used to 
 *                clone end vertes. Default = "VertexCloner". If set to "NONE",
 *                only a SmartRef to the end LHCb::Vertex is stored.
 *  ICloneProtoParticle: string, implementation of ICloneVertex interface used 
 *                to clone the particle's LHCb::ProtoParticle. 
 *                Default = "ProtoParticleCloner". If set to "NONE",
 *                only a SmartRef to the end LHCb::ProtoParticle is stored.
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class ParticleCloner : public extends1<MicroDSTTool, ICloneParticle> 
{

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

  std::vector<std::string> m_tesVetoList;

};

#endif // MICRODST_PARTICLECLONER_H

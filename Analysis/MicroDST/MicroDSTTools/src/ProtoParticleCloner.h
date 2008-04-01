// $Id: ProtoParticleCloner.h,v 1.1 2008-04-01 12:35:26 jpalac Exp $
#ifndef MICRODST_PROTOPARTICLEPARTICLECLONER_H 
#define MICRODST_PROTOPARTICLECLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneProtoParticle.h>            // Interface
#include <MicroDST/Functors.hpp>

class ICloneTrack;

/** @class ProtoParticleCloner ProtoParticleCloner.h src/ProtoParticleCloner.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-04-01
 */
class ProtoParticleCloner : public MicroDSTTool, 
                            virtual public ICloneProtoParticle {
public: 
  /// Standard constructor
  ProtoParticleCloner( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~ProtoParticleCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::ProtoParticle* operator() (const LHCb::ProtoParticle* protoParticle);

protected:

private:

  LHCb::ProtoParticle* clone(const LHCb::ProtoParticle* protoParticle);

private:

  typedef MicroDST::BasicItemCloner<LHCb::ProtoParticle> BasicProtoParticleCloner;

  ICloneTrack* m_trackCloner;

  std::string m_trackClonerName;

};
#endif // MICRODST_PROTOPARTICLECLONER_H

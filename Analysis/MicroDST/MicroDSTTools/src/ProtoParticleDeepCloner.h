// $Id: ProtoParticleDeepCloner.h,v 1.1 2010-08-20 13:56:03 jpalac Exp $
#ifndef MICRODST_PROTOPARTICLEPARTICLECLONER_H 
#define MICRODST_PROTOPARTICLECLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneProtoParticle.h>            // Interface
#include <MicroDST/Functors.hpp>
#include "Event/MuonPID.h"
#include "Event/RichPID.h"
#include "Event/CaloHypo.h"
class ICloneTrack;
class ICloneCaloHypo;
class ICloneMuonPID;

/** @class ProtoParticleDeepCloner ProtoParticleDeepCloner.h src/ProtoParticleDeepCloner.h
 *  
 *  Deep-clone an LHCb::ProtoParticle. It's associated LHCb::Track,
 *  LHCb::MuonPID and LHCb::CaloHypos are cloned by ICloneTrack, 
 *  ICloneMuonPID, ICloneCaloHypo implementations. The LHCb::RichPID is 
 *  shallow-cloned.
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-15
 */
class ProtoParticleDeepCloner : public extends1<MicroDSTTool, ICloneProtoParticle>
{

public: 

  /// Standard constructor
  ProtoParticleDeepCloner( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~ProtoParticleDeepCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::ProtoParticle* operator() (const LHCb::ProtoParticle* protoParticle);

private:

  LHCb::ProtoParticle* clone(const LHCb::ProtoParticle* protoParticle);

private:

  typedef MicroDST::BasicItemCloner<LHCb::ProtoParticle> BasicProtoParticleCloner;
  typedef MicroDST::BasicCopy<LHCb::MuonPID> MuonPIDCloner;
  typedef MicroDST::BasicCopy<LHCb::RichPID> RichPIDCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloHypo> CaloHypoCloner;

  ICloneTrack* m_trackCloner;
  ICloneCaloHypo* m_caloHypoCloner;

  std::string m_trackClonerName;
  std::string m_caloHypoClonerName;

};
#endif // MICRODST_PROTOPARTICLECLONER_H

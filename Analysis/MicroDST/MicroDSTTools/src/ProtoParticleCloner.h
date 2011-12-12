// $Id: ProtoParticleCloner.h,v 1.7 2010-08-20 13:57:39 jpalac Exp $
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

/** @class ProtoParticleCloner ProtoParticleCloner.h src/ProtoParticleCloner.h
 *
 *  Clone an LHCb::ProtoParticle, it's associated LHCb::Track, LHCb::RichPID
 *  and LHCb::MuonPID. The LHCb::Track cloning is performed by an
 *  implementation of the ICloneTrack interface, property ICloneTrack, with
 *  default value TrackCloner. The MuonPID and RichPID are cloned using a
 *  simple copy, and this tool takes care of correctly setting their
 *  LHCb::Track* fields.
 *
 *  @author Juan PALACIOS
 *  @date   2008-04-01
 */
class ProtoParticleCloner : public extends1<MicroDSTTool, ICloneProtoParticle>
{

public:

  /// Standard constructor
  ProtoParticleCloner( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~ProtoParticleCloner( ); ///< Destructor

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

  std::string m_trackClonerName;

};
#endif // MICRODST_PROTOPARTICLECLONER_H

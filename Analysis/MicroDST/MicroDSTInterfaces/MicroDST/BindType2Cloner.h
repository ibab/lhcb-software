// $Id: BindType2Cloner.h,v 1.1 2008-09-01 08:59:24 jpalac Exp $
#ifndef MICRODST_BINDTYPE2CLONER_H 
#define MICRODST_BINDTYPE2CLONER_H 1

// Forwrd declarations
class LHCb::FlavourTag;
class LHCb::MCParticle;
class LHCb::MCVertex;
class LHCb::Particle;
class LHCb::ProtoParticle;
class LHCb::RecVertex;
class LHCb::Track;
class LHCb::Vertex;

class ICloneFlavourTag;
class ICloneMCParticle;
class ICloneMCVertex;
class ICloneParticle;
class ICloneProtoParticle;
class ICloneRecVertex;
class ICloneTrack;
class ICloneVertex;
/** @class BindType2Cloner BindType2Cloner.h MicroDST/BindType2Cloner.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-09-01
 */

template <typename T> struct BindType2Cloner
{
};

template <> struct BindType2Cloner<LHCb::FlavourTag> 
{
  typedef LHCb::FlavourTag type;
  typedef ICloneFlavourTag cloner;
};

template <> struct BindType2Cloner<LHCb::MCParticle> 
{
  typedef LHCb::MCParticle type;
  typedef ICloneMCParticle cloner;
};

template <> struct BindType2Cloner<LHCb::MCVertex> 
{
  typedef LHCb::MCVertex type;
  typedef ICloneMCVertex cloner;
};

template <> struct BindType2Cloner<LHCb::Particle> 
{
  typedef LHCb::Particle type;
  typedef ICloneParticle cloner;
};

template <> struct BindType2Cloner<LHCb::ProtoParticle> 
{
  typedef LHCb::ProtoParticle type;
  typedef ICloneProtoParticle cloner;
};

template <> struct BindType2Cloner<LHCb::RecVertex> 
{
  typedef LHCb::RecVertex type;
  typedef ICloneRecVertex cloner;
};

template <> struct BindType2Cloner<LHCb::Track> 
{
  typedef LHCb::Track type;
  typedef ICloneTrack cloner;
};

template <> struct BindType2Cloner<LHCb::Vertex> 
{
  typedef LHCb::Vertex type;
  typedef ICloneVertex cloner;
};

#endif // MICRODST_BINDTYPE2CLONER_H

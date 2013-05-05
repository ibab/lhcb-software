// $Id: FlavourTagDeepCloner.h,v 1.4 2009-11-18 14:42:13 jpalac Exp $
#ifndef FLAVOURTAGDEEPCLONER_H
#define FLAVOURTAGDEEPCLONER_H 1

#include "ObjectClonerBase.h"

#include "MicroDST/ICloneFlavourTag.h"

namespace LHCb
{
  class FlavourTag;
}

class ICloneParticle;

/** @class FlavourTagDeepCloner FlavourTagDeepCloner.h
 *
 *  Clone an LHCb::FlavourTag
 *  Clone its LHCb::Taggers complete with all their tagger LHCb::Particles.
 *
 *  @author Juan PALACIOS
 *  @date   2009-11-17
 */
class FlavourTagDeepCloner : public extends1<ObjectClonerBase,ICloneFlavourTag>
{

public:

  /// Standard constructor
  FlavourTagDeepCloner( const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode initialize( );

  virtual ~FlavourTagDeepCloner( ); ///< Destructor

  virtual LHCb::FlavourTag* operator() (const LHCb::FlavourTag* tag);

private:

  LHCb::FlavourTag* clone(const LHCb::FlavourTag* tag);

  /**
   * Clone the taggers of an LHCb::FlavourTag and replace the
   * original taggers by the clones. The cloning of the taggers is
   * performed by the cloneTagger method.
   *
   *  @author Juan PALACIOS
   *  @date   2009-11-17
   *
   */
  void cloneTaggers(LHCb::FlavourTag* tag) const;

  /**
   *
   * Clone and LHCb::Tagger, replacing its tagger particles by clones.
   * The cloning of the tagger particles is performed by cloneParticle.
   *
   *  @author Juan PALACIOS
   *  @date   2009-11-17
   *
   */
  LHCb::Tagger cloneTagger(const LHCb::Tagger& tagger) const;

  /**
   *
   *  Clone an LHCb::Particle using an IParticleCloner.
   *
   *  @author Juan PALACIOS
   *  @date   2009-11-17
   *
   */
  const LHCb::Particle* cloneParticle(const LHCb::Particle* particle) const;

private:

  typedef MicroDST::BasicCopy<LHCb::FlavourTag> BasicFTCopy;

  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

  bool m_cloneTaggerParticles;

};

//=============================================================================

inline const LHCb::Particle*
FlavourTagDeepCloner::cloneParticle(const LHCb::Particle* particle) const
{
  return (*m_particleCloner)(particle);
}

#endif // FLAVOURTAGDEEPCLONER_H

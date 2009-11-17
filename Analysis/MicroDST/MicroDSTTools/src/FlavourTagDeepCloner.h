// $Id: FlavourTagDeepCloner.h,v 1.2 2009-11-17 16:46:43 jpalac Exp $
#ifndef FLAVOURTAGDEEPCLONER_H 
#define FLAVOURTAGDEEPCLONER_H 1

// Include files
// from MicroDST
#include "MicroDST/MicroDSTTool.h"
#include "MicroDST/ICloneFlavourTag.h"            // Interface

namespace LHCb
{
  class FlavourTag;
}

/** @class FlavourTagDeepCloner FlavourTagDeepCloner.h
 *  
 *  Clone an LHCb::FlavourTag
 *  Clone its LHCb::Taggers complete with all their tagger LHCb::Particles.
 * 
 *  @author Juan PALACIOS
 *  @date   2009-11-17
 */
class FlavourTagDeepCloner : public extends1<MicroDSTTool, ICloneFlavourTag> {
public: 
  /// Standard constructor
  FlavourTagDeepCloner( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode initialize( );

  virtual ~FlavourTagDeepCloner( ); ///< Destructor

  virtual LHCb::FlavourTag* operator() (const LHCb::FlavourTag* tag);

private:

  typedef MicroDST::BasicCopy<LHCb::FlavourTag> BasicFTCopy;

  LHCb::FlavourTag* clone(const LHCb::FlavourTag* tag);

  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

};
#endif // FLAVOURTAGDEEPCLONER_H

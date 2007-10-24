// $Id: CopyProtoParticles.h,v 1.3 2007-10-24 15:38:35 jpalac Exp $
#ifndef COPYPROTOPARTICLES_H 
#define COPYPROTOPARTICLES_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
/** @class CopyProtoParticles CopyProtoParticles.h
 *  
 *  Store the Protoparticles corresponding to a container
 *  of particles given by option InputLocation plus 
 *  ProtoParticles corresponding to all the daughters.
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-23
 */
class CopyProtoParticles : public CopyAndStoreData {
public: 
  /// Standard constructor
  CopyProtoParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyProtoParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  const LHCb::ProtoParticle* storeProtoParticle(const LHCb::Particle* particle);

  void scanParticleTree(const LHCb::Particle* particle);

  template <class T>
  void scanParticleSet(typename T::const_iterator begin,
                       typename T::const_iterator end     );

  typedef LHCb::ProtoParticle::Container ProtoParticles;
  typedef MicroDST::BasicItemCloner<LHCb::ProtoParticle> PPCloneFunctor;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::ProtoParticle, PPCloneFunctor> BasicPPCloner;

private:

};
#endif // COPYPROTOPARTICLES_H

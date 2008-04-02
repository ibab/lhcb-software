// $Id: CopyRelatedProtoParticles.h,v 1.2 2008-04-02 09:37:50 jpalac Exp $
#ifndef COPYRELATEDPROTOPARTICLES_H 
#define COPYRELATEDPROTOPARTICLES_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"
#include "Event/ProtoParticle.h"

class ICloneProtoParticle;
class Particle;

/** @class CopyRelatedProtoParticles CopyRelatedProtoParticles.h
 *
 * MicroDSTAlgorithm to clone LHCb::ProtoParticles related to LHCb::Particles
 * from one TES location to a parallel one LHCb::ProtoParticle location.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. LHCb::Particles are taken from the TES location 
 * defined by InputLocation, and their LHCb::ProtoParticles are cloned and 
 * put in a TES location constructed from the original LHCb::ProtoParticle's
 * location. The basic rule is that the location is the original, with 
 * OutputPrefix placed after the trailing "/Event", even if this is not
 * included explicitly. The procedure is recursive, taking the input 
 * LHCb::Particles and cloning LHCb::ProtoParticles for them and the full
 * decay tree, taken from the daughters and the outgoing particles when these
 * do not overlap.
 * The actual cloning of individual LHCb::ProtoParticles is performed by the 
 * ICloneProtoParticle, the implementation of which is set by the property 
 * ICloneProtoParticle (default ProtoParticleCloner). It is the 
 * ICloneProtoParticle that controls the depth of the cloning, ie 
 * what is done with related elements like LHCb::Tracks etc.
 *
 * @see ICloneProtoParticle
 * @see ProtoParticleCloner
 *
 * <b>Example</b>: Clone LHCb::ProtoParticles corresponding to LHCb::Particles
 *  from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" and their 
 *  descendancy tree to locaitons like "/Event/MyLocation/Rec/ProtoP/..." 
 *  using a ProtoParticleCloner
 *  @code
 *
 *  // Add a CopyProtoParticles instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyRelatedProtoParticles"};
 *  CopyRelatedProtoParticles.OutputPrefix = "MyLocation";
 *  CopyRelatedProtoParticles.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles";
 *  CopyRelatedProtoParticles.ICloneProtoParticle = "ProtoParticleCloner"
 *  @endcode
 * 
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-23
 */
class CopyRelatedProtoParticles : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyRelatedProtoParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyRelatedProtoParticles( ); ///< Destructor

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

  ICloneProtoParticle* m_cloner;
  std::string m_clonerName;

};
#endif // COPYRELATEDPROTOPARTICLES_H

// $Id: CopyProtoParticles.h,v 1.6 2008-04-01 14:43:52 jpalac Exp $
#ifndef COPYPROTOPARTICLES_H 
#define COPYPROTOPARTICLES_H 1

// Include files
// from MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
class ICloneProtoParticle;

// from LHCb
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
/** @class CopyProtoParticles CopyProtoParticles.h
 *  
 * MicroDSTAlgorithm to clone LHCb::ProtoParticles from one TES location 
 * to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::ProtoParticles are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 * a leading "/Event" it is removed.
 * The actual cloning of individual LHCb::ProtoParticles is performed by the 
 * IProtoParticleCloner, the implementation of which is set by the property 
 * ICloneParticle (default ParticleCloner). It is the ICloneParticle that
 * controls the depth of the cloning, ie what is done with related elements
 * like LHCb::Tracks etc.
 *
 * @see ICloneProtoParticle
 * @see ProtoParticleCloner
 *
 * <b>Example</b>: Clone particles from "/Event/Rec/ProtoP/Charged" to 
 * "/Event/MyLocation/Rec/ProtoP/Charged" using a ProtoParticleCloner
 *  @code
 *
 *  // Add a CopyProtoParticles instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyProtoParticles"};
 *  CopyProtoParticles.OutputPrefix = "MyLocation";
 *  CopyProtoParticles.InputLocation = "Rec/ProtoP/Charged";
 *  CopyProtoParticles.ICloneProtoParticle = "ProtoParticleCloner"
 *  @endcode
 * 
 *
 *  @todo Test.
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-23
 */
class CopyProtoParticles : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyProtoParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyProtoParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  typedef LHCb::ProtoParticle::Container ProtoParticles;
  typedef MicroDST::BasicItemCloner<LHCb::ProtoParticle> PPCloneFunctor;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::ProtoParticle, PPCloneFunctor> BasicPPCloner;

private:

  ICloneProtoParticle* m_cloner;
  std::string m_clonerName;

};
#endif // COPYPROTOPARTICLES_H

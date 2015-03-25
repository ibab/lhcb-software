#ifndef COPYSIGNALMCPARTICLES_H 
#define COPYSIGNALMCPARTICLES_H 1

// STL
#include <string>
#include <vector>

// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/ICloneMCParticle.h"
#include "MicroDST/ICloneProtoParticle.h"

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event model
#include "Event/MCParticle.h"

// MC association
#include "Kernel/Particle2MCLinker.h"

/** @class CopySignalMCParticles CopySignalMCParticles.h
 *  
 *  Clones all 'signal' MCParticles.
 * 
 *  @author Chris Jones
 *  @date   2015-03-24
 */

class CopySignalMCParticles : public MicroDSTAlgorithm
{

public: 

  /// Standard constructor
  CopySignalMCParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopySignalMCParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  typedef std::vector<const LHCb::ProtoParticle*> ProtosVector;

  /// Get the protos associated to a given MCParticle
  ProtosVector getProtos(const LHCb::MCParticle*) const;

private:

  /// Type of MCParticle cloner
  std::string m_mcpClonerName;

  /// MCParticle Cloner
  ICloneMCParticle * m_cloner;

  /// Type of ProtoParticle cloner
  std::string m_ppClonerName;

  /// ProtoParticle cloner
  ICloneProtoParticle* m_ppCloner;

  /// Location of MCParticles to clone
  std::string m_mcPsLoc;

  /// Flag to turn on the cloning of assocaited Reco level info.
  bool m_saveRecoInfo;

  ProtoParticle2MCLinker* m_pCPPAsct ; ///< Charged protoparticle associator
  ProtoParticle2MCLinker* m_pNPPAsct ; ///< Neutral protoparticle associator

};

#endif // COPYSIGNALMCPARTICLES_H

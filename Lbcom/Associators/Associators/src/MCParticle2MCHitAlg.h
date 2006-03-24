#ifndef _MCParticle2MCHitAlg_H_
#define _MCParticle2MCHitAlg_H_
//
// This File contains the declaration of the MCParticle2MCHitAlg-class
//
//   Author: Rutger van der Eijk
//   Created: 3-7-2002

// STL
#include <string>

// GaudiKernel
#include "GaudiAlg/GaudiAlgorithm.h"


class MCParticle2MCHitAlg : public GaudiAlgorithm {
  
public:

  /// Standard constructor
  MCParticle2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCParticle2MCHitAlg( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputData;  ///< location of Particles to associate
  std::string m_outputData; ///< location of relations table
  bool        m_makeLinker; ///< Option to construct the linker relations
};

#endif // _MCParticle2MCHitAlg_H_

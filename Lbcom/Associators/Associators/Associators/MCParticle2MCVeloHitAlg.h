#ifndef _MCParticle2MCVeloHitAlg_H_
#define _MCParticle2MCVeloHitAlg_H_
//
// This File contains the declaration of the MCParticle2MCVeloHitAlg-class
//
//   Author: Rutger van der Eijk
//   Created: 3-7-2002

// STL
#include <string>

// GaudiKernel
#include "GaudiKernel/Algorithm.h"


class MCParticle2MCVeloHitAlg : public Algorithm {
  // friend factory for instantiation 
  friend AlgFactory<MCParticle2MCVeloHitAlg>;
  
public:

  /// Standard constructor
  MCParticle2MCVeloHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCParticle2MCVeloHitAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Location of table in TES
  std::string outputData() { return m_outputData; }
  
protected:

private:
  std::string m_inputData;  ///< location of Particles to associate
  std::string m_outputData; ///< location of relations table  
};

#endif // _MCParticle2MCVeloHitAlg_H_

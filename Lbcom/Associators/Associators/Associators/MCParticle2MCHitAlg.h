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
#include "GaudiKernel/Algorithm.h"


class MCParticle2MCHitAlg : public Algorithm {
  // friend factory for instantiation 
  friend AlgFactory<MCParticle2MCHitAlg>;
  
public:

  /// Standard constructor
  MCParticle2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCParticle2MCHitAlg( ); ///< Destructor

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

#endif // _MCParticle2MCHitAlg_H_

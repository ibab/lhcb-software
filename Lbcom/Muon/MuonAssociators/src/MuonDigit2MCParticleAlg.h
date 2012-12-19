#ifndef MUONDIGIT2MCPARTICLEALG_H 
#define MUONDIGIT2MCPARTICLEALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MuonDigit.h"
#include "Event/MCParticle.h"


/** @class MuonDigit2MCParticleAlg MuonDigit2MCParticleAlg.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2005-12-30
 */
class MuonDigit2MCParticleAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonDigit2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonDigit2MCParticleAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  StatusCode associateToTruth(LHCb::MuonDigit* digit,
                              const LHCb::MCParticle*& mcpart);
protected:

private:
  bool m_associateAll;
  

};
#endif // MUONDIGIT2MCPARTICLEALG_H

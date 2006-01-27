// $Id: MuonDigit2MCParticleAlg.h,v 1.4 2006-01-27 17:15:59 asarti Exp $
#ifndef MUONDIGIT2MCPARTICLEALG_H 
#define MUONDIGIT2MCPARTICLEALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"


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
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode associateToTruth(LHCb::MuonDigit* digit,
                              const LHCb::MCParticle*& mcpart);
protected:

private:

};
#endif // MUONDIGIT2MCPARTICLEALG_H

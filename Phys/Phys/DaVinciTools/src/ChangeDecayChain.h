#ifndef CHANGEDECAYCHAIN_H
#define CHANGEDECAYCHAIN_H 1

// Include files
// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "DaVinciTools/DVAlgorithm.h"

class ParticleProperty;
class IParticleStuffer;
class IDecayFinder;

/** @class ChangeDecayChain ChangeDecayChain.h
 *  Algorithm that takes a decay mother->list of daughters
 *  and clones them to a new decay with different PIDs
 *  @author Luis Fernandez
 *  @date   2004-07-26
 */
class ChangeDecayChain : public DVAlgorithm {
public: 
  /// Standard constructor
  ChangeDecayChain( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChangeDecayChain( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  IDecayFinder* m_pDKFinder;
  std::string m_Decay;

  std::vector<std::string> m_oldNames;
  std::vector<std::string> m_newNames;
  std::vector<ParticleProperty*> m_oldPP;
  std::vector<ParticleProperty*> m_newPP;

};
#endif // CHANGEDECAYCHAIN_H

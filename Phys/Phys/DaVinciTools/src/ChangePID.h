// $Id: ChangePID.h,v 1.1 2004-07-09 13:23:51 pkoppenb Exp $
#ifndef CHANGEPID_H 
#define CHANGEPID_H 1

// Include files
// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "DaVinciTools/DVAlgorithm.h"

class ParticleProperty;
class IParticleStuffer;

/** @class ChangePID ChangePID.h
 *  
 * Very simple algorithm that takes all particles on the desktop
 * and *CLONES* them with a different particle ID
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-08
 */
class ChangePID : public DVAlgorithm {
public: 
  /// Standard constructor
  ChangePID( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChangePID( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::vector<std::string> m_oldNames; 
  std::vector<std::string> m_newNames; 
  std::vector<ParticleProperty*> m_oldPP; 
  std::vector<ParticleProperty*> m_newPP; 

};
#endif // CHANGEPID_H

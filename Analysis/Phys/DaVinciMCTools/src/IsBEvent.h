// $Id: IsBEvent.h,v 1.4 2010-06-01 09:38:21 pkoppenb Exp $
#ifndef ISBEVENT_H 
#define ISBEVENT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/MCParticle.h"

/** @class IsBEvent IsBEvent.h
 *  
 *  Test if required particles are there. One can also ask for a b-quark (default)
 *  or a c-quark (RequiredParticles = {"c"} ;)
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-12
 */
class IsBEvent : public GaudiAlgorithm {
public: 
  /// Standard constructor
  IsBEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~IsBEvent( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool goodEvent(LHCb::MCParticles*); ///< loop

private:
  std::vector<std::string> m_required; ///< List of required particles
  bool m_andMode; ///< Require all particles (default: no)
  
  std::vector<int> m_particles ; ///< required particles
};
#endif // ISBEVENT_H

// $Id: DebugTrackingLosses.h,v 1.1 2009-04-01 09:03:56 ocallot Exp $
#ifndef DEBUGTRACKINGLOSSES_H 
#define DEBUGTRACKINGLOSSES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

/** @class DebugTrackingLosses DebugTrackingLosses.h
 *  Debug which MCParticles are not reconstructed.
 *
 *  @author Olivier Callot
 *  @date   2009-03-26
 */
class DebugTrackingLosses : public GaudiAlgorithm {
public: 
  /// Standard constructor
  DebugTrackingLosses( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DebugTrackingLosses( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void printMCParticle ( const LHCb::MCParticle* part );
  
private:
  IParticlePropertySvc* m_ppSvc;

  bool m_velo;
  bool m_forward;
  bool m_clone;
  bool m_ghost;
  bool m_fromStrange;
  double m_minMomentum;
  bool m_saveList;
  std::vector<std::vector<int> > m_badGuys;
  int  m_eventNumber;
};
#endif // DEBUGTRACKINGLOSSES_H

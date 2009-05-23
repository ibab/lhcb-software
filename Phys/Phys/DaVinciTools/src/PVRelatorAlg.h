// $Id: PVRelatorAlg.h,v 1.1 2009-05-23 16:48:51 jpalac Exp $
#ifndef PVRELATORALG_H 
#define PVRELATORALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IOnOffline;
class IRelatedPVFinder;

/** @class PVRelatorAlg PVRelatorAlg.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2009-05-23
 */
class PVRelatorAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PVRelatorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVRelatorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string m_particleInputLocation;
  std::string m_PVInputLocation;
  std::string m_P2PVInputLocation;
  std::string m_P2PVOutputLocation;
  bool m_useTable;
  IOnOffline* m_OnOffline ; ///< context switch tool. To be deprecated.
  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV


};
#endif // PVRELATORALG_H

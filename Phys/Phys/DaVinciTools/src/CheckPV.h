// $Id$
#ifndef CHECKPV_H
#define CHECKPV_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class CheckPV CheckPV.h
 *
 *  Algorithm that selects events with a given range of
 *  Reconstructed PVs. In particular useful for
 *  requiring at least 1 PV.
 *  To be used in any sequencer
 *
 *  @code
 *  c = CheckPV("OnePV")
 *  c.MinPVs = 1
 *  # c.MaxPVs = 3 # uncomment for up to 3 PVs
 *  @endcode
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-18
 */
class CheckPV : public GaudiAlgorithm
{

public:

  /// Standard constructor
  CheckPV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckPV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  int m_minPV ;  ///< minimal number of reconstructed PVs required
  int m_maxPV ;  ///< maximal number of reconstructed PVs required. -1 means no requirement.
  bool m_print ; ///< Print the number of PVs
  std::string m_PVContainer; ///< PV location

};
#endif // CHECKPV_H

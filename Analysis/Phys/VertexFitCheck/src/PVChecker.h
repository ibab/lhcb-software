// $Id: PVChecker.h,v 1.4 2010-01-18 08:43:10 pkoppenb Exp $
#ifndef PVCHECKER_H 
#define PVCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

#include "Kernel/Particle2MCLinker.h"

/** @class PVChecker PVChecker.h
 *  
 *  PVChecker is an algorithm to check
 *  primary vertex fit performances
 *  using MC truth. It fills a ntuple with
 *  fitted values, true values and pulls of PV position.
 *
 *  @author Yuehong Xie
 *  @date   2006-5-25
 */
class PVChecker : public DaVinciTupleAlgorithm
{

public: 
  /// Standard constructor
  PVChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVChecker( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  const LHCb::MCVertex* PV2MCVertex(const LHCb::RecVertex* pv, int & nTrMatched);
  int countMatchedPVTrks(const LHCb::RecVertex* pv,  const LHCb::MCVertex* mcPV);
  const LHCb::RecVertex* MCPV2PV(const LHCb::MCVertex* mcpv, int & nTrMatched);

private:

};
#endif 

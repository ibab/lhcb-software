// $Id: PatVeloResetClusterUsed.h,v 1.1 2008-08-26 17:52:20 dhcroft Exp $
#ifndef TF_PATVELORESETCLUSTERUSED_H 
#define TF_PATVELORESETCLUSTERUSED_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"

namespace Tf {
  /** @class PatVeloResetClusterUsed PatVeloResetClusterUsed.h
   *
   *  Algorithm to drop in where reseting the R and/or phi 
   *  PatVelo hit used flags is required
   *
   *  @author David Hutchcroft
   *  @date   2008-08-26
   */

  class PatVeloResetClusterUsed : public GaudiAlgorithm {
    public: 
      /// Standard constructor
      PatVeloResetClusterUsed( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~PatVeloResetClusterUsed( ); ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialization
      virtual StatusCode execute   (); ///< Algorithm execution
      virtual StatusCode finalize  (); ///< Algorithm finalization

    protected:

    private:
      
      std::string m_rHitManagerName;         ///< Name of R hit manager
      std::string m_phiHitManagerName;       ///< Name of Phi hit manager
      
      PatVeloRHitManager* m_rHitManager;     ///< R hit storage
      PatVeloPhiHitManager* m_phiHitManager; ///< Phi hit storage

      bool m_resetR;                         ///< Reset the R clusters
      bool m_resetPhi;                       ///< Reset the Phi clusters
    
      bool m_isDebug;                        ///< check the debug state

  };

}
#endif // TF_PATVELORESETCLUSTERUSED_H

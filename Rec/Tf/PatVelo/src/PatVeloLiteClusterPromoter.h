// $Id: PatVeloLiteClusterPromoter.h,v 1.1 2008-10-16 17:00:31 dhcroft Exp $
#ifndef TF_PATVELOLITECLUSTERPROMOTER_H
#define TF_PATVELOLITECLUSTERPROMOTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPromoteClusters.h"
#include "PatVeloPhiHitManager.h"
#include "PatVeloRHitManager.h"
#include "Event/VeloCluster.h"

namespace LHCb {
  class Track;
}

namespace Tf {
  class PatVeloRHitManager;
  class PatVeloPhiHitManager;

  /** @class PatVeloLiteClusterPromoter PatVeloLiteClusterPromoter.h
   *  Tool to make "fake" full VELO clusters from a list of LHCbIDs
   *  hacked ADC values from lite clusters instead real clusters
   *
   *  @author David Hutchcroft
   *  @date   2008-10-15
   */
  class PatVeloLiteClusterPromoter : public extends<GaudiTool, IPromoteClusters> {
  public:

    /// Standard constructor
    PatVeloLiteClusterPromoter( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

    ~PatVeloLiteClusterPromoter( ) override; ///< Destructor

    StatusCode initialize() override; ///< initialize

    /// promote all of the lite clusters asscoaited to the LHCbID list
    /// to faked full VELO clusters
    StatusCode
    promoteClusters(std::vector<LHCb::LHCbID> const & inputIDs ) const override;


  private:
    /// promote a VeloLiteCluster
    void promoteLiteCluster(LHCb::VeloLiteCluster const &liteCluster,
			    LHCb::VeloClusters* fakeClusters) const;

    /// the specialized r hit manager
    PatVeloRHitManager* m_rHitManager;

    /// the specialized phi hit manager
    PatVeloPhiHitManager* m_phiHitManager;

    /// Name of TES container for "fake" VELO clusters to
    std::string m_fakeVELOClusterLocation;
    /// the name of the R hit manager instance
    std::string m_rHitManagerName;
    /// the name of the Phi hit manager instance
    std::string m_phiHitManagerName;

  };
}
#endif // TF_PATVELOLITECLUSTERPROMOTER_H

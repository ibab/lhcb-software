// $Id: PatVeloPhiHitManager.h,v 1.1.1.1 2007-08-26 21:03:29 krinnert Exp $
#ifndef INCLUDE_TF_PATVELOPHIHITMANAGER_H
#define INCLUDE_TF_PATVELOPHIHITMANAGER_H 1

#include "PatVeloHit.h"
#include "TfKernel/ExtendedVeloPhiHitManager.h"

namespace Tf {
  static const InterfaceID IID_PatVeloPhiHitManager( "Tf::PatVeloPhiHitManager", 1, 0 );

  /** @class PatVeloPhiHitManager PatVeloPhiHitManager.h
   * Mananges specialised phi hits for VELO space tracking. 
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-08
   */
  class PatVeloPhiHitManager : public Tf::ExtendedVeloPhiHitManager<PatVeloPhiHit> {

    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_PatVeloPhiHitManager; }

      /// Standard Constructor
      PatVeloPhiHitManager(const std::string& type,
          const std::string& name,
          const IInterface* parent);

      virtual ~PatVeloPhiHitManager() {;} ///< Destructor

      StatusCode initialize(); ///< Tool initialization
      StatusCode   finalize(); ///< Tool finalize

      virtual void prepareHits();

  };
}
#endif // INCLUDE_TF_PATVELOPHIHITMANAGER_H


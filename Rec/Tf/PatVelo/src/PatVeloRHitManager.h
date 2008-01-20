// $Id: PatVeloRHitManager.h,v 1.2 2008-01-20 15:46:37 krinnert Exp $
#ifndef INCLUDE_TF_PATVELORHITMANAGER_H
#define INCLUDE_TF_PATVELORHITMANAGER_H 1

#include <bitset>

#include "PatVeloHit.h"
#include "TfKernel/ExtendedVeloRHitManager.h"

namespace Tf {

  static const InterfaceID IID_PatVeloRHitManager( "PatVeloRHitManager", 1, 0 );

  /** @class PatVeloRHitManager PatVeloRHitManager.h
   * Mananges specialised phi hits for VELO space tracking. 
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-08
   */
  class PatVeloRHitManager : public Tf::ExtendedVeloRHitManager<PatVeloRHit> {

    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_PatVeloRHitManager; }

      /// Standard Constructor
      PatVeloRHitManager(const std::string& type,
          const std::string& name,
          const IInterface* parent);

      virtual ~PatVeloRHitManager() {;} ///< Destructor

      StatusCode initialize(); ///< Tool initialization
      StatusCode   finalize(); ///< Tool finalize

      virtual void prepareHits(); ///< Prepare all hits

      void prepareHits(StationIterator it) { prepareHits(*it); }          ///< Prepare hits for one station only
      void prepareHits(StationReverseIterator rit) { prepareHits(*rit); } ///< Prepare hits for one station only
      void prepareHits(Station* station);        ///< Prepare hits for one station only, implementation
      
  };
}
#endif // INCLUDE_TF_PATVELORHITMANAGER_H


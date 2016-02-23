// $Id: PatVeloRHitManager.h,v 1.5 2010-01-20 17:16:42 krinnert Exp $
#ifndef INCLUDE_TF_PATVELORHITMANAGER_H
#define INCLUDE_TF_PATVELORHITMANAGER_H 1

#include <bitset>

#include "PatVeloHit.h"
#include "TfKernel/ExtendedVeloRHitManager.h"

namespace Tf {

  static const InterfaceID IID_PatVeloRHitManager( "PatVeloRHitManager", 1, 0 );

  /** @class PatVeloRHitManager PatVeloRHitManager.h
   * Mananges specialised r hits for VELO tracking. 
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

      void resetUsedFlagOfHits(); ///< Reset all used flags to unused

    private:
      /// Max number of clusters in R zone before region killed as too hot
      unsigned int m_maxRClustersZone;
      /// Use CCE scan pattern recognition
      bool m_CCEscan;
      /// CCE scan sensor list to kill
      std::vector<unsigned int> m_killSensorList;
      /// CCE step for voltage determination
      int m_cceStep;
  };
}
#endif // INCLUDE_TF_PATVELORHITMANAGER_H


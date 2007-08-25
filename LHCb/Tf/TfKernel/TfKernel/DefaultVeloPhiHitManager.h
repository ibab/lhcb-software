// $Id: DefaultVeloPhiHitManager.h,v 1.3 2007-08-25 16:55:01 jonrob Exp $
#ifndef INCLUDE_TF_DEFAULTVELOPHIHITMANAGER_H
#define INCLUDE_TF_DEFAULTVELOPHIHITMANAGER_H 1

#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

#include "TfKernel/DefaultVeloHitManager.h"


namespace Tf {

  static const InterfaceID IID_DefaultVeloPhiHitManager( "Tf::DefaultVeloPhiHitManager", 1, 0 );

  /** @class DefaultVeloPhiHitManager DefaultVeloPhiHitManager.h
   *
   *  Default Hit manager for Velo Phi hits
   *  
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-07
   */
  class DefaultVeloPhiHitManager : public DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2> {

  public:

    typedef DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2>::Station                Station;
    typedef DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2>::StationIterator        StationIterator;
    typedef DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2>::StationReverseIterator StationReverseIterator;

  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_DefaultVeloPhiHitManager; }

    /// Standard Constructor
    DefaultVeloPhiHitManager(const std::string& type,
                             const std::string& name,
                             const IInterface* parent);

    virtual ~DefaultVeloPhiHitManager() {;} ///< Destructor

    StatusCode initialize(); ///< Tool initialization
    StatusCode   finalize(); ///< Tool finalize
  };

}
#endif // INCLUDE_TF_DEFAULTVELOPHIHITMANAGER_H


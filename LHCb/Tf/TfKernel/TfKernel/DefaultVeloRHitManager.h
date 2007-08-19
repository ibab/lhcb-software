// $Id: DefaultVeloRHitManager.h,v 1.2 2007-08-19 16:54:41 jonrob Exp $
#ifndef INCLUDE_TF_DEFAULTVELORHITMANAGER_H
#define INCLUDE_TF_DEFAULTVELORHITMANAGER_H 1

#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

#include "TfKernel/DefaultVeloHitManager.h"


namespace Tf {

  static const InterfaceID IID_DefaultVeloRHitManager( "Tf::DefaultVeloRHitManager", 1, 0 );

  /** @class DefaultVeloRHitManager DefaultVeloRHitManager.h
   *
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-07-31
   */
  class DefaultVeloRHitManager : public DefaultVeloHitManager<DeVeloRType,VeloRHit,4> {

  public:

    typedef DefaultVeloHitManager<DeVeloRType,VeloRHit,4>::Station                Station;
    typedef DefaultVeloHitManager<DeVeloRType,VeloRHit,4>::StationIterator        StationIterator;
    typedef DefaultVeloHitManager<DeVeloRType,VeloRHit,4>::StationReverseIterator StationReverseIterator;

  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_DefaultVeloRHitManager; }

    /// Standard Constructor
    DefaultVeloRHitManager(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

    virtual ~DefaultVeloRHitManager() {;} ///< Destructor

    StatusCode initialize(); ///< Tool initialization
    StatusCode   finalize(); ///< Tool finalize
  };

}
#endif // INCLUDE_TF_DEFAULTVELORHITMANAGER_H


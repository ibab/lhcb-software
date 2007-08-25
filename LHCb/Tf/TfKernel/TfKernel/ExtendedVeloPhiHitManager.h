// $Id: ExtendedVeloPhiHitManager.h,v 1.3 2007-08-25 16:57:06 jonrob Exp $
#ifndef INCLUDE_EXTENDENDVELOPHIHITMANAGER_H
#define INCLUDE_EXTENDENDVELOPHIHITMANAGER_H 1

#include "TfKernel/DefaultVeloPhiHitManager.h"

namespace Tf {

  static const InterfaceID IID_ExtendedVeloPhiHitManager( "Tf::ExtendedVeloPhiHitManager", 1, 0 );

  /** @class ExtendedVeloPhiHitManager ExtendedVeloPhiHitManager.h
   * Base class for user derived VeloPhi HitManagers.
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-07
   */
  template<typename HITEXTENSION>
  class ExtendedVeloPhiHitManager : public VeloHitManager<DeVeloPhiType,HITEXTENSION,2> {

  public:

    typedef VeloHitManager<DeVeloPhiType,HITEXTENSION,2>    HitManagerBase;         ///< shortcut to base class type
    typedef typename HitManagerBase::Station                Station;                ///< shortcut for station
    typedef typename HitManagerBase::StationIterator        StationIterator;        ///< shortcut for station iterator
    typedef typename HitManagerBase::StationReverseIterator StationReverseIterator; ///< shortcut for station reverse iterator

    typedef typename DefaultVeloPhiHitManager::Station                DefaultStation;                ///< shortcut for default manager access
    typedef typename DefaultVeloPhiHitManager::StationIterator        DefaultStationIterator;        ///< shortcut for default manager access
    typedef typename DefaultVeloPhiHitManager::StationReverseIterator DefaultStationReverseIterator; ///< shortcut for default manager access

  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ExtendedVeloPhiHitManager; }

    /// Standard Constructor
    ExtendedVeloPhiHitManager(const std::string& type,
                              const std::string& name,
                              const IInterface* parent);

    virtual ~ExtendedVeloPhiHitManager() {;} ///< Destructor

    StatusCode initialize(); ///< Tool initialization
    StatusCode   finalize(); ///< Tool finalize

  protected:

    DefaultVeloPhiHitManager*   m_defaultHitManager;

  private:
  };

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  template<typename HITEXTENSION>
  ExtendedVeloPhiHitManager<HITEXTENSION>::ExtendedVeloPhiHitManager(const std::string& type,
                                                                     const std::string& name,
                                                                     const IInterface* parent)
    : VeloHitManager<DeVeloPhiType,HITEXTENSION,2>(type, name, parent)
  {
    GaudiTool::declareInterface<ExtendedVeloPhiHitManager<HITEXTENSION> >(this);
  }

  //=============================================================================
  // Initialization
  //=============================================================================
  template<typename HITEXTENSION>
  StatusCode ExtendedVeloPhiHitManager<HITEXTENSION>::initialize()
  {
    StatusCode sc = VeloHitManager<DeVeloPhiType,HITEXTENSION,2>::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    GaudiTool::debug() << "==> Initialize" << endmsg;

    m_defaultHitManager = GaudiTool::tool<Tf::DefaultVeloPhiHitManager>( "Tf::DefaultVeloPhiHitManager" );

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  template<typename HITEXTENSION>
  StatusCode ExtendedVeloPhiHitManager<HITEXTENSION>::finalize()
  {
    GaudiTool::debug() << "==> Finalize" << endmsg;

    return VeloHitManager<DeVeloPhiType,HITEXTENSION,2>::finalize();  // must be called after all other actions
  }

}
#endif // INCLUDE_EXTENDENDVELOPHIHITMANAGER_H


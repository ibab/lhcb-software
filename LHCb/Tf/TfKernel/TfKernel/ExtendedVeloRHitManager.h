// $Id: ExtendedVeloRHitManager.h,v 1.2 2007-08-25 16:57:06 jonrob Exp $
#ifndef INCLUDE_EXTENDENDVELORHITMANAGER_H
#define INCLUDE_EXTENDENDVELORHITMANAGER_H 1

#include "TfKernel/DefaultVeloRHitManager.h"

namespace Tf {
  
  static const InterfaceID IID_ExtendedVeloRHitManager( "Tf::ExtendedVeloRHitManager", 1, 0 );

  /** @class ExtendedVeloRHitManager ExtendedVeloRHitManager.h
   * Base class for user dereved VeloR HitManagers. 
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-07
   */
  template<typename HITEXTENSION>
  class ExtendedVeloRHitManager : public VeloHitManager<DeVeloRType,HITEXTENSION,4> {

    public:

      typedef VeloHitManager<DeVeloRType,HITEXTENSION,4>      HitManagerBase;         ///< shortcut to base class type
      typedef typename HitManagerBase::Station                Station;                ///< shortcut for station
      typedef typename HitManagerBase::StationIterator        StationIterator;        ///< shortcut for station iterator
      typedef typename HitManagerBase::StationReverseIterator StationReverseIterator; ///< shortcut for station reverse iterator
      
      typedef typename DefaultVeloRHitManager::Station                DefaultStation;                ///< shortcut for default manager access
      typedef typename DefaultVeloRHitManager::StationIterator        DefaultStationIterator;        ///< shortcut for default manager access
      typedef typename DefaultVeloRHitManager::StationReverseIterator DefaultStationReverseIterator; ///< shortcut for default manager access
        
    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ExtendedVeloRHitManager; }

      /// Standard Constructor
      ExtendedVeloRHitManager(const std::string& type,
          const std::string& name,
          const IInterface* parent);

      virtual ~ExtendedVeloRHitManager() {;} ///< Destructor

      StatusCode initialize(); ///< Tool initialization
      StatusCode   finalize(); ///< Tool finalize

    protected:

      DefaultVeloRHitManager*   m_defaultHitManager;

    private:
  };

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  template<typename HITEXTENSION>
  ExtendedVeloRHitManager<HITEXTENSION>::ExtendedVeloRHitManager(const std::string& type,
      const std::string& name,
      const IInterface* parent)
    : VeloHitManager<DeVeloRType,HITEXTENSION,4>(type, name, parent)
    {
      GaudiTool::declareInterface<ExtendedVeloRHitManager<HITEXTENSION> >(this);
    }

  //=============================================================================
  // Initialization
  //=============================================================================
  template<typename HITEXTENSION>
  StatusCode ExtendedVeloRHitManager<HITEXTENSION>::initialize()
  {
    StatusCode sc = VeloHitManager<DeVeloRType,HITEXTENSION,4>::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    GaudiTool::debug() << "==> Initialize" << endmsg;

    m_defaultHitManager = GaudiTool::tool<Tf::DefaultVeloRHitManager>( "Tf::DefaultVeloRHitManager" );

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  template<typename HITEXTENSION>
  StatusCode ExtendedVeloRHitManager<HITEXTENSION>::finalize()
  {
    GaudiTool::debug() << "==> Finalize" << endmsg;

    return VeloHitManager<DeVeloRType,HITEXTENSION,4>::finalize();  // must be called after all other actions
  }

}
#endif // INCLUDE_EXTENDENDVELORHITMANAGER_H


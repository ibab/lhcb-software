// $Id: PatVeloRHitManager.cpp,v 1.1.1.1 2007-08-26 21:03:29 krinnert Exp $

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "PatVeloRHitManager.h"

namespace Tf {

  //-----------------------------------------------------------------------------
  // Implementation file for class : PatVeloRHitManager
  //
  // 2007-08-08 : Kurt Rinnert <kurt.rinnert@cern.ch>
  //-----------------------------------------------------------------------------


  DECLARE_TOOL_FACTORY( PatVeloRHitManager );


  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloRHitManager::PatVeloRHitManager(const std::string& type,
      const std::string& name,
      const IInterface* parent)
    : Tf::ExtendedVeloRHitManager<PatVeloRHit>(type, name, parent)
    {
      declareInterface<PatVeloRHitManager>(this);
    }

  //=============================================================================
  // Initialization
  //=============================================================================
  StatusCode PatVeloRHitManager::initialize()
  {
    StatusCode sc = Tf::ExtendedVeloRHitManager<PatVeloRHit>::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    debug() << "==> Initialize" << endmsg;

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  StatusCode PatVeloRHitManager::finalize()
  {
    debug() << "==> Finalize" << endmsg;

    return Tf::ExtendedVeloRHitManager<PatVeloRHit>::finalize();  // must be called after all other actions
  }

  //=============================================================================
  // Preparation of measurements
  //=============================================================================
  void PatVeloRHitManager::prepareHits() 
  {
    for (unsigned int half=0; half<m_nHalfs; ++half) { // loop over velo halfs
      DefaultStationIterator si   = m_defaultHitManager->stationsHalfBegin(half);
      DefaultStationIterator send = m_defaultHitManager->stationsHalfEnd(half);

      for ( ; si != send; ++si) {
        DefaultStation*    defaultStation = *si;
        unsigned int defaultStationNumber = defaultStation->stationNumber();
        Station* station =  m_stationsHalf[half][defaultStationNumber];
        station->clear();

        for (unsigned int zone=0; zone<m_nZones; ++zone) { // loop over inner/outer zones
          Tf::VeloRHitRange hits = defaultStation->hits(zone);
          Tf::VeloRHits::const_iterator hi   = hits.begin();
          Tf::VeloRHits::const_iterator hend = hits.end();

          m_data[half][defaultStationNumber][zone].reserve(std::distance(hi,hend));
          for ( ; hi != hend; ++hi ) { // import all hits
            m_data[half][defaultStationNumber][zone].push_back(PatVeloRHit(*hi)); 
            station->zone(zone).push_back(&(m_data[half][defaultStationNumber][zone].back()));
          }
        }
      }
    }
    m_dataValid = true;
  }
}







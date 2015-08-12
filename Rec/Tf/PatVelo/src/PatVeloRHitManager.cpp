#include "PatVeloRHitManager.h"

namespace Tf {

  //-----------------------------------------------------------------------------
  // Implementation file for class : PatVeloRHitManager
  //
  // 2007-08-08 : Kurt Rinnert <kurt.rinnert@cern.ch>
  //-----------------------------------------------------------------------------


  DECLARE_TOOL_FACTORY( PatVeloRHitManager )


  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloRHitManager::PatVeloRHitManager(const std::string& type,
      const std::string& name,
      const IInterface* parent)
    : Tf::ExtendedVeloRHitManager<PatVeloRHit>(type, name, parent)
    {
      declareInterface<PatVeloRHitManager>(this);
      // currently disarmed: only 512 strips in a zone...
      declareProperty("MaxClustersRZone", m_maxRClustersZone = 999) ; 
    }

  //=============================================================================
  // Initialization
  //=============================================================================
  StatusCode PatVeloRHitManager::initialize()
  {
    StatusCode sc = Tf::ExtendedVeloRHitManager<PatVeloRHit>::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    debug() << "==> Initialize" << endmsg;

    if( m_maxRClustersZone < 512 ) {
      info() << "Kill hits in R zone with more than " << m_maxRClustersZone
        << " clusters" << endmsg;
    }
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
  // Reset all used flags to unused
  //=============================================================================
  void PatVeloRHitManager::resetUsedFlagOfHits() 
  {
    for (unsigned int half=0; half<m_nHalfs; ++half) { // loop over velo halfs
      DefaultStationIterator si   = m_defaultHitManager->stationsHalfBegin(half);
      DefaultStationIterator send = m_defaultHitManager->stationsHalfEnd(half);

      for ( ; si != send; ++si) {
        for (unsigned int zone=0; zone<m_nZones; ++zone) { // loop over inner/outer zones
          Tf::VeloRHitRange hits = (*si)->hits(zone);
          Tf::VeloRHits::const_iterator hi   = hits.begin();
          Tf::VeloRHits::const_iterator hend = hits.end();
          for ( ; hi != hend; ++hi ) { // import all hits
            (*hi)->resetUsedFlag();
          }
        }
      }
    }
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
        if (station->hitsPrepared()) continue;
        station->clear();

        for (unsigned int zone=0; zone<m_nZones; ++zone) { // loop over r sectors
          Tf::VeloRHitRange hits = defaultStation->hits(zone);
          bool markUsed = false;
          if ( hits.size() > m_maxRClustersZone ){
            Warning("Very hot VELO R zone: ignoring clusters",
                StatusCode::SUCCESS, 0 ).ignore();
            markUsed = true;
          }
          Tf::VeloRHits::const_iterator hi   = hits.begin();
          Tf::VeloRHits::const_iterator hend = hits.end();

          m_data[half][defaultStationNumber][zone].reserve(std::distance(hi,hend));
          for ( ; hi != hend; ++hi ) { // import all hits
            if ( (*hi)->ignore() ) { continue; } // don't use hit if ignore flag is set
            if ( markUsed ) (*hi)->setUsed(true); // hot region hit
            m_data[half][defaultStationNumber][zone].push_back(PatVeloRHit(*hi)); 
            station->zone(zone).push_back(&(m_data[half][defaultStationNumber][zone].back()));
          }
        }
        station->setHitsPrepared(true);
      }
    }
    m_dataValid = true;
  }

  //=============================================================================
  // Preparation of measurements for one station, actual implementation
  //=============================================================================
  void PatVeloRHitManager::prepareHits(Station* station) 
  {
    DefaultStation*    defaultStation = m_defaultHitManager->stationNoPrep(station->sensorNumber());
    if (!defaultStation->hitsPrepared()) {
      m_defaultHitManager->prepareHits(defaultStation);
    }
    unsigned int defaultStationNumber = defaultStation->stationNumber();
    unsigned int half = station->veloHalf();

    for (unsigned int zone=0; zone<m_nZones; ++zone) { // loop over r sectors
      Tf::VeloRHitRange hits = defaultStation->hits(zone);
      bool markUsed = false;
      if ( hits.size() > m_maxRClustersZone ){
        Warning("Very hot VELO R zone: ignoring clusters",
            StatusCode::SUCCESS, 0 ).ignore();
        markUsed = true;
      }
      Tf::VeloRHits::const_iterator hi   = hits.begin();
      Tf::VeloRHits::const_iterator hend = hits.end();

      m_data[half][defaultStationNumber][zone].reserve(std::distance(hi,hend));
      for ( ; hi != hend; ++hi ) { // import all hits
        if ( (*hi)->ignore() ) { continue; } // don't use hit if ignore flag is set
        if ( markUsed ) (*hi)->setUsed(true); // hot region hit
        m_data[half][defaultStationNumber][zone].push_back(PatVeloRHit(*hi)); 
        station->zone(zone).push_back(&(m_data[half][defaultStationNumber][zone].back()));
      }
    }
    station->setHitsPrepared(true);
  }

}





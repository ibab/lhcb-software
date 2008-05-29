// $Id: DefaultVeloHitManager.h,v 1.13 2008-05-29 10:55:11 smenzeme Exp $
#ifndef INCLUDE_TF_DEFAULTVELOHITMANAGER_H
#define INCLUDE_TF_DEFAULTVELOHITMANAGER_H 1

#include <iostream>
#include <algorithm>

#include "LoKi/Range.h"

#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

#include "VeloDet/DeVeloRType.h"
#include "TfKernel/VeloRHit.h"

#include "TfKernel/VeloHitManager.h"


namespace Tf {

  static const InterfaceID IID_DefaultVeloHitManager( "Tf::DefaultVeloHitManager", 1, 0 );

  /** @class DefaultVeloHitManager DefaultVeloHitManager.h
   *
   *  Default Hit manager for Velo hits
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-07
   */
  template <typename SENSORTYPE, typename HIT, int NZONES>
  class DefaultVeloHitManager : public VeloHitManager<SENSORTYPE,HIT,NZONES> {

    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_nHalfs;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_nStations;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_nZones;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_data;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_dataValid;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_eventExpired;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_velo;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_stationsAll;
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_stationBySensorNumber;

  public:

    typedef typename VeloHitManager<SENSORTYPE,HIT,NZONES>::Station                Station;
    typedef typename VeloHitManager<SENSORTYPE,HIT,NZONES>::StationIterator        StationIterator;
    typedef typename VeloHitManager<SENSORTYPE,HIT,NZONES>::StationReverseIterator StationReverseIterator;

  private:
    
    typedef LoKi::Range_<LHCb::VeloClusters>     ClusterRange;
    typedef LoKi::Range_<LHCb::VeloLiteCluster::FastContainer> LiteClusterRange;
  
  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_DefaultVeloHitManager; }

    /// Standard Constructor
    DefaultVeloHitManager(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

    virtual ~DefaultVeloHitManager() {;} ///< Destructor

    StatusCode initialize(); ///< Tool initialization
    StatusCode   finalize(); ///< Tool finalize

    virtual void prepareHits();

    void prepareHits(StationIterator it) { prepareHits(*it); }          ///< Prepare hits for one station only
    void prepareHits(StationReverseIterator rit) { prepareHits(*rit); } ///< Prepare hits for one station only
    void prepareHits(Station* station);        ///< Prepare hits for one station only, implementation
      
  private:

    void prepareClusterRanges();
    void addHit(const LHCb::VeloLiteCluster& clu, double signal, HitBase::velo_rhit_tag);
    void addHit(const LHCb::VeloLiteCluster& clu, double signal, HitBase::velo_phihit_tag);
    void createPointerListsAndSort();

  private:

    //== configuration
    bool m_fromClusters;
    bool m_ignoreLowClusters;
    double m_lowThreshold;
    std::string m_clusterLocation;
    std::string m_liteClusterLocation;

    //= cache 
    std::vector<ClusterRange>     m_clusterRanges;
    std::vector<LiteClusterRange> m_liteClusterRanges;
  };

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::DefaultVeloHitManager(const std::string& type,
                                                                      const std::string& name,
                                                                      const IInterface* parent)
    : VeloHitManager<SENSORTYPE,HIT,NZONES>(type, name, parent)
  {
    GaudiTool::declareInterface<DefaultVeloHitManager<SENSORTYPE,HIT,NZONES> >(this);

    declareProperty("FromClusters",m_fromClusters=false);
    declareProperty("IgnoreLowClusters",m_ignoreLowClusters=false);
    declareProperty("LowThreshold",m_lowThreshold=22.0);
    declareProperty("ClusterLocation",m_clusterLocation=LHCb::VeloClusterLocation::Default);
    declareProperty("LiteClusterLocation",m_liteClusterLocation=LHCb::VeloLiteClusterLocation::Default);
  }

  //=============================================================================
  // Initialization
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  StatusCode DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::initialize()
  {
    StatusCode sc = VeloHitManager<SENSORTYPE,HIT,NZONES>::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    m_clusterRanges.resize(m_stationBySensorNumber.size());
    m_liteClusterRanges.resize(m_stationBySensorNumber.size());

    GaudiTool::debug() << "==> Initialize" << endmsg;

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  StatusCode DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::finalize()
  {
    GaudiTool::debug() << "==> Finalize" << endmsg;


    return VeloHitManager<SENSORTYPE,HIT,NZONES>::finalize();  // must be called after all other actions
  }

  //=============================================================================
  // Prepare the cluster ranges for hit preparation on demand
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::prepareClusterRanges()
  {
    // the new event wil be loaded once this method returns
    m_eventExpired = false;

    if ( m_fromClusters ) { // use full velo clusters as input
      LHCb::VeloClusters* clusters = GaudiTool::get<LHCb::VeloClusters>(m_clusterLocation);

      // invalidate all ranges
      std::fill(m_clusterRanges.begin(), m_clusterRanges.end(),ClusterRange(clusters->end(),clusters->end()));
      
      if (clusters->empty()) return; // nothing to see here, please move along
      
      LHCb::VeloClusters::iterator iClus = clusters->begin();
      LHCb::VeloClusters::iterator start = iClus;
      LHCb::VeloClusters::iterator stop  = start;
      unsigned int currentSensorNumber   = (*start)->channelID().sensor();
      
      for( ; iClus != clusters->end(); ++iClus) {
        if ( (*iClus)->channelID().sensor() != currentSensorNumber ) {
          if (currentSensorNumber < m_stationBySensorNumber.size() && m_stationBySensorNumber[currentSensorNumber]) {
            stop = iClus; 
            m_clusterRanges[currentSensorNumber] = ClusterRange(start,stop);
          }
          start = iClus;
          currentSensorNumber = (*iClus)->channelID().sensor();
        } 
      } // end loop over clusters

      // set the end interator of the last cluster range
      if (currentSensorNumber < m_stationBySensorNumber.size() && m_stationBySensorNumber[currentSensorNumber]) {
        stop = iClus; 
        m_clusterRanges[currentSensorNumber] = ClusterRange(start,stop);
      }
    } else { // the lite cluster case
      LHCb::VeloLiteCluster::FastContainer * liteClusters =
        GaudiTool::get<LHCb::VeloLiteCluster::FastContainer>(m_liteClusterLocation);

      // invalidate all ranges
      std::fill(m_liteClusterRanges.begin(), m_liteClusterRanges.end(),LiteClusterRange(liteClusters->end(),liteClusters->end()));
      
      if (liteClusters->empty()) return; // nothing to see here, please move along
      
      LHCb::VeloLiteCluster::FastContainer::iterator iClus = liteClusters->begin();
      LHCb::VeloLiteCluster::FastContainer::iterator start = iClus;
      LHCb::VeloLiteCluster::FastContainer::iterator stop  = start;
      unsigned int currentSensorNumber   = start->channelID().sensor();

      for(; iClus != liteClusters->end(); ++iClus) {
        if ( iClus->channelID().sensor() != currentSensorNumber ) {
          if (currentSensorNumber < m_stationBySensorNumber.size() && m_stationBySensorNumber[currentSensorNumber]) {
            stop = iClus; 
            m_liteClusterRanges[currentSensorNumber] = LiteClusterRange(start,stop);
          }
          start = iClus;
          currentSensorNumber = iClus->channelID().sensor();
        } 
      } // end loop over lite clusters

      // set the end interator of the last lite cluster range
      if (currentSensorNumber < m_stationBySensorNumber.size() && m_stationBySensorNumber[currentSensorNumber]) {
        stop = iClus; 
        m_liteClusterRanges[currentSensorNumber] = LiteClusterRange(start,stop);
      }      
    }
  }

  //=============================================================================
  // Prepare the hits from Velo(Lite)Clusters in a given station
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::prepareHits(Station* station)
  {
    if ( m_dataValid || station->hitsPrepared() ) return;
    if ( m_eventExpired ) {
      prepareClusterRanges();
    }
    
    if ( m_fromClusters ) { // use full velo clusters as input

      LHCb::VeloClusters::const_iterator iClus = m_clusterRanges[station->sensorNumber()].begin();
      LHCb::VeloClusters::const_iterator stop  = m_clusterRanges[station->sensorNumber()].end();

      for( ; iClus != stop; ++iClus) {

        double signal = (*iClus)->totalCharge();

        // don't use this cluster if we run in ingore mode and the total charge is too low
        if ( m_ignoreLowClusters && (signal < m_lowThreshold) ) continue;

        LHCb::VeloLiteCluster tmpClu( (*iClus)->channelID(),
                                      (*iClus)->interStripFraction(),
                                      (*iClus)->pseudoSize(),
                                      (*iClus)->highThreshold() );

        addHit(tmpClu, signal, typename HIT::hit_type_tag());

      } // end loop over clusters

    } else { // the lite cluster case

      LHCb::VeloLiteCluster::FastContainer::const_iterator iClus = m_liteClusterRanges[station->sensorNumber()].begin();
      LHCb::VeloLiteCluster::FastContainer::const_iterator stop  = m_liteClusterRanges[station->sensorNumber()].end();

      unsigned int cnt = 0;
      for( ; iClus != stop; ++iClus, ++cnt) {

        double signal = 100.0;
        // in ignore low charge cluster mode skip this cluster if high threshold bit unset
        if( !iClus->highThreshold() ) {
          if( m_ignoreLowClusters ) continue;
          signal = 1.0;
        }

        addHit(*iClus, signal, typename HIT::hit_type_tag());
        
      } // end loop over lite clusters
    }

    // create pointer lists and sort
    station->clear();
    unsigned int stationNumber = station->stationNumber();
    unsigned int half          = station->veloHalf();
    for (unsigned int zone=0; zone<NZONES; ++zone) {
      typename std::vector<HIT>::iterator iH  = VeloHitManager<SENSORTYPE,HIT,NZONES>::m_data[half][stationNumber][zone].begin();
      typename std::vector<HIT>::iterator end = VeloHitManager<SENSORTYPE,HIT,NZONES>::m_data[half][stationNumber][zone].end();

      for ( ; iH != end; ++iH ) {
        station->zone(zone).push_back(&*iH);
      }
    }
    station->sort();
    station->setHitsPrepared(true);
  }

  //=============================================================================
  // Prepare the hits from Velo(Lite)Clusters
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::prepareHits()
  {
    for (StationIterator si = m_stationsAll.begin(); si != m_stationsAll.end(); ++si) {
      if ( !(*si)->hitsPrepared() ) {
        prepareHits(si);
      }
    }
    m_dataValid = true;
  }

  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::addHit(const LHCb::VeloLiteCluster& clu, double signal, HitBase::velo_rhit_tag)
  {
    // only import r clusters
    if ( !clu.isRType() ) return;

    const DeVeloRType* rs = m_velo->rSensor(clu.channelID().sensor());
    const unsigned int stationNumber = rs->station();
    const unsigned int half          = static_cast<unsigned int>(rs->isRight());
    const unsigned int zone          = rs->globalZoneOfStrip(clu.channelID().strip());
    m_data[half][stationNumber][zone].push_back(VeloRHit(rs,clu,signal));
  }

  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::addHit(const LHCb::VeloLiteCluster& clu, double signal, HitBase::velo_phihit_tag)
  {
    // only import r clusters
    if ( !clu.isPhiType() ) return;

    const DeVeloPhiType* ps = m_velo->phiSensor(clu.channelID().sensor());
    const unsigned int stationNumber = ps->station();
    const unsigned int half          = static_cast<unsigned int>(ps->isRight());
    const unsigned int zone          = ps->zoneOfStrip(clu.channelID().strip());
    m_data[half][stationNumber][zone].push_back(VeloPhiHit(ps,clu,signal));
  }

}
#endif // INCLUDE_TF_DEFAULTVELOHITMANAGER_H


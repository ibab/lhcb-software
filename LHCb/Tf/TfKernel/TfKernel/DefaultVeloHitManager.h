// $Id: DefaultVeloHitManager.h,v 1.9 2007-09-19 13:48:19 krinnert Exp $
#ifndef INCLUDE_TF_DEFAULTVELOHITMANAGER_H
#define INCLUDE_TF_DEFAULTVELOHITMANAGER_H 1

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
    using VeloHitManager<SENSORTYPE,HIT,NZONES>::m_velo;

  public:

    typedef typename VeloHitManager<SENSORTYPE,HIT,NZONES>::Station                Station;
    typedef typename VeloHitManager<SENSORTYPE,HIT,NZONES>::StationIterator        StationIterator;
    typedef typename VeloHitManager<SENSORTYPE,HIT,NZONES>::StationReverseIterator StationReverseIterator;

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

  private:

    void addHit(const LHCb::VeloLiteCluster& clu, float signal, HitBase::velo_rhit_tag);
    void addHit(const LHCb::VeloLiteCluster& clu, float signal, HitBase::velo_phihit_tag);
    void createPointerListsAndSort();

  private:

    //== configuration
    bool m_fromClusters;
    bool m_ignoreLowClusters;
    double m_lowThreshold;
    std::string m_clusterLocation;
    std::string m_liteClusterLocation;

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
  // Prepare the hits from Velo(Lite)Clusters
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::prepareHits()
  {
    if ( m_fromClusters ) { // use full velo clusters as input
      LHCb::VeloClusters* clusters = GaudiTool::get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);

      LHCb::VeloClusters::const_iterator iClus;

      for(iClus = clusters->begin(); iClus != clusters->end(); ++iClus) {

        float signal = static_cast<float>((*iClus)->totalCharge());

        // don't use this cluster if we run in ingore mode and the total charge is too low
        if ( m_ignoreLowClusters && (signal < m_lowThreshold) ) continue;

        LHCb::VeloLiteCluster tmpClu( (*iClus)->channelID(),
                                      (*iClus)->interStripFraction(),
                                      (*iClus)->pseudoSize(),
                                      (*iClus)->highThreshold() );

        addHit(tmpClu, signal, typename HIT::hit_type_tag());

      } // end loop over clusters

    } else { // the lite cluster case
      LHCb::VeloLiteCluster::FastContainer * liteClusters =
        GaudiTool::get<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default);

      LHCb::VeloLiteCluster::FastContainer::const_iterator iClus;

      for(iClus = liteClusters->begin(); iClus != liteClusters->end(); ++iClus) {

        float signal = 100.0;
        // in ignore low charge cluster mode skip this cluster if high threshold bit unset
        if( !iClus->highThreshold() ) {
          if( m_ignoreLowClusters ) continue;
          signal = 1.0;
        }

        addHit(*iClus, signal, typename HIT::hit_type_tag());

      } // end loop over lite clusters
    }

    createPointerListsAndSort();

    m_dataValid = true;
  }

  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::addHit(const LHCb::VeloLiteCluster& clu, float signal, HitBase::velo_rhit_tag)
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
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::addHit(const LHCb::VeloLiteCluster& clu, float signal, HitBase::velo_phihit_tag)
  {
    // only import r clusters
    if ( !clu.isPhiType() ) return;

    const DeVeloPhiType* ps = m_velo->phiSensor(clu.channelID().sensor());
    const unsigned int stationNumber = ps->station();
    const unsigned int half          = static_cast<unsigned int>(ps->isRight());
    const unsigned int zone          = ps->zoneOfStrip(clu.channelID().strip());
    m_data[half][stationNumber][zone].push_back(VeloPhiHit(ps,clu,signal));
  }

  template <typename SENSORTYPE, typename HIT, int NZONES>
  void DefaultVeloHitManager<SENSORTYPE,HIT,NZONES>::createPointerListsAndSort()
  {
    for (unsigned int half=0; half<VeloHitManager<SENSORTYPE,HIT,NZONES>::m_nHalfs; ++ half) {

      for (StationIterator iS = VeloHitManager<SENSORTYPE,HIT,NZONES>::m_stationsHalf[half].begin();
           iS != VeloHitManager<SENSORTYPE,HIT,NZONES>::m_stationsHalf[half].end();
           ++iS ) {
        Station* station = *iS;
        station->clear();
        unsigned int stationNumber = station->stationNumber();
        for (unsigned int zone=0; zone<NZONES; ++zone) {
          typename std::vector<HIT>::iterator iH  = VeloHitManager<SENSORTYPE,HIT,NZONES>::m_data[half][stationNumber][zone].begin();
          typename std::vector<HIT>::iterator end = VeloHitManager<SENSORTYPE,HIT,NZONES>::m_data[half][stationNumber][zone].end();

          for ( ; iH != end; ++iH ) {
            station->zone(zone).push_back(&*iH);
          }
        }
        station->sort();
      }

    }
  }

}
#endif // INCLUDE_TF_DEFAULTVELOHITMANAGER_H


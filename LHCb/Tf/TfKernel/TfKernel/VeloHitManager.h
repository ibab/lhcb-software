// $Id: VeloHitManager.h,v 1.5 2007-08-26 14:10:41 krinnert Exp $
#ifndef INCLUDE_TF_VELOHITMANAGER_H
#define INCLUDE_TF_VELOHITMANAGER_H 1

#include <vector>
#include <string>

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiAlg/GaudiTool.h"

#include "VeloDet/DeVelo.h"

#include "TfKernel/RegionID.h"
#include "TfKernel/VeloSensorHits.h"
#include "TfKernel/TfIDTypes.h"

class DeVelo;

namespace Tf
{
  /** @class VeloHitManager VeloHitManager.h
   *  Base class for structured access to VELO measurements.
   *
   *
   * @see DeVelo
   * @see DeVeloSensor
   * @see VeloSensorHits
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-07-30
   */

  static const InterfaceID IID_VeloHitManager( "Tf::VeloHitManager", 1, 0 );

  template <typename SENSORTYPE, typename HIT, int NZONES>
  class VeloHitManager
    : public GaudiTool
    , public IIncidentListener
  {
  public:

    typedef VeloSensorHits<SENSORTYPE,HIT,NZONES> Station;                ///< how we call a specific station
    typedef std::vector<Station*>                 Stations;               ///< how we call list of specific stations
    typedef typename Stations::iterator           StationIterator;        ///< shortcut for station iterator
    typedef typename Stations::reverse_iterator   StationReverseIterator; ///< shortcut for station reverse iterator

  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_VeloHitManager; }

    /// Standard Constructor
    VeloHitManager(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    ~VeloHitManager();  ///< Destructor

    StatusCode initialize(); ///< Tool initialization
    StatusCode   finalize(); ///< Tool finalize

    /// Retrieve station iterator based on sensor number
    Station* station(const VeloSensorID sensorNumber)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationBySensorNumber[sensorNumber]; }

    /// Retrieve iterator into VELO half station list by sensor number
    StationIterator stationIterHalf(const VeloSensorID sensorNumber)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationIterHalfBySensorNumber[sensorNumber]; }
    
    /// Retrieve iterator into full VELO station list by sensor number
    StationIterator stationIterAll(const VeloSensorID sensorNumber)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationIterAllBySensorNumber[sensorNumber]; }
    
    /// Retrieve iterator to first station in a VELO half
    StationIterator stationsHalfBegin(const VeloHalfID half)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsHalf[half].begin(); }

    /// Retrieve iterator to the end of stations in a VELO half
    StationIterator stationsHalfEnd(const VeloHalfID half)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsHalf[half].end(); }

    /// Retrieve reverse iterator to last station in a VELO half
    StationReverseIterator stationsHalfReverseBegin(const VeloHalfID  half)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsHalf[half].rbegin(); }

    /// Retrieve reverse iterator to the reverse end of stations in a VELO half
    StationReverseIterator stationsHalfReverseEnd(const VeloHalfID  half)
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsHalf[half].rend(); }

    /// Retrieve iterator to first station in a VELO half
    StationIterator stationsAllBegin()
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsAll.begin(); }

    /// Retrieve iterator to the end of stations in a VELO half
    StationIterator stationsAllEnd()
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsAll.end(); }

    /// Retrieve reverse iterator to last station in a VELO half
    StationReverseIterator stationsAllReverseBegin()
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsAll.rbegin(); }

    /// Retrieve reverse iterator to the reverse end of stations in a VELO half
    StationReverseIterator stationsAllReverseEnd()
    { if (!m_dataValid) { this->prepareHits(); } ; return m_stationsAll.rend(); }

    /// The preparation of hits, implemented by derived classes
    virtual void prepareHits() = 0;

    /// update of geometry related information
    StatusCode updateStationStructure();

    /// incident service handle
    void handle( const Incident& incident );

  protected:

    /// Clear the hits.
    virtual void clearHits();

  protected:

    static const unsigned int m_nHalfs    = RegionID::VeloRIndex::kNHalfs;
    static const unsigned int m_nStations = RegionID::VeloRIndex::kNStations;
    static const unsigned int m_nZones    = NZONES;

    /// The list of stations by VELO half. Sorted by z in each half.
    mutable Stations m_stationsHalf[m_nHalfs];

    /// The list of all stations sorted by z.
    mutable Stations m_stationsAll;

    /// mapping from sensor numbers to stations
    mutable Stations m_stationBySensorNumber;

    /// mapping from sensor number to station iterators in a VELO half
    mutable std::vector<StationIterator> m_stationIterHalfBySensorNumber;

    /// mapping from sensor number to station iterators in the whole VELO
    mutable std::vector<StationIterator> m_stationIterAllBySensorNumber;
    
    /// The data
    mutable std::vector<HIT> m_data[m_nHalfs][m_nStations][NZONES];

    /// Cache validity flag
    mutable bool m_dataValid;

    /// access to VELO detector element
    DeVelo* m_velo;

    //== configuration
    std::string m_detectorLocation;
  };


  //=============================================================================
  // Standard constructor
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  VeloHitManager<SENSORTYPE,HIT,NZONES>::VeloHitManager(const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent)
    : GaudiTool(type, name, parent)
    , m_dataValid(false)
  {
    declareInterface<VeloHitManager<SENSORTYPE,HIT,NZONES> >(this);

    declareProperty("DetectorLocation",m_detectorLocation=DeVeloLocation::Default);
  }

  //=============================================================================
  // Destructor
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  VeloHitManager<SENSORTYPE,HIT,NZONES>::~VeloHitManager()
  {
    for (StationIterator iS = m_stationsAll.begin();
        iS !=  m_stationsAll.end();
        ++iS) {
      delete *iS;
    }
  }

  //=============================================================================
  // Initialization
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  StatusCode VeloHitManager<SENSORTYPE,HIT,NZONES>::initialize()
  {
    StatusCode sc = GaudiTool::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    debug() << "==> Initialize" << endmsg;

    m_velo = getDet<DeVelo>( m_detectorLocation );

    // make sure we are up-to-date on populated VELO stations
    registerCondition(m_velo->geometry(), &Tf::VeloHitManager<SENSORTYPE,HIT,NZONES>::updateStationStructure);

    // first update
    sc = updMgrSvc()->update(this);
    if(!sc.isSuccess()) {
      error() << "Failed to update station structure." << endreq;
      return sc;
    }
  
    // invalidate measurement cache at the end of each event
    incSvc()->addListener(this, IncidentType::EndEvent);

    return sc;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  StatusCode VeloHitManager<SENSORTYPE,HIT,NZONES>::finalize()
  {
    debug() << "==> Finalize" << endmsg;


    return GaudiTool::finalize();  // must be called after all other actions
  }

  //=============================================================================
  // Update the station structure
  //=============================================================================
  template <typename SENSORTYPE, typename HIT, int NZONES>
  StatusCode VeloHitManager<SENSORTYPE,HIT,NZONES>::updateStationStructure()
  {
    // clean up
    for (unsigned int s=0; s<m_stationsAll.size(); ++s) {
      delete m_stationsAll[s];
    }
    m_stationsAll.clear();
    for (unsigned int half=0; half<m_nHalfs; ++half) {
      m_stationsHalf[half].clear();
    }

    // the maximum sensor number, determines size of sparse vector used for mapping
    unsigned int maxSensorNumber=0;

    typename std::vector<DeVeloSensor*>::const_iterator sensorIter = m_velo->sensorsBegin();
    typename std::vector<DeVeloSensor*>::const_iterator sensorsEnd = m_velo->sensorsEnd();

    for ( ; sensorIter != sensorsEnd; ++sensorIter ) {
      if ( (*sensorIter)->isPileUp() ) continue; // ignore pile up sensors
      SENSORTYPE* s = dynamic_cast<SENSORTYPE*>(*sensorIter);
      if ( !s ) continue; // only create stations for the correct type
      if (s->sensorNumber() > maxSensorNumber) maxSensorNumber = s->sensorNumber();
      m_stationsAll.push_back(new VeloSensorHits<SENSORTYPE,HIT,NZONES>(s));
      m_stationsHalf[static_cast<unsigned int>(s->isRight())].push_back(m_stationsAll.back());
    }

    // sort by station z position
    for (unsigned int half=0; half<m_nHalfs; ++half) {
      std::sort(m_stationsHalf[half].begin(),m_stationsHalf[half].end(),typename VeloSensorHits<SENSORTYPE,HIT,NZONES>::ZLessThan());
    }
    std::sort(m_stationsAll.begin(),m_stationsAll.end(),typename VeloSensorHits<SENSORTYPE,HIT,NZONES>::ZLessThan());

    // create mapping from sensor numbers to station iterators
    m_stationBySensorNumber.resize(maxSensorNumber+1);
    m_stationIterHalfBySensorNumber.resize(maxSensorNumber+1);
    m_stationIterAllBySensorNumber.resize(maxSensorNumber+1);

    for (unsigned int half=0; half<m_nHalfs; ++half) {
      for (StationIterator si = m_stationsHalf[half].begin();
           si != m_stationsHalf[half].end();
           ++si) {
        m_stationIterHalfBySensorNumber[(*si)->sensor()->sensorNumber()] = si;
      }
    }
    for (StationIterator si = m_stationsAll.begin();
        si != m_stationsAll.end();
        ++si) {
      m_stationIterAllBySensorNumber[(*si)->sensor()->sensorNumber()] = si;
      m_stationBySensorNumber[(*si)->sensor()->sensorNumber()] = *si;
    }

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Incident handle
  //=============================================================================

  template <typename SENSORTYPE, typename HIT, int NZONES>
  void VeloHitManager<SENSORTYPE,HIT,NZONES>::handle ( const Incident& incident )
  {
    if ( IncidentType::EndEvent == incident.type() ){
      this->clearHits();
      m_dataValid = false;
    }
  }

  //=============================================================================
  // Cleaner
  //=============================================================================

  template <typename SENSORTYPE, typename HIT, int NZONES>
  void VeloHitManager<SENSORTYPE,HIT,NZONES>::clearHits()
  {
    for (unsigned int half=0; half<m_nHalfs; ++ half) {
      for (unsigned int station=0; station<m_nStations; ++station) {
        for (unsigned int zone=0; zone<NZONES; ++zone) {
          m_data[half][station][zone].clear();
        }
      }
    }
    for (StationIterator si=m_stationsAll.begin(); si!=m_stationsAll.end(); ++si) {
      (*si)->clear();
    }

    m_dataValid = false;
  }

} // namespace Tf
#endif // INCLUDE_TF_VELOHITMANAGER_H


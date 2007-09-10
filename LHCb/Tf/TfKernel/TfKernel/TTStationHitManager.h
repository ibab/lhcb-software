
//-----------------------------------------------------------------------------
/** @file TTStationHitManager.h
 *
 *  Header file for class : Tf::TTStationHitManager
 *
 *  $Id: TTStationHitManager.h,v 1.15 2007-09-10 08:54:31 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_TTSTATIONHITMANAGER_H
#define TFKERNEL_TTSTATIONHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/ITTHitCreator.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"
#include "TfKernel/TfIDTypes.h"
#include "TfKernel/RegionSelectors.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// boost
//#include <boost/lambda/lambda.hpp>
//#include <boost/lambda/bind.hpp>

/// Static interface ID
static const InterfaceID IID_TTStationHitManager ( "TTStationHitManager", 1, 0 );

namespace Tf
{

  /** @class TTStationHitManager TTStationHitManager.h TfKernel/TTStationHitManager.h 
   *
   *  TT station hit manager. Used to manage extended hit objects for the TT
   *  Stations. 
   *
   *  Methods are provided to return the hits in a selected part of the detectors.
   *  E.g.
   *
   *  @code
   *  // Get all the hits in the TT stations
   *  Tf::TTStationHitManager::HitRange range = hitMan->hits();
   *
   *  // Get all the hits in one specific TT station
   *  Tf::TTStationID sta = ...;
   *  Tf::TTStationHitManager::HitRange range = hitMan->hits(sta);
   *
   *  // Get all the hits in one specific layer of one TT station
   *  Tf::TTStationID sta = ...;
   *  Tf::TTLayerID   lay = ...;
   *  Tf::TTStationHitManager::HitRange range = hitMan->hits(sta,lay);
   *
   *  // Get all the hits in a specific 'region' of one layer of one TT station
   *  Tf::TTStationID sta = ...;
   *  Tf::TTLayerID   lay = ...;
   *  Tf::TTRegionID  reg = ...;
   *  Tf::TTStationHitManager::HitRange range = hitMan->hits(sta,lay,reg);
   *  @endcode
   *
   *  In addition, it is possible to perform a custom selection of hits based on
   *  a user defined selection object :-
   *
   *  @code
   *  // Get all the hits selected by a specfic selector object
   *  LHCb::State * test_state = ....;
   *  const double nsigma = 3.0;
   *  StateRegionSelector selector( *test_state, nsigma );
   *  hitMan->prepareHits(selector);
   *  // Can now use any of the hits(..) methods to access hit ranges, e.g.
   *  Tf::TTStationHitManager::HitRange range = hitMan->hits();
   *  // Which gives the selected hits in all stations, layers and regions in the TT stations
   *  @endcode
   *
   *  The only requirement on the selector object used is it must implement one specific method.
   *  See Tf::StateRegionSelector for an example of such an object.
   *
   *  In all cases the returned Range object acts like a standard vector or container :-
   *
   *  @code
   *   // Iterate over the returned range
   *  for ( Tf::TTStationHitManager::HitRange::const_iterator iR = range.begin();
   *        iR != range.end(); ++iR )
   *  {
   *    // do something with the hit
   *  }
   *  @endcode
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  template < class Hit >
  class TTStationHitManager : public GaudiTool,
                              public IIncidentListener
  {

  public:

    /// Type for container of Hits
    typedef std::vector< Hit* > Hits;

    /// range object for Hits
    typedef LoKi::Range_<Hits> HitRange;

  public:

    /// InterfaceID for this tool
    static const InterfaceID& interfaceID() { return IID_TTStationHitManager; }

    /// Standard Constructor
    TTStationHitManager( const std::string& type,
                         const std::string& name,
                         const IInterface* parent ) ;

    /// Destructor
    ~TTStationHitManager( ) { }

    /// Tool initialization
    StatusCode initialize ( );

    /// Tool finalization
    StatusCode finalize ( );

    /// Handle method for incident service callbacks
    void handle ( const Incident& incident )
    {
      if ( IncidentType::EndEvent == incident.type() ) clearHits() ;
    }

  public:

    /** Initialise the hits for the current event using the given selector object
     *
     *  Any selector object can be used, the only requirement is it must implement the
     *  method :-
     *  @code
     *    inline XYSearchWindow searchWindow( const double z ) const
     *  @endcode
     *  Which returns the search window for a given z position.
     *
     *  Example usage, using the StateRegionSelector class which creates an N sigma
     *  search window around a track state, using a 2nd order parameterisation :-
     *  @code
     *   LHCb::State * test_state = ....;
     *   const double nsigma = 3.0;
     *   StateRegionSelector selector( *test_state, nsigma );
     *   hitMan->prepareHits(selector);
     *  @endcode 
     *
     *  @param[in] selector The selector object.
     */
    template < typename SELECTOR >
    void prepareHitsInWindow( const SELECTOR & selector );

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const TTStationID sta,
                          const TTLayerID lay,
                          const TTRegionID region ) const
    {
      if (!allHitsPrepared(sta,lay,region)) prepareHits(sta,lay,region);
      return HitRange( m_hits[sta][lay][region].begin(),
                       m_hits[sta][lay][region].end() );
    }

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const TTStationID sta,
                          const TTLayerID lay ) const
    {
      if (!allHitsPrepared(sta,lay)) prepareHits(sta,lay);
      return HitRange( m_hits_layers[sta][lay].begin(),
                       m_hits_layers[sta][lay].end() );
    }

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const TTStationID sta ) const
    {
      if (!allHitsPrepared(sta)) prepareHits(sta);
      return HitRange( m_hits_stations[sta].begin(),
                       m_hits_stations[sta].end() );
    }

    /** Load the all hits
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( ) const
    {
      if (!allHitsPrepared()) prepareHits();
      return HitRange( m_hits_all.begin(),
                       m_hits_all.end() );
    }

    /** Load the hits for a given region of interest
     *  In addition, specify a minimum x value
     *
     *  @param[in] xMin   Minimum x value of hit (at y=0)
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hitsWithMinX( const double xMin,
                                  const TTStationID sta,
                                  const TTLayerID lay,
                                  const TTRegionID region ) const
    {
      if (!allHitsPrepared(sta,lay,region)) prepareHits(sta,lay,region);
      return HitRange( std::lower_bound( m_hits[sta][lay][region].begin(),
                                         m_hits[sta][lay][region].end(),
                                         xMin,
                                         Tf::compByX_LB< Hit >() ),
                       m_hits[sta][lay][region].end() );
    }

    /** Retrieve the STRegion for a certain TT region ID. The region
     *   knows its 'size' and gives access to its hits.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return Pointer to the STRegion object
     */
    inline const ITTHitCreator::STRegion* region(const TTStationID iStation,
                                                 const TTLayerID iLayer,
                                                 const TTRegionID iRegion) const
    {
      return this->ttHitCreator()->region(iStation,iLayer,iRegion);
    }

  public:

    /** Sort the hits in a given station, layer and region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    template < typename SORTER >
    inline void sortHits( const TTStationID sta,
                          const TTLayerID lay,
                          const TTRegionID region )
    {
      std::sort ( m_hits[sta][lay][region].begin(), m_hits[sta][lay][region].end(), SORTER() );
    }

    /** Sort the hits in a given station and layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    template < typename SORTER >
    inline void sortHits( const TTStationID sta,
                          const TTLayerID lay )
    {
      std::sort ( m_hits_layers[sta][lay].begin(), m_hits_layers[sta][lay].end(), SORTER() );
    }

    /** Sort the hits in a given station
     *  @param[in] sta    Station ID
     */
    template < typename SORTER >
    inline void sortHits( const TTStationID sta )
    {
      std::sort ( m_hits_stations[sta].begin(), m_hits_stations[sta].end(), SORTER() );
    }

    /** Sort the all the hits
     */
    template < typename SORTER >
    inline void sortHits( )
    {
      std::sort ( m_hits_all.begin(), m_hits_all.end(), SORTER() );
    }

    /// Maximum number of stations
    inline TTStationID maxStations() const { return TTStationID(m_nSta); }

    /// Maximum number of layers
    inline TTLayerID maxLayers()     const { return TTLayerID(m_nLay); }

    /// Maximum number of regions
    inline TTRegionID maxRegions()   const { return TTRegionID(m_nReg); }

  protected:

    /// Direct access to the underlying hit creator
    inline const Tf::ITTHitCreator * ttHitCreator() const { return &*m_tthitcreator; }

    /** Add a hit to the container
     *  @param hit Pointer to the hit to add
     *  @param[in] sta    The station number
     *  @param[in] lay    The layer number
     *  @param[in] region The region number
     */
    inline void addHit( Hit * hit,
                        const TTStationID sta,
                        const TTLayerID lay,
                        const TTRegionID region ) const
    {
      m_hits[sta][lay][region].push_back(hit);
      // temporary hack, to get things working. Needs to be done better
      m_hits_layers[sta][lay].push_back(hit);
      m_hits_stations[sta].push_back(hit);
      m_hits_all.push_back(hit);
    }

  public:

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    void prepareHits(const TTStationID sta,
                     const TTLayerID lay,
                     const TTRegionID region) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    void prepareHits(const TTStationID sta,
                     const TTLayerID lay) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     */
    void prepareHits(const TTStationID sta) const;

    /** Initialise all the hits for the current event
     */
    virtual void prepareHits() const;

  protected:

    /// Clear the hit containers for a new event
    void clearHits () const;

    /** Set the hits ready flag for given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const TTStationID sta,
                                    const TTLayerID lay,
                                    const TTRegionID region,
                                    const bool ok ) const { m_hits_ready[sta][lay][region] = ok; }

    /** Set the hits ready flag for given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const TTStationID sta,
                                    const TTLayerID lay,
                                    const bool ok ) const { m_hits_layers_ready[sta][lay] = ok; }

    /** Set the hits ready flag for given station
     *  @param[in] sta    Station ID
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const TTStationID sta,
                                    const bool ok ) const { m_hits_stations_ready[sta] = ok; }

    /** Set the hits ready flag for all hits
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const bool ok ) const { m_hits_all_ready = ok; }

    /** Are all the hits ready
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared() const { return m_hits_all_ready; }

    /** Are all the hits ready in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const TTStationID sta,
                                const TTLayerID lay,
                                const TTRegionID region) const
    { return m_hits_ready[sta][lay][region]; }

    /** Are all the hits ready in the given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const TTStationID sta,
                                const TTLayerID lay) const
    { return m_hits_layers_ready[sta][lay]; }

    /** Are all the hits ready in the given station
     *  @param[in] sta    Station ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const TTStationID sta) const
    { return m_hits_stations_ready[sta]; }

  private:

    /// max number of stations
    static const int m_nSta = Tf::RegionID::TTIndex::kNStations;
    /// max number of layers
    static const int m_nLay = Tf::RegionID::TTIndex::kNLayers;
    /// max number of regions
    static const int m_nReg = Tf::RegionID::TTIndex::kNRegions;

    /// The underlying TT hit creator
    ToolHandle<Tf::ITTHitCreator> m_tthitcreator ;

    mutable Hits m_hits[m_nSta][m_nLay][m_nReg]; ///< Hits in individual regions
    // CRJ : This is ugly but just a first bash to provide *something* that works
    //       Need to try and find a neater way to handle these different pointer containers
    mutable Hits m_hits_layers[m_nSta][m_nLay]; ///< Hits in individual layers
    mutable Hits m_hits_stations[m_nSta];       ///< Hits in individual stations
    mutable Hits m_hits_all;                    ///< All hits

    // Flags to indicate which hits are ready
    mutable bool m_hits_ready[m_nSta][m_nLay][m_nReg]; ///< Flags to indicate which regions have hits ready
    mutable bool m_hits_layers_ready[m_nSta][m_nLay];  ///< Flag to indicate which layers are ready
    mutable bool m_hits_stations_ready[m_nSta];        ///< Flag to indicate which stations are ready
    mutable bool m_hits_all_ready;                     ///< Flag to indicate all hits are ready

  };

  template<class Hit>
  TTStationHitManager<Hit>::TTStationHitManager( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
    GaudiTool (type, name, parent),
    m_tthitcreator("Tf::STHitCreator<Tf::TT>/TTHitCreator")
  {
    declareInterface<TTStationHitManager<Hit> >(this);
  }

  template<class Hit>
  StatusCode TTStationHitManager<Hit>::initialize ( )
  {
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    sc = m_tthitcreator.retrieve();
    if ( sc.isFailure() ) return sc;

    // set up to be told about each new event
    incSvc()->addListener(this, IncidentType::EndEvent);

    // make sure we are ready for first event
    this->clearHits();

    return sc;
  }

  template<class Hit>
  StatusCode TTStationHitManager<Hit>::finalize ( )
  {
    this->clearHits();
    return GaudiTool::finalize();
  }

  template<class Hit>
  void TTStationHitManager<Hit>::clearHits() const
  {
    m_hits_all.clear();
    this->setAllHitsPrepared(false);
    for ( TTStationID s = 0; s<maxStations(); ++s )
    {
      this->setAllHitsPrepared(s,false);
      m_hits_stations[s].clear();
      for ( TTLayerID l = 0; l<maxLayers(); ++l )
      {
        this->setAllHitsPrepared(s,l,false);
        m_hits_layers[s][l].clear();
        for ( TTRegionID t = 0; t<maxRegions(); ++t )
        {
          this->setAllHitsPrepared(s,l,t,false);
          for ( typename Hits::iterator iHit = m_hits[s][l][t].begin();
                iHit != m_hits[s][l][t].end(); ++iHit )
          {
            delete *iHit;
          }
          m_hits[s][l][t].clear();
        }
      }
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits() const
  {
    if ( !this->allHitsPrepared() )
    {
      for (TTStationID sta=0; sta<maxStations(); ++sta )
      {
        this->prepareHits(sta);
      }
      setAllHitsPrepared(true);
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits(const TTStationID sta) const
  {
    if ( !this->allHitsPrepared(sta) )
    {
      for (TTLayerID lay=0; lay<maxLayers(); ++lay )
      {
        this->prepareHits(sta,lay);
      }
      setAllHitsPrepared(sta,true);
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits(const TTStationID sta,
                                             const TTLayerID lay) const
  {
    if ( !this->allHitsPrepared(sta,lay) )
    {
      for (TTRegionID reg=0; reg<maxRegions(); ++reg )
      {
        this->prepareHits(sta,lay,reg);
      }
      setAllHitsPrepared(sta,lay,true);
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits(const TTStationID sta,
                                             const TTLayerID lay,
                                             const TTRegionID region) const
  {
    if ( !this->allHitsPrepared(sta,lay,region) )
    {
      Tf::STHitRange tthits = ttHitCreator()->hits(sta,lay,region) ;
      for (Tf::STHitRange::const_iterator itTTH = tthits.begin();
           itTTH < tthits.end(); itTTH++)
      {
        this -> addHit(new Hit(**itTTH),sta,lay,region);
      }
      setAllHitsPrepared(sta,lay,region,true);
    }
  }

  template < class Hit         >
  template < typename SELECTOR >
  void TTStationHitManager<Hit>::prepareHitsInWindow( const SELECTOR & selector )
  {
    this->clearHits();
    for (TTStationID sta=0; sta<maxStations(); ++sta )
    {
      this->setAllHitsPrepared(sta,true);
      for (TTLayerID lay=0; lay<maxLayers(); ++lay )
      {
        this->setAllHitsPrepared(sta,lay,true);
        for (TTRegionID reg=0; reg<maxRegions(); ++reg )
        {
          this->setAllHitsPrepared(sta,lay,reg,true);

          // The z value for this region
          const double z = this->ttHitCreator()->region(sta,lay,reg)->z();
          // get the search window
          XYSearchWindow win = selector.searchWindow(z);
          // get the hits in the search window
          Tf::STHitRange tthits = this->ttHitCreator()->hits(sta,lay,reg,
                                                             win.minX(),win.maxX(),
                                                             win.minY(),win.maxY() );
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << tthits.size() << " TTHits for station=" << sta
                    << " layer=" << lay << " region=" << reg << endreq;
          }
          for (Tf::STHitRange::const_iterator itTTH = tthits.begin();
               itTTH < tthits.end(); itTTH++)
          {
            this -> addHit(new Hit(**itTTH),sta,lay,reg);
          }
        }
      }// layer
    } // station
  }

} // Tf namespace

#endif // TFKERNEL_TTSTATIONHITMANAGER_H

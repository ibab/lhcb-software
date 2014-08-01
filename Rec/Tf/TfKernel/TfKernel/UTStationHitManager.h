//-----------------------------------------------------------------------------
/** @file UTStationHitManager.h
 *
 *  Header file for class : Tf::UTStationHitManager
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_UTSTATIONHITMANAGER_H
#define TFKERNEL_UTSTATIONHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/IndexedHitContainer.h"
#include "TfKernel/IndexedBitArray.h"
#include "TfKernel/IUTHitCreator.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"
#include "TfKernel/TfIDTypes.h"
#include "TfKernel/RegionSelectors.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// boost
#include "boost/iterator/transform_iterator.hpp"

/// Static interface ID
static const InterfaceID IID_UTStationHitManager ( "UTStationHitManager", 1, 0 );

namespace Tf
{

  /** @class UTStationHitManager UTStationHitManager.h TfKernel/UTStationHitManager.h 
   *
   *  UT station hit manager. Used to manage extended hit objects for the UT
   *  Stations. 
   *
   *  Methods are provided to return the hits in a selected part of the detectors.
   *  E.g.
   *
   *  @code
   *  // Get all the hits in the UT stations
   *  Tf::UTStationHitManager::HitRange range = hitMan->hits();
   *
   *  // Get all the hits in one specific UT station
   *  Tf::UTStationID sta = ...;
   *  Tf::UTStationHitManager::HitRange range = hitMan->hits(sta);
   *
   *  // Get all the hits in one specific layer of one UT station
   *  Tf::UTStationID sta = ...;
   *  Tf::UTLayerID   lay = ...;
   *  Tf::UTStationHitManager::HitRange range = hitMan->hits(sta,lay);
   *
   *  // Get all the hits in a specific 'region' of one layer of one UT station
   *  Tf::UTStationID sta = ...;
   *  Tf::UTLayerID   lay = ...;
   *  Tf::UTRegionID  reg = ...;
   *  Tf::UTStationHitManager::HitRange range = hitMan->hits(sta,lay,reg);
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
   *  Tf::UTStationHitManager::HitRange range = hitMan->hits();
   *  // Which gives the selected hits in all stations, layers and regions in the UT stations
   *  @endcode
   *
   *  The only requirement on the selector object used is it must implement one specific method.
   *  See Tf::StateRegionSelector for an example of such an object.
   *
   *  In all cases the returned Range object acts like a standard vector or container :-
   *
   *  @code
   *   // Iterate over the returned range
   *  for ( Tf::UTStationHitManager::HitRange::const_iterator iR = range.begin();
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
  class UTStationHitManager : public GaudiTool,
                              public IIncidentListener
  {

  public:

    /// Type for container of Hits
    typedef  IndexedHitContainer< Tf::RegionID::UTIndex::kNStations, 
                                  Tf::RegionID::UTIndex::kNLayers, 
                                  Tf::RegionID::UTIndex::kNRegions,
                                  Hit* > Hits;

    /// range object for Hits
    typedef typename Hits::HitRange HitRange;

  public:

    /// InterfaceID for this tool
    static const InterfaceID& interfaceID() { return IID_UTStationHitManager; }

    /// Standard Constructor
    UTStationHitManager( const std::string& type,
                         const std::string& name,
                         const IInterface* parent ) ;

    /// Destructor
    ~UTStationHitManager( ) { }

    /// Tool initialization
    StatusCode initialize ( );

    /// Tool finalization
    StatusCode finalize ( );

    /// Handle method for incident service callbacks
    void handle ( const Incident& incident )
    {
      if ( IncidentType::BeginEvent == incident.type() ) clearHits() ;
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
    inline HitRange hits( const UTStationID sta,
                          const UTLayerID lay,
                          const UTRegionID region ) const
    {
      if (!allHitsPrepared(sta,lay,region)) prepareHits(sta,lay,region);
      return m_hits.range(sta,lay,region);
    }

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const UTStationID sta,
                          const UTLayerID lay ) const
    {
      if (!allHitsPrepared(sta,lay)) prepareHits(sta,lay);
      return m_hits.range(sta,lay);
    }

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const UTStationID sta ) const
    {
      if (!allHitsPrepared(sta)) prepareHits(sta);
      return m_hits.range(sta);
    }

    /** Load the all hits
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( ) const
    {
      if (!allHitsPrepared()) prepareHits();
      return m_hits.range();
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
                                  const UTStationID sta,
                                  const UTLayerID lay,
                                  const UTRegionID region ) const
    {
      if (!allHitsPrepared(sta,lay,region)) prepareHits(sta,lay,region);
      HitRange range = m_hits.range(sta,lay,region);
      return HitRange( std::lower_bound( range.begin(),
                                         range.end(),
                                         xMin,
                                         Tf::compByX_LB< Hit >() ),
                       range.end() );
    }

    /** Retrieve the STRegion for a certain UT region ID. The region
     *   knows its 'size' and gives access to its hits.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return Pointer to the STRegion object
     */
    inline const IUTHitCreator::STRegion* region(const UTStationID iStation,
                                                 const UTLayerID iLayer,
                                                 const UTRegionID iRegion) const
    {
      return this->utHitCreator()->region(iStation,iLayer,iRegion);
    }

  public:

    /** Sort the hits in a given station, layer and region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    template < typename SORTER >
    inline void sortHits( const UTStationID sta,
                          const UTLayerID lay,
                          const UTRegionID region )
    {
      std::pair<typename Hits::iterator,typename Hits::iterator>  rng = m_hits.range_(sta,lay,region);
      std::sort ( rng.first, rng.second, SORTER() );
    }

    /// Maximum number of stations
    inline UTStationID maxStations() const { return UTStationID(Tf::RegionID::UTIndex::kNStations); }

    /// Maximum number of layers
    inline UTLayerID maxLayers()     const { return UTLayerID(Tf::RegionID::UTIndex::kNLayers); }

    /// Maximum number of regions
    inline UTRegionID maxRegions()   const { return UTRegionID(Tf::RegionID::UTIndex::kNRegions); }

  protected:

    /// Direct access to the underlying hit creator
    inline const Tf::IUTHitCreator * utHitCreator() const { return &*m_uthitcreator; }

  public:

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    void prepareHits(const UTStationID sta,
                     const UTLayerID lay,
                     const UTRegionID region) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    void prepareHits(const UTStationID sta,
                     const UTLayerID lay) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     */
    void prepareHits(const UTStationID sta) const;

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
    inline void setAllHitsPrepared( const UTStationID sta,
                                    const UTLayerID lay,
                                    const UTRegionID region
                                  ) const { m_hits_ready.set(sta,lay,region); }

    /** Are all the hits ready
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared() const { return m_hits_ready.all(); }

    /** Are all the hits ready in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const UTStationID sta,
                                const UTLayerID lay,
                                const UTRegionID region) const
    { return m_hits_ready.test(sta,lay,region); }

    /** Are all the hits ready in the given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const UTStationID sta,
                                const UTLayerID lay) const
    { return m_hits_ready.all(sta,lay); }

    /** Are all the hits ready in the given station
     *  @param[in] sta    Station ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const UTStationID sta) const
    { return m_hits_ready.all(sta); }

  private:


    /// The underlying UT hit creator
    ToolHandle<Tf::IUTHitCreator> m_uthitcreator ;

    mutable Hits m_hits;

    // Flags to indicate which hits are ready
    mutable IndexedBitArray< Tf::RegionID::UTIndex::kNStations, 
                             Tf::RegionID::UTIndex::kNLayers, 
                             Tf::RegionID::UTIndex::kNRegions 
                           >   m_hits_ready; ///< Flags to indicate which regions have hits ready

  };

  template<class Hit>
  UTStationHitManager<Hit>::UTStationHitManager( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
    GaudiTool (type, name, parent),
    m_uthitcreator("Tf::STHitCreator<Tf::UT>/UTHitCreator")
  {
    declareInterface<UTStationHitManager<Hit> >(this);
  }

  template<class Hit>
  StatusCode UTStationHitManager<Hit>::initialize ( )
  {
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    sc = m_uthitcreator.retrieve();
    if ( sc.isFailure() ) return sc;

    // set up to be told about each new event
    incSvc()->addListener(this, IncidentType::BeginEvent);

    // make sure we are ready for first event
    this->clearHits();

    return sc;
  }

  template<class Hit>
  StatusCode UTStationHitManager<Hit>::finalize ( )
  {
    this->clearHits();
    return GaudiTool::finalize();
  }

  template<class Hit>
  void UTStationHitManager<Hit>::clearHits() const
  {
    HitRange rng = m_hits.range();
    std::for_each( rng.begin(), rng.end(), std::default_delete<Hit>() ) ; 
    m_hits.clear();
    m_hits_ready.clear();
  }

  template<class Hit>
  void UTStationHitManager<Hit>::prepareHits() const
  {
    if ( !this->allHitsPrepared() )
    {
      for (UTStationID sta=0; sta<maxStations(); ++sta )
      {
        this->prepareHits(sta);
      }
    }
  }

  template<class Hit>
  void UTStationHitManager<Hit>::prepareHits(const UTStationID sta) const
  {
    if ( !this->allHitsPrepared(sta) )
    {
      for (UTLayerID lay=0; lay<maxLayers(); ++lay )
      {
        this->prepareHits(sta,lay);
      }
    }
  }

  template<class Hit>
  void UTStationHitManager<Hit>::prepareHits(const UTStationID sta,
                                             const UTLayerID lay) const
  {
    if ( !this->allHitsPrepared(sta,lay) )
    {
      for (UTRegionID reg=0; reg<maxRegions(); ++reg )
      {
        this->prepareHits(sta,lay,reg);
      }
    }
  }

  template<class Hit>
  void UTStationHitManager<Hit>::prepareHits(const UTStationID sta,
                                             const UTLayerID lay,
                                             const UTRegionID region) const
  {
    if ( !this->allHitsPrepared(sta,lay,region) )
    {
      Tf::STHitRange uthits = utHitCreator()->hits(sta,lay,region) ;
      auto fun = [](Tf::STHitRange::const_reference hit) { return new Hit(*hit); };
      auto b =  boost::make_transform_iterator(std::begin(uthits), std::cref(fun));
      auto e =  boost::make_transform_iterator(std::end(uthits), std::cref(fun));
      m_hits.insert( sta,lay,region, b, e);
      setAllHitsPrepared(sta,lay,region);
    }
  }

  template < class Hit         >
  template < typename SELECTOR >
  void UTStationHitManager<Hit>::prepareHitsInWindow( const SELECTOR & selector )
  {
    this->clearHits();
    for (UTStationID sta=0; sta<maxStations(); ++sta )
    {
      for (UTLayerID lay=0; lay<maxLayers(); ++lay )
      {
        for (UTRegionID reg=0; reg<maxRegions(); ++reg )
        {

          // The z value for this region
          const double z = this->utHitCreator()->region(sta,lay,reg)->z();
          // get the search window
          XYSearchWindow win = selector.searchWindow(z);
          // get the hits in the search window
          Tf::STHitRange uthits = this->utHitCreator()->hits(sta,lay,reg,
                                                             win.minX(),win.maxX(),
                                                             win.minY(),win.maxY() );
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << uthits.size() << " UTHits for station=" << sta
                    << " layer=" << lay << " region=" << reg << endmsg;
          }
          auto fun = [](Tf::STHitRange::const_reference hit) { return new Hit(*hit); };
          auto b =  boost::make_transform_iterator(std::begin(uthits), std::cref(fun));
          auto e =  boost::make_transform_iterator(std::end(uthits), std::cref(fun));
          m_hits.insert( sta, lay, reg, b, e);
          this->setAllHitsPrepared(sta,lay,reg);
        }
      }// layer
    } // station
  }

} // Tf namespace

#endif // TFKERNEL_UTSTATIONHITMANAGER_H

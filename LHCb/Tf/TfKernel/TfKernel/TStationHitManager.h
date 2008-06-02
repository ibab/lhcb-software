
//-----------------------------------------------------------------------------
/** @file TStationHitManager.h
 *
 *  Header file for class : Tf::TStationHitManager
 *
 *  $Id: TStationHitManager.h,v 1.24 2008-06-02 13:49:53 smenzeme Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFTOOLS_TSTATIONHITMANAGER_H
#define TFTOOLS_TSTATIONHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Tf framework
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
#include "TfKernel/IOTHitCleaner.h"
#include "TfKernel/ISTHitCleaner.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"
#include "TfKernel/TfIDTypes.h"
#include "TfKernel/RegionSelectors.h"


/// Static interface ID
static const InterfaceID IID_TStationHitManager ( "TStationHitManager", 1, 0 );

namespace Tf
{

  class IStationSelector {
  public:
    virtual XYSearchWindow searchWindow(double z) const = 0;
  };

  /** @class TStationHitManager TStationHitManager.h TfKernel/TStationHitManager.h 
   *
   *  T station hit manager. Used to manage extended hit objects for the T
   *  Stations (OT and IT). 
   *
   *  Methods are provided to return the hits in a selected part of the detectors.
   *  E.g.
   *
   *  @code
   *  // Get all the hits in the T stations
   *  Tf::TStationHitManager::HitRange range = hitMan->hits();
   *
   *  // Get all the hits in one specific T station
   *  Tf::TStationID sta = ...;
   *  Tf::TStationHitManager::HitRange range = hitMan->hits(sta);
   *
   *  // Get all the hits in one specific layer of one T station
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::TStationHitManager::HitRange range = hitMan->hits(sta,lay);
   *
   *  // Get all the hits in a specific 'region' of one layer of one T station
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::TRegionID  reg = ...;
   *  Tf::TStationHitManager::HitRange range = hitMan->hits(sta,lay,reg);
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
   *  Tf::TStationHitManager::HitRange range = hitMan->hits();
   *  // Which gives the selected hits in all stations, layers and regions in the T stations
   *  @endcode
   *
   *  The only requirement on the selector object used is it must implement one specific method.
   *  See Tf::StateRegionSelector for an example of such an object.
   *
   *  In all cases the returned Range object acts like a standard vector or container :-
   *
   *  @code
   *   // Iterate over the returned range
   *  for ( Tf::TStationHitManager::HitRange::const_iterator iR = range.begin();
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
  class TStationHitManager : public GaudiTool,
                             public IIncidentListener
  {

  public:

    /// Type for container of Hits
    typedef std::vector< Hit* > Hits;

    /// range object for Hits
    typedef LoKi::Range_<Hits> HitRange;

  public:

    /// InterfaceID for this tool
    static const InterfaceID& interfaceID() { return IID_TStationHitManager; }

    /// Standard Constructor
    TStationHitManager( const std::string& type,
                        const std::string& name,
                        const IInterface* parent ) ;

    /// Destructor
    ~TStationHitManager( ) { }

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
    virtual void prepareHitsInWindow( const IStationSelector & selector );

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const TStationID sta,
                          const TLayerID   lay,
                          const TRegionID  region ) const
    {
      if ( !allHitsPrepared(sta,lay,region) ) { prepareHits(sta,lay,region); }
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
    inline HitRange hits( const TStationID sta,
                          const TLayerID   lay ) const
    {
      if ( !allHitsPrepared(sta,lay) ) { prepareHits(sta,lay); }
      return HitRange( m_hits_layers[sta][lay].begin(),
                       m_hits_layers[sta][lay].end() );
    }

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const TStationID sta ) const
    {
      if ( !allHitsPrepared(sta) ) { prepareHits(sta); }
      return HitRange( m_hits_stations[sta].begin(),
                       m_hits_stations[sta].end() );
    }

    /** Load the all hits
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( ) const
    {
      if ( !allHitsPrepared() ) { prepareHits(); }
      return HitRange( m_hits_all.begin(),
                       m_hits_all.end() );
    }

    // Not clear to me if these following methods  should be in the common interface
    // Are they pat specific, so should move to the pat implemation ?

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
                                  const TStationID sta,
                                  const TLayerID   lay,
                                  const TRegionID  region ) const
    {
      if ( !allHitsPrepared(sta,lay,region) ) { prepareHits(sta,lay,region); }
      return HitRange( std::lower_bound( m_hits[sta][lay][region].begin(),
                                         m_hits[sta][lay][region].end(),
                                         xMin,
                                         Tf::compByX_LB< Hit >() ),
                       m_hits[sta][lay][region].end() );
    }

    /** Retrieve the Region for a certain IT or OT region ID. The region
     *   knows its 'size' and gives access to its hits.
     *
     *  Note that the method returns a pointer to the base class Tf::EnvelopeBase
     *  This object knows most basic questions, but for more complex tasks the user
     *  can dynamic cast it to the actual region object for OT or IT
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return Pointer to the Tf::EnvelopeBase object
     */
    inline const Tf::EnvelopeBase* region( const TStationID iStation,
                                           const TLayerID   iLayer,
                                           const TRegionID  iRegion ) const
    {
      return ( iRegion > maxOTRegions()-1 ?
               static_cast<const Tf::EnvelopeBase*>(this->itHitCreator()->region(iStation,
                                                                                 iLayer,
                                                                                 iRegion.itRegionID())) :
               static_cast<const Tf::EnvelopeBase*>(this->otHitCreator()->region(iStation,
                                                                                 iLayer,
                                                                                 iRegion.otRegionID())) );
    }

  public:

    /** Sort the hits in a given station, layer and region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    template < typename SORTER >
    inline void sortHits( const TStationID sta,
                          const TLayerID   lay,
                          const TRegionID  region ) const
    {
      std::sort ( m_hits[sta][lay][region].begin(), m_hits[sta][lay][region].end(), SORTER() );
    }

    /** Sort the hits in a given station and layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    template < typename SORTER >
    inline void sortHits( const TStationID sta,
                          const TLayerID   lay ) const
    {
      std::sort ( m_hits_layers[sta][lay].begin(), m_hits_layers[sta][lay].end(), SORTER() );
    }

    /** Sort the hits in a given station
     *  @param[in] sta    Station ID
     */
    template < typename SORTER >
    inline void sortHits( const TStationID sta ) const
    {
      std::sort ( m_hits_stations[sta].begin(), m_hits_stations[sta].end(), SORTER() );
    }

    /** Sort the all the hits
     */
    template < typename SORTER >
    inline void sortHits( ) const
    {
      std::sort ( m_hits_all.begin(), m_hits_all.end(), SORTER() );
    }

    
    /// Clear the hit containers for a new event or for running on 
    /// full event after decoding on demand
    void clearHits () const;
  
  protected:


    /** Set the hits ready flag for all hits
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const bool ok ) const { m_hits_all_ready = ok; }

    /** Set the hits ready flag for given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const TStationID sta,
                                    const TLayerID   lay,
                                    const TRegionID  region,
                                    const bool ok ) const { m_hits_ready[sta][lay][region] = ok; }

    /** Set the hits ready flag for given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const TStationID sta,
                                    const TLayerID   lay,
                                    const bool ok ) const { m_hits_layers_ready[sta][lay] = ok; }

    /** Set the hits ready flag for given station
     *  @param[in] sta    Station ID
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const TStationID sta,
                                    const bool ok ) const { m_hits_stations_ready[sta] = ok; }

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
    inline bool allHitsPrepared(const TStationID sta,
                                const TLayerID   lay,
                                const TRegionID  region) const
    { return m_hits_ready[sta][lay][region]; }

    /** Are all the hits ready in the given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const TStationID sta,
                                const TLayerID   lay) const
    { return m_hits_layers_ready[sta][lay]; }

    /** Are all the hits ready in the given station
     *  @param[in] sta    Station ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const TStationID sta) const
    { return m_hits_stations_ready[sta]; }

  protected:

    /// The underlying OT hit creator
    inline const Tf::IOTHitCreator * otHitCreator() const { return &*m_othitcreator; }

    /// The underlying IT hit creator
    inline const Tf::IITHitCreator * itHitCreator() const { return &*m_ithitcreator; }

    /** Add a hit to the container
     *  @param hit Pointer to the hit to add
     *  @param[in] sta    The station number
     *  @param[in] lay    The layer number
     *  @param[in] region The region number
     */
    inline void addHit( Hit * hit,
                        const TStationID sta,
                        const TLayerID   lay,
                        const TRegionID  region ) const
    {
      m_hits[sta][lay][region].push_back(hit);
      // temporary hack, to get things working. Needs to be done better
      m_hits_layers[sta][lay].push_back(hit);
      m_hits_stations[sta].push_back(hit);
      m_hits_all.push_back(hit);
    }

    /// Access the maximum number of stations
    inline TStationID maxStations()   const { return TStationID(m_nSta);   }
    /// Access the maximum number of layers
    inline TLayerID maxLayers()       const { return TLayerID(m_nLay);     }
    /// Access the maximum number of OT regions
    inline OTRegionID maxOTRegions()  const { return OTRegionID(m_nOTReg); }
    /// Access the maximum number of IT regions
    inline ITRegionID maxITRegions()  const { return ITRegionID(m_nITReg); }
    /// Access the maximum number of regions
    inline TRegionID maxRegions()     const { return TRegionID(m_nReg);    }

    /// Initialise the IT hits for the current event using the given selector object
    void prepareITHitsInWindow( const IStationSelector & selector );

    /// Initialise the OT hits for the current event using the given selector object
    void prepareOTHitsInWindow( const IStationSelector & selector );

    /// Is OT hit cleaning activated
    inline bool cleanOTHits() const { return m_cleanOTHits; }

    /// Is IT hit cleaning activated
    inline bool cleanITHits() const { return m_cleanITHits; }

    /** Process an OT hit range (create extended hits)
     *  @param[in] othits The OTHit range to process
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    void processRange( const Tf::OTHitRange & othits,
                       const TStationID sta,
                       const TLayerID   lay,
                       const TRegionID  region ) const;

    /** Process an ST hit range (create extended hits)
     *  @param[in] sthits The STHit range to process
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    void processRange( const Tf::STHitRange & sthits,
                       const TStationID sta,
                       const TLayerID   lay,
                       const TRegionID  region ) const;

  public:

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    void prepareHits(const TStationID sta,
		     const TLayerID   lay,
		     const TRegionID  region) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    void prepareHits(const TStationID sta,
                     const TLayerID   lay) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     */
    void prepareHits(const TStationID sta) const;
    
    /** Initialise all the hits for the current event
     */
    virtual void prepareHits() const;

  protected:

    
    /** Method that controls what happens when an extended hit is made from an OTHit.
     *  virtual, to allow users to reimplement the method as they see fit
     *  @param[in] othit Reference to the underlying OTHit to make an extended hit from
     */
    virtual Hit * createHit( const Tf::OTHit & othit ) const;

    /** Method that controls what happens when an extended hit is made from an STHit.
     *  virtual, to allow users to reimplement the method as they see fit
     *  @param[in] sthit Reference to the underlying STHit to make an extended hit from
     */
    virtual Hit * createHit( const Tf::STHit & sthit ) const;

  private:

     /** Prepare all hits in the given IT region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */

    void prepareITHits(const TStationID sta,
                       const TLayerID   lay,
                       const ITRegionID region) const;

    /** Prepare all hits in the given OT region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     */
    void prepareOTHits(const TStationID sta,
                       const TLayerID   lay,
                       const OTRegionID region) const;

    /// max number of stations
    static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
    /// max number of layers
    static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
    /// max number of regions in OT
    static const unsigned int m_nOTReg = Tf::RegionID::OTIndex::kNRegions;
    /// max number of regions in IT
    static const unsigned int m_nITReg = Tf::RegionID::ITIndex::kNRegions;
    /// max number of regions
    static const unsigned int m_nReg = ( Tf::RegionID::OTIndex::kNRegions +
                                         Tf::RegionID::ITIndex::kNRegions );

    /// The underlying OT hit creator
    ToolHandle<Tf::IOTHitCreator> m_othitcreator ;

    /// The underlying IT hit creator
    ToolHandle<Tf::IITHitCreator> m_ithitcreator ;

    /// The OT hit cleaner
    Tf::IOTHitCleaner * m_otCleaner;

    /// The ST hit cleaner
    Tf::ISTHitCleaner * m_itCleaner;

    mutable Hits m_hits[m_nSta][m_nLay][m_nReg]; ///< Hits in individual regions
    // CRJ : This is ugly but just a first bash to provide *something* that works
    //       Need to try and find a neater way to handle these different pointer containers
    mutable Hits m_hits_layers[m_nSta][m_nLay];  ///< Hits in individual layers
    mutable Hits m_hits_stations[m_nSta];        ///< Hits in individual stations
    mutable Hits m_hits_all;                     ///< All hits

    // Flags to indicate which hits are ready
    mutable bool m_hits_ready[m_nSta][m_nLay][m_nReg]; ///< Flags to indicate which regions have hits ready
    mutable bool m_hits_layers_ready[m_nSta][m_nLay];  ///< Flag to indicate which layers are ready
    mutable bool m_hits_stations_ready[m_nSta];        ///< Flag to indicate which stations are ready
    mutable bool m_hits_all_ready;                     ///< Flag to indicate all hits are ready

    /// Should OT hit cleaning be performed ?
    bool m_cleanOTHits;

    /// Should IT hit cleaning be performed ?
    bool m_cleanITHits;

  };

  template<class Hit>
  TStationHitManager<Hit>::TStationHitManager( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
    GaudiTool (type, name, parent),
    m_othitcreator ( "Tf::OTHitCreator/OTHitCreator" ),
    m_ithitcreator ( "Tf::STHitCreator<Tf::IT>/ITHitCreator" ),
    m_otCleaner    ( NULL ),
    m_itCleaner    ( NULL )
  {
    declareInterface<TStationHitManager<Hit> >(this);
    declareProperty( "CleanOTHits", m_cleanOTHits = false );
    declareProperty( "CleanITHits", m_cleanITHits = false );
  }

  template<class Hit>
  StatusCode TStationHitManager<Hit>::initialize ( )
  {
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    // set up to be told about each new event
    incSvc()->addListener(this, IncidentType::BeginEvent);

    // load our hit creators
    sc = ( m_othitcreator.retrieve() &&
           m_ithitcreator.retrieve() );
    if ( sc.isFailure() ) return sc;

    // load private hit cleaners, if needed
    if ( cleanOTHits() ) m_otCleaner = this->tool<Tf::IOTHitCleaner>("Tf::OTHitCleaner","OTHitCleaner",this);
    if ( cleanITHits() ) m_itCleaner = this->tool<Tf::ISTHitCleaner>("Tf::STHitCleaner","ITHitCleaner",this);

    // make sure we are ready for first event
    this->clearHits();

    return sc;
  }

  template<class Hit>
  inline StatusCode TStationHitManager<Hit>::finalize ( )
  {
    this->clearHits();
    return GaudiTool::finalize();
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::clearHits() const
  {
    m_hits_all.clear();
    this->setAllHitsPrepared(false);
    for(TStationID s=0; s<maxStations(); ++s)
    {
      this->setAllHitsPrepared(s,false);
      m_hits_stations[s].clear();
      for (TLayerID l=0; l<maxLayers(); ++l)
      {
        this->setAllHitsPrepared(s,l,false);
        m_hits_layers[s][l].clear();
        for (TRegionID t=0; t<maxRegions(); ++t)
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
  Hit * TStationHitManager<Hit>::createHit( const Tf::OTHit & othit ) const
  {
    return new Hit(othit);
  }

  template<class Hit>
  Hit * TStationHitManager<Hit>::createHit( const Tf::STHit & sthit ) const
  {
    return new Hit(sthit);
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::processRange( const Tf::OTHitRange & othits,
                                                     const TStationID sta,
                                                     const TLayerID   lay,
                                                     const TRegionID  region ) const
  {
    if ( cleanOTHits() )
    {
      // clean hits
      Tf::OTHits selectedhits;
      m_otCleaner->cleanHits( othits, selectedhits );
      // convert only those selected
      for ( OTHits::const_iterator itOTH = selectedhits.begin();
            itOTH != selectedhits.end(); ++itOTH )
      {
        this -> addHit ( this->createHit(**itOTH), sta, lay, region );
      }
    }
    else
    {
      // no cleaning, so just convert everything
      for ( OTHitRange::const_iterator itOTH = othits.begin();
            itOTH < othits.end(); ++itOTH )
      {
        this -> addHit ( this->createHit(**itOTH), sta, lay, region );
      }
    }
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::processRange( const Tf::STHitRange & sthits,
                                                     const TStationID sta,
                                                     const TLayerID   lay,
                                                     const TRegionID  region ) const
  {
    if ( cleanITHits() )
    {
      // clean hits
      Tf::STHits selectedhits;
      m_itCleaner->cleanHits( sthits, selectedhits );
      // convert only those selected
      for ( STHits::const_iterator itSTH = selectedhits.begin();
            itSTH != selectedhits.end(); ++itSTH )
      {
        this -> addHit ( this->createHit(**itSTH), sta, lay, region );
      }
    }
    else
    {
      // no cleaning, so just convert everything
      for ( STHitRange::const_iterator itSTH = sthits.begin();
            itSTH < sthits.end(); ++itSTH )
      {
        this -> addHit ( this->createHit(**itSTH), sta, lay, region );
      }
    }
  }


  template<class Hit>
  inline void TStationHitManager<Hit>::prepareITHits(const TStationID sta,
                                                     const TLayerID   lay,
                                                     const ITRegionID region) const
  {
    Tf::STHitRange sthits = this->itHitCreator()->hits(sta,lay,region) ;
    this -> processRange ( sthits, sta, lay, TRegionID(region) );
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::prepareOTHits(const TStationID sta,
                                                     const TLayerID   lay,
                                                     const OTRegionID region) const
  {
    Tf::OTHitRange othits = this->otHitCreator()->hits(sta,lay,region) ;
    this -> processRange ( othits, sta, lay, TRegionID(region) );
  }

  template<class Hit>
  void TStationHitManager<Hit>::prepareHits() const
  {
    if ( !this->allHitsPrepared() )
    {
      for ( TStationID sta = 0; sta < this->maxStations(); ++sta )
      {
        this->prepareHits(sta);
      }
      this -> setAllHitsPrepared(true);
    }
  }

  template<class Hit>
  void TStationHitManager<Hit>::prepareHits(const TStationID sta) const
  {
    if ( !this->allHitsPrepared(sta) )
    {
      for ( TLayerID lay = 0; lay<this->maxLayers(); ++lay )
      {
        this->prepareHits(sta,lay);
      }
      this -> setAllHitsPrepared(sta,true);
    }
  }

  template<class Hit>
  void TStationHitManager<Hit>::prepareHits(const TStationID sta,
                                            const TLayerID   lay) const
  {
    if ( !this->allHitsPrepared(sta,lay) )
    {
      for ( TRegionID ir = 0; ir < maxRegions(); ++ir )
      {
        this->prepareHits(sta,lay,ir);
      }
      this -> setAllHitsPrepared(sta,lay,true);
    }
  }

  template<class Hit>
  void TStationHitManager<Hit>::prepareHits(const TStationID sta,
                                            const TLayerID lay,
                                            const TRegionID region) const
  {
    if ( !this->allHitsPrepared(sta,lay,region) )
    {
      if ( region >= this->maxOTRegions() )
      {
        this->prepareITHits(sta,lay,region.itRegionID());
      }
      else
      {
        this->prepareOTHits(sta,lay,region.otRegionID());
      }
      this->setAllHitsPrepared(sta,lay,region,true);
    }
  }

  //--------------------------------------------------------------------------------------------

  template < class Hit         >
  void TStationHitManager<Hit>::prepareHitsInWindow( const IStationSelector & selector )
  {
    this->clearHits();             // Clear any previous hits
    this->prepareOTHitsInWindow(selector); // select the OT hits
    this->prepareITHitsInWindow(selector); // select the IT hits
    // Signifiy all hits for this event are ready - I.e. no decoding on demand
    this->setAllHitsPrepared(true);
  }

  template < class Hit         >
  void TStationHitManager<Hit>::prepareITHitsInWindow( const IStationSelector & selector )
  {
    for (TStationID sta=0; sta < this->maxStations(); ++sta)
    {
      this->setAllHitsPrepared(sta,true);
      for (TLayerID lay=0; lay < this->maxLayers(); ++lay)
      {
        this->setAllHitsPrepared(sta,lay,true);
        for (ITRegionID it=0; it < this->maxITRegions(); ++it)
        {
          this->setAllHitsPrepared(sta,lay,it,true);

          // The z value for this region
          const double z = this->itHitCreator()->region(sta,lay,it)->z();
          // get the search window
          XYSearchWindow win = selector.searchWindow(z);
          // get the hits in the search window
          Tf::STHitRange sthits = this->itHitCreator()->hits(sta,lay,it,
                                                             win.minX(),win.maxX(),
                                                             win.minY(),win.maxY() );
	  processRange ( sthits, sta, lay, it + this->maxOTRegions() );
        }
      }// layer
    } // station
  }

  template < class Hit         >
  void TStationHitManager<Hit>::prepareOTHitsInWindow( const IStationSelector & selector )
  {
    for (TStationID sta=0; sta<this->maxStations(); ++sta)
    {
      this->setAllHitsPrepared(sta,true);
      for (TLayerID lay=0; lay<this->maxLayers(); ++lay)
      {
        this->setAllHitsPrepared(sta,lay,true);
        for (OTRegionID ot=0; ot<this->maxOTRegions(); ++ot)
        {
          this->setAllHitsPrepared(sta,lay,ot,true);
          
          // The z value for this region
          const double z = this->otHitCreator()->region(sta,lay,ot)->z();
          // get the search window
          XYSearchWindow win = selector.searchWindow(z);
          // get the hits in the search window
          Tf::OTHitRange othits = this->otHitCreator()->hits(sta,lay,ot,
                                                             win.minX(),win.maxX(),
                                                             win.minY(),win.maxY() );
	  processRange ( othits, sta, lay, ot );
        }
      }// layer
    } // station
  }

} // end Tf namespace

#endif // TFTOOLS_TSTATIONHITMANAGER_H

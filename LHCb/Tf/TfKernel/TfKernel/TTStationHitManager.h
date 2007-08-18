
//-----------------------------------------------------------------------------
/** @file TTStationHitManager.h
 *
 *  Header file for class : Tf::TTStationHitManager
 *
 *  $Id: TTStationHitManager.h,v 1.4 2007-08-18 15:02:05 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_TTSTATIONHITMANAGER_H
#define TFKERNEL_TTSTATIONHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/ISTHitCreator.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// boost
//#include <boost/lambda/lambda.hpp>
//#include <boost/lambda/bind.hpp>

/// Static interface ID
static const InterfaceID IID_TTStationHitManager ( "TTStationHitManager", 1, 0 );

namespace Tf
{

  /** @class TTStationHitManager
   *  Base class for all TT-station extended hit data managers
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
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

    /** Load the hits for a given region of interest
     *
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] region Region within the layer
     *
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( const unsigned int sta,
                          const unsigned int lay,
                          const unsigned int region ) const
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
    inline HitRange hits( const unsigned int sta,
                          const unsigned int lay ) const
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
    inline HitRange hits( const unsigned int sta ) const
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
                                  const unsigned int sta,
                                  const unsigned int lay,
                                  const unsigned int region ) const
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
    inline const ISTHitCreator::STRegion* region(const unsigned int iStation,
                                                 const unsigned int iLayer,
                                                 const unsigned int iRegion) const
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
    inline void sortHits( const unsigned int sta,
                          const unsigned int lay,
                          const unsigned int region )
    {
      std::sort ( m_hits[sta][lay][region].begin(), m_hits[sta][lay][region].end(), SORTER() );
    }

    /** Sort the hits in a given station and layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    template < typename SORTER >
    inline void sortHits( const unsigned int sta,
                          const unsigned int lay )
    {
      std::sort ( m_hits_layers[sta][lay].begin(), m_hits_layers[sta][lay].end(), SORTER() );
    }

    /** Sort the hits in a given station
     *  @param[in] sta    Station ID
     */
    template < typename SORTER >
    inline void sortHits( const unsigned int sta )
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
    inline unsigned int maxStations()  const { return m_nSta; }

    /// Maximum number of layers
    inline unsigned int maxLayers()    const { return m_nLay; }

    /// Maximum number of regions
    inline unsigned int maxRegions()   const { return m_nReg; }

  protected:

    /// Direct access to the underlying hit creator
    inline const Tf::ISTHitCreator * ttHitCreator() const { return &*m_tthitcreator; }

    /** Add a hit to the container
     *  @param hit Pointer to the hit to add
     *  @param[in] sta    The station number
     *  @param[in] lay    The layer number
     *  @param[in] region The region number
     */
    inline void addHit( Hit * hit,
                        const unsigned int sta,
                        const unsigned int lay,
                        const unsigned int region ) const
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
    void prepareHits(const unsigned int sta,
                     const unsigned int lay,
                     const unsigned int region) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     */
    void prepareHits(const unsigned int sta,
                     const unsigned int lay) const;

    /** Initialise all the hits for the current event in the given region
     *  @param[in] sta    Station ID
     */
    void prepareHits(const unsigned int sta) const;

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
    inline void setAllHitsPrepared( const unsigned int sta,
                                    const unsigned int lay,
                                    const unsigned int region,
                                    const bool ok ) const { m_hits_ready[sta][lay][region] = ok; }

    /** Set the hits ready flag for given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const unsigned int sta,
                                    const unsigned int lay,
                                    const bool ok ) const { m_hits_layers_ready[sta][lay] = ok; }

    /** Set the hits ready flag for given station
     *  @param[in] sta    Station ID
     *  @param[in] ok     The status flag (true means hits ready, false means not ready)
     */
    inline void setAllHitsPrepared( const unsigned int sta,
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
    inline bool allHitsPrepared(const unsigned int sta,
                                const unsigned int lay,
                                const unsigned int region) const
    { return m_hits_ready[sta][lay][region]; }

    /** Are all the hits ready in the given layer
     *  @param[in] sta    Station ID
     *  @param[in] lay    Station layer ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const unsigned int sta,
                                const unsigned int lay) const
    { return m_hits_layers_ready[sta][lay]; }

    /** Are all the hits ready in the given station
     *  @param[in] sta    Station ID
     *  @return boolean indicating if all the hits in the given region are ready or not
     *  @retval TRUE  Hits are ready
     *  @retval FALSE Hits are not ready
     */
    inline bool allHitsPrepared(const unsigned int sta) const
    { return m_hits_stations_ready[sta]; }

  private:

    /// max number of stations
    static const unsigned int m_nSta = Tf::RegionID::TTIndex::kNStations;
    /// max number of layers
    static const unsigned int m_nLay = Tf::RegionID::TTIndex::kNLayers;
    /// max number of regions
    static const unsigned int m_nReg = Tf::RegionID::TTIndex::kNRegions;

    /// The underlying TT hit creator
    ToolHandle<Tf::ISTHitCreator> m_tthitcreator ;

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
    m_tthitcreator("Tf::STHitCreator/TTHitCreator")
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
    for(unsigned int s=0; s<maxStations(); s++)
    {
      this->setAllHitsPrepared(s,false);
      m_hits_stations[s].clear();
      for (unsigned int l=0; l<maxLayers(); l++)
      {
        this->setAllHitsPrepared(s,l,false);
        m_hits_layers[s][l].clear();
        for (unsigned int t=0; t<maxRegions(); t++)
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
      for (unsigned int sta=0; sta<maxStations(); ++sta )
      {
        this->prepareHits(sta);
      }
      setAllHitsPrepared(true);
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits(const unsigned int sta) const
  {
    if ( !this->allHitsPrepared(sta) )
    {
      for (unsigned int lay=0; lay<maxLayers(); ++lay )
      {
        this->prepareHits(sta,lay);
      }
      setAllHitsPrepared(sta,true);
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits(const unsigned int sta,
                                             const unsigned int lay) const
  {
    if ( !this->allHitsPrepared(sta,lay) )
    {
      for (unsigned int reg=0; reg<maxRegions(); ++reg )
      {
        this->prepareHits(sta,lay,reg);
      }
      setAllHitsPrepared(sta,lay,true);
    }
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits(const unsigned int sta,
                                             const unsigned int lay,
                                             const unsigned int region) const
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

} // Tf namespace

#endif // TFKERNEL_TTSTATIONHITMANAGER_H

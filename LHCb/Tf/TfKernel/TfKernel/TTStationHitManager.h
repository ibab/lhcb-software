
//-----------------------------------------------------------------------------
/** @file TTStationHitManager.h
 *
 *  Header file for class : Tf::TTStationHitManager
 *
 *  $Id: TTStationHitManager.h,v 1.2 2007-08-16 12:54:00 jonrob Exp $
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
      if (!allHitsPrepared()) prepareHits();
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
      if (!allHitsPrepared()) prepareHits();
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
      if (!allHitsPrepared()) prepareHits();
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
      if (!allHitsPrepared()) prepareHits();
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

  protected:

    /** Initialise all the hits for the current event
     *  @attention No need for users to directly call this themselves.
     *             It is called on demand when needed 
     */
    virtual void prepareHits() const;

  protected:

    /// Clear the hit containers for a new event
    void clearHits () const;

    /// Set the hits ready flag
    inline void setAllHitsPrepared( const bool ok ) const { m_allHitsPrepared = ok; }

    /// Are all the hits ready
    inline bool allHitsPrepared() const { return m_allHitsPrepared; }

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

    /// Flag to indicate if all hits are ready
    mutable bool m_allHitsPrepared;

  };

  template<class Hit>
  TTStationHitManager<Hit>::TTStationHitManager( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
    GaudiTool (type, name, parent),
    m_tthitcreator("Tf::STHitCreator/TTHitCreator"),
    m_allHitsPrepared(false)
  {
    declareInterface<TTStationHitManager<Hit> >(this);
  }

  template<class Hit>
  StatusCode TTStationHitManager<Hit>::initialize ( )
  {
    const StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    m_tthitcreator.retrieve().ignore();

    incSvc()->addListener(this, IncidentType::EndEvent);

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
    for(unsigned int s=0; s<maxStations(); s++)
    {
      m_hits_stations[s].clear();
      for (unsigned int l=0; l<maxLayers(); l++)
      {
        m_hits_layers[s][l].clear();
        for (unsigned int t=0; t<maxRegions(); t++)
        {
          for ( typename Hits::iterator iHit = m_hits[s][l][t].begin();
                iHit != m_hits[s][l][t].end(); ++iHit )
          {
            delete *iHit;
          }
          m_hits[s][l][t].clear();
        }
      }
    }
    setAllHitsPrepared(false);
  }

  template<class Hit>
  void TTStationHitManager<Hit>::prepareHits() const
  {
    clearHits();
    for (unsigned int sta=0; sta<maxStations(); sta++)
    {
      for (unsigned int lay=0; lay<maxLayers(); lay++)
      {
        for (unsigned int reg=0; reg<maxRegions(); reg++)
        {
          Tf::STHitRange tthits = ttHitCreator()->hits(sta,lay,reg) ;
          for (Tf::STHitRange::const_iterator itTTH = tthits.begin();
               itTTH < tthits.end(); itTTH++)
          {
            this -> addHit(new Hit(**itTTH),sta,lay,reg);
          }
        }
      }
    }
    setAllHitsPrepared(true);
  }

} // Tf namespace

#endif // TFKERNEL_TTSTATIONHITMANAGER_H

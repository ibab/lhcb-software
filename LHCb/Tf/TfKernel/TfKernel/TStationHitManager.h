
//-----------------------------------------------------------------------------
/** @file TStationHitManager.h
 *
 *  Header file for class : Tf::TStationHitManager
 *
 *  $Id: TStationHitManager.h,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
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

// Event model
#include "Event/State.h"

// Tf framework
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/ISTHitCreator.h"
#include "TfKernel/IOTHitCleaner.h"
#include "TfKernel/ISTHitCleaner.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"

// From STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTLayer.h"

// From OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Boost
//#include <boost/lambda/lambda.hpp>
//#include <boost/lambda/bind.hpp>
//#include <vector>
//#include <algorithm>
//using namespace std;
//using namespace boost::lambda;

/// Static interface ID
static const InterfaceID IID_TStationHitManager ( "TStationHitManager", 1, 0 );

namespace Tf
{

  /** @class TStationHitManager TStationHitManager.h
   *  T station hit manager
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
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
      if ( IncidentType::EndEvent == incident.type() ) clearHits() ;
    }

  public:

    /// Initialise the hits for the current event
    virtual void prepareHits();

    /// Initialise the hits for the current event
    virtual void prepareHits(const LHCb::State & aState, const double nSigma);

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
      return HitRange( m_hits_stations[sta].begin(),
                       m_hits_stations[sta].end() );
    }

    /** Load the all hits
     *  @return Range object for the hits in the selected region of interest
     */
    inline HitRange hits( ) const
    {
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
                                  const unsigned int sta,
                                  const unsigned int lay,
                                  const unsigned int region ) const
    {
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
    inline const Tf::EnvelopeBase* region( const unsigned int iStation,
                                           const unsigned int iLayer,
                                           const unsigned int iRegion ) const
    {
      return ( iRegion > maxOTRegions()-1 ?
               static_cast<const Tf::EnvelopeBase*>(this->itHitCreator()->region(iStation,
                                                                                 iLayer,
                                                                                 iRegion-maxOTRegions())) :
               static_cast<const Tf::EnvelopeBase*>(this->otHitCreator()->region(iStation,
                                                                                 iLayer,
                                                                                 iRegion)) );
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

  protected:

    /// Clear the hit containers for a new event
    void clearHits ();

    /// Set the hits ready flag
    inline void setAllHitsPrepared( const bool ok ) { m_allHitsPrepared = ok; }
    /// Are all the hits ready
    inline bool allHitsPrepared() const { return m_allHitsPrepared; }

    /// The underlying OT hit creator
    inline const Tf::IOTHitCreator * otHitCreator() const { return &*m_othitcreator; }

    /// The underlying IT hit creator
    inline const Tf::ISTHitCreator * itHitCreator() const { return &*m_ithitcreator; }

    /** Add a hit to the container
     *  @param hit Pointer to the hit to add
     *  @param[in] sta    The station number
     *  @param[in] lay    The layer number
     *  @param[in] region The region number
     */
    inline void addHit( Hit * hit,
                        const unsigned int sta,
                        const unsigned int lay,
                        const unsigned int region )
    {
      m_hits[sta][lay][region].push_back(hit);
      // temporary hack, to get things working. Needs to be done better
      m_hits_layers[sta][lay].push_back(hit);
      m_hits_stations[sta].push_back(hit);
      m_hits_all.push_back(hit);
    }

    inline unsigned int maxStations()  const { return m_nSta;    }
    inline unsigned int maxLayers()    const { return m_nLay;    }
    inline unsigned int maxOTRegions() const { return m_nOTReg;  }
    inline unsigned int maxITRegions() const { return m_nITReg;  }
    inline unsigned int maxRegions()   const { return m_nReg;    }

    /// Prepare all hits in IT
    void prepareITHits();

    /// Prepare all hits in OT
    void prepareOTHits();

    /// Prepare all hits in IT around the given state within the given tolerance
    void prepareITHits(const LHCb::State & aState, const double nSigma);

    /// Prepare all hits in OT around the given state within the given tolerance
    void prepareOTHits(const LHCb::State & aState, const double nSigma);

    /// Is OT hit cleaning activated
    inline bool cleanOTHits() const { return m_cleanOTHits; }

    /// Is IT hit cleaning activated
    inline bool cleanITHits() const { return m_cleanITHits; }

    /// Process an OT hit range
    void processRange( const Tf::OTHitRange & othits,
                       const unsigned int sta,
                       const unsigned int lay,
                       const unsigned int region );

    /// Process an ST hit range
    void processRange( const Tf::STHitRange & sthits,
                       const unsigned int sta,
                       const unsigned int lay,
                       const unsigned int region );

  private:

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
    ToolHandle<Tf::ISTHitCreator> m_ithitcreator ;

    /// The OT hit cleaner
    Tf::IOTHitCleaner * m_otCleaner;

    /// The ST hit cleaner
    Tf::ISTHitCleaner * m_itCleaner;

    Hits m_hits[m_nSta][m_nLay][m_nReg]; ///< Hits in individual regions
    // CRJ : This is ugly but just a first bash to provide *something* that works
    //       Need to try and find a neater way to handle these different pointer containers
    Hits m_hits_layers[m_nSta][m_nLay]; ///< Hits in individual layers
    Hits m_hits_stations[m_nSta];       ///< Hits in individual stations
    Hits m_hits_all;                    ///< All hits

    /// Flag to indicate if all hits are ready
    bool m_allHitsPrepared;

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
    m_ithitcreator ( "Tf::STHitCreator/ITHitCreator" ),
    m_otCleaner    ( NULL ),
    m_itCleaner    ( NULL ),
    m_allHitsPrepared(false)
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

    incSvc()->addListener(this, IncidentType::EndEvent);

    // load our hit creators
    sc = ( m_othitcreator.retrieve() &&
           m_ithitcreator.retrieve() );

    // load private hit cleaners, if needed
    if ( cleanOTHits() ) m_otCleaner = this->tool<Tf::IOTHitCleaner>("Tf::OTHitCleaner","OTHitCleaner",this);
    if ( cleanITHits() ) m_itCleaner = this->tool<Tf::ISTHitCleaner>("Tf::STHitCleaner","ITHitCleaner",this);

    return sc;
  }

  template<class Hit>
  inline StatusCode TStationHitManager<Hit>::finalize ( )
  {
    this->clearHits();
    return GaudiTool::finalize();
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::clearHits()
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
  inline void TStationHitManager<Hit>::processRange( const Tf::OTHitRange & othits,
                                                     const unsigned int sta,
                                                     const unsigned int lay,
                                                     const unsigned int region )
  {
    if ( cleanOTHits() )
    {
      // clean hits
      Tf::OTHits selectedhits;
      m_otCleaner->cleanHits( othits, selectedhits );
      // convert only those selected
      // CRJ : Can't seem to get this boost stuff to work...
      //std::for_each( selectedhits.begin(), selectedhits.end(),
      //               bind( **_1, sta, lay, region )(this->addHit) );
      for ( OTHits::const_iterator iH = selectedhits.begin(); iH != selectedhits.end(); ++iH )
      {
        this -> addHit ( new Hit(**iH), sta, lay, region );
      }
    }
    else
    {
      // no cleaning, so just convert everything
      for ( Tf::OTHitRange::const_iterator itOTH = othits.begin();
            itOTH < othits.end(); ++itOTH )
      {
        this -> addHit ( new Hit(**itOTH), sta, lay, region );
      }
    }
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::processRange( const Tf::STHitRange & sthits,
                                                     const unsigned int sta,
                                                     const unsigned int lay,
                                                     const unsigned int region )
  {
    if ( cleanITHits() )
    {
      // clean hits
      Tf::STHits selectedhits;
      m_itCleaner->cleanHits( sthits, selectedhits );
      // convert only those selected
      for ( STHits::const_iterator iH = selectedhits.begin(); iH != selectedhits.end(); ++iH )
      {
        this -> addHit ( new Hit(**iH), sta, lay, region );
      }
    }
    else
    {
      for ( Tf::STHitRange::const_iterator itSTH = sthits.begin();
            itSTH < sthits.end(); ++itSTH )
      {
        this -> addHit ( new Hit(**itSTH), sta, lay, region );
      }
    }
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::prepareITHits(const LHCb::State & aState, const double nSigma)
  {

    // WARNING : This is temporary solution. A better method that allows an arbitary hit window
    //           (using trajectories ??) should be implemented instead

    double m_y = aState.ty();
    double c_y = aState.y() - m_y*aState.z();

    double a_x = aState.qOverP()/42.;
    double b_x = aState.tx() -2*aState.z()*a_x;
    double c_x = aState.x() - aState.z()*(b_x + a_x*aState.z());

    double dy = nSigma*sqrt(aState.errY2());
    double dx = nSigma*sqrt(aState.errX2());

    double x, y, z;

    for (unsigned int sta=0; sta < this->maxStations(); ++sta)
    {
      for (unsigned int lay=0; lay < this->maxLayers(); ++lay)
      {
        // IT hits
        for (unsigned int it=0; it < this->maxITRegions(); ++it)
        {
          z = this->itHitCreator()->region(sta,lay,it)->z();
          y = z*((a_x*z)+b_x)+c_x;
          x = m_y*z+c_y;

          Tf::STHitRange sthits = this->itHitCreator()->hits(sta,lay,it,x-dx,x+dx,y-dy,y+dy) ;
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << sthits.size() << " ITHits for station=" << sta
                    << " layer=" << lay << " region=" << it << endreq;
          }
          processRange ( sthits, sta, lay, it + this->maxOTRegions() );
        }

      }// layer
    } // station
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::prepareOTHits(const LHCb::State & aState, const double nSigma)
  {
    double m_y = aState.ty();
    double c_y = aState.y() - m_y*aState.z();

    double a_x = aState.qOverP()/42.;
    double b_x = aState.tx() -2*aState.z()*a_x;
    double c_x = aState.x() - aState.z()*(b_x + a_x*aState.z());

    double dy = nSigma*sqrt(aState.errY2());
    double dx = nSigma*sqrt(aState.errX2());

    double x, y, z;

    for (unsigned int sta=0; sta<this->maxStations(); ++sta)
    {
      for (unsigned int lay=0; lay<this->maxLayers(); ++lay)
      {
        // OT hits
        for (unsigned int ot=0; ot<this->maxOTRegions(); ++ot)
        {
          z = this->itHitCreator()->region(sta,lay,ot)->z();
          y = z*((a_x*z)+b_x)+c_x;
          x = m_y*z+c_y;

          Tf::OTHitRange othits = this->otHitCreator()->hits(sta,lay,ot,x-dx,x+dx,y-dy,y+dy) ;
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << othits.size() << " OTHits for station=" << sta
                    << " layer=" << lay << " region=" << ot << endreq;
          }
          processRange ( othits, sta, lay, ot );
        }

      }// layer
    } // station
  }

  template<class Hit>
  inline void TStationHitManager<Hit>::prepareITHits()
  {

    for (unsigned int sta=0; sta < this->maxStations(); ++sta)
    {
      for (unsigned int lay=0; lay < this->maxLayers(); ++lay)
      {

        // IT hits
        for (unsigned int it=0; it < this->maxITRegions(); ++it)
        {
          Tf::STHitRange sthits = this->itHitCreator()->hits(sta,lay,it) ;
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << sthits.size() << " ITHits for station=" << sta
                    << " layer=" << lay << " region=" << it << endreq;
          }
          processRange ( sthits,  sta, lay, it + this->maxOTRegions() );
        }

      }// layer
    } // station

  }

  template<class Hit>
  inline void TStationHitManager<Hit>::prepareOTHits()
  {
    for (unsigned int sta=0; sta<this->maxStations(); ++sta)
    {
      for (unsigned int lay=0; lay<this->maxLayers(); ++lay)
      {
        // OT hits
        for (unsigned int ot=0; ot<this->maxOTRegions(); ++ot)
        {
          Tf::OTHitRange othits = this->otHitCreator()->hits(sta,lay,ot) ;
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << othits.size() << " OTHits for station=" << sta
                    << " layer=" << lay << " region=" << ot << endreq;
          }
          processRange ( othits, sta, lay, ot );
        }

      }// layer
    } // station
  }

  template<class Hit>
  void TStationHitManager<Hit>::prepareHits()
  {
    if ( !allHitsPrepared() )
    {
      this->clearHits();
      this->prepareOTHits();
      this->prepareITHits();
      setAllHitsPrepared(true);
    }
  }

  template<class Hit>
  void TStationHitManager<Hit>::prepareHits(const LHCb::State & aState, const double nSigma)
  {
    setAllHitsPrepared(false);
    this->clearHits();
    this->prepareOTHits(aState, nSigma);
    this->prepareITHits(aState, nSigma);
  }
}

#endif // TFTOOLS_TSTATIONHITMANAGER_H

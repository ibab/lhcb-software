
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorBase.cpp
 *
 *  Implementation file for tool base class : RichPixelCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorBase.cpp,v 1.18 2007-03-09 18:04:34 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/04/2005
 */
//---------------------------------------------------------------------------------

// local
#include "RichRecBase/RichPixelCreatorBase.h"

//-----------------------------------------------------------------------------

// RICH software namespace
namespace Rich
{
  namespace Rec
  {

    // Standard constructor, initializes variables
    PixelCreatorBase::PixelCreatorBase( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
      : RichRecToolBase ( type, name, parent ),
        m_allDone       ( false ),
        m_richSys       ( NULL  ),
        m_recGeom       ( NULL  ),
        m_hpdOcc        ( Rich::NRiches ),
        m_hpdClus       ( Rich::NRiches ),
        m_idTool        ( NULL  ),
        m_decoder       ( NULL  ),
        m_pixels        ( NULL  ),
        m_bookKeep      ( false ),
        m_hpdCheck      ( false ),
        m_clusterHits   ( Rich::NRiches, false ),
        m_usedDets      ( Rich::NRiches, true  ),
        m_richRecPixelLocation ( LHCb::RichRecPixelLocation::Default ),
        m_begins        ( boost::extents[Rich::NRiches][Rich::NHPDPanelsPerRICH] ),
        m_ends          ( boost::extents[Rich::NRiches][Rich::NHPDPanelsPerRICH] ),
        m_Nevts         ( 0 ),
        m_hasBeenCalled ( false ),
        m_applyPixelSuppression ( true  )
    {

      // Define the interface
      declareInterface<IPixelCreator>(this);

      if      ( context() == "Offline" )
      {
        m_richRecPixelLocation = LHCb::RichRecPixelLocation::Offline;
      }
      else if ( context() == "HLT" )
      {
        m_richRecPixelLocation = LHCb::RichRecPixelLocation::HLT;
      }

      // Define job option parameters
      declareProperty( "DoBookKeeping",       m_bookKeep  );
      declareProperty( "UseDetectors",        m_usedDets  );
      declareProperty( "CheckHPDsAreActive",  m_hpdCheck  );
      declareProperty( "ApplyPixelSuppression", m_applyPixelSuppression );
      declareProperty( "ApplyPixelClustering",  m_clusterHits );

      // Initialise
      m_hpdOcc[Rich::Rich1]  = NULL;
      m_hpdOcc[Rich::Rich2]  = NULL;
      m_hpdClus[Rich::Rich1] = NULL;
      m_hpdClus[Rich::Rich2] = NULL;

    }

    StatusCode PixelCreatorBase::initialize()
    {
      // base class initilize
      const StatusCode sc = RichRecToolBase::initialize();
      if ( sc.isFailure() ) { return sc; }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "RichRecPixel location : " << m_richRecPixelLocation << endreq;
      }

      // get tools
      acquireTool( "RichRecGeometry", m_recGeom );
      if ( m_hpdCheck )
      {
        m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );
        Warning( "Will check each pixel for HPD status. Takes additional CPU.",
                 StatusCode::SUCCESS );
      }

      // Check which detectors to use
      if ( !m_usedDets[Rich::Rich1] )
        Warning( "Pixels for RICH1 are disabled", StatusCode::SUCCESS );
      if ( !m_usedDets[Rich::Rich2] )
        Warning( "Pixels for RICH2 are disabled", StatusCode::SUCCESS );

      // Setup incident services
      incSvc()->addListener( this, IncidentType::BeginEvent );
      incSvc()->addListener( this, IncidentType::EndEvent   );

      // Intialise counts
      m_hitCount[Rich::Rich1]           = 0;
      m_hitCount[Rich::Rich2]           = 0;
      m_suppressedHitCount[Rich::Rich1] = 0;
      m_suppressedHitCount[Rich::Rich2] = 0;

      // load hit suppression tools (avoids loading during first event)
      //if ( m_applyPixelSuppression && m_usedDets[Rich::Rich1] ) { hpdSuppTool(Rich::Rich1); }
      //if ( m_applyPixelSuppression && m_usedDets[Rich::Rich2] ) { hpdSuppTool(Rich::Rich2); }

      return sc;
    }

    StatusCode PixelCreatorBase::finalize()
    {
      // print stats
      printStats();

      // base class finalize
      return RichRecToolBase::finalize();
    }

    void PixelCreatorBase::printStats() const
    {
      if ( m_Nevts > 0
           && !( m_hitCount[Rich::Rich1] == 0 &&
                 m_hitCount[Rich::Rich2] == 0 &&
                 m_suppressedHitCount[Rich::Rich1] == 0 &&
                 m_suppressedHitCount[Rich::Rich2] == 0
                 ) )
      {
        const StatDivFunctor occ("%8.2f +-%7.2f");
        info() << "======================================================================================" << endreq
               << "                       Pixel Creator Summary for " << m_Nevts << " events :-" << endreq
               << "  Selected   :  RICH1 = " << occ(m_hitCount[Rich::Rich1],m_Nevts)
               << "  RICH2 = " << occ(m_hitCount[Rich::Rich2],m_Nevts)
               << "  pixels/event" << endreq
               << "  Rejected   :  RICH1 = " << occ(m_suppressedHitCount[Rich::Rich1],m_Nevts)
               << "  RICH2 = " << occ(m_suppressedHitCount[Rich::Rich2],m_Nevts)
               << "  pixels/event" << endreq
               << "======================================================================================" << endreq;
      }
      else
      {
        Warning( "No pixels produced", StatusCode::SUCCESS );
      }
    }

    LHCb::RichRecPixel *
    PixelCreatorBase::buildPixel( const Rich::HPDPixelCluster& cluster ) const
    {

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << " -> Creating RichRecPixel from cluster " << cluster << endreq;
      }

      // the core cluster ID
      const LHCb::RichSmartID id = cluster.primaryID();

      // See if this RichRecPixel already exists
      LHCb::RichRecPixel * pixel = ( bookKeep() && m_pixelDone[id] ? m_pixelExists[id] : 0 );
      if ( pixel ) return pixel;

      // Check this hit is OK
      if ( pixelIsOK(id) )
      {

        // Make a new RichRecPixel
        const Gaudi::XYZPoint gPos = smartIDTool()->globalPosition( cluster );
        pixel = new LHCb::RichRecPixel( cluster,                         // SmartID cluster for pixel
                                        gPos,                            // position in global coords
                                        smartIDTool()->globalToPDPanel(gPos), // position in local coords
                                        Rich::Rec::PixelParent::RawBuffer,    // parent type
                                        NULL                             // pointer to parent (not available)
                                        );

        // compute corrected local coordinates
        computeRadCorrLocalPositions( pixel );

        // save to TES container in tool
        savePixel( pixel );

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Created pixel " << *pixel << endreq;
        }

      }

      // Add to reference map
      if ( bookKeep() )
      {
        m_pixelExists[ id ] = pixel;
        m_pixelDone  [ id ] = true;
      }

      return pixel;
    }

    StatusCode PixelCreatorBase::newPixels() const
    {
      if ( !m_allDone )
      {
        m_allDone = true; // only once per event

        // Obtain RichSmartIDs
        const Rich::DAQ::PDMap & smartIDs = smartIDdecoder()->allRichSmartIDs();

        // Reserve space
        richPixels()->reserve( smartIDs.size() );

        // Loop over HPDs and RichSmartIDs and create working pixels
        for ( Rich::DAQ::PDMap::const_iterator iHPD = smartIDs.begin();
              iHPD != smartIDs.end(); ++iHPD )
        {

          // apply HPD pixel suppression
          // NB : taking a copy of the smartIDs here since we might remove
          // some, and we cannot change the raw data from smartIDdecoder()
          LHCb::RichSmartID::Vector smartIDs = (*iHPD).second;
          applyPixelSuppression( (*iHPD).first, smartIDs );

          // if any left, proceed and make pixels
          if ( !smartIDs.empty() )
          {

            // which rich
            const Rich::DetectorType rich = (*iHPD).first.rich();

            // perform clustering on the remaining pixels
            HPDPixelClusters::ConstSharedPtn clusters = hpdClusTool(rich)->findClusters( smartIDs );

            // get the clusters
            if ( msgLevel(MSG::DEBUG) )
            {
              debug() << "From " << smartIDs.size() << " found "
                      << clusters->clusters().size() << " clusters" << endreq;
            }

            // loop over the clusters
            for ( HPDPixelClusters::Cluster::PtnVector::const_iterator iC = clusters->clusters().begin();
                  iC != clusters->clusters().end(); ++iC )
            {

              if ( m_clusterHits[(*iHPD).first.rich()] )
              {
                // make a single pixel for this cluster
                LHCb::RichRecPixel * pixel = buildPixel( (*iC)->pixels() );
                pixel->setAssociatedCluster( (*iC)->pixels() );
              }
              else
              {
                // make a smartID for each channel in the cluster
                for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iC)->pixels().smartIDs().begin();
                      iID != (*iC)->pixels().smartIDs().end(); ++iID )
                {
                  LHCb::RichRecPixel * pixel = buildPixel( *iID );
                  pixel->setAssociatedCluster( (*iC)->pixels() );
                }
              }

            } // loop over clusters

          } // any smartids left after clustering ?

        } // loop over HPDs

        // find iterators
        // note : we are relying on the sorting of the input RichSmartIDs here, so we don't
        // manually sort the RichRecPixels for speed unlike in other tool implementations
        fillIterators();

      }

      return StatusCode::SUCCESS;
    }

    void PixelCreatorBase::fillIterators() const
    {

      // Reset to defaults
      resetIterators();

      // if there are some pixels, compute iterators
      // Should eventually look into if this can be done
      // whilst filling, avoiding the need for the "after-the -fact" loop

      if ( !richPixels()->empty() )
      {

        LHCb::RichRecPixels::iterator iPix = richPixels()->begin();
        Rich::DetectorType rich      = (*iPix)->detector();
        Rich::Side        panel      = (*iPix)->panel().panel();
        LHCb::RichSmartID hpd        = (*iPix)->hpd().hpdID();
        Rich::DetectorType lastrich  = rich;
        Rich::Side        lastpanel  = panel;
        LHCb::RichSmartID lasthpd    = hpd;
        ++iPix; // skip first pixel

        // loop over remaining pixels
        for ( ; iPix != richPixels()->end(); ++iPix )
        {
          if ( panel != (*iPix)->panel().panel() )
          {
            panel = (*iPix)->panel().panel();
            m_begins[(*iPix)->detector()][panel] = iPix;
            m_ends[lastrich][lastpanel]          = iPix;
            lastpanel = panel;
          }
          if ( rich != (*iPix)->detector() )
          {
            rich                = (*iPix)->detector();
            m_richBegin[rich]   = iPix;
            m_richEnd[lastrich] = iPix;
            lastrich            = rich;
          }
          if ( hpd != (*iPix)->hpd().hpdID() )
          {
            hpd                 = (*iPix)->hpd().hpdID();
            m_hpdBegin[hpd]     = iPix;
            m_hpdEnd[lasthpd]   = iPix;
            lasthpd             = hpd;
          }
        }

        // Set final iterators
        m_richEnd[rich]     = iPix;
        m_ends[rich][panel] = iPix;
        m_hpdEnd[hpd]       = iPix;

      }

    }

    LHCb::RichRecPixels * PixelCreatorBase::richPixels() const
    {

      // If first time for this event, either create or load the RichRecPixels
      if ( !m_pixels )
      {

        if ( !exist<LHCb::RichRecPixels>(pixelLocation()) )
        {

          // Reinitialise the Pixel Container
          m_pixels = new LHCb::RichRecPixels();

          // Register new RichRecPhoton container to Gaudi data store
          put( m_pixels, pixelLocation() );

        }
        else
        {

          // get pixels from TES
          m_pixels = get<LHCb::RichRecPixels>( pixelLocation() );
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << m_pixels->size() << " pre-existing RichRecPixels in TES at "
                    << pixelLocation() << endreq;
          }

          if ( bookKeep() )
          {
            // Remake local pixel reference map
            for ( LHCb::RichRecPixels::const_iterator iPixel = m_pixels->begin();
                  iPixel != m_pixels->end(); ++iPixel )
            {
              // should look into making this book keeping map use the full vector of IDs ?
              m_pixelExists [ (*iPixel)->hpdPixelCluster().primaryID() ] = *iPixel;
              m_pixelDone   [ (*iPixel)->hpdPixelCluster().primaryID() ] = true;
            }
          }

          // find iterators
          fillIterators();

        }
      }

      return m_pixels;
    }

    LHCb::RichRecPixels::iterator
    PixelCreatorBase::begin( const Rich::DetectorType rich ) const
    {
      return m_richBegin[rich];
    }

    LHCb::RichRecPixels::iterator
    PixelCreatorBase::end( const Rich::DetectorType rich ) const
    {
      return m_richEnd[rich];
    }

    LHCb::RichRecPixels::iterator
    PixelCreatorBase::begin( const Rich::DetectorType rich,
                             const Rich::Side         panel ) const
    {
      return m_begins[rich][panel];
    }

    LHCb::RichRecPixels::iterator
    PixelCreatorBase::end( const Rich::DetectorType rich,
                           const Rich::Side         panel ) const
    {
      return m_ends[rich][panel];
    }

    LHCb::RichRecPixels::iterator
    PixelCreatorBase::begin( const LHCb::RichSmartID hpdID ) const
    {
      HPDItMap::iterator i = m_hpdBegin.find(hpdID);
      // If not found, default to first pixel
      return ( i == m_hpdBegin.end() ? richPixels()->begin() : (*i).second );
    }

    LHCb::RichRecPixels::iterator
    PixelCreatorBase::end( const LHCb::RichSmartID hpdID ) const
    {
      HPDItMap::iterator i = m_hpdEnd.find(hpdID);
      // If not found, default to first pixel
      return ( i == m_hpdEnd.end() ? richPixels()->begin() : (*i).second );
    }

    void PixelCreatorBase::handle ( const Incident& incident )
    {
      // Update prior to start of event. Used to re-initialise data containers
      if ( IncidentType::BeginEvent == incident.type() )
      {
        this->InitNewEvent();
      }
      // Debug printout at the end of each event
      else if ( IncidentType::EndEvent == incident.type() )
      {
        this->FinishEvent();
      }
    }

    void PixelCreatorBase::InitNewEvent()
    {
      m_hasBeenCalled = false;
      m_allDone = false;
      m_pixels  = 0;
      if ( m_bookKeep )
      {
        m_pixelExists.clear();
        m_pixelDone.clear();
      }
    }

    void PixelCreatorBase::FinishEvent()
    {
      if ( m_hasBeenCalled ) ++m_Nevts;
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Created " << richPixels()->size() << " RichRecPixels at "
                << pixelLocation() << endreq;
      }
    }

  }
} // RICH

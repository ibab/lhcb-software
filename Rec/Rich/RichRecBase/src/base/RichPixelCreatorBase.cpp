
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorBase.cpp
 *
 *  Implementation file for tool base class : RichPixelCreatorBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/04/2005
 */
//---------------------------------------------------------------------------------

// STL
#include <sstream>

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
      : Rich::Rec::ToolBase ( type, name, parent ),
        m_allDone       ( false ),
        m_richSys       ( NULL  ),
        m_hpdOcc        ( Rich::NRiches ),
        m_hpdClus       ( Rich::NRiches ),
        m_idTool        ( NULL  ),
        m_decoder       ( NULL  ),
        m_geomTool      ( NULL  ),
        m_pixels        ( NULL  ),
        m_bookKeep      ( true  ),
        m_hpdCheck      ( true  ),
        m_clusterHits   ( Rich::NRiches, false ),
        m_noClusterFinding ( false ),
        m_usedDets      ( Rich::NRiches, true  ),
        m_begins        ( boost::extents[Rich::NRiches][Rich::NPDPanelsPerRICH] ),
        m_ends          ( boost::extents[Rich::NRiches][Rich::NPDPanelsPerRICH] ),
        m_Nevts         ( 0 ),
        m_hasBeenCalled ( false ),
        m_applyPixelSuppression ( true  )
    {

      // Define the interface
      declareInterface<IPixelCreator>(this);

      // Define job option parameters
      declareProperty( "RichRecPixelLocation",
                       m_richRecPixelLocation = contextSpecificTES(LHCb::RichRecPixelLocation::Default),
                       "The TES location for the transient RichRecPixel objects" );
      declareProperty( "DoBookKeeping",          m_bookKeep  );
      declareProperty( "UseDetectors",           m_usedDets  );
      declareProperty( "CheckHPDsAreActive",     m_hpdCheck  );
      declareProperty( "ApplyPixelSuppression",  m_applyPixelSuppression );
      declareProperty( "ApplyPixelClustering",   m_clusterHits );
      declareProperty( "SuppressClusterFinding", m_noClusterFinding );
      declareProperty( "MaxPixels",              m_maxPixels = 999999 );

      // Initialise
      m_hpdOcc[Rich::Rich1]  = NULL;
      m_hpdOcc[Rich::Rich2]  = NULL;
      m_hpdClus[Rich::Rich1] = NULL;
      m_hpdClus[Rich::Rich2] = NULL;

    }

    StatusCode PixelCreatorBase::initialize()
    {
      // base class initilize
      const StatusCode sc = Rich::Rec::ToolBase::initialize();
      if ( sc.isFailure() ) { return sc; }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "RichRecPixel location : " << m_richRecPixelLocation << endmsg;
      }

      // get tools and det elems
      acquireTool( "RichRecGeometry", m_geomTool );
      m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

      // Check which detectors to use
      if ( !m_usedDets[Rich::Rich1] )
        Warning( "Pixels for RICH1 are disabled", StatusCode::SUCCESS );
      if ( !m_usedDets[Rich::Rich2] )
        Warning( "Pixels for RICH2 are disabled", StatusCode::SUCCESS );

      // Setup incident services
      incSvc()->addListener( this, IncidentType::BeginEvent );
      incSvc()->addListener( this, IncidentType::EndEvent   );

      // Some tricks to avoid loading tools during first event
      // load hit suppression tools
      if ( m_applyPixelSuppression && m_usedDets[Rich::Rich1] ) { hpdSuppTool(Rich::Rich1); }
      if ( m_applyPixelSuppression && m_usedDets[Rich::Rich2] ) { hpdSuppTool(Rich::Rich2); }
      // load clustering tools
      if ( !m_noClusterFinding ) { hpdClusTool(Rich::Rich1); hpdClusTool(Rich::Rich2); }

      _ri_debug << "Maximum number of pixels per event = " << m_maxPixels << endmsg;

      return sc;
    }

    StatusCode PixelCreatorBase::finalize()
    {
      // print stats
      printStats();

      // base class finalize
      return Rich::Rec::ToolBase::finalize();
    }

    void PixelCreatorBase::printStats() const
    {
      if ( m_Nevts > 0 )
      {
        const StatDivFunctor occ("%8.2f +-%6.2f");
        const std::string & lines
          = "=====================================================================================================";
        info() << lines << endmsg
               << "                       Pixel Cluster Creator Summary for " << m_Nevts << " events :-" << endmsg;

        info() << " Selected         :  RICH1 = " << occ(m_hitCount[Rich::Rich1].numClusters,m_Nevts)
               << "  RICH2 = " << occ(m_hitCount[Rich::Rich2].numClusters,m_Nevts)
               << " HPD pixel clusters / event" << endmsg;

        info() << " Av. Cluster Size :  RICH1 = "
               << occ(m_hitCount[Rich::Rich1].numPixels,m_hitCount[Rich::Rich1].numClusters)
               << "  RICH2 = " << occ(m_hitCount[Rich::Rich2].numPixels,m_hitCount[Rich::Rich2].numClusters)
               << " HPD pixels / cluster" << endmsg;

        info() << " Rejected         :  RICH1 = " << occ(m_hitCount[Rich::Rich1].rejectedPixels,m_Nevts)
               << "  RICH2 = " << occ(m_hitCount[Rich::Rich2].rejectedPixels,m_Nevts)
               << " HPD pixels / event" << endmsg;
        info() << lines << endmsg;
      }
    }

    LHCb::RichRecPixel *
    PixelCreatorBase::buildPixel( const LHCb::RichSmartID & id ) const
    {
      _ri_debug << " -> Creating RichRecPixel from RichSmartID " << id << endmsg;

      // See if this RichRecPixel already exists
      LHCb::RichRecPixel * pixel = ( bookKeep() && m_pixelDone[id] ? m_pixelExists[id] : NULL );
      if ( pixel ) return pixel;

      // Check this hit is OK
      if ( pixelIsOK(id) )
      {

        // Make a new RichRecPixel
        Gaudi::XYZPoint gPos;
        const StatusCode sc = smartIDTool()->globalPosition( id, gPos );
        if ( sc.isSuccess() )
        {
          pixel = new LHCb::RichRecPixel( id,                              // SmartID for pixel
                                          gPos,                            // position in global coords
                                          smartIDTool()->globalToPDPanel(gPos), // position in local coords
                                          Rich::Rec::PixelParent::RawBuffer,    // parent type
                                          NULL                             // pointer to parent (not available)
                                          );

          // set the corrected local positions
          geomTool()->setCorrLocalPos(pixel,id.rich());

          // save to TES container in tool
          savePixel( pixel );

          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Created pixel " << *pixel << endmsg;
            if ( msgLevel(MSG::VERBOSE) )
            {
              // Test global -> local transformations
              verbose() << id << endmsg;
              verbose() << " Global before = " << gPos << endmsg;
              const Gaudi::XYZPoint lPos = smartIDTool()->globalToPDPanel(gPos);
              verbose() << "  -> Local     = " << lPos << endmsg;
              verbose() << "   -> Global   = " << smartIDTool()->globalPosition( lPos, id.rich(), id.panel() )
                        << endmsg;
            }
          }

        } // smartID -> gloval coordinate OK

      } // smartID is valid

      // Add to reference map
      if ( bookKeep() )
      {
        m_pixelExists[ id ] = pixel;
        m_pixelDone  [ id ] = true;
      }

      return pixel;
    }

    LHCb::RichRecPixel *
    PixelCreatorBase::buildPixel( const Rich::HPDPixelCluster& cluster ) const
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << " -> Creating RichRecPixel from cluster " << cluster << endmsg;
      }

      // the core cluster ID
      const LHCb::RichSmartID id = cluster.primaryID();

      // if single pixel cluster, use dedicated method
      if ( 1 == cluster.size() ) return buildPixel(id);

      // See if this RichRecPixel already exists
      LHCb::RichRecPixel * pixel = ( bookKeep() && m_pixelDone[id] ? m_pixelExists[id] : NULL );
      if ( pixel ) return pixel;

      // Check this hit is OK
      if ( pixelIsOK(id) )
      {

        // Make a new RichRecPixel
        Gaudi::XYZPoint gPos;
        const StatusCode sc = smartIDTool()->globalPosition( cluster, gPos );
        if ( sc.isSuccess() )
        {
          pixel = new LHCb::RichRecPixel( cluster,                         // SmartID cluster for pixel
                                          gPos,                            // position in global coords
                                          smartIDTool()->globalToPDPanel(gPos), // position in local coords
                                          Rich::Rec::PixelParent::RawBuffer,    // parent type
                                          NULL                             // pointer to parent (not available)
                                          );

          // set the corrected local positions
          geomTool()->setCorrLocalPos(pixel,id.rich());

          // save to TES container in tool
          savePixel( pixel );

          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Created pixel " << *pixel << endmsg;
          }

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

        // Check for event aborts. If already aborted stop here
        if ( !procStatus()->aborted() )
        {

          // Obtain the raw RICH data from the DAQ decoding
          const Rich::DAQ::L1Map & data = smartIDdecoder()->allRichSmartIDs();

          // Reserve space
          richPixels()->reserve( 2000 );

          // Loop over L1 boards
          for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
                iL1 != data.end(); ++iL1 )
          {
            // loop over ingresses for this L1 board
            for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
                  iIn != (*iL1).second.end(); ++iIn )
            {
              // Loop over HPDs in this ingress
              for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
                    iHPD != (*iIn).second.hpdData().end(); ++iHPD )
              {

                // inhibited HPD ?
                if ( (*iHPD).second.header().inhibit() ) { continue; }

                // Is the HPD OK
                if ( !hpdIsOK((*iHPD).second.hpdID()) )  { continue; }

                // Do we have any hits in this HPD ?
                if ( !(*iHPD).second.smartIDs().empty() )
                {

                  // apply HPD pixel suppression
                  // NB : taking a copy of the smartIDs here since we might remove
                  // some, and we cannot change the raw data from smartIDdecoder()
                  LHCb::RichSmartID::Vector smartIDs = (*iHPD).second.smartIDs();
                  applyPixelSuppression( (*iHPD).second.hpdID(), smartIDs );

                  // if any left, proceed and make pixels
                  if ( !smartIDs.empty() )
                  {

                    // which rich
                    const Rich::DetectorType rich = (*iHPD).second.hpdID().rich();

                    if ( m_noClusterFinding )
                    {

                      // just loop over the raw RichSmartIDs and make a pixel for each
                      // note that the associated cluster is not set in this mode ...
                      for ( LHCb::RichSmartID::Vector::const_iterator iID = smartIDs.begin();
                            iID != smartIDs.end(); ++iID )
                      {
                        LHCb::RichRecPixel * pixel = buildPixel( *iID );
                        if (pixel) pixel->setPhotonDetOccupancy(smartIDs.size());
                      }

                    }
                    else
                    {
                      // perform clustering on the remaining pixels in this HPD
                      const HPDPixelClusters * clusters = hpdClusTool(rich)->findClusters( smartIDs );
                      if ( msgLevel(MSG::DEBUG) )
                      {
                        debug() << "From " << smartIDs.size() << " RichSmartIDs found "
                                << clusters->clusters().size() << " clusters" << endmsg;
                      }

                      // loop over the clusters
                      for ( HPDPixelClusters::Cluster::PtnVector::const_iterator iC = clusters->clusters().begin();
                            iC != clusters->clusters().end(); ++iC )
                      {

                        if ( m_clusterHits[rich] )
                        {
                          // make a single pixel for this cluster
                          LHCb::RichRecPixel * pixel = buildPixel( (*iC)->pixels() );
                          if (pixel) 
                          {
                            pixel->setAssociatedCluster( (*iC)->pixels() );
                            pixel->setPhotonDetOccupancy(smartIDs.size());
                          }
                        }
                        else
                        {
                          // make a smartID for each channel in the cluster
                          for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iC)->pixels().smartIDs().begin();
                                iID != (*iC)->pixels().smartIDs().end(); ++iID )
                          {
                            LHCb::RichRecPixel * pixel = buildPixel( *iID );
                            if (pixel) 
                            {
                              pixel->setAssociatedCluster( (*iC)->pixels() );
                              pixel->setPhotonDetOccupancy(smartIDs.size());
                            }
                          }
                        }

                      } // loop over clusters

                      // cleanup
                      delete clusters;

                    } // do clustering if

                  } // any smartids left after clustering ?

                } // non-empty starting smartID list
                else
                {
                  std::ostringstream mess;
                  mess << "Empty HPD data block : L0ID = " << (*iHPD).second.header().l0ID();
                  Warning( mess.str(), StatusCode::SUCCESS, 0 ).ignore();
                }

              } // loop over HPDs
            } // Ingresses
          } // L1 boards

          // Too many pixels ?
          if ( richPixels()->size() > m_maxPixels )
          {
            std::ostringstream mess;
            mess << "Number of selected pixels exceeds maximum of " << m_maxPixels << " -> Abort";
            Warning( mess.str(), StatusCode::SUCCESS, 0 ).ignore();
            richPixels()->clear();
            procStatus()->addAlgorithmStatus( name(), "RICH", "ReachedPixelLimit", 
                                              Rich::Rec::ReachedPixelLimit, true );
          }

        } // ProcStat aborted
        
        // Must always do the following, even for empty comtainers.
        
        // sort the final pixels
        sortPixels();
        
        // find iterators
        fillIterators();
        
      } // all already done

      return StatusCode::SUCCESS;
    }

    void PixelCreatorBase::fillIterators() const
    {

      // Reset to defaults
      resetIterators();

      // if there are some pixels, compute iterators
      // Should eventually look into if this can be done
      // whilst filling, avoiding the need for the "after-the-fact" loop

      if ( !richPixels()->empty() )
      {

        LHCb::RichRecPixels::iterator iPix = richPixels()->begin();
        Rich::DetectorType rich      = (*iPix)->detector();
        Rich::Side        panel      = (*iPix)->panel().panel();
        LHCb::RichSmartID hpd        = (*iPix)->hpd().pdID();
        Rich::DetectorType lastrich  = rich;
        Rich::Side        lastpanel  = panel;
        LHCb::RichSmartID lasthpd    = hpd;

        // set first HPD pixel iterator
        m_hpdIts[hpd].first = iPix;

        ++iPix; // skip first pixel

        // loop over remaining pixels
        for ( ; iPix != richPixels()->end(); ++iPix )
        {
          if ( panel != (*iPix)->panel().panel() || rich != (*iPix)->detector() )
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
          if ( hpd != (*iPix)->hpd().pdID() )
          {
            hpd                 = (*iPix)->hpd().pdID();
            m_hpdIts[hpd].first = iPix;
            m_hpdIts[lasthpd].second = iPix;
            lasthpd             = hpd;
          }
        }

        // Set final iterators
        m_richEnd[rich]      = iPix;
        m_ends[rich][panel]  = iPix;
        m_hpdIts[hpd].second = iPix;

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
                    << pixelLocation() << endmsg;
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

    IPixelCreator::PixelRange
    PixelCreatorBase::range( const Rich::DetectorType rich ) const
    {
      return IPixelCreator::PixelRange(m_richBegin[rich],m_richEnd[rich]);
    }

    IPixelCreator::PixelRange
    PixelCreatorBase::range( const Rich::DetectorType rich,
                             const Rich::Side         panel ) const
    {
      return IPixelCreator::PixelRange(m_begins[rich][panel],m_ends[rich][panel]);
    }

    IPixelCreator::PixelRange
    PixelCreatorBase::range( const LHCb::RichSmartID pdID ) const
    {
      HPDItMap::iterator i = m_hpdIts.find(pdID);
      return ( i == m_hpdIts.end() ?
               IPixelCreator::PixelRange(richPixels()->begin()) :
               IPixelCreator::PixelRange(i->second.first,i->second.second) );
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
                << pixelLocation() << endmsg;
      }
    }

  }
} // RICH

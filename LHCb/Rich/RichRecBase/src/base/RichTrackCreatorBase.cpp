
//---------------------------------------------------------------------------------
/** @file RichTrackCreatorBase.cpp
 *
 *  Implementation file for tool base class : RichTrackCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorBase.cpp,v 1.15 2008-01-11 11:51:10 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------

// local
#include "RichRecBase/RichTrackCreatorBase.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

// RICH
namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------

    // Standard constructor, initializes variables
    TrackCreatorBase::TrackCreatorBase( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
      : Rich::Rec::ToolBase    ( type, name, parent ),
        m_hasBeenCalled        ( false              ),
        m_smartIDTool          ( NULL               ),
        m_rayTrace             ( NULL               ),
        m_tracks               ( 0                  ),
        m_richRecTrackLocation ( LHCb::RichRecTrackLocation::Default ),
        m_Nevts                ( 0                  ),
        m_bookKeep             ( false              ),
        m_traceModeRad         ( Rich::NRadiatorTypes,
                                 LHCb::RichTraceMode(LHCb::RichTraceMode::IgnoreHPDAcceptance) )
    {

      // Define the interface
      declareInterface<ITrackCreator>(this);

      if      ( context() == "Offline" )
      {
        m_richRecTrackLocation = LHCb::RichRecTrackLocation::Offline;
      }
      else if ( context() == "HLT" )
      {
        m_richRecTrackLocation = LHCb::RichRecTrackLocation::HLT;
      }

      // job options
      declareProperty( "DoBookKeeping", m_bookKeep );

    }

    StatusCode TrackCreatorBase::initialize()
    {
      // base class initilize
      const StatusCode sc = Rich::Rec::ToolBase::initialize();
      if ( sc.isFailure() ) { return sc; }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "RichRecTrack location : " << m_richRecTrackLocation << endreq;
      }

      // tools
      acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
      acquireTool( "RichRayTracing",  m_rayTrace             );
      acquireTool( "TrackSelector", m_trSelector, this       );

      // Setup incident services
      incSvc()->addListener( this, IncidentType::BeginEvent );
      incSvc()->addListener( this, IncidentType::EndEvent   );

      // track ray tracing
      m_traceModeRad[Rich::Aerogel].setAeroRefraction(true);
      info() << "Aerogel  Track " << m_traceModeRad[Rich::Aerogel]  << endreq;
      info() << "Rich1Gas Track " << m_traceModeRad[Rich::Rich1Gas] << endreq;
      info() << "Rich2Gas Track " << m_traceModeRad[Rich::Rich2Gas] << endreq;

      return sc;
    }

    StatusCode TrackCreatorBase::finalize()
    {
      // print stats
      printStats();

      // base class finalize
      return Rich::Rec::ToolBase::finalize();
    }

    // Method that handles various Gaudi "software events"
    void TrackCreatorBase::handle ( const Incident& incident )
    {
      // Update prior to start of event. Used to re-initialise data containers
      if ( IncidentType::BeginEvent == incident.type() )
      {
        InitNewEvent();
      }
      // Debug printout at the end of each event
      else if ( IncidentType::EndEvent == incident.type() )
      {
        FinishEvent();
      }
    }

    void TrackCreatorBase::printStats() const
    {

      if ( m_Nevts > 0 && !m_nTracksAll.empty() )
      {

        // Statistical tools
        const PoissonEffFunctor eff("%6.2f +-%5.2f");
        const StatDivFunctor    occ("%8.2f +-%5.2f");

        // Print out final track stats
        info() << "=================================================================================" << endreq;
        info() << "                     Track Selection Summary : " << m_Nevts << " events" << endreq;
        info() << "---------------------------------------------------------------------------------" << endreq;
        for ( TrackTypeCount::iterator i = m_nTracksAll.begin();
              i != m_nTracksAll.end(); ++i )
        {
          std::string name =
            ( (*i).first.second ? "Unique " : "NonUnique " ) + Rich::text( (*i).first.first );
          name.resize(18,' ');
          info() << "  " << name << " :" << occ((*i).second.selectedTracks,m_Nevts)
                 << " tracks/event : RICH eff " << eff((*i).second.selectedTracks,(*i).second.triedTracks)
                 << " % " << endreq;
          if ( (*i).second.aeroSegs>0 )
            info() << "                     :"
                   << occ((*i).second.aeroSegs,m_Nevts)  << " Aerogel  segments/event" << endreq;
          if ( (*i).second.rich1GasSegs>0 )
            info() << "                     :"
                   << occ((*i).second.rich1GasSegs,m_Nevts) << " Rich1Gas segments/event" << endreq;
          if ( (*i).second.rich2GasSegs>0 )
            info() << "                     :"
                   << occ((*i).second.rich2GasSegs,m_Nevts)   << " Rich2Gas segments/event" << endreq;
        }
        info() << "=================================================================================" << endreq;

      }

    }

    void TrackCreatorBase::InitNewEvent()
    {
      m_hasBeenCalled = false;
      if ( bookKeep() ) m_trackDone.clear();
      m_tracks   = NULL;
      if ( msgLevel(MSG::DEBUG) )
      {
        m_nTracksLast = m_nTracksAll;
      }
    }

    void TrackCreatorBase::FinishEvent()
    {
      if ( m_hasBeenCalled ) 
      {
        ++m_Nevts;
        if ( msgLevel(MSG::DEBUG) )
        {
          // Print out of track count for this event
          unsigned int nTotTried(0), nTotSel(0);
          for ( TrackTypeCount::iterator i = m_nTracksAll.begin(); i != m_nTracksAll.end(); ++i )
          {
            nTotTried += (*i).second.triedTracks - m_nTracksLast[(*i).first].triedTracks;
            nTotSel   += (*i).second.selectedTracks - m_nTracksLast[(*i).first].selectedTracks;
          }
          debug() << "Selected " << nTotSel << "/" << nTotTried << " Tracks :";
          for ( TrackTypeCount::iterator i = m_nTracksAll.begin(); i != m_nTracksAll.end(); ++i )
          {
            const std::string name =
              ( (*i).first.second ? "Unique:" : "NonUnique:" ) + Rich::text( (*i).first.first );
            debug() << " " << name << "=("
                    << (*i).second.selectedTracks - m_nTracksLast[(*i).first].selectedTracks << "/"
                    << (*i).second.triedTracks - m_nTracksLast[(*i).first].triedTracks << ")";
          }
          debug() << endreq;
        } // debug
      } // has been called
    }

    LHCb::RichRecTracks * TrackCreatorBase::richTracks() const
    {
      if ( !m_tracks ) {

        if ( !exist<LHCb::RichRecTracks>(m_richRecTrackLocation) )
        {

          // Reinitialise the track Container
          m_tracks = new LHCb::RichRecTracks();

          // Register new RichRecTrack container to Gaudi data store
          put( m_tracks, m_richRecTrackLocation );

        }
        else
        {

          // get tracks from TES
          m_tracks = get<LHCb::RichRecTracks>(m_richRecTrackLocation);
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "Found " << m_tracks->size() << " pre-existing RichRecTracks in TES at "
                    << m_richRecTrackLocation << endreq;
          }

          if ( m_bookKeep )
          {
            // Remake local track reference map
            for ( LHCb::RichRecTracks::const_iterator iTrack = m_tracks->begin();
                  iTrack != m_tracks->end();
                  ++iTrack )
            {
              m_trackDone[(*iTrack)->key()] = true;
            }
          }

        }

      }

      return m_tracks;
    }

    StatusCode
    TrackCreatorBase::rayTraceHPDPanelPoints( const LHCb::RichTrackSegment & trSeg,
                                              LHCb::RichRecSegment * newSegment ) const
    {
      // best start point and direction
      const Gaudi::XYZVector & trackDir = trSeg.bestMomentum();
      const Gaudi::XYZPoint  & trackPtn = trSeg.bestPoint();

      // Get primary PD panel impact point
      //Gaudi::XYZPoint hitPoint;
      LHCb::RichGeomPhoton photon;
      const Gaudi::XYZPoint & hitPoint = photon.detectionPoint();
      LHCb::RichTraceMode::RayTraceResult result 
        = rayTraceTool()->traceToDetector( trSeg.rich(),
                                           trackPtn,
                                           trackDir,
                                           photon,
                                           m_traceModeRad[trSeg.radiator()],
                                           Rich::top, 
                                           trSeg.avPhotonEnergy() );
      const bool OK = m_traceModeRad[trSeg.radiator()].traceWasOK(result);
      if ( OK )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "   -> Segment traces to HPD panel at " << hitPoint << endreq;

        // set global hit point
        newSegment->setPdPanelHitPoint( hitPoint );

        // set closest HPD
        newSegment->setClosestHPD(photon.smartID().hpdID());

        // Get PD panel hit point in local coordinates
        newSegment->setPdPanelHitPointLocal( smartIDTool()->globalToPDPanel(hitPoint) );

        // Set the forced side ray traced points
        LHCb::RichTraceMode tmpTraceMode(m_traceModeRad[trSeg.radiator()]);
        tmpTraceMode.setForcedSide(true);

        // left/top
        result = rayTraceTool()->traceToDetector( trSeg.rich(),
                                                  trackPtn,
                                                  trackDir,
                                                  photon,
                                                  tmpTraceMode,
                                                  Rich::left,
                                                  trSeg.avPhotonEnergy() );
        if ( tmpTraceMode.traceWasOK(result) )
        {
          newSegment->setPdPanelHitPoint( hitPoint, Rich::left );
          newSegment->setPdPanelHitPointLocal( smartIDTool()->globalToPDPanel(hitPoint), Rich::left );
          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "    -> Segment force traces to left/top HPD panel at " << hitPoint << endreq;
        }

        // right/bottom
        result = rayTraceTool()->traceToDetector( trSeg.rich(),
                                                  trackPtn,
                                                  trackDir,
                                                  photon,
                                                  tmpTraceMode,
                                                  Rich::right,
                                                  trSeg.avPhotonEnergy() );
        if ( tmpTraceMode.traceWasOK(result) )
        {
          newSegment->setPdPanelHitPoint( hitPoint, Rich::right );
          newSegment->setPdPanelHitPointLocal( smartIDTool()->globalToPDPanel(hitPoint), Rich::right );
          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "    -> Segment force traces to right/bottom HPD panel at " << hitPoint << endreq;
        }

      }

      return ( OK ? StatusCode::SUCCESS : StatusCode::FAILURE );
    }

  }
} // RICH

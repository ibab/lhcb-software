// Include files 

#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/MCVertex.h"

// local
#include "FastPVMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastPVMonitor
//
// 2011-11-22 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastPVMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastPVMonitor::FastPVMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
#ifdef DEBUG_HISTO
  : GaudiTupleAlg ( name , pSvcLocator )
#else
  : GaudiAlgorithm ( name , pSvcLocator )
#endif

{
  declareProperty( "MinIPForTrack",    m_minIPForTrack  = 0.150 * Gaudi::Units::mm );
  declareProperty( "MaxIPForTrack",    m_maxIPForTrack  = 3.000 * Gaudi::Units::mm );
}
//=============================================================================
// Destructor
//=============================================================================
FastPVMonitor::~FastPVMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastPVMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_nEvent    = 0;
  m_nVertices = 0;
  m_nLargeIP  = 0;

  m_s0  = 0.;
  m_sx  = 0.;
  m_sy  = 0.;
  m_sz  = 0.;
  m_sx2 = 0.;
  m_sy2 = 0.;
  m_sz2 = 0.;

#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastPVMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* tracks   = get<LHCb::Tracks>( LHCb::TrackLocation::Velo );
  // LHCb::Tracks* tracks   = get<LHCb::Tracks>( LHCb::TrackLocation::VP );                  // Reconstructed tracks
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Primary );      // Reconstructed vertices

  LHCb::MCVertices*  MCvs = getIfExists<LHCb::MCVertices>( LHCb::MCVertexLocation::Default );  // MC vertices
  std::vector<LHCb::MCVertex *> mcPrimaryVertices;                                          // MC primary vertices
  if(MCvs) // MC data is not always there - in such case skip MC processing
  {
    LHCb::MCVertices::iterator itMCv;
    for ( itMCv = MCvs->begin(); MCvs->end() != itMCv ; ++itMCv )
    { if( (*itMCv)->isPrimary() )
      { // printf("MC Prim. Vertex: [%+6.3f,%+6.3f,%+8.3f] [mm]\n",
        //        (*itMCv)->position().x(), (*itMCv)->position().y(), (*itMCv)->position().z() );
#ifdef DEBUG_HISTO
        plot((*itMCv)->position().z(), "McVertexZ", "FastPVMonitor: MC vertex Z [mm]",
             -202.0, +202.0, 101);
        plot2D((*itMCv)->position().x(), (*itMCv)->position().y(), "McVertexYvsX", "FastPVMonitor: MC vertex Y vx X [mm]",
               -0.6, +0.6, -0.6, +0.6, 60, 60 );
#endif
        mcPrimaryVertices.push_back(*itMCv); }
    }

#ifdef DEBUG_HISTO
    plot(pvs->size(), "RecVerticesPerEvent", "FastPVMonitor: reconstructed vertices per event",
         -0.5, 10.5, 11);
    plot2D(mcPrimaryVertices.size(), pvs->size(), "NumRecVsMcVertices", "FastPVMonitor: number of reconstructed vs MC vertices per event",
         -0.5, 10.5, -0.5, 10.5, 11, 11 );
#endif
  } // of MCvs

  m_nEvent    += 1;
  m_nVertices += pvs->size();
  LHCb::RecVertices::iterator itPv;
  for ( itPv = pvs->begin(); pvs->end() != itPv ; ++itPv ) {      // loop over reconstructed primary vertices
    double xv = (*itPv)->position().x();
    double yv = (*itPv)->position().y();
    double zv = (*itPv)->position().z();
    m_s0  += 1.;
    m_sx  += xv;
    m_sy  += yv;
    m_sz  += zv;
    m_sx2 += xv * xv;
    m_sy2 += yv * yv;
    m_sz2 += zv * zv;
    // printf("Rec. Vertex of %3d tracks: [%+6.3f,%+6.3f,%+8.3f] +/- [%5.3f,%5.3f,%5.3f] [mm]\n",
    //        (*itPv)->tracks().size(),
    //        xv, yv, zv,
    //         sqrt((*itPv)->covMatrix()(0,0)), sqrt((*itPv)->covMatrix()(1,1)), sqrt((*itPv)->covMatrix()(2,2)) );
    if ( msgLevel( MSG::DEBUG ) )
    { info() << format( "x%7.3f y%7.3f z%7.2f nTr%5d  errx %7.3f errY %7.3f errZ %7.3f",
                        xv, yv, zv, (*itPv)->tracks().size(),
                        sqrt((*itPv)->covMatrix()(0,0)),
                        sqrt((*itPv)->covMatrix()(1,1)),
                        sqrt((*itPv)->covMatrix()(2,2)) ) << endmsg; }

#ifdef DEBUG_HISTO
    if(MCvs)                                                   // here for a given reconstructed vertex we find the closest MC vertex
    {
      Gaudi::XYZVector minDist;                                // minimum distance (vector) to a MC vertex
      double           minDist2 = 0;                           // minimum distance square
      LHCb::MCVertex  *bestMCv = 0;                            // best MC vertex (pointer to)
      std::vector<LHCb::MCVertex *>::iterator itMCv;
      for (itMCv = mcPrimaryVertices.begin(); mcPrimaryVertices.end() != itMCv ; ++itMCv ) // loop over MC primary vertices
      { LHCb::MCVertex *MCv = (*itMCv);                                    // MC vertex (pointer to)
        Gaudi::XYZVector Dist = MCv->position() - (*itPv)->position();     // distance (vector) to this MC vertex
        double Dist2 = Dist.mag2();                                        // distance square
        if( (bestMCv==0) || (Dist2<minDist2) )                             // if best so far,
        { minDist2 = Dist2;                                                // store it as the minimum one
          minDist  = Dist;
          bestMCv=MCv; }
      }

      if(bestMCv)
      { plot(minDist.x(), "RecVertexToMcX", "FastPVMonitor: Reconstructed vs MC vertex X [mm]",
             -0.25, +0.25, 100);
        plot(minDist.y(), "RecVertexToMcY", "FastPVMonitor: Reconstructed vs MC vertex Y [mm]",
             -0.25, +0.25, 100);
        plot(minDist.z(), "RecVertexToMcZ", "FastPVMonitor: Reconstructed vs MC vertex Z [mm]",
             -1.00, +1.00, 100);
        plot2D(minDist.x(), minDist.y(), "RecVertexToMcXY", "FastPVMonitor: Reconstructed vs MC vertex X-Y [mm]",
             -0.25, +0.25, -0.25, +0.25, 50, 50);
        plot2D( (*itPv)->tracks().size(), minDist.x(),
                "RecVertexToMcXvsNoT", "FastPVMonitor: Reconstructed vs MC vertex X [mm] vs number of tracks",
                -0.5, 203.5, -0.20, +0.20, 51, 40);
        plot2D( (*itPv)->tracks().size(), minDist.y(),
                "RecVertexToMcYvsNoT", "FastPVMonitor: Reconstructed vs MC vertex Y [mm] vs number of tracks",
                -0.5, 203.5, -0.20, +0.20, 51, 40);
        plot2D( (*itPv)->tracks().size(), minDist.z(),
                "RecVertexToMcZvsNoT", "FastPVMonitor: Reconstructed vs MC vertex Z [mm] vs number of tracks",
                -0.5, 203.5, -0.80, +0.80, 51, 40);
      }
      plot((*itPv)->tracks().size(), "TracksPerRecVertex", "FastPVMonitor: Number of tracks per reconstructed vertex",
           -0.5, 201.5, 101);
      plot(zv, "RecVertexZ", "FastPVMonitor: Reconstructed vertex Z [mm]",
           -202.0, +202.0, 101);
      plot2D(xv, yv, "RecVertexYvsX", "FastPVMonitor: Reconstructed vertex Y vx X [mm]",
             -0.6, +0.6, -0.6, +0.6, 60, 60 );
      plot(sqrt((*itPv)->covMatrix()(0,0)), "RecVertexErrX", "FastPVMonitor: Reconstructed vertex X est. error [mm]",
           0.0, 0.15, 50);
      plot(sqrt((*itPv)->covMatrix()(1,1)), "RecVertexErrY", "FastPVMonitor: Reconstructed vertex Y est. error [mm]",
           0.0, 0.15, 50);
      plot(sqrt((*itPv)->covMatrix()(2,2)), "RecVertexErrZ", "FastPVMonitor: Reconstructed vertex Z est. error [mm]",
           0.0, 1.0, 50);
      plot2D( (*itPv)->tracks().size(), sqrt((*itPv)->covMatrix()(0,0)),
              "RecVertexErrXvsNoT", "FastPVMonitor: Reconstructed vertex X error [mm] vs number of vertex tracks",
              -0.5, 203.5, 0.0, 0.10, 51, 20);
      plot2D( (*itPv)->tracks().size(), sqrt((*itPv)->covMatrix()(1,1)),
              "RecVertexErrYvsNoT", "FastPVMonitor: Reconstructed vertex Y error [mm] vs number of vertex tracks",
              -0.5, 203.5, 0.0, 0.10, 51, 20);
      plot2D( (*itPv)->tracks().size(), sqrt((*itPv)->covMatrix()(2,2)),
              "RecVertexErrZvsNoT", "FastPVMonitor: Reconstructed vertex Z error [mm] vs number of vertex tracks",
              -0.5, 203.5, 0.0, 0.80, 51, 20);
    }
#endif
  }

  // Number of tracks with IP between min and max values
  for ( LHCb::Tracks::iterator itT = tracks->begin(); tracks->end() != itT ; ++itT ) {   // loop over tracks
    if ( (*itT)->checkFlag( LHCb::Track::Backward ) ) continue;                          // avoid backward tracks
    Gaudi::XYZPoint point = (*itT)->position();                                          // track position (first hit ?)
    Gaudi::XYZVector dir  = (*itT)->slopes();                                            // track direction

    // here for a given reconstructed track we find the closest reconstructed vertex
    double lowestIP  = 0;
#ifdef DEBUG_HISTO
    double lowestIPx = 0;
    double lowestIPy = 0;
#endif
    LHCb::RecVertex* bestVertex = 0;                                                     // best vertex pointer
    for (  itPv = pvs->begin(); pvs->end() != itPv ; ++itPv ) {                          // loop over vertices
      double dx = point.x() + ( (*itPv)->position().z() - point.z() ) * dir.x() - (*itPv)->position().x(); // track position X at vertex Z
      double dy = point.y() + ( (*itPv)->position().z() - point.z() ) * dir.y() - (*itPv)->position().y(); // track position Y at vertex Z
      double ip2 = dx*dx + dy*dy;                                                        // squared distance vertex-track ?
      if ( (bestVertex==0) || (ip2<lowestIP) )
      { lowestIP   = ip2;
#ifdef DEBUG_HISTO
        lowestIPx  = dx;
        lowestIPy  = dy;
#endif
        bestVertex = *itPv;                                                                    // remember the best vertex
      }
    }

#ifdef DEBUG_HISTO
  if(bestVertex)
  { plot2D(lowestIPx, lowestIPy, "TrackDistToRecVertex", "FastPVMonitor: Rec. forward track distance [X,Y] [mm] to closest rec. vertex",
         -1.1, +1.1, -1.1, +1.1, 110, 110 );
    plot(lowestIPx, "TrackDistToRecVertexX", "FastPVMonitor: Rec. forward track distance X [mm] to closest rec. vertex",
         -1.1, +1.1, 220);
    plot(lowestIPy, "TrackDistToRecVertexY", "FastPVMonitor: Rec. forward track distance Y [mm] to closest rec. vertex",
         -1.1, +1.1, 220);
    plot2D( (*itT)->pseudoRapidity(), lowestIPx,
            "TrackDistToRecVertexVsEtaX", "FastPVMonitor: Rec. forward track distance X [mm] to closest rec. vertex vs Eta",
            1.5, 5.0, -0.2, +0.2, 35, 40);
    plot2D( (*itT)->pseudoRapidity(), lowestIPy,
            "TrackDistToRecVertexVsEtaY", "FastPVMonitor: Rec. forward track distance Y [mm] to closest rec. vertex vs Eta",
            1.5, 5.0, -0.2, +0.2, 35, 40);

    plot2D( (*itT)->chi2PerDoF(), lowestIPx,
            "TrackDistToRecVertexVsChi2X", "FastPVMonitor: Rec. track distance X [mm] to closest rec. vertex vs Chi^2/DoF",
            0.0, 8.0, -0.2, +0.2, 40, 40);
    plot2D( (*itT)->chi2PerDoF(), lowestIPy,
            "TrackDistToRecVertexVsChi2Y", "FastPVMonitor: Rec. track distance Y [mm] to closest rec. vertex vs Chi^2/DoF",
            0.0, 8.0, -0.2, +0.2, 40, 40);

  }
#endif

    lowestIP  = sqrt(lowestIP);
    if ( bestVertex && (lowestIP > m_minIPForTrack) && (lowestIP < m_maxIPForTrack) )
    { if ( msgLevel( MSG::DEBUG ) )
      { info() << format( "Large IP Track %3d ip %7.3f ", (*itT)->key(), lowestIP )  << endmsg; }
      m_nLargeIP++;
    }
  }

#ifdef DEBUG_HISTO
  if(MCvs)
  {
    for ( LHCb::Tracks::iterator itT = tracks->begin(); tracks->end() != itT ; ++itT )     // loop over tracks
    { // if ( (*itT)->checkFlag( LHCb::Track::Backward ) ) continue;                       // avoid backward tracks
      Gaudi::XYZPoint point = (*itT)->position();                                          // track position (first hit ?)
      Gaudi::XYZVector dir  = (*itT)->slopes();                                            // track direction
      // yprintf("[%+7.3f, %+7.3f, %+7.3f] => [%+6.3f, %+6.3f, %+6.3f]\n",
      //       point.x(), point.y(), point.z(), dir.x(), dir.y(), dir.z() );
      double minDistX=0; double minDistY=0;                                      // minimum distance (vector) to a MC vertex
      double minDist2 = 0;                                                       // minimum distance square
      LHCb::MCVertex  *bestMCv = 0;                                              // best MC vertex (pointer to)
      std::vector<LHCb::MCVertex *>::iterator itMCv;
      for (itMCv = mcPrimaryVertices.begin(); mcPrimaryVertices.end() != itMCv ; ++itMCv ) // loop over MC primary vertices
      { LHCb::MCVertex *MCv = (*itMCv);                                    // MC vertex (pointer to)
        double dx = point.x() + ( MCv->position().z() - point.z() ) * dir.x() - MCv->position().x(); // track position X at vertex Z
        double dy = point.y() + ( MCv->position().z() - point.z() ) * dir.y() - MCv->position().y(); // track position Y at vertex Z
        // printf(" [%+6.3f, %+6.3f]\n", dx, dy);
        double Dist2 = dx*dx + dy*dy;                                      // squared distance vertex-track ?
        if( (bestMCv==0) || (Dist2<minDist2) )                             // if best so far,
        { minDist2 = Dist2;                                                // store it as the minimum one
          minDistX=dx; minDistY=dy;
          bestMCv=MCv; }
      }

      plot( (*itT)->chi2PerDoF(), "TrackChi2perDoF", "FastPVMonitor: Rec. track Chi^2/DoF",
             0.0, 10.0, 100 );
      if(bestMCv)
      { 
        plot2D(minDistX, minDistY, "TrackDistToMcVertex", "FastPVMonitor: Rec. track distance [X,Y] [mm] to closest MC vertex",
             -1.1, +1.1, -1.1, +1.1, 110, 110 );
        plot(minDistX, "TrackDistToMcVertexX", "FastPVMonitor: Rec. track distance X [mm] to closest MC vertex",
             -1.1, +1.1, 220 );
        plot(minDistY, "TrackDistToMcVertexY", "FastPVMonitor: Rec. track distance Y [mm] to closest MC vertex",
             -1.1, +1.1, 220 );

        plot2D( (*itT)->pseudoRapidity(), minDistX,
                "TrackDistToMcVertexVsEtaX", "FastPVMonitor: Rec. track distance X [mm] to closest MC vertex vs Eta",
                1.5, 5.0, -0.2, +0.2, 35, 40);
        plot2D( (*itT)->pseudoRapidity(), minDistY,
                "TrackDistToMcVertexVsEtaY", "FastPVMonitor: Rec. track distance Y [mm] to closest MC vertex vs Eta",
                1.5, 5.0, -0.2, +0.2, 35, 40);

        plot2D( (*itT)->chi2PerDoF(), minDistX,
                "TrackDistToMcVertexVsChi2X", "FastPVMonitor: Rec. track distance X [mm] to closest MC vertex vs Chi^2/DoF",
                0.0, 8.0, -0.2, +0.2, 40, 40);
        plot2D( (*itT)->chi2PerDoF(), minDistY,
                "TrackDistToMcVertexVsChi2Y", "FastPVMonitor: Rec. track distance Y [mm] to closest MC vertex vs Chi^2/DoF",
                0.0, 8.0, -0.2, +0.2, 40, 40);

      }
    }
  }
#endif

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastPVMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << format( "Produced %7.3f vertices per event", float( m_nVertices) / float( m_nEvent ) ) << endmsg;
  double trackRate = double( m_nLargeIP ) / double ( m_nEvent );
  info() << format( "Number of large ( %5.3f to %5.3f mm) IP tracks/event = %7.2f",
                    m_minIPForTrack, m_maxIPForTrack, trackRate )
         << endmsg;

  double xm = m_sx / m_s0;
  double ym = m_sy / m_s0;
  double zm = m_sz / m_s0;
  info() << format( "  X position %7.3f sigma %7.3f", xm, sqrt( m_sx2/m_s0 - xm * xm ) ) << endmsg;
  info() << format( "  Y position %7.3f sigma %7.3f", ym, sqrt( m_sy2/m_s0 - ym * ym ) ) << endmsg;
  info() << format( "  Z position %7.3f sigma %7.3f", zm, sqrt( m_sz2/m_s0 - zm * zm ) ) << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

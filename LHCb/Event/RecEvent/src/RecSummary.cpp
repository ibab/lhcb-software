
//-----------------------------------------------------------------------------
/** @file RecSummary.cpp
 *
 *  Implementation file for class : LHCb::RecSummary
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    19/02/2011
 */
//-----------------------------------------------------------------------------

#include "Event/RecSummary.h"

//-----------------------------------------------------------------------------

std::ostream& LHCb::RecSummary::fillStream(std::ostream& s) const
{
  s << "{ Summary Info :";

  s << " nPVs = " << info( LHCb::RecSummary::nPVs, 0 );

  s << " nTracks = " << info( LHCb::RecSummary::nTracks, 0 );
  s << " nLongTracks = " << info( LHCb::RecSummary::nLongTracks, 0 );
  s << " nDownstreamTracks = " << info( LHCb::RecSummary::nDownstreamTracks, 0 );
  s << " nUpstreamTracks = " << info( LHCb::RecSummary::nUpstreamTracks, 0 );
  s << " nVeloTracks = " << info( LHCb::RecSummary::nVeloTracks, 0 );
  s << " nBackTracks = " << info( LHCb::RecSummary::nBackTracks, 0 );
  s << " nTTracks = " << info( LHCb::RecSummary::nTTracks, 0 );
  s << " nGhosts = " << info( LHCb::RecSummary::nGhosts, 0 );
  
  s << " nRich1Hits = " << info( LHCb::RecSummary::nRich1Hits, 0 );
  s << " nRich2Hits = " << info( LHCb::RecSummary::nRich2Hits, 0 );

  s << " nVeloClusters = " << info( LHCb::RecSummary::nVeloClusters, 0 );

  s << " nTTClusters = " << info( LHCb::RecSummary::nTTClusters, 0 );

  s << " nUTClusters = " << info( LHCb::RecSummary::nUTClusters, 0 );

  s << " nFTClusters = " << info( LHCb::RecSummary::nFTClusters, 0 );

  s << " nITClusters = " << info( LHCb::RecSummary::nITClusters, 0 );

  s << " nOTClusters = " << info( LHCb::RecSummary::nOTClusters, 0 );

  s << " nSPDhits = " << info( LHCb::RecSummary::nSPDhits, 0 );

  s << " nMuonCoordsS0 = " << info( LHCb::RecSummary::nMuonCoordsS0, 0 );
  s << " nMuonCoordsS1 = " << info( LHCb::RecSummary::nMuonCoordsS1, 0 );
  s << " nMuonCoordsS2 = " << info( LHCb::RecSummary::nMuonCoordsS2, 0 );
  s << " nMuonCoordsS3 = " << info( LHCb::RecSummary::nMuonCoordsS3, 0 );
  s << " nMuonCoordsS4 = " << info( LHCb::RecSummary::nMuonCoordsS4, 0 );
  s << " nMuonTracks = "   << info( LHCb::RecSummary::nMuonTracks, 0 );

  return s << " }";
}


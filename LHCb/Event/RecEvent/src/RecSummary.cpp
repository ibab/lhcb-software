// $Id: $

#include "Event/RecSummary.h"

//-----------------------------------------------------------------------------

std::ostream& LHCb::RecSummary::fillStream(std::ostream& s) const
{
  s << "{ Summary Info :";

  if ( hasInfo(LHCb::RecSummary::nPVs) )
    s << " nPVs = " << info( LHCb::RecSummary::nPVs, 0 );

  if ( hasInfo(LHCb::RecSummary::nLongTracks) )
    s << " nLongTracks = " << info( LHCb::RecSummary::nLongTracks, 0 );
  if ( hasInfo(LHCb::RecSummary::nDownstreamTracks) )
    s << " nDownstreamTracks = " << info( LHCb::RecSummary::nDownstreamTracks, 0 );
  if ( hasInfo(LHCb::RecSummary::nUpstreamTracks) )
    s << " nUpstreamTracks = " << info( LHCb::RecSummary::nUpstreamTracks, 0 );
  if ( hasInfo(LHCb::RecSummary::nVeloTracks) )
    s << " nVeloTracks = " << info( LHCb::RecSummary::nVeloTracks, 0 );
  if ( hasInfo(LHCb::RecSummary::nBackTracks) )
    s << " nBackTracks = " << info( LHCb::RecSummary::nBackTracks, 0 );
  if ( hasInfo(LHCb::RecSummary::nTTracks) )
    s << " nTTracks = " << info( LHCb::RecSummary::nTTracks, 0 );

  if ( hasInfo(LHCb::RecSummary::nRich1Hits) )
    s << " nRich1Hits = " << info( LHCb::RecSummary::nRich1Hits, 0 );
  if ( hasInfo(LHCb::RecSummary::nRich2Hits) )
    s << " nRich2Hits = " << info( LHCb::RecSummary::nRich2Hits, 0 );

  if ( hasInfo(LHCb::RecSummary::nVeloClusters) )
    s << " nVeloClusters = " << info( LHCb::RecSummary::nVeloClusters, 0 );

  if ( hasInfo(LHCb::RecSummary::nITClusters) )
    s << " nITClusters = " << info( LHCb::RecSummary::nITClusters, 0 );

  if ( hasInfo(LHCb::RecSummary::nOTClusters) )
    s << " nOTClusters = " << info( LHCb::RecSummary::nOTClusters, 0 );

  if ( hasInfo(LHCb::RecSummary::nSPDhits) )
    s << " nSPDhits = " << info( LHCb::RecSummary::nSPDhits, 0 );

  return s << " }";
}


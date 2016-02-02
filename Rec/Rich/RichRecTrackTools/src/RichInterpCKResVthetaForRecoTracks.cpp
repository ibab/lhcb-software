
//-----------------------------------------------------------------------------
/** @file RichInterpCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : Rich::Rec::InterpCKResVthetaForRecoTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichInterpCKResVthetaForRecoTracks.h"

// RICH reco software
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( InterpCKResVthetaForRecoTracks )

// Standard constructor
InterpCKResVthetaForRecoTracks::
InterpCKResVthetaForRecoTracks ( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : ToolBase   ( type, name, parent )
{

  declareInterface<ICherenkovResolution>(this);

  // forward tracks
  declareProperty( "ForwardAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Rec::Track::Forward)] );
  declareProperty( "ForwardRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Rec::Track::Forward)] );
  declareProperty( "ForwardRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Rec::Track::Forward)] );

  // match tracks
  declareProperty( "MatchAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Rec::Track::Match)] );
  declareProperty( "MatchRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Rec::Track::Match)] );
  declareProperty( "MatchRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Rec::Track::Match)] );

  // Seed tracks
  declareProperty( "SeedRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Rec::Track::Seed)] );

  // VeloTT tracks
  declareProperty( "VeloTTAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Rec::Track::VeloTT)] );
  declareProperty( "VeloTTRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Rec::Track::VeloTT)] );

  // KsTrack tracks
  declareProperty( "KsTrackAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Rec::Track::KsTrack)] );
  declareProperty( "KsTrackRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Rec::Track::KsTrack)] );
  declareProperty( "KsTrackRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Rec::Track::KsTrack)] );

}

StatusCode InterpCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  info() << "Using interpolated Cherenkov theta resolution" << endmsg;

  return sc;
}

StatusCode InterpCKResVthetaForRecoTracks::finalize()
{
  // delete interpolators
  for ( auto& i : m_ckRes ) { delete i.second; }

  // Execute base class method
  return Rich::Rec::ToolBase::finalize();
}

double
InterpCKResVthetaForRecoTracks::ckThetaResolution( LHCb::RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const
{
  // Protect against the non-physical below threshold hypothesis
  if ( Rich::BelowThreshold == id ) return 0;

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {
    double res = 0;

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( thetaExp > 0.000001 )
    {

      // Reference to track ID object
      const auto & tkID = segment->richRecTrack()->trackID();

      // Check track parent type is Track
      if ( Rich::Rec::TrackParent::Track != tkID.parentType() )
      {
        Exception( "Track parent type is not Track" );
      }

      // track type
      const auto type = tkID.trackType();

      // which radiator
      const auto rad = segment->trackSegment().radiator();

      // compute the res
      res = getInterp(rad,type)->value(thetaExp);

    }

    // compute the interpolated resolution
    segment->setCKThetaResolution( id, (LHCb::RichRecSegment::FloatType)(res) );

  }

  return segment->ckThetaResolution( id );

}

const Rich1DTabFunc *
InterpCKResVthetaForRecoTracks::getInterp( const Rich::RadiatorType rad,
                                           const Rich::Rec::Track::Type track ) const
{
  const InterKey key(rad,track);
  const auto i = m_ckRes.find(key);
  if ( i == m_ckRes.end() )
  {
    _ri_verbo << "Found CK resolution data for " << rad << " " << track << endmsg;
    m_ckRes[key] = new Rich1DTabFunc(m_joData[key]);
    if ( !m_ckRes[key]->valid() )
    {
      Exception( "Initialisation of interpolator for "+Rich::text(rad)
                 +" "+Rich::text(track)+ " failed" );
    }
    return m_ckRes[key];
  }
  return i->second;
}

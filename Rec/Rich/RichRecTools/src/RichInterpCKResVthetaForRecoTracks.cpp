
//-----------------------------------------------------------------------------
/** @file RichInterpCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : RichInterpCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichInterpCKResVthetaForRecoTracks.cpp,v 1.4 2006-08-28 11:34:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichInterpCKResVthetaForRecoTracks.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichInterpCKResVthetaForRecoTracks>          s_factory ;
const        IToolFactory& RichInterpCKResVthetaForRecoTracksFactory = s_factory ;

// Standard constructor
RichInterpCKResVthetaForRecoTracks::
RichInterpCKResVthetaForRecoTracks ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_ckAngle       ( NULL )
{

  declareInterface<IRichCherenkovResolution>(this);

  // forward tracks
  declareProperty( "ForwardAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Track::Forward)] );
  declareProperty( "ForwardRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Track::Forward)] );
  declareProperty( "ForwardRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Track::Forward)] );

  // match tracks
  declareProperty( "MatchAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Track::Match)] );
  declareProperty( "MatchRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Track::Match)] );
  declareProperty( "MatchRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Track::Match)] );

  // Seed tracks
  declareProperty( "SeedRich2Gas", m_joData[InterKey(Rich::Rich2Gas,Rich::Track::Seed)] );

  // VeloTT tracks
  declareProperty( "VeloTTAerogel",  m_joData[InterKey(Rich::Aerogel, Rich::Track::VeloTT)] );
  declareProperty( "VeloTTRich1Gas", m_joData[InterKey(Rich::Rich1Gas,Rich::Track::VeloTT)] );

}

StatusCode RichInterpCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  info() << "Using interpolated Cherenkov theta resolution" << endreq;

  return sc;
}

StatusCode RichInterpCKResVthetaForRecoTracks::finalize()
{
  // delete interpolators
  for ( Interps::iterator i = m_ckRes.begin(); i != m_ckRes.end(); ++i )
  {
    delete i->second;
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichInterpCKResVthetaForRecoTracks::ckThetaResolution( RichRecSegment * segment,
                                                       const Rich::ParticleIDType id ) const
{

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {
    double res = 0;

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( thetaExp > 0.000001 )
    {

      // Reference to track ID object
      const RichTrackID & tkID = segment->richRecTrack()->trackID();

      // Check track parent type is Track
      if ( Rich::TrackParent::Track != tkID.parentType() )
      {
        Exception( "Track parent type is not Track" );
      }

      // track type
      const Rich::Track::Type type = tkID.trackType();

      // which radiator
      const Rich::RadiatorType rad = segment->trackSegment().radiator();

      // compute the res
      res = getInterp(rad,type)->value(thetaExp);

    }

    // compute the interpolated resolution
    segment->setCKThetaResolution( id, res );

  }

  return segment->ckThetaResolution( id );

}

const Rich1DTabFunc *
RichInterpCKResVthetaForRecoTracks::getInterp( const Rich::RadiatorType rad,
                                               const Rich::Track::Type track ) const
{
  const InterKey key(rad,track);
  Interps::const_iterator i = m_ckRes.find(key);
  if ( i == m_ckRes.end() )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Found CK resolution data for " << rad << " " << track << endreq;
      verbose() << " -> " << m_joData[key] << endreq;
    }
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

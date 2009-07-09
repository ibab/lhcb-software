
//-----------------------------------------------------------------------------
/** @file RichBinnedCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : Rich::Rec::BinnedCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichBinnedCKResVthetaForRecoTracks.cpp,v 1.3 2009-07-09 11:21:24 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichBinnedCKResVthetaForRecoTracks.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( BinnedCKResVthetaForRecoTracks );

// Standard constructor
BinnedCKResVthetaForRecoTracks::
BinnedCKResVthetaForRecoTracks ( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_ckAngle       ( NULL ),
    m_richPartProp  ( NULL )
{

  declareInterface<ICherenkovResolution>(this);

  // Define job option parameters

  declareProperty( "NAerogelResBins",   m_binEdges[Rich::Aerogel] );
  declareProperty( "ForwardAerogelRes", m_theerr[Rich::Aerogel][Rich::Rec::Track::Forward] );
  declareProperty( "MatchAerogelRes",   m_theerr[Rich::Aerogel][Rich::Rec::Track::Match] );
  declareProperty( "KsTrackAerogelRes", m_theerr[Rich::Aerogel][Rich::Rec::Track::KsTrack] );
  declareProperty( "VeloTTAerogelRes",  m_theerr[Rich::Aerogel][Rich::Rec::Track::VeloTT] );
  declareProperty( "SeedAerogelRes",    m_theerr[Rich::Aerogel][Rich::Rec::Track::Seed] );
  declareProperty( "MCRichTrackAerogelRes", m_theerr[Rich::Aerogel][Rich::Rec::Track::MCRichTrack] );

  declareProperty( "NRich1GasResBins",   m_binEdges[Rich::Rich1Gas] );
  declareProperty( "ForwardRich1GasRes", m_theerr[Rich::Rich1Gas][Rich::Rec::Track::Forward] );
  declareProperty( "MatchRich1GasRes",   m_theerr[Rich::Rich1Gas][Rich::Rec::Track::Match] );
  declareProperty( "KsTrackRich1GasRes", m_theerr[Rich::Rich1Gas][Rich::Rec::Track::KsTrack] );
  declareProperty( "VeloTTRich1GasRes",  m_theerr[Rich::Rich1Gas][Rich::Rec::Track::VeloTT] );
  declareProperty( "SeedRich1GasRes",    m_theerr[Rich::Rich1Gas][Rich::Rec::Track::Seed] );
  declareProperty( "MCRichTrackRich1GasRes", m_theerr[Rich::Rich1Gas][Rich::Rec::Track::MCRichTrack] );

  declareProperty( "NRich2GasResBins",   m_binEdges[Rich::Rich2Gas] );
  declareProperty( "ForwardRich2GasRes", m_theerr[Rich::Rich2Gas][Rich::Rec::Track::Forward] );
  declareProperty( "MatchRich2GasRes",   m_theerr[Rich::Rich2Gas][Rich::Rec::Track::Match] );
  declareProperty( "KsTrackRich2GasRes", m_theerr[Rich::Rich2Gas][Rich::Rec::Track::KsTrack] );
  declareProperty( "VeloTTRich2GasRes",  m_theerr[Rich::Rich2Gas][Rich::Rec::Track::VeloTT] );
  declareProperty( "SeedRich2GasRes",    m_theerr[Rich::Rich2Gas][Rich::Rec::Track::Seed] );
  declareProperty( "MCRichTrackRich2GasRes", m_theerr[Rich::Rich2Gas][Rich::Rec::Track::MCRichTrack] );

  // Normalise the resolutions
  declareProperty( "NormaliseRes", m_normalise = true );

}

StatusCode BinnedCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  info() << "Using binned Cherenkov theta resolution" << endreq;

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endreq;

  return sc;
}

StatusCode BinnedCKResVthetaForRecoTracks::finalize()
{
  // Execute base class method
  return Rich::Rec::ToolBase::finalize();
}

double
BinnedCKResVthetaForRecoTracks::ckThetaResolution( LHCb::RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const
{

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {

    if ( m_normalise )
    {
      // make sure all hypos have same value

      // If the different hypos are found to have different (non-zero) values
      // then the lowest value is used for all hypos

      // find the smallest, successfully calculated resolution
      double res(999999), tmp_res(0);
      bool OK[Rich::NParticleTypes];
      Rich::Particles::const_iterator hypo;
      for ( hypo = m_pidTypes.begin(); hypo != m_pidTypes.end(); ++hypo )
      {
        OK[*hypo] = ckThetaResolution_Imp(segment,*hypo,tmp_res);
        if ( OK[*hypo] && tmp_res < res ) { res = tmp_res; }
      }

      // give each hypo that was OK, the smallest resolution
      for ( hypo = m_pidTypes.begin(); hypo != m_pidTypes.end(); ++hypo )
      {
        segment->setCKThetaResolution( *hypo, OK[*hypo] ? static_cast<LHCb::RichRecSegment::FloatType>(res) : 0.0f );
      }

      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
              hypo != m_pidTypes.end(); ++hypo )
        {
          verbose() << "Segment " << segment->key() << " : " << *hypo
                    << " ckRes=" << segment->ckThetaResolution( *hypo ) << endreq;
        }
      }

    }
    else
    {
      // just use the raw value for each hypo

      // means that different hypos could be in different bins and thus
      // get very different resolutions

      double res(0);
      ckThetaResolution_Imp(segment,id,res);
      segment->setCKThetaResolution( id, static_cast<LHCb::RichRecSegment::FloatType>(res) );

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Segment " << segment->key() << " : " << id
                  << " ckRes=" << segment->ckThetaResolution( id ) << endreq;
      }

    }

  }

  // return the final value
  return segment->ckThetaResolution( id );
}

bool
BinnedCKResVthetaForRecoTracks::ckThetaResolution_Imp( LHCb::RichRecSegment * segment,
                                                       const Rich::ParticleIDType id,
                                                       double & res ) const
{

  // Reference to track ID object
  const RichTrackID & tkID = segment->richRecTrack()->trackID();

  // Check track parent type is Track
  //if ( Rich::Rec::TrackParent::Track != tkID.parentType() )
  // {
  //  Exception( "Track parent type is not 'Track'" );
  // }

  res = 0;
  bool OK(false);

  // Expected Cherenkov theta angle
  const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( thetaExp > 1e-6 )
  {

    // radiator type
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // track type
    const Rich::Rec::Track::Type type = tkID.trackType();

    // error data
    const BinData & theerr = m_theerr[rad][type];
    if ( theerr.empty() )
    {
      Exception( "Track parent type "+Rich::Rec::text(type)+" not known" );
    }

    // check data is OK
    if ( m_binEdges[rad].empty() ||
         (m_binEdges[rad].size() != theerr.size()) )
    {
      Exception( "Mis-match in binned CK data for : " + Rich::text(rad) + " " + Rich::text(type) );
    }

    // search the bins
    BinEdges::const_reverse_iterator iBins = (m_binEdges[rad]).rbegin();
    BinData::const_reverse_iterator  iData = theerr.rbegin();
    for ( ; iBins != (m_binEdges[rad]).rend() && iData != theerr.rend();
          ++iBins, ++iData )
    {
      if ( thetaExp >= iBins->first && thetaExp < iBins->second )
      {
        res = *iData;
        OK = true;
        break;
      }
    }
    if ( !OK )
    {
      Error( "Failed to find binned CK resolution for : " + Rich::text(rad) + " " + Rich::text(type) ).ignore();
    }

  }

  // return the final status
  return OK;
}

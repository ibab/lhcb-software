
//-----------------------------------------------------------------------------
/** @file RichBinnedCKResVthetaForRecoTracks.cpp
 *
 *  Implementation file for tool : RichBinnedCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichBinnedCKResVthetaForRecoTracks.cpp,v 1.6 2006-11-01 18:03:02 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichBinnedCKResVthetaForRecoTracks.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichBinnedCKResVthetaForRecoTracks );

// Standard constructor
RichBinnedCKResVthetaForRecoTracks::
RichBinnedCKResVthetaForRecoTracks ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_ckAngle       ( NULL )
{

  declareInterface<IRichCherenkovResolution>(this);

  // Define job option parameters

  declareProperty( "NAerogelResBins",   m_binEdges[Rich::Aerogel] );
  declareProperty( "ForwardAerogelRes", m_theerr[Rich::Aerogel][Rich::Track::Forward] );
  declareProperty( "MatchAerogelRes",   m_theerr[Rich::Aerogel][Rich::Track::Match] );
  declareProperty( "KsTrackAerogelRes", m_theerr[Rich::Aerogel][Rich::Track::KsTrack] );
  declareProperty( "VeloTTAerogelRes",  m_theerr[Rich::Aerogel][Rich::Track::VeloTT] );

  declareProperty( "NRich1GasResBins",   m_binEdges[Rich::Rich1Gas] );
  declareProperty( "ForwardRich1GasRes", m_theerr[Rich::Rich1Gas][Rich::Track::Forward] );
  declareProperty( "MatchRich1GasRes",   m_theerr[Rich::Rich1Gas][Rich::Track::Match] );
  declareProperty( "KsTrackRich1GasRes", m_theerr[Rich::Rich1Gas][Rich::Track::KsTrack] );
  declareProperty( "VeloTTRich1GasRes",  m_theerr[Rich::Rich1Gas][Rich::Track::VeloTT] );

  declareProperty( "NRich2GasResBins",   m_binEdges[Rich::Rich2Gas] );
  declareProperty( "ForwardRich2GasRes", m_theerr[Rich::Rich2Gas][Rich::Track::Forward] );
  declareProperty( "MatchRich2GasRes",   m_theerr[Rich::Rich2Gas][Rich::Track::Match] );
  declareProperty( "KsTrackRich2GasRes", m_theerr[Rich::Rich2Gas][Rich::Track::KsTrack] );
  declareProperty( "SeedRich2GasRes",    m_theerr[Rich::Rich2Gas][Rich::Track::Seed] );

  declareProperty( "NormaliseRes", m_normalise = true );

}

StatusCode RichBinnedCKResVthetaForRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  info() << "Using binned Cherenkov theta resolution" << endreq;

  return sc;
}

StatusCode RichBinnedCKResVthetaForRecoTracks::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichBinnedCKResVthetaForRecoTracks::ckThetaResolution( RichRecSegment * segment,
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
      const bool el_ok = ckThetaResolution_Imp(segment,Rich::Electron,tmp_res);
      if ( el_ok && tmp_res < res ) { res = tmp_res; }
      const bool mu_ok = ckThetaResolution_Imp(segment,Rich::Muon,tmp_res);
      if ( mu_ok && tmp_res < res ) { res = tmp_res; }
      const bool pi_ok = ckThetaResolution_Imp(segment,Rich::Pion,tmp_res);
      if ( pi_ok && tmp_res < res ) { res = tmp_res; }
      const bool ka_ok = ckThetaResolution_Imp(segment,Rich::Kaon,tmp_res); 
      if ( ka_ok && tmp_res < res ) { res = tmp_res; }
      const bool pr_ok = ckThetaResolution_Imp(segment,Rich::Proton,tmp_res);
      if ( pr_ok && tmp_res < res ) { res = tmp_res; }

      // give each hypo that was OK, the smallest resolution
      segment->setCKThetaResolution( Rich::Electron, el_ok ? res : 0 );
      segment->setCKThetaResolution( Rich::Muon,     mu_ok ? res : 0 );
      segment->setCKThetaResolution( Rich::Pion,     pi_ok ? res : 0 );
      segment->setCKThetaResolution( Rich::Kaon,     ka_ok ? res : 0 );
      segment->setCKThetaResolution( Rich::Proton,   pr_ok ? res : 0 );

      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
        {
          const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);
          verbose() << "Segment " << segment->key() << " : " << hypo 
                    << " ckRes=" << segment->ckThetaResolution( hypo ) << endreq;
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
      segment->setCKThetaResolution( id, res );

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
RichBinnedCKResVthetaForRecoTracks::ckThetaResolution_Imp( LHCb::RichRecSegment * segment,
                                                           const Rich::ParticleIDType id,
                                                           double & res ) const
{

  // Reference to track ID object
  const RichTrackID & tkID = segment->richRecTrack()->trackID();

  // Check track parent type is Track
  if ( Rich::TrackParent::Track != tkID.parentType() )
  {
    Exception( "Track parent type is not 'Track'" );
  }

  res = 0;
  bool OK(false);

  // Expected Cherenkov theta angle
  const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( thetaExp > 1e-6 )
  {

    // radiator type
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // track type
    const Rich::Track::Type type = tkID.trackType();

    // check data is OK
    if ( m_binEdges[rad].empty() ||
         (m_binEdges[rad].size() != m_theerr[rad][type].size()) )
    {
      Exception( "Mis-match in binned CK data for : " + Rich::text(rad) + " " + Rich::text(type) );
    }

    // search the bins
    BinEdges::const_reverse_iterator iBins = (m_binEdges[rad]).rbegin();
    BinData::const_reverse_iterator  iData = (m_theerr[rad][type]).rbegin();
    for ( ; iBins != (m_binEdges[rad]).rend() && iData != (m_theerr[rad][type]).rend();
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
      Error( "Failed to find binned CK resolution for : " + Rich::text(rad) + " " + Rich::text(type) );
    }

  }

  // return the final status
  return OK;
}

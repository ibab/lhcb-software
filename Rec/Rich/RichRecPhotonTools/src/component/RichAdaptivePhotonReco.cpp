
//-----------------------------------------------------------------------------
/** @file RichAdaptivePhotonReco.cpp
 *
 * Implementation file for class : Rich::Rec::AdaptivePhotonReco
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @author Antonis Papanestis
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichAdaptivePhotonReco.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AdaptivePhotonReco::
AdaptivePhotonReco( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : PhotonRecoBase  ( type, name, parent ),
    m_nomSatCKTheta ( Rich::NRadiatorTypes, -1 )
{
  // Job options                                          Aero R1gas R2gas
  declareProperty( "MaxFracCKTheta", m_maxFracCKtheta = { 0.0, 0.95, 0.95 } );
  declareProperty( "MinFracCKTheta", m_minFracCKtheta = { 1.0, 0.00, 0.00 } );

  //setProperty( "Outputlevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
AdaptivePhotonReco::~AdaptivePhotonReco() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode AdaptivePhotonReco::initialize()
{
  // initialise base class
  const StatusCode sc = PhotonRecoBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",     m_ckAngle        );
  acquireTool( "AdaptiveSlowPhotonReco", m_recoSlow, this );
  acquireTool( "AdaptiveFastPhotonReco", m_recoFast, this );
 
  return sc;
}

//-------------------------------------------------------------------------
//  reconstruct a photon from track segment and pixel
//-------------------------------------------------------------------------
StatusCode
AdaptivePhotonReco::
reconstructPhoton ( const LHCb::RichRecSegment * segment,
                    const LHCb::RichRecPixel * pixel,
                    LHCb::RichGeomPhoton& gPhoton ) const
{
  // need to remove this ...
  LHCb::RichRecSegment * seg = const_cast<LHCb::RichRecSegment*>(segment);

  // radiator for this segment
  const auto rad = segment->trackSegment().radiator();

  // How far from CK theta saturation (for kaon) is this segment ?
  const auto kaonFracCKThetaSat = 
    ( m_ckAngle->avgCherenkovTheta(seg,Rich::Kaon) / nomSatCKTheta(rad) );
      
  // Delegate the photon reconstruction
  return ( kaonFracCKThetaSat > m_minFracCKtheta[rad] &&
           kaonFracCKThetaSat < m_maxFracCKtheta[rad] ? 
           m_recoFast : m_recoSlow ) -> reconstructPhoton(segment,pixel,gPhoton);  
}

//=============================================================================
// Declaration of the Algorithm Factory
//=============================================================================

DECLARE_TOOL_FACTORY ( AdaptivePhotonReco )

//=============================================================================


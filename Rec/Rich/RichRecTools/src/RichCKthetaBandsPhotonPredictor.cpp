
//-----------------------------------------------------------------------------
/** @file RichCKthetaBandsPhotonPredictor.cpp
 *
 *  Implementation file for tool : Rich::Rec::CKthetaBandsPhotonPredictor
 *
 *  CVS Log :-
 *  $Id: RichCKthetaBandsPhotonPredictor.cpp,v 1.2 2007-08-10 18:15:27 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   26/07/2007
 */
//-----------------------------------------------------------------------------

// local
#include "RichCKthetaBandsPhotonPredictor.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( CKthetaBandsPhotonPredictor );

// Standard constructor
CKthetaBandsPhotonPredictor::
CKthetaBandsPhotonPredictor( const std::string& type,
                             const std::string& name,
                             const IInterface* parent )
  : Rich::Rec::ToolBase( type, name, parent ),
    m_geomTool      ( NULL ),
    m_ckAngle       ( NULL ),
    m_ckRes         ( NULL ),
    m_richPartProp  ( NULL ),
    m_minROI        ( Rich::NRadiatorTypes, 0 ),
    m_maxROI        ( Rich::NRadiatorTypes, 0 ),
    m_ckThetaMax    ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax       ( Rich::NRadiatorTypes, 0 ),
    m_nSigma        ( Rich::NRadiatorTypes, 0 ),
    m_scale         ( Rich::NRadiatorTypes, 0 )
{

  // interface
  declareInterface<IPhotonPredictor>(this);

  // job options

  m_minROI[Rich::Aerogel]  = 110; // aerogel
  m_minROI[Rich::Rich1Gas] = 0;   // rich1Gas
  m_minROI[Rich::Rich2Gas] = 0;   // rich2Gas
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI[Rich::Aerogel]  = 390;   // aerogel
  m_maxROI[Rich::Rich1Gas] =  86;   // rich1Gas
  m_maxROI[Rich::Rich2Gas] = 165;   // rich2Gas
  declareProperty( "MaxTrackROI", m_maxROI );

  m_ckThetaMax[Rich::Aerogel]  = 0.24;
  m_ckThetaMax[Rich::Rich1Gas] = 0.0520;
  m_ckThetaMax[Rich::Rich2Gas] = 0.03;
  declareProperty( "CKthetaMax", m_ckThetaMax );

  m_sepGMax[Rich::Aerogel]   = 342;
  m_sepGMax[Rich::Rich1Gas]  = 75;
  m_sepGMax[Rich::Rich2Gas]  = 130;
  declareProperty( "SepGMax", m_sepGMax );

  m_nSigma[Rich::Aerogel]   = 6.5;
  m_nSigma[Rich::Rich1Gas]  = 6.5;
  m_nSigma[Rich::Rich2Gas]  = 12;
  declareProperty( "NSigma", m_nSigma );

}

StatusCode CKthetaBandsPhotonPredictor::initialize()
{
  // Initialise base class
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichCherenkovAngle",        m_ckAngle  );
  acquireTool( "RichRecGeometry",           m_geomTool );
  acquireTool( "RichCherenkovResolution",   m_ckRes    );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // loop over radiators
  for ( int rad = 0; rad < Rich::NRadiatorTypes; ++rad )
  {
    // cache some numbers
    m_minROI2.push_back( m_minROI[rad]*m_minROI[rad] );
    m_maxROI2.push_back( m_maxROI[rad]*m_maxROI[rad] );
    m_scale[rad] = (m_ckThetaMax[rad]/m_sepGMax[rad]);
    // printout for this rad
    std::string trad = Rich::text((Rich::RadiatorType)rad);
    trad.resize(8,' ');
    info() << trad << " : Sep. range     "
           << boost::format("%5.1f") % m_minROI[rad] << " -> "
           << boost::format("%5.1f") % m_maxROI[rad] << " mm  : Tol. "
           << boost::format("%5.1f") % m_nSigma[rad] << " # sigma" << endreq;
  }

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endreq;

  return sc;
}

// fast decision on whether a photon is possible
bool
CKthetaBandsPhotonPredictor::photonPossible( LHCb::RichRecSegment * segment,
                                             LHCb::RichRecPixel * pixel ) const
{

  // Default to not selected
  bool OK = false;

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() == pixel->detector() )
  {

    // segment / hit separation squared
    const double sep2 = m_geomTool->trackPixelHitSep2(segment,pixel);

    // which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Check overall boundaries
    if ( sep2 < m_maxROI2[rad] && sep2 > m_minROI2[rad] )
    {

      // estimated CK theta
      const double ckThetaEsti = sqrt(sep2)*m_scale[rad];

      // Loop over mass hypos and check finer grained boundaries
      for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
            hypo != m_pidTypes.end(); ++hypo )
      {

        // expected CK theta
        const double expCKtheta = m_ckAngle->avgCherenkovTheta(segment,*hypo);
        // expected CK theta resolution
        const double expCKres   = m_ckRes->ckThetaResolution(segment,*hypo);

        // is this pixel/segment pair in the accepted range
        if ( fabs(expCKtheta-ckThetaEsti) < m_nSigma[rad]*expCKres )
        {
          OK = true;
          break;
        }

      } // loop over hypos

    } // boundary check

  } // detector check

  return OK;
}


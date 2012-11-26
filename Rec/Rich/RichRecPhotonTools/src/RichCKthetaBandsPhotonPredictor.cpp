
//-----------------------------------------------------------------------------
/** @file RichCKthetaBandsPhotonPredictor.cpp
 *
 *  Implementation file for tool : Rich::Rec::CKthetaBandsPhotonPredictor
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

DECLARE_TOOL_FACTORY( CKthetaBandsPhotonPredictor )

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
    m_maxROI        ( Rich::NRadiatorTypes, 0 ),
    m_ckThetaMax    ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax       ( Rich::NRadiatorTypes, 0 ),
    m_nSigma        ( Rich::NRadiatorTypes, 0 ),
    m_scale         ( Rich::NRadiatorTypes, 0 ),
    m_minXlocal     ( Rich::NRadiatorTypes, 0 ),
    m_minYlocal     ( Rich::NRadiatorTypes, 0 )
{
  using namespace boost::assign;
  using namespace Gaudi::Units;

  // interface
  declareInterface<IPhotonPredictor>(this);

  std::vector<double>
    // default values    Aero    R1Gas   R2Gas
    t1 =        list_of  (110)   (0)     (0)     ,
    t2 =        list_of  (390)   (86)    (165)   ,
    t3 =        list_of  (0.24)  (0.052) (0.03)  ,
    t4 =        list_of  (342)   (75)    (130)   ,
    t5 =        list_of  (-1*mm) (-1*mm) (-1*mm) ,
    t6 =        list_of  (-1*mm) (-1*mm) (-1*mm) ,
    t7 = (contextContains("HLT"))
         ?      list_of  (4.5)   (4.5)   (10.5)    // Online
         :      list_of  (5.5)   (5.5)   (11.5)  ; // Offline
  m_minROI     = t1;
  m_maxROI     = t2;
  m_ckThetaMax = t3;
  m_sepGMax    = t4;
  m_minXlocal  = t5;
  m_minYlocal  = t6;
  m_nSigma     = t7;
  
  // job options
  declareProperty( "MinTrackROI", m_minROI );
  declareProperty( "MaxTrackROI", m_maxROI );
  declareProperty( "CKthetaMax", m_ckThetaMax );
  declareProperty( "SepGMax", m_sepGMax );
  declareProperty( "NSigma", m_nSigma );
  declareProperty( "MinPixelXLocal", m_minXlocal );
  declareProperty( "MinPixelYLocal", m_minYlocal );

}

StatusCode CKthetaBandsPhotonPredictor::initialize()
{
  // Initialise base class
  const StatusCode sc = ToolBase::initialize();
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
           << boost::format("%5.1f") % m_nSigma[rad] << " # sigma" << endmsg;
  }

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endmsg;

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

    // which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Central region cut
    // Most useful for aerogel since true photons tend to be in the outer regions of
    // the detector plane, and cutting central photons removes a significant number
    // of fake photon candidates
    const Gaudi::XYZPoint& locPos = pixel->radCorrLocalPositions().position(rad);
    if ( fabs(locPos.x()) > m_minXlocal[rad] ||
         fabs(locPos.y()) > m_minYlocal[rad] )
    {

      // segment / hit separation squared
      const double sep2 = m_geomTool->trackPixelHitSep2(segment,pixel);

      // Check overall boundaries
      if ( sep2 < m_maxROI2[rad] && sep2 > m_minROI2[rad] )
      {

        // estimated CK theta
        const double ckThetaEsti = std::sqrt(sep2)*m_scale[rad];

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

    } // central region check

  } // detector check

  return OK;
}

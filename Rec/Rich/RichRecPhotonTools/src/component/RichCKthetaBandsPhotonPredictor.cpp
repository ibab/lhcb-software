
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
    m_scale         ( Rich::NRadiatorTypes, 0 )
{
  using namespace Gaudi::Units;

  // interface
  declareInterface<IPhotonPredictor>(this);

  // default values                                 Aero    R1Gas   R2Gas
  // job options
  declareProperty( "MinTrackROI",    m_minROI    = { 110,     0,       0 } );
  declareProperty( "MaxTrackROI",    m_maxROI    = { 390,    86,     165 } );
  declareProperty( "NSigma",         m_nSigma    = { 5.5,   5.5,    11.5 } );
  declareProperty( "MinPixelXLocal", m_minXlocal = { -1*mm, -1*mm, -1*mm } );
  declareProperty( "MinPixelYLocal", m_minYlocal = { -1*mm, -1*mm, -1*mm } );
}

StatusCode CKthetaBandsPhotonPredictor::initialize()
{
  // Initialise base class
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichCherenkovAngle",          m_ckAngle  );
  acquireTool( "RichRecGeometry",             m_geomTool );
  acquireTool( "RichCherenkovResolution",     m_ckRes    );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // Are we upgrade detector ?
  DeRich1 * rich1DE = getDet<DeRich1>( DeRichLocations::Rich1 );
  const bool isUpgrade = rich1DE->RichGeometryConfig() == 1;

  // Setup scaling parameters
  std::vector<double> ckThetaMax; // Scaling parameter - Max CK theta point
  std::vector<double> sepGMax;    // Scaling parameter - Max separation point
  // RICH1 Gas rings are larger in the upgrade ...
  if ( !isUpgrade )
  {
    ckThetaMax = { 0.24, 0.052, 0.03 };
    sepGMax    = { 342,  75,     130 };
  }
  else
  {
    ckThetaMax = { 0.24, 0.052, 0.03 };
    sepGMax    = { 342,  98.2,   130 };
  }

  // loop over radiators
  for ( int rad = 0; rad < Rich::NRadiatorTypes; ++rad )
  {
    // cache some numbers
    m_minROI2.push_back( m_minROI[rad]*m_minROI[rad] );
    m_maxROI2.push_back( m_maxROI[rad]*m_maxROI[rad] );
    m_scale[rad] = (ckThetaMax[rad]/sepGMax[rad]);
    // printout for this rad
    std::string trad = Rich::text((Rich::RadiatorType)rad);
    trad.resize(8,' ');
    _ri_debug << trad << " : Sep. range     "
              << boost::format("%5.1f") % m_minROI[rad] << " -> "
              << boost::format("%5.1f") % m_maxROI[rad] << " mm  : Tol. "
              << boost::format("%5.1f") % m_nSigma[rad] << " # sigma" << endmsg;
  }

  m_pidTypes = m_richPartProp->particleTypes();
  _ri_debug << "Particle types considered = " << m_pidTypes << endmsg;

  return sc;
}

// fast decision on whether a photon is possible
bool
CKthetaBandsPhotonPredictor::photonPossible( LHCb::RichRecSegment * segment,
                                             LHCb::RichRecPixel * pixel ) const
{

  // Default to not selected
  bool OK = false;

  // cache reference to track segment (avoids mulitple obj pointer lookup checks).
  const auto & tkSeg = segment->trackSegment();

  // Are they in the same Rich detector ?
  if ( tkSeg.rich() == pixel->detector() )
  {

    // which radiator
    const Rich::RadiatorType rad = tkSeg.radiator();

    // Central region cut
    // Most useful for aerogel since true photons tend to be in the outer regions of
    // the detector plane, and cutting central photons removes a significant number
    // of fake photon candidates
    const Gaudi::XYZPoint& locPos = pixel->radCorrLocalPositions().position(rad);
    if ( fabs(locPos.x()) > m_minXlocal[rad] ||
         fabs(locPos.y()) > m_minYlocal[rad] )
    {

      // segment / hit separation squared
      //const double sep2 = m_geomTool->trackPixelHitSep2(segment,pixel);

      // ---------------------------------------------------------------------------
      // WARNING - Pasting code from virtual call here for speed
      // ---------------------------------------------------------------------------

      double sep2 = 99999999;

      // Pixel position, in local HPD coords corrected for average radiator distortion
      const Gaudi::XYZPoint & pixP
        = pixel->radCorrLocalPositions().position(tkSeg.radiator());

      // segment position ray traced to HPD panel, in local HPD coords
      const Gaudi::XYZPoint & segP = segment->pdPanelHitPointLocal();
      
      if ( ( Rich::Rich1 == pixel->detector() && // RICH1
             ( ( ( pixP.y() > 0 && segment->photonsInYPlus()  ) ||
                 ( pixP.y() < 0 && segment->photonsInYMinus() ) ) || 
               pixP.y()*segP.y() > 0 ) )
           || // RICH2
           ( ( ( pixP.x() > 0 && segment->photonsInXPlus()  ) ||
               ( pixP.x() < 0 && segment->photonsInXMinus() ) ) || 
             pixP.x()*segP.x() > 0 ) )
      {
        const Gaudi::XYZPoint & segPanelPnt = 
          segment->pdPanelHitPointLocal(pixel->hpdPixelCluster().panel().panel());
        
        const double dx = fabs( pixP.x() - segPanelPnt.x() );
        if ( dx < m_maxROI[rad] )
        {
          const double dy = fabs( pixP.y() - segPanelPnt.y() );
          if ( dy < m_maxROI[rad] )
          {
            //sep2 = (pixP-segPanelPnt).Mag2();
            sep2 = gsl_pow_2(dx) + gsl_pow_2(dy);
          }
        }
      }

     // ---------------------------------------------------------------------------

      // Check overall boundaries
      if ( sep2 < m_maxROI2[rad] && sep2 > m_minROI2[rad] )
      {

        // estimated CK theta
        const double ckThetaEsti = std::sqrt(sep2)*m_scale[rad];

        // Loop over mass hypos and check finer grained boundaries
        for ( const auto hypo : m_pidTypes )
        {

          // expected CK theta
          const double expCKtheta = m_ckAngle->avgCherenkovTheta(segment,hypo);
          // expected CK theta resolution
          const double expCKres   = m_ckRes->ckThetaResolution(segment,hypo);

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


//-----------------------------------------------------------------------------
/** @file RichRecGeomTool.cpp
 *
 *  Implementation file for tool : Rich::Rec::GeomTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecGeomTool.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( GeomTool )

// Standard constructor
GeomTool::GeomTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : ToolBase      ( type, name, parent ),
    m_detParams   ( NULL ),
    m_ckAngle     ( NULL ),
    m_radScale    ( Rich::NRadiatorTypes, 0 )
{
  // interface
  declareInterface<IGeomTool>(this);

  // job options
  m_radScale[Rich::Aerogel]  =  0.03;
  m_radScale[Rich::Rich1Gas] =  0.017;
  m_radScale[Rich::Rich2Gas] = -0.014;
  declareProperty( "RadOpticalCorrections", m_radScale );
}

StatusCode GeomTool::initialize()
{
  // Set s up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichDetParameters",  m_detParams, NULL, true );
  acquireTool( "RichCherenkovAngle", m_ckAngle   );

  // Cache the acceptance data
  m_radOutLimLoc[Rich::Aerogel]  = m_detParams->AvAcceptOuterLimitsLocal(Rich::Aerogel);
  m_radOutLimLoc[Rich::Rich1Gas] = m_detParams->AvAcceptOuterLimitsLocal(Rich::Rich1Gas);
  m_radOutLimLoc[Rich::Rich2Gas] = m_detParams->AvAcceptOuterLimitsLocal(Rich::Rich2Gas);

  // info printout
  _ri_debug << "Av. optical distortion correction parameters : " << m_radScale << endmsg;

  return sc;
}

double GeomTool::trackPixelHitSep2( const LHCb::RichRecSegment * segment,
                                    const LHCb::RichRecPixel * pixel ) const
{
  double sep2 = 99999999;

  // NOTE : The code here for speed has been pasted into RichCKthetaBandsPhotonPredictor.cpp
  //        in Rich/RichRecPhotonTools.
  //        Should review if there is a better way to package this to avoid code duplication.

  // Pixel position, in local HPD coords corrected for average radiator distortion
  const Gaudi::XYZPoint & pixP
    = pixel->radCorrLocalPositions().position(segment->trackSegment().radiator());

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
    //sep2 = (pixP-segPanelPnt).Mag2();
    sep2 = gsl_pow_2(pixP.x()-segPanelPnt.x()) + gsl_pow_2(pixP.y()-segPanelPnt.y());
  }

  return sep2;
}

double GeomTool::hpdPanelAcceptance( LHCb::RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) const
{
  // Protect against below threshold case
  if ( Rich::BelowThreshold == id ) return 0;

  // ==============================================
  // CRJ : NB : Method still under construction !!
  // ==============================================

  // The acceptance
  double acc = 0;

  // Cherenkov angle for this mass hypothsis
  const double ckTheta = m_ckAngle->avgCherenkovTheta(segment,id);

  if ( ckTheta > 0 )   // only for tracks above threshold
  {
    acc = 1;

    // radius of ring for given hypothesis
    const double ckRadius = m_ckAngle->avCKRingRadiusLocal(segment,id);

    // Track impact point on HPD panel
    const Gaudi::XYZPoint & tkPoint = segment->pdPanelHitPointLocal();

    // radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // limits
    const IDetParameters::RadLimits & lims = m_radOutLimLoc[rad];

    // Calculate acceptance
    if ( fabs(tkPoint.x()) > (lims.maxX()+ckRadius) ||
         fabs(tkPoint.y()) > (lims.maxY()+ckRadius) ||
         fabs(tkPoint.x()) < (lims.minX()-ckRadius) ||
         fabs(tkPoint.y()) < (lims.minY()-ckRadius) )
    {
      acc = 0.;
    }
    else
    {
      // Near max X positive
      if ( fabs(tkPoint.x()-lims.maxX()) < ckRadius )
      {
        acc *= vdt::fast_acos( (tkPoint.x()-lims.maxX())/ckRadius ) / M_PI;
      }
      // Near max X negative
      else if ( fabs(tkPoint.x()+lims.maxX()) < ckRadius )
      {
        acc *= vdt::fast_acos( (-tkPoint.x()-lims.maxX())/ckRadius ) / M_PI;
      }
      if ( lims.minX() > 0 )
      {
        // Near min X positive
        if ( fabs(tkPoint.x()-lims.minX()) < ckRadius )
        {
          acc *= vdt::fast_acos( (-tkPoint.x()+lims.minX())/ckRadius ) / M_PI;
        }
        // Near min X negative
        else if ( fabs(tkPoint.x()+lims.minX()) < ckRadius )
        {
          acc *= vdt::fast_acos( (tkPoint.x()+lims.minX())/ckRadius ) / M_PI;
        }
      }

      // Near max Y positive
      if ( fabs(tkPoint.y()-lims.maxY()) < ckRadius )
      {
        acc *= vdt::fast_acos( (tkPoint.y()-lims.maxY())/ckRadius ) / M_PI;
      }
      // Near max Y negative
      else if ( fabs(tkPoint.y()+lims.maxY()) < ckRadius )
      {
        acc *= vdt::fast_acos( (-tkPoint.y()-lims.maxY())/ckRadius ) / M_PI;
      }
      if ( lims.minY() > 0 )
      {
        // Near min Y positive
        if ( fabs(tkPoint.y()-lims.minY()) < ckRadius )
        {
          acc *= vdt::fast_acos( (-tkPoint.y()+lims.minY())/ckRadius ) / M_PI;
        }
        // Near min Y negative
        else if ( fabs(tkPoint.y()+lims.minY()) < ckRadius )
        {
          acc *= vdt::fast_acos( (tkPoint.y()+lims.minY())/ckRadius ) / M_PI;
        }
      }

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Segment " << segment->key() << " " << segment->trackSegment().radiator()
              << " " << id << " " << acc << endmsg;
    }

  }

  return acc;
}

void GeomTool::setCorrLocalPos( const LHCb::RichRecPixel * pixel,
                                const Rich::DetectorType rich ) const
{
  LHCb::RichRecPixel * pix = const_cast<LHCb::RichRecPixel*>(pixel);
  RadCorrLocalPositions & pos = pix->radCorrLocalPositions();
  if ( Rich::Rich1 == rich )
  {
    pos.setPosition(Rich::Aerogel, getCorrPos(pixel->localPosition(),Rich::Aerogel));
    pos.setPosition(Rich::Rich1Gas,getCorrPos(pixel->localPosition(),Rich::Rich1Gas));
  }
  else
  {
    pos.setPosition(Rich::Rich2Gas,getCorrPos(pixel->localPosition(),Rich::Rich2Gas));
  }
}

Gaudi::XYZPoint GeomTool::radCorrLocalPos ( const Gaudi::XYZPoint & lpos,
                                            const Rich::RadiatorType rad ) const
{
  return getCorrPos( lpos, rad );
}

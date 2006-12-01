
//-----------------------------------------------------------------------------
/** @file RichRecGeomTool.cpp
 *
 *  Implementation file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.cpp,v 1.13 2006-12-01 17:05:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecGeomTool.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichRecGeomTool );

// Standard constructor
RichRecGeomTool::RichRecGeomTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_detParams     ( NULL ),
    m_ckAngle       ( NULL ),
    m_radScale      ( Rich::NRadiatorTypes, 0 )
{

  // interface
  declareInterface<IRichRecGeomTool>(this);

  // job options
  m_radScale[Rich::Aerogel]  =  0.03;
  m_radScale[Rich::Rich1Gas] =  0.017;
  m_radScale[Rich::Rich2Gas] = -0.014;
  declareProperty( "RadOpticalCorrections", m_radScale );

}

StatusCode RichRecGeomTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichDetParameters",  m_detParams );
  acquireTool( "RichCherenkovAngle", m_ckAngle   );

  // Cache the acceptance data
  m_radOutLimLoc[Rich::Aerogel]  = m_detParams->AvAcceptOuterLimitsLocal(Rich::Aerogel);
  m_radOutLimLoc[Rich::Rich1Gas] = m_detParams->AvAcceptOuterLimitsLocal(Rich::Rich1Gas);
  m_radOutLimLoc[Rich::Rich2Gas] = m_detParams->AvAcceptOuterLimitsLocal(Rich::Rich2Gas);

  // info printout
  info() << "Av. optical distortion correction parameters : " << m_radScale << endreq;

  return sc;
}

StatusCode RichRecGeomTool::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichRecGeomTool::trackPixelHitSep2( const RichRecSegment * segment,
                                           const RichRecPixel * pixel ) const
{
  double sep2 = 99999999;

  // Which radiator
  const Rich::RadiatorType rad = segment->trackSegment().radiator();
  
  // Pixel position, in local HPD coords corrected for average radiator distortion
  const Gaudi::XYZPoint & pixP = pixel->localPosition(rad);

  // Which detector side is the hit on
  const Rich::Side side = pixel->smartID().panel();

  // segment position ray traced to HPD panel, in local HPD coords
  const Gaudi::XYZPoint & segP = segment->pdPanelHitPointLocal();
  
  // segment position ray traced to same HPD panel as hit, in local HPD coords
  const Gaudi::XYZPoint & segPForce = segment->pdPanelHitPointLocal(side);
  
    // Same RICH ?
  if ( segment->trackSegment().rich() == pixel->detector() )
  {
    if ( Rich::Rich1 == pixel->detector() )
    {
      if ( pixP.y()*segP.y() > 0 ||
           ( ( pixP.y() > 0 && segment->photonsInYPlus()  ) ||
             ( pixP.y() < 0 && segment->photonsInYMinus() ) ) )
      {
        sep2 = (pixP-segPForce).Mag2();
      }
    }
    else // RICH2
    {
      if ( pixP.x()*segP.x() > 0 ||
           ( ( pixP.x() > 0 && segment->photonsInXPlus()  ) ||
             ( pixP.x() < 0 && segment->photonsInXMinus() ) ) )
      {
        sep2 = (pixP-segPForce).Mag2();
      }
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "  -> segment at " << segP << " pixel at " << pixP << " : sep2=" << sep2 << endreq;
  }

  return sep2;
}

double RichRecGeomTool::hpdPanelAcceptance( RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) const
{

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
    const IRichDetParameters::RadLimits & lims = m_radOutLimLoc[rad];

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
        acc *= acos( (tkPoint.x()-lims.maxX())/ckRadius ) / M_PI;
      }
      // Near max X negative
      else if ( fabs(tkPoint.x()+lims.maxX()) < ckRadius )
      {
        acc *= acos( (-tkPoint.x()-lims.maxX())/ckRadius ) / M_PI;
      }
      if ( lims.minX() > 0 )
      {
        // Near min X positive
        if ( fabs(tkPoint.x()-lims.minX()) < ckRadius )
        {
          acc *= acos( (-tkPoint.x()+lims.minX())/ckRadius ) / M_PI;
        }
        // Near min X negative
        else if ( fabs(tkPoint.x()+lims.minX()) < ckRadius )
        {
          acc *= acos( (tkPoint.x()+lims.minX())/ckRadius ) / M_PI;
        }
      }

      // Near max Y positive
      if ( fabs(tkPoint.y()-lims.maxY()) < ckRadius )
      {
        acc *= acos( (tkPoint.y()-lims.maxY())/ckRadius ) / M_PI;
      }
      // Near max Y negative
      else if ( fabs(tkPoint.y()+lims.maxY()) < ckRadius )
      {
        acc *= acos( (-tkPoint.y()-lims.maxY())/ckRadius ) / M_PI;
      }
      if ( lims.minY() > 0 )
      {
        // Near min Y positive
        if ( fabs(tkPoint.y()-lims.minY()) < ckRadius )
        {
          acc *= acos( (-tkPoint.y()+lims.minY())/ckRadius ) / M_PI;
        }
        // Near min Y negative
        else if ( fabs(tkPoint.y()+lims.minY()) < ckRadius )
        {
          acc *= acos( (tkPoint.y()+lims.minY())/ckRadius ) / M_PI;
        }
      }

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Segment " << segment->key() << " " << segment->trackSegment().radiator()
              << " " << id << " " << acc << endreq;
    }

  }

  return acc;
}

Gaudi::XYZPoint RichRecGeomTool::correctAvRadiatorDistortion( const Gaudi::XYZPoint & point,
                                                              const Rich::RadiatorType rad ) const
{
  return Gaudi::XYZPoint( (1-m_radScale[rad]) * point.x(),
                          (1+m_radScale[rad]) * point.y(),
                          point.z() );
}


//-----------------------------------------------------------------------------
/** @file RichRecGeomTool.cpp
 *
 *  Implementation file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.cpp,v 1.10 2006-05-05 11:01:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecGeomTool.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecGeomTool>          s_factory ;
const        IToolFactory& RichRecGeomToolFactory = s_factory ;

// Standard constructor
RichRecGeomTool::RichRecGeomTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_detParams     ( 0 ),
    m_ckAngle       ( 0 ),
    m_radScale      ( Rich::NRadiatorTypes, 0 )
{

  // interface
  declareInterface<IRichRecGeomTool>(this);

  // job options
  m_radScale[Rich::Aerogel] =  0.037;
  m_radScale[Rich::Rich1Gas]   =  0.0105;
  m_radScale[Rich::Rich2Gas]     = -0.014;
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
  m_radOutLimLoc[Rich::Aerogel] = m_detParams->AvAcceptOuterLimitsLocal(Rich::Aerogel);
  m_radOutLimLoc[Rich::Rich1Gas]   = m_detParams->AvAcceptOuterLimitsLocal(Rich::Rich1Gas);
  m_radOutLimLoc[Rich::Rich2Gas]     = m_detParams->AvAcceptOuterLimitsLocal(Rich::Rich2Gas);

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

  // in same RICH ?
  if ( segment->trackSegment().rich() == pixel->detector() )
  {

    // Which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    // Pixel position, in local HPD coords corrected for average radiator distortion
    const Gaudi::XYZPoint & pixP = pixel->localPosition(rad);
    // segment position ray traced to HPD panel, in local HPD coords
    const Gaudi::XYZPoint & segP = segment->pdPanelHitPointLocal();

    if ( Rich::Rich1 == pixel->detector() )
    {
      if ( pixP.y()*segP.y() > 0 )
      {
        return (pixP-segP).Mag2(); 
      }
      else if ( ( pixP.y() > 0 && segment->photonsInYPlus()  ) ||
                ( pixP.y() < 0 && segment->photonsInYMinus() ) )
      {
        return ( pixP - Gaudi::XYZPoint(segP.x(),-segP.y(),segP.z()) ).Mag2();
      }
    }
    else // RICH2
    {
      if ( pixP.x()*segP.x() > 0 )
      {
        return (pixP-segP).Mag2(); 
      }
      else if ( ( pixP.x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixP.x() < 0 && segment->photonsInXMinus() ) )
      {
        return ( pixP - Gaudi::XYZPoint(-segP.x(),segP.y(),segP.z()) ).Mag2();
      }
    }

  }

  return 99999999;
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

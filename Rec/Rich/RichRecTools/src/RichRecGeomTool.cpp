
//-----------------------------------------------------------------------------
/** @file RichRecGeomTool.cpp
 *
 *  Implementation file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.cpp,v 1.5 2005-05-13 15:20:38 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecGeomTool.h"

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
    m_ckAngle       ( 0 )
{
  declareInterface<IRichRecGeomTool>(this);
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
  m_radOutLimLoc[Rich::C4F10]   = m_detParams->AvAcceptOuterLimitsLocal(Rich::C4F10);
  m_radOutLimLoc[Rich::CF4]     = m_detParams->AvAcceptOuterLimitsLocal(Rich::CF4);

  return sc;
}

StatusCode RichRecGeomTool::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichRecGeomTool::trackPixelHitSep2Global( const RichRecSegment * segment,
                                                 const RichRecPixel * pixel ) const
{

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().y() * segment->pdPanelHitPoint().y() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->globalPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      return segment->pdPanelHitPoint().distance2( HepPoint3D( pixel->globalPosition().x(),
                                                               -pixel->globalPosition().y(),
                                                               pixel->globalPosition().z() ) );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().x() * segment->pdPanelHitPoint().x() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->globalPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      return segment->pdPanelHitPoint().distance2( HepPoint3D( -pixel->globalPosition().x(),
                                                               pixel->globalPosition().y(),
                                                               pixel->globalPosition().z() ) );
    }

  }

  return 99999999.9;
}

double RichRecGeomTool::trackPixelHitSep2Local( const RichRecSegment * segment,
                                                const RichRecPixel * pixel ) const
{

  if ( Rich::Rich1 == segment->trackSegment().rich() )
  {

    if ( pixel->localPosition().y() * segment->pdPanelHitPointLocal().y() > 0 )
    {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    }
    else if ( ( pixel->localPosition().y() > 0 && segment->photonsInYPlus() ) ||
              ( pixel->localPosition().y() < 0 && segment->photonsInYMinus() ) )
    {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( pixel->localPosition().x(),
                                                                    -pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }

  }
  else // RICH 2
  {

    if ( pixel->localPosition().x() * segment->pdPanelHitPointLocal().x() > 0 )
    {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    }
    else if ( ( pixel->localPosition().x() > 0 && segment->photonsInXPlus()  ) ||
              ( pixel->localPosition().x() < 0 && segment->photonsInXMinus() ) )
    {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( -pixel->localPosition().x(),
                                                                    pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }
  }

  return 99999999.9;
}

double RichRecGeomTool::hpdPanelAcceptance( RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) const
{

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
    const HepPoint3D & tkPoint = segment->pdPanelHitPointLocal();

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


//-----------------------------------------------------------------------------
/** @file RichRecGeomTool.cpp
 *
 *  Implementation file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.cpp,v 1.3 2005-02-02 10:09:29 jonrob Exp $
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

  //return segment->pdPanelHitPointLocal().distance2( pixel->localPosition() );

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->localPosition().y() * segment->pdPanelHitPointLocal().y() > 0 ) {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    } else if ( ( pixel->localPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->localPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( pixel->localPosition().x(),
                                                                    -pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->localPosition().x() * segment->pdPanelHitPointLocal().x() > 0 ) {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    } else if ( ( pixel->localPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->localPosition().x() < 0 && segment->photonsInXMinus() ) ) {
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

  const double yMin1 = 180.;  // approximate edge of optical image from RICH1 gas
  const double xMin2 = 690.;  // approximate edge along x of RICH2 HPD plane
  const double yMin2 = 696.;  // approximate edge along y of RICH2 HPD plane

  // The acceptance
  double acc = 0;

  // Cherenkov angle for this mass hypothsis
  const double ckTheta = m_ckAngle->avgCherenkovTheta(segment,id);

  if ( ckTheta > 0 ) {  // only for tracks above threshold

    // radius of ring for given hypothesis
    const double rSig = m_ckAngle->avCKRingRadiusLocal(segment,id);

    // which radiator
    const Rich::RadiatorType iRad = segment->trackSegment().radiator();

    // Track impact point on HPD panel
    const HepPoint3D & tkPoint = segment->pdPanelHitPointLocal();

    acc = 1;

    // Simple (too simple?) acceptance calculation
    if ( iRad == Rich::C4F10 )  // RICH-1 gas
    {
      // Account for rings that are close to the edges of the detector plane
      if ( fabs(tkPoint.y()) < yMin1-rSig ) {
        acc = 0.;
      } else {
        if ( fabs(tkPoint.y()-yMin1) < rSig ) acc *= acos( -(tkPoint.y()-yMin1)/rSig )/M_PI;
        if ( fabs(tkPoint.y()+yMin1) < rSig ) acc *= acos( (tkPoint.y()+yMin1)/rSig  )/M_PI;
      }
    } else if ( iRad == Rich::CF4 ) // RICH-2 gas
    {
      if ( fabs(tkPoint.x()) > xMin2+rSig ||
           fabs(tkPoint.y()) > yMin2+rSig ) {
        acc = 0.;
      } else {
        if ( fabs(tkPoint.x())       < rSig ) acc *= acos(  -fabs(tkPoint.x())/rSig  )/M_PI;
        if ( fabs(tkPoint.x()-xMin2) < rSig ) acc *= acos(  (tkPoint.x()-xMin2)/rSig )/M_PI;
        if ( fabs(tkPoint.x()+xMin2) < rSig ) acc *= acos( -(tkPoint.x()+xMin2)/rSig )/M_PI;
        if ( fabs(tkPoint.y()-yMin2) < rSig ) acc *= acos(  (tkPoint.y()-yMin2)/rSig )/M_PI;
        if ( fabs(tkPoint.y()+yMin2) < rSig ) acc *= acos( -(tkPoint.y()+yMin2)/rSig )/M_PI;
      }
    } else if ( iRad == Rich::Aerogel ) // Aerogel
    {
      // nothing here yet....
    }

    //info() << "Segment " << segment->key() << " " << iRad << " " << id << " " << acc << endreq;

  }

  return acc;
}
